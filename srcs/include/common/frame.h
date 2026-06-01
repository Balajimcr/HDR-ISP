#ifndef ADAS_ISP_FRAME_H
#define ADAS_ISP_FRAME_H

#include "common.h"
#include <memory>

class Frame
{
private:
    int FrameDateMalloc(int width, int height);
    std::unique_ptr<uint8_t[]> raw_u8_i_buf_;
    std::unique_ptr<uint16_t[]> raw_u16_i_buf_;
    std::unique_ptr<uint16_t[]> raw_u16_o_buf_;
    std::unique_ptr<int32_t[]> raw_s32_i_buf_;
    std::unique_ptr<int32_t[]> raw_s32_o_buf_;

    std::unique_ptr<int32_t[]> bgr_s32_i_buf_;
    std::unique_ptr<int32_t[]> bgr_s32_o_buf_;
    std::unique_ptr<uint8_t[]> bgr_u8_o_buf_;

    std::unique_ptr<float[]> yuv_f32_i_y_buf_;
    std::unique_ptr<float[]> yuv_f32_i_u_buf_;
    std::unique_ptr<float[]> yuv_f32_i_v_buf_;
    std::unique_ptr<float[]> yuv_f32_o_y_buf_;
    std::unique_ptr<float[]> yuv_f32_o_u_buf_;
    std::unique_ptr<float[]> yuv_f32_o_v_buf_;

    std::unique_ptr<uint8_t[]> yuv_u8_i_y_buf_;
    std::unique_ptr<uint8_t[]> yuv_u8_i_u_buf_;
    std::unique_ptr<uint8_t[]> yuv_u8_i_v_buf_;
    std::unique_ptr<uint8_t[]> yuv_u8_o_y_buf_;
    std::unique_ptr<uint8_t[]> yuv_u8_o_u_buf_;
    std::unique_ptr<uint8_t[]> yuv_u8_o_v_buf_;
public:
    ImageInfo info;
    ImageMem data;

public:
    Frame(const ImageInfo &img_info);
    Frame(const Frame &) = delete;
    Frame &operator=(const Frame &) = delete;
    Frame(Frame &&) = delete;
    Frame &operator=(Frame &&) = delete;

    ~Frame();

    int ReadFileToFrame(const std::string &file, size_t size);

    int RawMemToFrame(void *src, size_t len);
};

#endif // !ADAS_ISP_FRAME_H
