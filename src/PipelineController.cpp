#include "PipelineController.hpp"
#include "FrameDecoder.hpp"
#include "ResultSerializer.hpp"
#include "InferenceTask.hpp"

#include <spdlog/spdlog.h>

PipelineController::PipelineController()
    : inferenceEngine_(std::make_unique<InferenceEngine>()),
      threadPool_(std::make_unique<ThreadPool>(kDefaultWorkerCount)),
      networkManager_(std::make_unique<NetworkManager>())
{
    spdlog::info("[PipelineController] - Initialized with {} worker threads", kDefaultWorkerCount);
}

PipelineController::~PipelineController()
{
    stop();
}

void PipelineController::start()
{
    networkManager_->setRequestHandler([this](ClientRequest request) {
        threadPool_->submit([this,
                             clientId = std::move(request.clientId),
                             encodedFrame = std::move(request.encodedFrame)]() {
            // Decode JPEG
            auto frame = FrameDecoder::decodeJPEG(encodedFrame);
            if (!frame)
                return;

            // Submit inference task (promise/future)
            std::promise<std::vector<Detection>> promise;
            auto future = promise.get_future();
            inferenceEngine_->submitTask(InferenceTask(std::move(*frame), std::move(promise)));

            // Wait for result, serialize, respond
            auto detections = future.get();
            auto json = ResultSerializer::toJson(detections);
            networkManager_->enqueueResponse(clientId, json.dump());
        });
    });

    inferenceEngine_->start();
    networkManager_->start();

    spdlog::info("[PipelineController] - Pipeline started");
}

void PipelineController::stop()
{
    networkManager_->stop();
    threadPool_->shutdown();
    inferenceEngine_->stop();
    spdlog::info("[PipelineController] - Pipeline stopped");
}