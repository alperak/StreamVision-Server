#ifndef FRAMEHANDLER_HPP_
#define FRAMEHANDLER_HPP_

#include "ConfigXML.hpp"
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
 * @brief ZeroMQ based REQ-REP server for receiving frames and sending inference results
 *
 * Implements a server that:
 *  - Receives JPEG-encoded frames from clients via ZeroMQ
 *  - Stores the latest frame for processing
 *  - Sends back JSON formatted inference results (with 1 frame latency)
 *
 * Server bind address and port are loaded from ConfigXML
 *
 * @note Thread safe for concurrent access to frames and results
 */
class FrameHandler {
public:
    FrameHandler(const FrameHandler&) = delete;
    FrameHandler& operator=(const FrameHandler&) = delete;
    FrameHandler(FrameHandler&&) = delete;
    FrameHandler& operator=(FrameHandler&&) = delete;

    /**
     * @brief Constructor - initializes ZeroMQ context and binds to address from ConfigXML
     * @note Loads bind IP and port from ConfigXML singleton
     */
    FrameHandler();

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
     * @brief Retrieves and moves the most recently received frame
     * @return Encoded frame data as byte vector (moved from internal buffer)
     */
    std::vector<uchar> getLatestFrame();

    /**
     * @brief Sets the inference result to be sent to client
     * @param jsonResult JSON object containing detection results
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

    std::thread handleThread_;                  ///< Background receive-send thread
    std::atomic<bool> isRunning_{false};        ///< Thread state flag

    std::vector<uchar> latestFrame_{};          ///< Latest received frame buffer
    std::mutex frameMutex_;                     ///< Protects frame buffer access

    nlohmann::json latestJsonResult_;           ///< Latest detection result
    std::mutex jsonResultMutex_;                ///< Protects result data access
};

#endif