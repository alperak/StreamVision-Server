#ifndef CLIENTREQUEST_HPP_
#define CLIENTREQUEST_HPP_

#include <string>
#include <vector>
#include <opencv2/core/hal/interface.h> //uchar

/**
 * @brief Encapsulates a single client request received over ZeroMQ
 *
 * Move only type that pairs a ROUTER socket identity with the raw
 * JPEG encoded frame payload. Consumed by PipelineController's
 * request handler callback.
 */
struct ClientRequest {
    ClientRequest() = default;
    ~ClientRequest() = default;

    ClientRequest(std::string id, std::vector<uchar> frame)
        : clientId(std::move(id)), encodedFrame(std::move(frame)){};

    ClientRequest(const ClientRequest&) = delete;
    ClientRequest& operator=(const ClientRequest&) = delete;
    ClientRequest(ClientRequest&&) = default;
    ClientRequest& operator=(ClientRequest&&) = default;

    std::string clientId;               ///< ZeroMQ ROUTER identity
    std::vector<uchar> encodedFrame;    ///< Raw JPEG encoded bytes
};
#endif