#include "FrameDecoder.hpp"

cv::Mat FrameDecoder::decodeJPEG(const std::vector<uchar>& encodedFrame)
{
    if (encodedFrame.empty()) {
        return {};
    }
    return cv::imdecode(encodedFrame, cv::IMREAD_COLOR);
}