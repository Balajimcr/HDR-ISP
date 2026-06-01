/**
 * @file cns.cpp
 * @author Balaji R (balajimcr@gmail.com)
 * @brief Chroma noise filter
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
#include <algorithm>

#define MOD_NAME "cns"

static int Cns(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    const int width = frame->info.width;
    const int height = frame->info.height;

    uint8_t *u_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.u);
    uint8_t *v_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.v);
    uint8_t *u_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.u);
    uint8_t *v_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.v);


    constexpr int boundary_pixel = 1;
    constexpr int filter_size = (2 * boundary_pixel + 1) * (2 * boundary_pixel + 1);
    constexpr int filter_center = filter_size >> 1;
    uint8_t u[filter_size];
    uint8_t v[filter_size];

    FOR_ITER(ih, height)
    {
        FOR_ITER(iw, width)
        {
            int pixel_idx = ih * width + iw;
            if ((iw < boundary_pixel) || (iw >= (width - boundary_pixel)) || (ih < boundary_pixel) || (ih >= (height - boundary_pixel))) {
                u_o[pixel_idx] = u_i[pixel_idx];
                v_o[pixel_idx] = v_i[pixel_idx];
                continue;
            }

            int sub_index = 0;
            for (int idy = -boundary_pixel; idy <= boundary_pixel; ++idy) {
                for (int idx = -boundary_pixel; idx <= boundary_pixel; ++idx) {
                    int filer_pixel_idx = GET_PIXEL_INDEX((iw + idx), (ih + idy), width);
                    u[sub_index] = u_i[filer_pixel_idx];
                    v[sub_index] = v_i[filer_pixel_idx];
                    ++sub_index;
                }
            }
            // median filter
            std::nth_element(u, u + filter_center, u + filter_size);
            std::nth_element(v, v + filter_center, v + filter_size);
                
            u_o[pixel_idx] = u[filter_center];
            v_o[pixel_idx] = v[filter_center];
        }
    }

    SwapMem<void>(frame->data.yuv_u8_i.u, frame->data.yuv_u8_o.u);
    SwapMem<void>(frame->data.yuv_u8_i.v, frame->data.yuv_u8_o.v);

    return 0;
}

void RegisterCnsMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::YUV;
    mod.out_domain = ColorDomains::YUV;

    mod.name = MOD_NAME;
    mod.version = "001";
    mod.run_function = Cns;

    RegisterIspModule(mod);
}
