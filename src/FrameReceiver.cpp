#include "FrameReceiver.hpp"

void FrameReceiver::start()
{
    if (isRunning_) { // Prevent multiple threads
        return;
    }
    isRunning_ = true;
    receiverThread_ = std::thread(&FrameReceiver::receiveFrame, this);
}

void FrameReceiver::stop()
{
    isRunning_ = false;
    // Should I add socket close or does zeromq handle it?
    if (receiverThread_.joinable()) {
        receiverThread_.join();
    }
}

std::shared_ptr<std::vector<uchar>> FrameReceiver::getLatestFrame()
{
    std::lock_guard<std::mutex> lock(frameMutex_);
    return latestFrame_;
}

void FrameReceiver::receiveFrame()
{
    while (isRunning_) {
        zmq::message_t msg;
        auto result = serverSocket_.recv(msg, zmq::recv_flags::none);

        if (result) {
            {
                std::lock_guard<std::mutex> lock(frameMutex_);
                latestFrame_ = std::make_shared<std::vector<uchar>>(
                                        static_cast<uchar*>(msg.data()),
                                        static_cast<uchar*>(msg.data()) + msg.size());
            }
        }
    }
}