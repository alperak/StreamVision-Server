#include "FrameHandler.hpp"

FrameHandler::FrameHandler(const int port) : context_{ioThreadCount_}, serverSocket_{context_, zmq::socket_type::rep}, port_{port}
{
    // Need Exception handling.
    serverSocket_.bind("tcp://localhost:" + std::to_string(port_));
    std::cout << "Successfully binded to localhost:" + std::to_string(port_) << '\n';
}

FrameHandler::~FrameHandler()
{
    FrameHandler::stop();
}

void FrameHandler::start()
{
    if (isRunning_) { // Prevent multiple threads
        return;
    }
    isRunning_ = true;
    handleThread_ = std::thread(&FrameHandler::receiveFrameAndSendResult, this);
}

void FrameHandler::stop()
{
    isRunning_ = false;
    // Should I add socket close or does zeromq handle it with RAII?
    if (handleThread_.joinable()) {
        handleThread_.join();
    }
}

std::shared_ptr<std::vector<uchar>> FrameHandler::getLatestFrame()
{
    std::lock_guard<std::mutex> lock(frameMutex_);
    return latestFrame_;
}

void FrameHandler::setFrameResult(const nlohmann::json& jsonResult) 
{
    {
        std::lock_guard<std::mutex> lock(jsonResultMutex_);
        latestJsonResult_ = jsonResult;
    }
}

void FrameHandler::receiveFrameAndSendResult()
{
    while (isRunning_) {
        zmq::message_t receivedMsg;
        auto isFrameReceived = serverSocket_.recv(receivedMsg, zmq::recv_flags::none);
        if (isFrameReceived) {
            {
                std::lock_guard<std::mutex> lock(frameMutex_);
                latestFrame_ = std::make_shared<std::vector<uchar>>(
                                        static_cast<uchar*>(receivedMsg.data()),
                                        static_cast<uchar*>(receivedMsg.data()) + receivedMsg.size());
            }
        }

        // In the ZMQ REQ/REP pattern, every recv() must be followed by a send().
        // Therefore, we always send a response for each received frame.
        // Because the detection pipeline runs asynchronously:
        //   - On the first frame, no inference result is available yet -> returns empty JSON
        //   - Each response actually contains the result of the *previous* frame
        //   - This introduces a natural "1" frame latency
        {
            std::lock_guard<std::mutex> lock(jsonResultMutex_);
            // Along with the above comment, the value of jsonString will be "null" in the first frame
            // because latestJsonResult_.dump() is empty and will return "null" when use dump().
            // The client should pay attention to whether the response is "null" or not when receiving it.
            std::string jsonString = latestJsonResult_.dump();
            zmq::message_t responseMsg(jsonString.begin(), jsonString.end());
            serverSocket_.send(responseMsg, zmq::send_flags::none);
            latestJsonResult_.clear();
        }
    }
}