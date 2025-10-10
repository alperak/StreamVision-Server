#include "FrameHandler.hpp"

FrameHandler::FrameHandler(const int port) : context_{ioThreadCount_}, serverSocket_{context_, zmq::socket_type::rep}, port_{port}
{
    serverSocket_.bind("tcp://0.0.0.0:" + std::to_string(port_));
    std::cout << "Successfully binded to 0.0.0.0:" + std::to_string(port_) << '\n';
}

FrameHandler::~FrameHandler()
{
    FrameHandler::stop();
}

void FrameHandler::start()
{
    if (isRunning_) {
        return;
    }
    isRunning_ = true;
    handleThread_ = std::thread(&FrameHandler::receiveFrameAndSendResult, this);
}

void FrameHandler::stop()
{
    isRunning_ = false;
    if (handleThread_.joinable()) {
        handleThread_.join();
    }
}

std::vector<uchar> FrameHandler::getLatestFrame()
{
    std::lock_guard<std::mutex> lock(frameMutex_);
    return std::move(latestFrame_);
}

void FrameHandler::setFrameResult(const nlohmann::json& jsonResult) 
{
    std::lock_guard<std::mutex> lock(jsonResultMutex_);
    latestJsonResult_ = jsonResult;
}

void FrameHandler::receiveFrameAndSendResult()
{
    while (isRunning_) {
        // Receive encoded frame from client
        zmq::message_t receivedMsg;
        auto isFrameReceived = serverSocket_.recv(receivedMsg, zmq::recv_flags::none);

        if (isFrameReceived) {
            // Store received frame for processing
            {
                std::lock_guard<std::mutex> lock(frameMutex_);
                latestFrame_ = std::vector<uchar>(
                                        static_cast<uchar*>(receivedMsg.data()),
                                        static_cast<uchar*>(receivedMsg.data()) + receivedMsg.size());
            }
        }

        // REQ-REP pattern: every recv() requires a send()
        // Response contains result from previous frame (1 frame latency)
        // Response is "null" on first frame (no inference completed yet)
        {
            std::lock_guard<std::mutex> lock(jsonResultMutex_);
            std::string jsonString = latestJsonResult_.dump(); // Returns "null" if empty
            zmq::message_t responseMsg(jsonString.begin(), jsonString.end());
            serverSocket_.send(responseMsg, zmq::send_flags::none);
        }
    }
}