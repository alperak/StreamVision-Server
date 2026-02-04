#ifndef NETWORKMANAGER_HPP_
#define NETWORKMANAGER_HPP_

#include "ClientRequest.hpp"
#include "ConfigXML.hpp"
#include "ThreadSafeQueue.hpp"

#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include <zmq.hpp>

struct ResponseMessage {
    std::string clientId;    ///< ROUTER identity
    std::string payload;     ///< Serialized data (JSON detection results)
};

/**
 * @brief Manages multi client network I/O over a single ZMQ ROUTER socket.
 *
 * NetworkManager owns a ROUTER socket bound to a configurable endpoint.
 *
 * ZMQ sockets are NOT thread-safe. All recv/send operations are confined
 * to the network I/O thread. External code interacts with NetworkManager
 * exclusively via:
 *  - setRequestHandler(): called once before start()
 *  - enqueueResponse(): pushes into a ThreadSafeQueue
 */
class NetworkManager {
public:
    /**
     * @brief Callback type for processing incoming client requests.
     */
    using RequestHandler = std::function<void(ClientRequest)>;

    /**
     * @brief Constructs and binds the ROUTER socket.
     *
     * Reads the endpoint configuration from the ConfigXML singleton.
     * Sets ZMQ_ROUTER_MANDATORY (error on unknown identity) and
     * drop unsent messages on close (ZMQ_LINGER = 0).
     *
     * @throws std::runtime_error if socket creation or bind fails
     */
    NetworkManager();
    ~NetworkManager();

    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    NetworkManager(NetworkManager&&) = delete;
    NetworkManager& operator=(NetworkManager&&) = delete;

    /**
     * @brief Starts the dedicated network I/O thread.
     */
    void start();

    /**
     * @brief Performs a hard shutdown and joins the network I/O thread.
     *
     * Calls zmq_ctx_shutdown(), causing any blocking recv() in the
     * network I/O thread to fail with ETERM. Pending responses in the
     * queue are dropped.
     */
    void stop();

    /**
     * @brief Registers the callback used to process incoming client requests.
     *
     * Must be called before start().
     * Set by PipelineController to forward requests into the worker thread pool.
     *
     * @param handler Callback invoked with a ClientRequest (move only)
     */
    void setRequestHandler(RequestHandler handler);

    /**
     * @brief Enqueues an outgoing response for delivery to a client.
     *
     * Thread-safe.
     *
     * @param clientId ROUTER identity of the target client
     * @param payload  Serialized response payload (JSON)
     */
    void enqueueResponse(const std::string& clientId, const std::string& payload);

private:
    /**
     * @brief Main I/O loop running on the dedicated network thread.
     *
     * Blocks on recv() waiting for incoming encoded frames. After processing
     * each received frame, drains the response queue and sends all pending
     * outgoing frames.
     *
     * Exits when recv() fails with ETERM (triggered by context_.shutdown()
     * in stop()) or when isRunning_ becomes false.
     */
    void networkIoLoop();

    // ZMQ I/O threads
    static constexpr int kIoThreadCount_{1};
    zmq::context_t context_;
    zmq::socket_t routerSocket_;

    ThreadSafeQueue<ResponseMessage> responseQueue_;

    RequestHandler requestHandler_;  ///< Callback for incoming requests (set by PipelineController)

    std::thread networkThread_;
    std::atomic<bool> isRunning_{false};
};

#endif
