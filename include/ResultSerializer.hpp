#ifndef RESULTSERIALIZER_HPP_
#define RESULTSERIALIZER_HPP_

#include <InferenceEngine.hpp>
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
    static nlohmann::json toJson(const std::vector<Detection>& detections);

private:
};

#endif