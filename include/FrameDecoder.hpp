#ifndef FRAMEDECODER_HPP_
#define FRAMEDECODER_HPP_

#include <vector>
#include <opencv2/imgcodecs.hpp>

/**
 * @class FrameDecoder
 * @brief Static utility class for decoding JPEG encoded frames
 *
 * Provides decoding function for converting compressed JPEG data
 * back to cv::Mat format for processing.
 */
class FrameDecoder {
public:
    FrameDecoder() = delete;
    ~FrameDecoder() = delete;
    FrameDecoder(const FrameDecoder&) = delete;
    FrameDecoder& operator=(const FrameDecoder&) = delete;
    FrameDecoder(FrameDecoder&&) = delete;
    FrameDecoder& operator=(FrameDecoder&&) = delete;

    /**
     * @brief Decodes JPEG encoded frame data to cv::Mat
     * @param encodedFrame JPEG encoded frame data as byte vector
     * @return Decoded frame in BGR color format
     * @throws std::runtime_error if decoding fails
     */
    static cv::Mat decodeJPEG(const std::vector<uchar>& encodedFrame);
private:
};

#endif