#ifndef CLIENTREQUEST_HPP_
#define CLIENTREQUEST_HPP_

#include <DetectionTypes.hpp>
#include <string>
#include <vector>
#include <future>
#include <opencv2/core/mat.hpp>
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

struct InferenceTask {
    InferenceTask() = default;
    ~InferenceTask() = default;

    InferenceTask(cv::Mat f, std::promise<std::vector<Detection>> p)
        : frame(std::move(f)), resultPromise(std::move(p)){};

    InferenceTask(const InferenceTask&) = delete;
    InferenceTask& operator=(const InferenceTask&) = delete;
    InferenceTask(InferenceTask&&) = default;
    InferenceTask& operator=(InferenceTask&&) = default;

    cv::Mat frame;
    std::promise<std::vector<Detection>> resultPromise;
};

#endif