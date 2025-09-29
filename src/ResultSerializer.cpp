#include "ResultSerializer.hpp"

nlohmann::json toJson(const std::vector<Detection>& detections)
{
    nlohmann::json detectionsJson;
    detectionsJson["detections"] = nlohmann::json::array();

    for (const auto& detection : detections) {
        detectionsJson["detections"].push_back({
            {"classId", detection.classId},
            {"label", detection.className},
            {"confidence", detection.confidence},
            {"boundingBox", {
                {"x", detection.boundingBox.x},
                {"y", detection.boundingBox.y},
                {"width", detection.boundingBox.width},
                {"height", detection.boundingBox.height}
            }}
        });
    }

    return detectionsJson;
}