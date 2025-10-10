#ifndef PIPELINECONTROLLER_HPP_
#define PIPELINECONTROLLER_HPP_

#include "FrameHandler.hpp"
#include "InferenceEngine.hpp"
#include "FrameDecoder.hpp"
#include "ResultSerializer.hpp"

/**
 * @class PipelineController
 * @brief Orchestrates the complete detection server pipeline
 *
 * Coordinates frame receiving, decoding, inference, serialization, and result sending.
 * Manages the complete lifecycle of all pipeline components.
 *
 * Pipeline flow:
 * 1. Receive encoded frame from client (FrameHandler::getLatestFrame)
 * 2. Decode JPEG to cv::Mat (FrameDecoder::decodeJPEG)
 * 3. Validate decoded frame (skip if empty/invalid)
 * 4. Run YOLO inference (InferenceEngine::pushFrame)
 * 5. Retrieve detections (InferenceEngine::getDetections)
 * 6. Serialize to JSON (ResultSerializer::toJson)
 * 7. Send results back to client (FrameHandler::setFrameResult)
 */
class PipelineController {
public:
    PipelineController(const PipelineController&) = delete;
    PipelineController& operator= (const PipelineController&) = delete;
    PipelineController(PipelineController&&) = delete;
    PipelineController& operator=(PipelineController&&) = delete;

    /**
     * @brief Constructor - initializes all pipeline components with default configuration
     */
    PipelineController();

    /**
     * @brief Destructor - stops pipeline and releases resources
     */
    ~PipelineController();

    /**
     * @brief Starts all pipeline components and processing thread
     */
    void start();

    /**
     * @brief Stops all pipeline components and processing thread
     * @note Blocks until all components terminate gracefully
     */
    void stop();

private:
    /**
     * @brief Main processing loop coordinating all pipeline stages
     */
    void process();

    std::thread pipelineThread_;            ///< Pipeline thread
    std::atomic<bool> isRunning_{false};    ///< Pipeline thread state flag

    FrameHandler frameHandler_;             ///< ZeroMQ frame receiver, result sender
    InferenceEngine inferenceEngine_;       ///< YOLO object detection engine
};

#endif