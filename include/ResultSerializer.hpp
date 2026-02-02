#ifndef RESULTSERIALIZER_HPP_
#define RESULTSERIALIZER_HPP_

#include "DetectionTypes.hpp"
#include <nlohmann/json.hpp>

/**
 * @class ResultSerializer
 * @brief Static utility class for serializing detection results to JSON
 *
 * Converts Detection objects into JSON format suitable for network transmission
 * to client.
 */
class ResultSerializer {
public:
    ResultSerializer() = delete;
    ~ResultSerializer() = delete;
    ResultSerializer(const ResultSerializer&) = delete;
    ResultSerializer& operator=(const ResultSerializer&) = delete;
    ResultSerializer(ResultSerializer&&) = delete;
    ResultSerializer& operator=(ResultSerializer&&) = delete;

    /**
     * @brief Converts detection results to JSON format
     * @param detections Vector of detection objects to serialize
     * @return JSON object containing detections array
     * @note Returns {"detections": []} for empty input
     *
     * Output format:
     * @code
     * {
     *   "detections": [
     *     {
     *       "classId": 0,
     *       "className": "person",
     *       "confidence": 0.95,
     *       "boundingBox": {"x": 100, "y": 150, "width": 50, "height": 80}
     *     }
     *   ]
     * }
     * @endcode
     */
    inline static nlohmann::json toJson(const std::vector<Detection>& detections) {
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
};

#endif