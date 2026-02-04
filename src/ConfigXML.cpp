#include "ConfigXML.hpp"

#include <sstream>
#include <iomanip>
#include <spdlog/spdlog.h>

ConfigXML& ConfigXML::getInstance()
{
    static ConfigXML instance;
    return instance;
}

void ConfigXML::initialize()
{
    load();
    printCurrentConfig();
}

void ConfigXML::load()
{
    // Set defaults first  to ensures all values are valid
    setDefaults();

    // Check if config.xml exists
    if (!std::filesystem::exists(kconfigPath_)) {
        spdlog::warn("[ConfigXML] - File not found: {}. Creating with default values.", kconfigPath_);
        save();
        return;
    }

    // Try to load config.xml
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(kconfigPath_.c_str()) != tinyxml2::XML_SUCCESS) {
        spdlog::warn("[ConfigXML] - XML parsing error. Defaults will be used.");
        save();
        return;
    }

    // Verify root element exists
    tinyxml2::XMLElement* rootElem = doc.FirstChildElement("Config");
    if (!rootElem) {
        spdlog::warn("[ConfigXML] - Invalid XML format (root missing). Defaults will be used.");
        save();
        return;
    }

    // Load values from XML (overrides defaults where available)
    loadValues(rootElem);
    // Save to complete any missing elements
    save();
}

void ConfigXML::loadValues(tinyxml2::XMLElement* rootElem)
{
    // Load Model settings
    tinyxml2::XMLElement* modelElem = rootElem->FirstChildElement("Model");
    if (modelElem) {
        tinyxml2::XMLElement* modelPathElem = modelElem->FirstChildElement("Path");
        if (modelPathElem && modelPathElem->GetText()) {
            modelPath_ = modelPathElem->GetText();
        }

        tinyxml2::XMLElement* modelInputWidthElem = modelElem->FirstChildElement("InputWidth");
        if (modelInputWidthElem && modelInputWidthElem->GetText()) {
            modelInputWidth_ = std::stoi(modelInputWidthElem->GetText());
        }

        tinyxml2::XMLElement* modelInputHeightElem = modelElem->FirstChildElement("InputHeight");
        if (modelInputHeightElem && modelInputHeightElem->GetText()) {
            modelInputHeight_ = std::stoi(modelInputHeightElem->GetText());
        }

        tinyxml2::XMLElement* labelsPathElem = modelElem->FirstChildElement("LabelsPath");
        if (labelsPathElem && labelsPathElem->GetText()) {
            labelsPath_ = labelsPathElem->GetText();
        }

        tinyxml2::XMLElement* modelScoreThresholdElem = modelElem->FirstChildElement("ScoreThreshold");
        if (modelScoreThresholdElem && modelScoreThresholdElem->GetText()) {
            modelScoreThreshold_ = std::stof(modelScoreThresholdElem->GetText());
        }

        tinyxml2::XMLElement* modelNMSThresholdElem = modelElem->FirstChildElement("NMSThreshold");
        if (modelNMSThresholdElem && modelNMSThresholdElem->GetText()) {
            modelNMSThreshold_ = std::stof(modelNMSThresholdElem->GetText());
        }

        tinyxml2::XMLElement* inferenceTargetElem = modelElem->FirstChildElement("InferenceTarget");
        if (inferenceTargetElem && inferenceTargetElem->GetText()) {
            inferenceTarget_ = inferenceTargetElem->GetText();
        }
    }

    // Load Server settings
    tinyxml2::XMLElement* serverElem = rootElem->FirstChildElement("Server");
    if (serverElem) {
        tinyxml2::XMLElement* ipElem = serverElem->FirstChildElement("IP");
        if (ipElem && ipElem->GetText()) {
            serverIP_ = ipElem->GetText();
        }

        tinyxml2::XMLElement* portElem = serverElem->FirstChildElement("Port");
        if (portElem && portElem->GetText()) {
            serverPort_ = std::stoi(portElem->GetText());
        }
    }
}

void ConfigXML::save() const
{
    // Create config directory if it doesn't exist
    std::filesystem::create_directories(std::filesystem::path(kconfigPath_).parent_path());

    tinyxml2::XMLDocument doc;

    // Create Root element
    tinyxml2::XMLElement* rootElem = doc.NewElement("Config");
    doc.InsertFirstChild(rootElem);

    // Build Model section
    tinyxml2::XMLElement* modelElem = doc.NewElement("Model");
    {
        tinyxml2::XMLElement* modelPathElem = doc.NewElement("Path");
        modelPathElem->SetText(modelPath_.c_str());
        modelElem->InsertEndChild(modelPathElem);

        tinyxml2::XMLElement* modelInputWidthElem = doc.NewElement("InputWidth");
        modelInputWidthElem->SetText(modelInputWidth_);
        modelElem->InsertEndChild(modelInputWidthElem);

        tinyxml2::XMLElement* modelInputHeightElem = doc.NewElement("InputHeight");
        modelInputHeightElem->SetText(modelInputHeight_);
        modelElem->InsertEndChild(modelInputHeightElem);

        tinyxml2::XMLElement* labelsPathElem = doc.NewElement("LabelsPath");
        labelsPathElem->SetText(labelsPath_.c_str());
        modelElem->InsertEndChild(labelsPathElem);

        tinyxml2::XMLElement* modelScoreThresholdElem = doc.NewElement("ScoreThreshold");
        std::ostringstream modelScoreThresholdStream;
        modelScoreThresholdStream << std::fixed << std::setprecision(2) << modelScoreThreshold_;
        modelScoreThresholdElem->SetText(modelScoreThresholdStream.str().c_str());
        modelElem->InsertEndChild(modelScoreThresholdElem);

        tinyxml2::XMLElement* modelNMSThresholdElem = doc.NewElement("NMSThreshold");
        std::ostringstream modelNMSThresholdStream;
        modelNMSThresholdStream << std::fixed << std::setprecision(2) << modelNMSThreshold_;
        modelNMSThresholdElem->SetText(modelNMSThresholdStream.str().c_str());
        modelElem->InsertEndChild(modelNMSThresholdElem);

        tinyxml2::XMLComment* inferenceTargetComment = doc.NewComment(" InferenceTarget: CPU or GPU ");
        modelElem->InsertEndChild(inferenceTargetComment);

        tinyxml2::XMLElement* inferenceTargetElem = doc.NewElement("InferenceTarget");
        inferenceTargetElem->SetText(inferenceTarget_.c_str());
        modelElem->InsertEndChild(inferenceTargetElem);

    }
    rootElem->InsertEndChild(modelElem);

    // Build Server section
    tinyxml2::XMLElement* serverElem = doc.NewElement("Server");
    {
        tinyxml2::XMLElement* ipElem = doc.NewElement("IP");
        ipElem->SetText(serverIP_.c_str());
        serverElem->InsertEndChild(ipElem);
        tinyxml2::XMLElement* portElem = doc.NewElement("Port");
        portElem->SetText(serverPort_);
        serverElem->InsertEndChild(portElem);
    }
    rootElem->InsertEndChild(serverElem);

    // Write to file
    tinyxml2::XMLError result = doc.SaveFile(kconfigPath_.c_str());
    if (result != tinyxml2::XML_SUCCESS) {
        spdlog::error("[ConfigXML] - config.xml could not be saved (error code: {})", static_cast<int>(result));
    } else {
        spdlog::info("[ConfigXML] - Settings saved: {}", kconfigPath_);
    }
}

void ConfigXML::setDefaults() noexcept
{
    modelPath_ = "../model/yolo11s.onnx";
    modelInputWidth_ = 640;
    modelInputHeight_ = 640;
    labelsPath_ = "../model/labels.txt";
    modelScoreThreshold_ = 0.45f;
    modelNMSThreshold_ = 0.50f;
    inferenceTarget_ = "GPU";

    serverIP_ = "0.0.0.0";
    serverPort_ = 5555;
}

void ConfigXML::printCurrentConfig() const noexcept
{
    spdlog::info("[ConfigXML] - Final Configuration Loaded:");
    spdlog::info("  Model:");
    spdlog::info("    Path              : {}", modelPath_);
    spdlog::info("    Input Width       : {}", modelInputWidth_);
    spdlog::info("    Input Height      : {}", modelInputHeight_);
    spdlog::info("    Labels Path       : {}", labelsPath_);
    spdlog::info("    Score Threshold   : {}", modelScoreThreshold_);
    spdlog::info("    NMS Threshold     : {}", modelNMSThreshold_);
    spdlog::info("    Inference Target  : {}", inferenceTarget_);
    spdlog::info("  Server:");
    spdlog::info("    IP     : {}", serverIP_);
    spdlog::info("    Port   : {}", serverPort_);
}

std::string ConfigXML::getModelPath() const noexcept { return modelPath_; }
int ConfigXML::getModelInputWidth() const noexcept { return modelInputWidth_; }
int ConfigXML::getModelInputHeight() const noexcept { return modelInputHeight_; }
std::string ConfigXML::getLabelsPath() const noexcept { return labelsPath_; }
float ConfigXML::getModelScoreThreshold() const noexcept { return modelScoreThreshold_; }
float ConfigXML::getModelNMSThreshold() const noexcept { return modelNMSThreshold_; }
std::string ConfigXML::getInferenceTarget() const noexcept { return inferenceTarget_; }

std::string ConfigXML::getServerIP() const noexcept { return serverIP_; }
int ConfigXML::getServerPort() const noexcept { return serverPort_; }