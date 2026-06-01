/**
 * @file rgb2yuv.cpp
 * @author Balaji R (balajimcr@gmail.com)
 * @brief RGB to YUV conversion
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
#include "modules/modules.h"

#define MOD_NAME "rgb2yuv"

static int Rgb2Yuv(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int32_t *bgr_i = reinterpret_cast<int32_t *>(frame->data.bgr_s32_i);
    uint8_t *y_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.y);
    uint8_t *u_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.u);
    uint8_t *v_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.v);

    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {
            int pixel_idx = GET_PIXEL_INDEX(iw, ih, frame->info.width);

            auto b = bgr_i[3 * pixel_idx + 0];
            auto g = bgr_i[3 * pixel_idx + 1];
            auto r = bgr_i[3 * pixel_idx + 2];

            auto y = static_cast<int32_t>(0.299 * r + 0.587 * g + 0.114 * b);
            auto u = static_cast<int32_t>(-0.147 * r - 0.289 * g + 0.436 * b + 128);
            auto v = static_cast<int32_t>(0.615 * r - 0.515 * g - 0.100 * b + 128);

            ClipMinMax<int32_t>(y, 255, 0);
            ClipMinMax<int32_t>(u, 255, 0);
            ClipMinMax<int32_t>(v, 255, 0);

            y_o[pixel_idx] = y;
            u_o[pixel_idx] = u;
            v_o[pixel_idx] = v;
        }
    }

    SwapMem<void>(frame->data.yuv_u8_o.y, frame->data.yuv_u8_i.y);
    SwapMem<void>(frame->data.yuv_u8_o.u, frame->data.yuv_u8_i.u);
    SwapMem<void>(frame->data.yuv_u8_o.v, frame->data.yuv_u8_i.v);

    return 0;
}

void RegisterRgb2YuvMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::BGR;
    mod.out_domain = ColorDomains::YUV;

    mod.name = MOD_NAME;
    mod.version = "001";
    mod.run_function = Rgb2Yuv;

    RegisterIspModule(mod);
}
