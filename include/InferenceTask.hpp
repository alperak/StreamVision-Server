#ifndef INFERENCETASK_HPP_
#define INFERENCETASK_HPP_

#include "DetectionTypes.hpp"
#include <future>
#include <vector>
#include <opencv2/core/mat.hpp>

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