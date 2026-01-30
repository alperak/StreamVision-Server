#ifndef INFERENCEENGINE_HPP_
#define INFERENCEENGINE_HPP_

#include "ConfigXML.hpp"
#include "DetectionTypes.hpp"

#include <thread>
#include <atomic>
#include <opencv2/dnn.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>

/**
 * @struct PaddingInfo
 * @brief Preprocessing transformation parameters
 *
 * Stores the letterbox padding information needed to map
 * detection coordinates back to the original frame.
 */
struct PaddingInfo
{
    cv::Mat paddedFrame{};  ///< Frame with letterbox padding applied
    float scale{};          ///< Scaling factor applied to original frame
    int top{};              ///< Top padding in pixels
    int left{};             ///< Left padding in pixels
};

/**
 * @enum InferenceTarget
 * @brief Specifies the compute target for inference
 *
 * GPU uses CUDA backend (requires OpenCV built with CUDA support)
 * CPU uses OpenCV's default CPU backend
 */
enum class InferenceTarget
{
    GPU,    ///< CUDA GPU acceleration
    CPU     ///< CPU-only inference
};

/**
 * @class InferenceEngine
 * @brief Performs asynchronous object detection using YOLOV11/12 ONNX models with OpenCV DNN
 *
 * Performs object detection using ONNX format YOLOv11/12 models.
 * Runs inference in a separate thread with automatic letterbox padding
 * and NMS post processing.
 *
 * @note All configuration parameters are const and initialized from ConfigXML
 * @note Requires ConfigXML to be initialized before InferenceEngine construction
 */
class InferenceEngine {
public:
    InferenceEngine(const InferenceEngine&) = delete;
    InferenceEngine& operator=(const InferenceEngine&) = delete;
    InferenceEngine(InferenceEngine&&) = delete;
    InferenceEngine& operator=(InferenceEngine&&) = delete;

    /**
     * @brief Constructor - Initializes inference engine from ConfigXML
     *
     * Loads all configuration parameters from ConfigXML singleton:
     *  - ONNX model path
     *  - Model input dimensions (width/height)
     *  - Class labels file path
     *  - Detection score threshold
     *  - NMS threshold
     *  - Inference target (CPU/GPU)
     *
     * @throws std::runtime_error if model file cannot be loaded
     * @throws std::runtime_error if labels file cannot be read
     */
    InferenceEngine();

    /**
     * @brief Destructor - stops inference thread and releases resources
     */
    ~InferenceEngine();

    /**
     * @brief Starts asynchronous inference thread
     */
    void start();

    /**
     * @brief Stops inference thread
     * @note Blocks until inference thread terminates
     */
    void stop();

    /**
     * @brief Submits a frame for inference
     * @param frame Input frame (moved)
     */
    void pushFrame(cv::Mat&& frame);

    /**
     * @brief Retrieves the latest detection results
     * @return Vector of detections from most recent inference
     */
    std::vector<Detection> getDetections() const;

private:
    /**
     * @brief Initializes model with specified backend
     * @throws cv::Exception if model file is invalid
     */
    void loadYoloONNX();

    /**
     * @brief Load COCO labels from text file
     * @throws std::runtime_error If file cannot be opened or has wrong number of labels
     * @note Expects exactly 80 labels for COCO dataset compatibility
     */
    void loadLabels();

    /**
     * @brief Applies letterbox padding for square input
     * @param sourceFrame Original frame of any aspect ratio
     * @return PaddingInfo containing padded frame and transformation parameters
     *
     * Letterbox padding ensures:
     *  - Aspect ratio is preserved (no distortion)
     *  - Frame is centered in square canvas
     *  - Padding uses gray (114) to match YOLO training
     */
    PaddingInfo letterboxPadding(const cv::Mat& frame) const;

    /**
     * @brief Main inference loop running in separate thread
     * @note Processes frames continuously with letterbox, inference, and NMS
     */
    void runInference();

    std::thread inferenceThread_;                   ///< Background inference thread
    std::atomic<bool> isRunning_{false};            ///< Thread state flag

    std::mutex frameMutex_;                         ///< Protects input frame
    cv::Mat latestFrame_;                           ///< Latest frame for inference

    mutable std::mutex resultMutex_;                ///< Protects detection results
    std::vector<Detection> latestDetections_;       ///< Latest inference results

    const std::string onnxModelPath_;               ///< Path to ONNX model
    const cv::Size modelInputSize_;                 ///< Model input dimensions
    const std::string labelsPath_;                  ///< Path to class labels
    const float modelScoreThreshold_;               ///< Minimum confidence threshold
    const float modelNMSThreshold_;                 ///< NMS IoU threshold
    const InferenceTarget inferenceTarget_;         ///< GPU/CPU backend

    std::vector<std::string> labelNames_{};         ///< Loaded COCO label names
    cv::dnn::Net net_;                              ///< OpenCV DNN network instance

    static constexpr int kCocoNumLabels{80};        ///< Expected COCO label count
    static constexpr int kOutputDim{84};            ///< YOLO output features (4 bbox + 80 classes)
    static constexpr int kNumPredictions{8400};     ///< YOLO prediction count
};

#endif