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

    // Start all pipeline components
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

    // Stop components in reverse order for clean shutdown
    inferenceEngine_.stop();
    frameHandler_.stop();
}

void PipelineController::process()
{
    while (isRunning_) {
        // Get encoded frame from client
        auto encodedFrame = frameHandler_.getLatestFrame();
        if (!encodedFrame.empty()) {
            // Decode JPEG frame
            auto decodedFrame = FrameDecoder::decodeJPEG(encodedFrame);
            if (!decodedFrame.empty()) {
                // Run inference
                inferenceEngine_.pushFrame(std::move(decodedFrame));
                auto detections = inferenceEngine_.getDetections();
                // Serialize results to JSON
                auto detectionsAsJson = ResultSerializer::toJson(detections);
                // Send results back to client
                frameHandler_.setFrameResult(detectionsAsJson);
            }
        }
    }
}