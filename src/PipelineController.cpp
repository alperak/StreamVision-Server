#include "PipelineController.hpp"

PipelineController::PipelineController() :
        frameHandler_{},
        inferenceEngine_{"../model/yolo11s.onnx", cv::Size(640, 640), "../model/labels.txt",
                        { .modelScoreThreshold = 0.45f, .modelNMSThreshold = 0.50f },
                        InferenceTarget::GPU}
{

}

PipelineController::~PipelineController()
{
    PipelineController::stop();
}

void PipelineController::start()
{
    if (isRunning_) {
        return;
    }

    frameHandler_.start();
    inferenceEngine_.start();

    isRunning_ = true;
    pipelineThread_ = std::thread(&PipelineController::process, this);
}

void PipelineController::stop()
{
    isRunning_ = false;

    if (pipelineThread_.joinable()) {
        pipelineThread_.join();
    }

    inferenceEngine_.stop();
    frameHandler_.stop();
}

void PipelineController::process()
{
    while (isRunning_) {
        auto encodedFrame = frameHandler_.getLatestFrame();
        if (encodedFrame && !encodedFrame->empty()) {
            auto decodedFrame = FrameDecoder::decodeJPEG(*encodedFrame);
            inferenceEngine_.pushFrame(std::move(decodedFrame));
            auto detections = inferenceEngine_.getDetections();
            auto detectionsAsJson = ResultSerializer::toJson(detections);
            frameHandler_.setFrameResult(detectionsAsJson);
        }
    }
}