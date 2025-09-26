#ifndef FRAMEDECODER_HPP_
#define FRAMEDECODER_HPP_

#include <vector>
#include <opencv2/imgcodecs.hpp>

class FrameDecoder {
public:
    FrameDecoder() = delete;
    ~FrameDecoder() = delete;
    FrameDecoder(const FrameDecoder&) = delete;
    FrameDecoder& operator=(const FrameDecoder&) = delete;
    FrameDecoder(FrameDecoder&&) = delete;
    FrameDecoder& operator=(FrameDecoder&&) = delete;

    static cv::Mat decodeJPEG(const std::vector<uchar>& encodedFrame);
private:
};

#endif