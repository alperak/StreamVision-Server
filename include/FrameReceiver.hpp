#ifndef FRAMERECEIVER_HPP_
#define FRAMERECEIVER_HPP_

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <zmq.hpp>
#include <opencv2/core/cvdef.h> // For uchar

class FrameReceiver {
public:
    FrameReceiver() = delete;
    FrameReceiver(const FrameReceiver&) = delete;
    FrameReceiver& operator=(const FrameReceiver&) = delete;
    FrameReceiver(FrameReceiver&&) = delete;
    FrameReceiver& operator=(FrameReceiver&&) = delete;

    FrameReceiver(const int port = kDefaultPort) :
                context_{ioThreadCount_}, serverSocket_{context_, zmq::socket_type::rep}, port_{port}
    {
        // Need Exception handling.
        serverSocket_.bind("tcp://localhost:" + std::to_string(port));
        std::cout << "Successfully binded to localhost:" + std::to_string(port) << '\n';
    }
    ~FrameReceiver()
    {
        stop();
    }
    void start();
    void stop();
    std::shared_ptr<std::vector<uchar>> getLatestFrame();

private:
    void receiveFrame();

    zmq::context_t context_;
    // Number of internal ZMQ I/O threads in the context(handles socket I/O internally)
    static constexpr int ioThreadCount_{1};
    zmq::socket_t serverSocket_;
    static constexpr int kDefaultPort{5555};
    int port_;

    std::thread receiverThread_;
    std::atomic<bool> isRunning_{false};

    std::shared_ptr<std::vector<uchar>> latestFrame_{};
    std::mutex frameMutex_;
};

#endif