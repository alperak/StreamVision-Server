#ifndef PIPELINECONTROLLER_HPP_
#define PIPELINECONTROLLER_HPP_

#include "FrameHandler.hpp"
#include "InferenceEngine.hpp"
#include "FrameDecoder.hpp"
#include "ResultSerializer.hpp"

class PipelineController {
public:
    PipelineController();
    ~PipelineController();

    void start();
    void stop();

private:
    void process();

    std::thread pipelineThread_;
    std::atomic<bool> isRunning_{false};

    FrameHandler frameHandler_;
    InferenceEngine inferenceEngine_;
};

#endif