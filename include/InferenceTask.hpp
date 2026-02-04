#ifndef INFERENCETASK_HPP_
#define INFERENCETASK_HPP_

#include "DetectionTypes.hpp"
#include <future>
#include <vector>
#include <opencv2/core/mat.hpp>

/**
 * @struct InferenceTask
 * @brief Pairs a decoded frame with a promise for detection results
 *
 * Move only type submitted to InferenceEngine's task queue.
 * The engine processes the frame and fulfills the promise with
 * detection results, allowing the submitter to block on the future.
 */
struct InferenceTask {
    InferenceTask() = default;
    ~InferenceTask() = default;

    InferenceTask(cv::Mat f, std::promise<std::vector<Detection>> p)
        : frame(std::move(f)), resultPromise(std::move(p)){};

    InferenceTask(const InferenceTask&) = delete;
    InferenceTask& operator=(const InferenceTask&) = delete;
    InferenceTask(InferenceTask&&) = default;
    InferenceTask& operator=(InferenceTask&&) = default;

    cv::Mat frame;                                          ///< Decoded frame for inference
    std::promise<std::vector<Detection>> resultPromise;     ///< Promise fulfilled with detection results
};
#endif