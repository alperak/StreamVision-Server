#ifndef DETECTIONTYPES_HPP_
#define DETECTIONTYPES_HPP_

#include <string>
#include <opencv2/core/core.hpp>

/**
 * @struct Detection
 * @brief Holds complete information about a single detected object
 */
struct Detection
{
    int classId;              ///< Class identifier (0-79 for COCO)
    std::string className;    ///< Readable class name
    float confidence;         ///< Detection confidence score [0.0 - 1.0]
    cv::Rect boundingBox;     ///< Bounding box in original frame coordinates

    /**
     * @brief Constructs a detection object
     * @param id Class identifier
     * @param name Class name
     * @param conf Confidence score
     * @param bbox Bounding box rectangle
     */
    Detection(int id, std::string name, float conf, cv::Rect bbox)
        : classId(id), className(std::move(name)), confidence(conf), boundingBox(bbox) {}
};
#endif