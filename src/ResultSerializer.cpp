#include "ResultSerializer.hpp"

nlohmann::json ResultSerializer::toJson(const std::vector<Detection>& detections)
{
    nlohmann::json detectionsJson;
    detectionsJson["detections"] = nlohmann::json::array();

    // Serialize each detection to JSON format
    for (const auto& detection : detections) {
        detectionsJson["detections"].push_back({
            {"classId", detection.classId},
            {"className", detection.className},
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