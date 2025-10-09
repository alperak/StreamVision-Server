#ifndef FRAMEHANDLER_HPP_
#define FRAMEHANDLER_HPP_

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <opencv2/core/cvdef.h> // For uchar

/**
 * @class FrameHandler
 * @brief Handles asynchronous encoded frame receiving and detection result sending via ZeroMQ
 *
 * Receives encoded frames from client and sends JSON formatted detection results
 * in a request-reply pattern. Runs in a separate thread
 *
 * @note Uses ZeroMQ REP socket bound to tcp://0.0.0.0:5555 (default)
 * @warning Introduces 1-frame latency due to asynchronous processing
 */
class FrameHandler {
public:
    FrameHandler(const FrameHandler&) = delete;
    FrameHandler& operator=(const FrameHandler&) = delete;
    FrameHandler(FrameHandler&&) = delete;
    FrameHandler& operator=(FrameHandler&&) = delete;

    /**
     * @brief Constructor - initializes ZeroMQ context and binds to specified port
     * @param port TCP port number to bind server socket
     */
    explicit FrameHandler(int port);

    /**
     * @brief Default constructor - uses default port 5555
     */
    FrameHandler() : FrameHandler(kDefaultPort) {}

    /**
     * @brief Destructor - stops communication and releases resources
     */
    ~FrameHandler();

    /**
     * @brief Starts asynchronous frame receiving, detection result sending thread
     */
    void start();

    /**
     * @brief Stops reception thread and closes socket
     * @note Blocks until handler thread terminates
     */
    void stop();

    /**
     * @brief Retrieves the most recently received frame
     * @return Copy of encoded frame data as byte vector
     */
    std::vector<uchar> getLatestFrame() const;

    /**
     * @brief Sets detection result to be sent in next response
     * @param jsonResult JSON object containing detection data
     */
    void setFrameResult(const nlohmann::json& jsonResult);

private:
    /**
     * @brief Main communication loop running in separate thread
     */
    void receiveFrameAndSendResult();

    zmq::context_t context_;                    ///< ZeroMQ context
    static constexpr int ioThreadCount_{1};     ///< I/O threads for ZeroMQ context
    zmq::socket_t serverSocket_;                ///< REP socket for client communication
    static constexpr int kDefaultPort{5555};    ///< Default server port
    int port_;                                  ///< Actual bound port

    std::thread handleThread_;                  ///< Background receive-send thread
    std::atomic<bool> isRunning_{false};        ///< Thread state flag

    std::vector<uchar> latestFrame_{};          ///< Latest received frame buffer
    mutable std::mutex frameMutex_;             ///< Protects frame buffer access

    nlohmann::json latestJsonResult_;           ///< Latest detection result
    std::mutex jsonResultMutex_;                ///< Protects result data access
};

#endif