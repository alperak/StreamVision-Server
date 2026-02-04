#ifndef INFERENCEENGINE_HPP_
#define INFERENCEENGINE_HPP_

#include "ConfigXML.hpp"
#include "DetectionTypes.hpp"
#include "InferenceTask.hpp"
#include "ThreadSafeQueue.hpp"

#include <thread>
#include <atomic>
#include <optional>
#include <opencv2/dnn.hpp>
#include <opencv2/core/mat.hpp>

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
 * @brief Queue based object detection engine using YOLOv11/12 ONNX models with OpenCV DNN
 *
 * Accepts InferenceTask objects via a ThreadSafeQueue. Each task contains a frame
 * and a std::promise that is fulfilled with detection results. Runs a single
 * inference thread (cv::dnn::Net is not thread safe) that blocks on the queue
 * and processes tasks sequentially.
 *
 * @note All configuration parameters are const and initialized from ConfigXML
 * @note Requires ConfigXML to be initialized before InferenceEngine construction
 * @note Shutdown via poison pill (std::nullopt) through the task queue
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
     * @brief Starts the inference processing thread
     */
    void start();

    /**
     * @brief Stops the inference thread via poison pill
     *
     * Pushes std::nullopt to the task queue to unblock and terminate
     * the processing thread. Blocks until the thread joins.
     */
    void stop();

    /**
     * @brief Submits an inference task to the processing queue
     *
     * Thread-safe. The task's promise will be fulfilled with detection results
     * when inference completes. If the engine is stopped, the promise is
     * immediately fulfilled with an empty vector.
     *
     * @param task InferenceTask containing frame and result promise
     */
    void submitTask(InferenceTask task);

private:
    /**
     * @brief Initializes model with specified backend
     * @throws std::runtime_error if model file cannot be loaded
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
     * @brief Main task-processing loop running in the inference thread
     *
     * Blocks on queue pop() waiting for tasks. Processes each task by running
     * inference and fulfilling the promise. Exits on poison pill (nullopt).
     */
    void processTaskQueue();

    /**
     * @brief Runs inference on a single frame
     *
     * Performs letterbox padding, blob conversion, DNN forward pass,
     * output parsing, and NMS post-processing.
     *
     * @param frame Input frame to process
     * @return Detections found in the frame, or std::nullopt if frame is empty or output is invalid
     */
    std::optional<std::vector<Detection>> processFrame(const cv::Mat& frame);

    std::thread inferenceThread_;
    std::atomic<bool> isRunning_{false};

    ThreadSafeQueue<std::optional<InferenceTask>> taskQueue_;

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