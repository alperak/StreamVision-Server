#ifndef CONFIGXML_HPP_
#define CONFIGXML_HPP_

#include <string>
#include <filesystem>
#include <iostream>
#include <tinyxml2.h>

/**
 * @class ConfigXML
 * @brief Thread safe singleton class for managing server configuration from XML file
 *
 * This class provides centralized configuration management for detection server and
 * model inference settings. It uses the Meyers Singleton pattern for thread safe
 * initialization.
 *
 * @note Initialize must be called from main thread before accessing config values
 *
 * Configuration structure:
 *  - Server: Bind IP and port for ZeroMQ socket
 *  - Model: ONNX model path, input dimensions, labels, thresholds and inference target
 *
 * Usage example:
 * @code
 * int main() {
 *     ConfigXML::getInstance().initialize();
 *     std::string modelPath = ConfigXML::getInstance().getModelPath();
 *     std::string target = ConfigXML::getInstance().getInferenceTarget();
 * }
 * @endcode
 */

class ConfigXML {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static ConfigXML& getInstance();

    /**
     * @brief Initialize configuration from XML file
     *
     * Loads configuration from config.xml. If file doesn't exist or is invalid,
     * creates a new file with default values. Missing elements are automatically
     * filled with defaults.
     */
    void initialize();

    /**
     * @brief Get ONNX model file path
     * @return Path to ONNX model file (default: "../model/yolo11s.onnx")
     */
    std::string getModelPath() const noexcept;

    /**
     * @brief Get model input width
     * @return Model input width in pixels (default: 640)
     */
    int getModelInputWidth() const noexcept;

    /**
     * @brief Get model input height
     * @return Model input height in pixels (default: 640)
     */
    int getModelInputHeight() const noexcept;

    /**
     * @brief Get class labels file path
     * @return Path to labels file (default: "../model/labels.txt")
     */
    std::string getLabelsPath() const noexcept;

    /**
     * @brief Get detection score threshold
     * @return Minimum confidence score for detections (default: 0.45)
     */
    float getModelScoreThreshold() const noexcept;

    /**
     * @brief Get Non Maximum Suppression threshold
     * @return NMS IoU threshold for duplicate detection removal (default: 0.50)
     */
    float getModelNMSThreshold() const noexcept;

    /**
     * @brief Get inference execution target
     * @return Inference target device: "CPU" or "GPU" (default: "GPU")
     */
    std::string getInferenceTarget() const noexcept;

    /**
     * @brief Get server bind IP address
     * @return IP address for ZeroMQ socket binding (default: "0.0.0.0")
     */
    std::string getServerIP() const noexcept;

    /**
     * @brief Get server bind port number
     * @return Port number for ZeroMQ socket binding (default: 5555)
     */
    int getServerPort() const noexcept;

private:
    ConfigXML() = default;
    ~ConfigXML() = default;

    ConfigXML(const ConfigXML&) = delete;
    ConfigXML& operator=(const ConfigXML&) = delete;
    ConfigXML(ConfigXML&&) = delete;
    ConfigXML& operator=(ConfigXML&&) = delete;

    /**
     * @brief Load configuration from XML file
     *
     * Attempts to load config.xml. If file doesn't exist or parsing fails,
     * uses default values and creates a new file.
     */
    void load();

    /**
     * @brief Save current configuration to XML file
     *
     * Creates config directory if needed and writes all settings to XML.
     */
    void save() const;

    /**
     * @brief Set all configuration values to defaults
     */
    void setDefaults() noexcept;

    /**
     * @brief Parse and load values from XML root element
     * @param rootElem Pointer to XML root element
     *
     * Only overrides values that exist in XML. Missing values keep their defaults.
     */
    void loadValues(tinyxml2::XMLElement* rootElem);

    /**
     * @brief Print current configuration to console
     */
    void printCurrentConfig() const noexcept;

    std::string modelPath_{};           ///< ONNX model file path
    int modelInputWidth_{};             ///< Model input width
    int modelInputHeight_{};            ///< Model input height
    std::string labelsPath_{};          ///< Class labels file path
    float modelScoreThreshold_{};       ///< Detection confidence threshold
    float modelNMSThreshold_{};         ///< NMS IoU threshold
    std::string inferenceTarget_{};     ///< Inference device (CPU/GPU)

    std::string serverIP_{};            ///< Server bind IP address
    int serverPort_{};                  ///< Server bind port number

    const std::string kconfigPath_{"../config/config.xml"}; ///< Config file path
};

#endif