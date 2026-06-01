/**
 * @file wbgain.cpp
 * @author Balaji R (balajimcr@gmail.com)
 * @brief White balance gain
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

#define MOD_NAME "wbgain"

static int WbGain(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;
    int pwl_idx = 0;

    int32_t *raw32_in = reinterpret_cast<int32_t *>(frame->data.raw_s32_i);
    int32_t *raw32_out = reinterpret_cast<int32_t *>(frame->data.raw_s32_o);

    // default d65
    float r_gain = isp_prm->wb_gains.d65_gain[0];
    float gr_gain = isp_prm->wb_gains.d65_gain[1];
    float gb_gain = isp_prm->wb_gains.d65_gain[2];
    float b_gain = isp_prm->wb_gains.d65_gain[3];

    FOR_ITER(h, frame->info.height)
    {
        FOR_ITER(w, frame->info.width)
        {
            pixel_idx = h * frame->info.width + w;

            int cfa_id = static_cast<int>(frame->info.cfa);
            switch (kPixelCfaLut[cfa_id][w % 2][h % 2])
            {
            case PixelCfaTypes::R:
                raw32_out[pixel_idx] = (int32_t)(raw32_in[pixel_idx] * r_gain);
                break;
            case PixelCfaTypes::GR:
                raw32_out[pixel_idx] = (int32_t)(raw32_in[pixel_idx] * gr_gain);
                break;
            case PixelCfaTypes::GB:
                raw32_out[pixel_idx] = (int32_t)(raw32_in[pixel_idx] * gb_gain);
                break;
            case PixelCfaTypes::B:
                raw32_out[pixel_idx] = (int32_t)(raw32_in[pixel_idx] * b_gain);
                break;
            default:
                break;
            }

            ClipMinMax<int32_t>(raw32_out[pixel_idx], (int32_t)isp_prm->info.max_val, 0);
        }
    }

    SwapMem<void>(frame->data.raw_s32_i, frame->data.raw_s32_o);

    return 0;
}

void RegisterWbGaincMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::RAW;
    mod.out_domain = ColorDomains::RAW;

    mod.name = MOD_NAME;
    mod.version = "001";
    mod.run_function = WbGain;

    RegisterIspModule(mod);
}