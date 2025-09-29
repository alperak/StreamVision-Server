#ifndef INFERENCEENGINE_HPP_
#define INFERENCEENGINE_HPP_

#include <thread>
#include <atomic>
#include <opencv2/dnn.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>

struct InferenceThreshold
{
    float modelScoreThreshold{};
    float modelNMSThreshold{};
};

struct Detection
{
    int classId{};
    std::string className{};
    float confidence{};
    cv::Rect boundingBox{};

    Detection(const int id, const std::string& name, const float conf, const cv::Rect& bbox)
        :   classId(id), className(name), confidence(conf),
            boundingBox(bbox) {}
};

struct PaddingInfo
{
    cv::Mat paddedFrame{};
    float scale{};
    int top{};
    int left{};
};

enum class InferenceTarget
{
    GPU,
    CPU
};


class InferenceEngine {
public:
    InferenceEngine() = delete;
    InferenceEngine(const InferenceEngine&) = delete;
    InferenceEngine& operator=(const InferenceEngine&) = delete;
    InferenceEngine(InferenceEngine&&) = delete;
    InferenceEngine& operator=(InferenceEngine&&) = delete;

    explicit InferenceEngine(const std::string& onnxModelPath, const cv::Size& modelInputSize,
                                const std::string& labelsPath, const InferenceThreshold& threshold,
                                const InferenceTarget target);

    ~InferenceEngine()
    {
        stop();
    }
    void start();
    void stop();
    void pushFrame(cv::Mat&& frame);
    std::vector<Detection> getDetections() const;

private:
    void loadYoloONNX();
    void loadLabels();
    PaddingInfo letterboxPadding(const cv::Mat& frame) const;
    void runInference();

    std::thread inferenceThread_;
    std::atomic<bool> isRunning_{false};

    std::mutex frameMutex_;
    cv::Mat latestFrame_;

    mutable std::mutex resultMutex_;
    std::vector<Detection> latestDetections_;

    const std::string onnxModelPath_;
    const cv::Size modelInputSize_;
    const std::string labelsPath_;
    const float modelScoreThreshold_;
    const float modelNMSThreshold_;
    const InferenceTarget target_;

    std::vector<std::string> labelNames_{};
    cv::dnn::Net net_;

    static constexpr int kCocoNumLabels{80};
    static constexpr int kOutputDim{84};
    static constexpr int kNumPredictions{8400};
};

#endif