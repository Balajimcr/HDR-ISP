/**
 * @file frame.h
 * @author Balaji R (balajimcr@gmail.com)
 * @brief Frame class for image buffer management
 * @version 0.1
 * @date 2026-06-01
 *
 * MIT License
 *
 * Copyright (c) 2026 Balaji R
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
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
