#ifndef RESULTSERIALIZER_HPP_
#define RESULTSERIALIZER_HPP_

#include <InferenceEngine.hpp>
#include <nlohmann/json.hpp>

class ResultSerializer {
public:
    ResultSerializer() = delete;
    ~ResultSerializer() = delete;
    ResultSerializer(const ResultSerializer&) = delete;
    ResultSerializer& operator=(const ResultSerializer&) = delete;
    ResultSerializer(ResultSerializer&&) = delete;
    ResultSerializer& operator=(ResultSerializer&&) = delete;

    static nlohmann::json toJson(const std::vector<Detection>& detections);

private:
};

#endif