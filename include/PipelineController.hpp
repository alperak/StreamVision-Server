#ifndef PIPELINECONTROLLER_HPP_
#define PIPELINECONTROLLER_HPP_

#include "NetworkManager.hpp"
#include "InferenceEngine.hpp"
#include "ThreadPool.hpp"

#include <memory>

/**
 * @class PipelineController
 * @brief Orchestrates the complete detection server pipeline
 *
 * Callback driven pipeline that connects NetworkManager, ThreadPool,
 * and InferenceEngine. No dedicated pipeline thread, the processing
 * is driven by NetworkManager's I/O thread invoking the request handler,
 * which dispatches work to the ThreadPool.
 *
 * Pipeline flow (per client request):
 * 1. NetworkManager I/O thread receives frame, calls requestHandler_
 * 2. requestHandler_ submits a task to ThreadPool
 * 3. ThreadPool worker: decode JPEG -> submit InferenceTask -> future.get()
 * 4. ThreadPool worker: serialize detections -> enqueueResponse()
 * 5. NetworkManager I/O thread sends response back to client
 */
class PipelineController {
public:
    PipelineController();
    ~PipelineController();
    PipelineController(const PipelineController&) = delete;
    PipelineController& operator=(const PipelineController&) = delete;
    PipelineController(PipelineController&&) = delete;
    PipelineController& operator=(PipelineController&&) = delete;

    /**
     * @brief Registers the request handler callback and starts all components
     */
    void start();

    /**
     * @brief Stops all components in correct order
     *
     * NetworkManager, stop accepting new requests
     * ThreadPool, drain existing work
     * InferenceEngine, no more tasks to process
     */
    void stop();

private:
    static constexpr std::size_t kDefaultWorkerCount{4};

    std::unique_ptr<InferenceEngine> inferenceEngine_;
    std::unique_ptr<ThreadPool> threadPool_;
    std::unique_ptr<NetworkManager> networkManager_;
};

#endif