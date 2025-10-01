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

class FrameHandler {
public:
    FrameHandler(const FrameHandler&) = delete;
    FrameHandler& operator=(const FrameHandler&) = delete;
    FrameHandler(FrameHandler&&) = delete;
    FrameHandler& operator=(FrameHandler&&) = delete;

    explicit FrameHandler(int port);
    FrameHandler() : FrameHandler(kDefaultPort) {}

    ~FrameHandler();

    void start();
    void stop();
    std::vector<uchar> getLatestFrame() const;
    void setFrameResult(const nlohmann::json& jsonResult);

private:
    void receiveFrameAndSendResult();

    zmq::context_t context_;
    // Number of internal ZMQ I/O threads in the context(handles socket I/O internally)
    static constexpr int ioThreadCount_{1};
    zmq::socket_t serverSocket_;
    static constexpr int kDefaultPort{5555};
    int port_;

    std::thread handleThread_;
    std::atomic<bool> isRunning_{false};

    std::vector<uchar> latestFrame_{};
    mutable std::mutex frameMutex_;

    nlohmann::json latestJsonResult_;
    std::mutex jsonResultMutex_;
};

#endif