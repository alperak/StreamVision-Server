#ifndef FRAMEDECODER_HPP_
#define FRAMEDECODER_HPP_

#include <vector>
#include <optional>
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
     * @brief Decodes a JPEG encoded frame to OpenCV Mat
     * @param encodedFrame JPEG encoded frame data as byte vector
     * @return Decoded BGR frame or empty Mat if input is empty/invalid
     */
    inline static std::optional<cv::Mat> decodeJPEG(const std::vector<uchar>& encodedFrame) {

        if (encodedFrame.empty())
            return std::nullopt;

        cv::Mat frame = cv::imdecode(encodedFrame, cv::IMREAD_COLOR);
        if (frame.empty())
            return std::nullopt;

        return frame;
    }

private:

};

#endif