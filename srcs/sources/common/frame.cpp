#include "common/frame.h"

int Frame::FrameDateMalloc(int width, int height)
{
    auto pixel_nums = (width + 8) * (height + 8);
    // raw domain
    raw_u8_i_buf_ = std::make_unique<uint8_t[]>(pixel_nums * 2); // max raw16
    raw_u16_i_buf_ = std::make_unique<uint16_t[]>(pixel_nums);   // max raw16
    raw_u16_o_buf_ = std::make_unique<uint16_t[]>(pixel_nums);   // max raw16
    raw_s32_i_buf_ = std::make_unique<int32_t[]>(pixel_nums);
    raw_s32_o_buf_ = std::make_unique<int32_t[]>(pixel_nums);
    data.raw_u8_i = raw_u8_i_buf_.get();
    data.raw_u16_i = raw_u16_i_buf_.get();
    data.raw_u16_o = raw_u16_o_buf_.get();
    data.raw_s32_i = raw_s32_i_buf_.get();
    data.raw_s32_o = raw_s32_o_buf_.get();
    // bgr domain
    bgr_s32_i_buf_ = std::make_unique<int32_t[]>(pixel_nums * 3);
    bgr_s32_o_buf_ = std::make_unique<int32_t[]>(pixel_nums * 3);
    data.bgr_s32_i = bgr_s32_i_buf_.get();
    data.bgr_s32_o = bgr_s32_o_buf_.get();
    // yuv domain
    yuv_f32_i_y_buf_ = std::make_unique<float[]>(pixel_nums);
    yuv_f32_i_u_buf_ = std::make_unique<float[]>(pixel_nums);
    yuv_f32_i_v_buf_ = std::make_unique<float[]>(pixel_nums);
    yuv_f32_o_y_buf_ = std::make_unique<float[]>(pixel_nums);
    yuv_f32_o_u_buf_ = std::make_unique<float[]>(pixel_nums);
    yuv_f32_o_v_buf_ = std::make_unique<float[]>(pixel_nums);
    data.yuv_f32_i.y = yuv_f32_i_y_buf_.get();
    data.yuv_f32_i.u = yuv_f32_i_u_buf_.get();
    data.yuv_f32_i.v = yuv_f32_i_v_buf_.get();
    data.yuv_f32_o.y = yuv_f32_o_y_buf_.get();
    data.yuv_f32_o.u = yuv_f32_o_u_buf_.get();
    data.yuv_f32_o.v = yuv_f32_o_v_buf_.get();
    // yuv final out
    yuv_u8_i_y_buf_ = std::make_unique<uint8_t[]>(pixel_nums);
    yuv_u8_i_u_buf_ = std::make_unique<uint8_t[]>(pixel_nums);
    yuv_u8_i_v_buf_ = std::make_unique<uint8_t[]>(pixel_nums);
    yuv_u8_o_y_buf_ = std::make_unique<uint8_t[]>(pixel_nums);
    yuv_u8_o_u_buf_ = std::make_unique<uint8_t[]>(pixel_nums);
    yuv_u8_o_v_buf_ = std::make_unique<uint8_t[]>(pixel_nums);
    data.yuv_u8_i.y = yuv_u8_i_y_buf_.get();
    data.yuv_u8_i.u = yuv_u8_i_u_buf_.get();
    data.yuv_u8_i.v = yuv_u8_i_v_buf_.get();
    data.yuv_u8_o.y = yuv_u8_o_y_buf_.get();
    data.yuv_u8_o.u = yuv_u8_o_u_buf_.get();
    data.yuv_u8_o.v = yuv_u8_o_v_buf_.get();

    bgr_u8_o_buf_ = std::make_unique<uint8_t[]>(pixel_nums * 3);
    data.bgr_u8_o = bgr_u8_o_buf_.get();

    return 0;
}

Frame::~Frame() = default;

Frame::Frame(const ImageInfo &img_info)
{
    info = img_info;
    FrameDateMalloc(info.width, info.height);
}

int Frame::RawMemToFrame(void *src, size_t len)
{
    if (len > static_cast<size_t>(info.width) * info.height * 2)
    {
        LOG(ERROR) << "Frame Size err";
        return -1;
    }
    if (data.raw_u8_i)
    {
        memcpy(data.raw_u8_i, src, len);
        return 0;
    }
    return -2;
}

int Frame::ReadFileToFrame(const std::string &file, size_t size)
{
    return ReadFileToMem(file, data.raw_u8_i, size);
}
