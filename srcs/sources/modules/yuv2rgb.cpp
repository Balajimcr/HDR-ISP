/**
 * @file yuv2rgb.cpp
 * @author Balaji R (balajimcr@gmail.com)
 * @brief YUV to RGB conversion
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

#define MOD_NAME "yuv2rgb"

static int Yuv2Bgr(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;

    uint8_t *bgr_o = reinterpret_cast<uint8_t *>(frame->data.bgr_u8_o);
    uint8_t *y_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.y);
    uint8_t *u_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.u);
    uint8_t *v_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.v);

    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {
            int pixel_idx = GET_PIXEL_INDEX(iw, ih, frame->info.width);

            auto &b = bgr_o[3 * pixel_idx + 0];
            auto &g = bgr_o[3 * pixel_idx + 1];
            auto &r = bgr_o[3 * pixel_idx + 2];

            int r_tmp = y_i[pixel_idx] + 0 + 1.114 * (v_i[pixel_idx] - 128);
            int g_tmp = y_i[pixel_idx] - 0.395 * (u_i[pixel_idx] - 128) - 0.581 * (v_i[pixel_idx] - 128);
            int b_tmp = y_i[pixel_idx] + 2.032 * (u_i[pixel_idx] - 128) + 0;

            ClipMinMax(r_tmp, 255, 0);
            ClipMinMax(g_tmp, 255, 0);
            ClipMinMax(b_tmp, 255, 0);

            r = static_cast<uint8_t>(r_tmp);
            g = static_cast<uint8_t>(g_tmp);
            b = static_cast<uint8_t>(b_tmp);
        }
    }

    return 0;
}

void RegisterYuv2RgbMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::YUV;
    mod.out_domain = ColorDomains::BGR;

    mod.name = MOD_NAME;
    mod.version = "001";
    mod.run_function = Yuv2Bgr;

    RegisterIspModule(mod);
}