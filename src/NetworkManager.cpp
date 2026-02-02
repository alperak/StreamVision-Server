#include "NetworkManager.hpp"

#include <stdexcept>
#include <spdlog/spdlog.h>

NetworkManager::NetworkManager()
    : context_{kIoThreadCount_}, routerSocket_{context_, zmq::socket_type::router}
{
    const std::string endpoint = "tcp://" + ConfigXML::getInstance().getServerIP() + ":" +
                                 std::to_string(ConfigXML::getInstance().getServerPort());

    try {
        // send() to an unknown identity throws instead of silently dropping the message.
        routerSocket_.set(zmq::sockopt::router_mandatory, 1);

        // Drop all unsent messages immediately on socket close.
        // zmq_close() would block waiting for delivery without this.
        routerSocket_.set(zmq::sockopt::linger, 0);

        routerSocket_.bind(endpoint);
    } catch (const zmq::error_t& e) {
        throw std::runtime_error("[NetworkManager] - Failed to bind to " + endpoint + ": " + e.what());
    }

    spdlog::info("[NetworkManager] - ROUTER socket bound to {}", endpoint);
}

NetworkManager::~NetworkManager() {
    spdlog::debug("[NetworkManager] - Destructor called");
    stop();
}

void NetworkManager::start() {
    if (isRunning_.exchange(true)) {
        spdlog::debug("[NetworkManager] - start() called but already running");
        return;
    }
    spdlog::info("[NetworkManager] - Starting network I/O thread");
    networkThread_ = std::thread(&NetworkManager::networkIoLoop, this);
}

void NetworkManager::stop() {
    if (!isRunning_.exchange(false)) {
        spdlog::debug("[NetworkManager] - stop() called but already stopped");
        return;
    }

    spdlog::info("[NetworkManager] - Stopping NetworkManager");

    // zmq_ctx_shutdown() causes ALL blocking ZMQ operations (recv, send, poll)
    // on this context to return immediately with error code ETERM.
    // This is how we break out of the blocking recv() in networkIoLoop().
    context_.shutdown();

    if (networkThread_.joinable()) {
        spdlog::debug("[NetworkManager] - Waiting for network thread to exit");
        networkThread_.join();
        spdlog::info("[NetworkManager] - Network thread joined");
    }

    // The network thread has exited after join.
    // The ROUTER socket and ZMQ context will be cleaned up automatically
    // by RAII when NetworkManager is destroyed.
}

void NetworkManager::setRequestHandler(RequestHandler handler) {
    requestHandler_ = std::move(handler);
}

void NetworkManager::enqueueResponse(const std::string& clientId, const std::string& payload) {
    responseQueue_.push({clientId, payload});
}

void NetworkManager::networkIoLoop() {
    spdlog::debug("[NetworkManager] - Network I/O loop started");
    while (isRunning_) {
        try {
            /**
             * ROUTER-DEALER
             *
             *  Frame 0: identity          (ROUTER adds this automatically)
             *  Frame 1: empty             (DEALER convention / REQ-REP compat)
             *  Frame 2: Encoded JPEG data (actual payload from client)
             *
             * recv() blocks until a complete multipart message arrives
             * or until context_.shutdown() triggers ETERM.
             */
            zmq::message_t identityFrame;
            zmq::message_t delimiterFrame;
            zmq::message_t dataFrame;

            routerSocket_.recv(identityFrame, zmq::recv_flags::none);
            routerSocket_.recv(delimiterFrame, zmq::recv_flags::none);
            routerSocket_.recv(dataFrame, zmq::recv_flags::none);

            std::string clientId(static_cast<char*>(identityFrame.data()), identityFrame.size());
            auto* rawData = static_cast<uchar*>(dataFrame.data());
            std::vector<uchar> frameData(rawData, rawData + dataFrame.size());

            // Forward to pipeline.
            if (requestHandler_) {
                requestHandler_(ClientRequest(std::move(clientId), std::move(frameData)));
            }

            // Send all pending responses that worker threads have enqueued.
            // tryPop() is non-blocking and returns std::nullopt when queue is empty.
            while (auto response = responseQueue_.tryPop()) {
                zmq::message_t id(response->clientId.data(), response->clientId.size());
                zmq::message_t empty;
                zmq::message_t data(response->payload.data(), response->payload.size());

                routerSocket_.send(id, zmq::send_flags::sndmore);
                routerSocket_.send(empty, zmq::send_flags::sndmore);
                routerSocket_.send(data, zmq::send_flags::dontwait);
            }

        } catch (const zmq::error_t& e) {
            if (e.num() == ETERM) {
                // context_.shutdown() was called in stop().
                // This is the expected exit path not an error.
                spdlog::debug("[NetworkManager] - Context terminated, exiting I/O loop");
                break;
            }
            spdlog::error("[NetworkManager] - Error in I/O loop: {}", e.what());
        }
    }
    spdlog::debug("[NetworkManager] - Network I/O loop exited");
}
