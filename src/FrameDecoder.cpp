#include "FrameDecoder.hpp"

cv::Mat FrameDecoder::decodeJPEG(const std::vector<uchar>& encodedFrame)
{
    cv::Mat decoded = cv::imdecode(encodedFrame, cv::IMREAD_COLOR);
    if (decoded.empty()) {
        throw std::runtime_error("Failed to decode frame");
    }
    return decoded;
}