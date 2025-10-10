#include "PipelineController.hpp"
#include "ConfigXML.hpp"
#include <condition_variable>

int main()
{
    // Initialize configuration from config.xml
    ConfigXML::getInstance().initialize();

    PipelineController pipeline;
    pipeline.start();

    std::mutex mtx;
    std::condition_variable cv;

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock); // The main thread sleeps and waits until it is killed with Ctrl + C.

    pipeline.stop();
}