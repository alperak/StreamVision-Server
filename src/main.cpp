#include "PipelineController.hpp"
#include "ConfigXML.hpp"

#include <csignal>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <spdlog/spdlog.h>

namespace {
    std::atomic<bool> g_running{true};
    std::mutex g_shutdownMutex;
    std::condition_variable g_shutdownCv;

    void signalHandler(int signal)
    {
        spdlog::info("[main] - Received signal {}, shutting down", signal);
        g_running = false;
        g_shutdownCv.notify_all();
    }
}

int main()
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        ConfigXML::getInstance().initialize();

        PipelineController pipeline;
        pipeline.start();

        // Block until SIGINT or SIGTERM
        std::unique_lock<std::mutex> lock(g_shutdownMutex);
        g_shutdownCv.wait(lock, [] { return !g_running; });

        pipeline.stop();
    } catch (const std::exception& e) {
        spdlog::error("[main] - Fatal error: {}", e.what());
        return 1;
    }

    spdlog::info("[main] - Clean exit");
    return 0;
}