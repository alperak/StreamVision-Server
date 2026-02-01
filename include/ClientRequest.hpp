#ifndef CLIENTREQUEST_HPP_
#define CLIENTREQUEST_HPP_

#include <string>
#include <vector>
#include <opencv2/core/hal/interface.h> //uchar

struct ClientRequest {
    ClientRequest() = default;
    ~ClientRequest() = default;

    ClientRequest(std::string id, std::vector<uchar> frame)
        : clientId(std::move(id)), encodedFrame(std::move(frame)){};

    ClientRequest(const ClientRequest&) = delete;
    ClientRequest& operator=(const ClientRequest&) = delete;
    ClientRequest(ClientRequest&&) = default;
    ClientRequest& operator=(ClientRequest&&) = default;

    std::string clientId;               // ROUTER identity
    std::vector<uchar> encodedFrame;    // JPEG bytes
};
#endif