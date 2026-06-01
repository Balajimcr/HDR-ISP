/**
 * @file demoasic.cpp
 * @author Balaji R (balajimcr@gmail.com)
 * @brief Demosaicing
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

#define MOD_NAME "demoasic"
/*
R G R G   B Gb B Gb
G B G B   Gr R Gr R
R G R G   B Gb B Gb
G B G B   Gr R Gr R
*/
static int Demoasic(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;

    int32_t *raw32_in = reinterpret_cast<int32_t *>(frame->data.raw_s32_i);
    int32_t *bgr_out = reinterpret_cast<int32_t *>(frame->data.bgr_s32_o);

    FOR_ITER(h, frame->info.height)
    {
        FOR_ITER(w, frame->info.width)
        {
            pixel_idx = h * frame->info.width + w;

            if ((w < 1) || (h < 1) || (w > (frame->info.width - 2)) || (h > (frame->info.height - 2)))
            {
                bgr_out[pixel_idx * 3 + 2] = raw32_in[(h)*frame->info.width + w];
                bgr_out[pixel_idx * 3 + 1] = raw32_in[(h)*frame->info.width + w];
                bgr_out[pixel_idx * 3 + 0] = raw32_in[(h)*frame->info.width + w];
                continue;
            }

            int32_t p0 = raw32_in[(h - 1) * frame->info.width + w - 1];
            int32_t p1 = raw32_in[(h - 1) * frame->info.width + w];
            int32_t p2 = raw32_in[(h - 1) * frame->info.width + w + 1];
            int32_t p3 = raw32_in[(h)*frame->info.width + w - 1];
            int32_t p4 = raw32_in[(h)*frame->info.width + w];
            int32_t p5 = raw32_in[(h)*frame->info.width + w + 1];
            int32_t p6 = raw32_in[(h + 1) * frame->info.width + w - 1];
            int32_t p7 = raw32_in[(h + 1) * frame->info.width + w];
            int32_t p8 = raw32_in[(h + 1) * frame->info.width + w + 1];

            int cfa_id = static_cast<int>(frame->info.cfa);
            switch (kPixelCfaLut[cfa_id][w % 2][h % 2])
            {
            case PixelCfaTypes::R:
                bgr_out[pixel_idx * 3 + 0] = (p0 + p2 + p6 + p8) >> 2; // B
                bgr_out[pixel_idx * 3 + 1] = (p1 + p3 + p5 + p7) >> 2; // G
                bgr_out[pixel_idx * 3 + 2] = p4;                       // R
                break;
            case PixelCfaTypes::GR:
                bgr_out[pixel_idx * 3 + 0] = (p3 + p5) >> 1;
                bgr_out[pixel_idx * 3 + 1] = p4;
                bgr_out[pixel_idx * 3 + 2] = (p1 + p7) >> 1;
                break;
            case PixelCfaTypes::GB:
                bgr_out[pixel_idx * 3 + 0] = (p1 + p7) >> 1;
                bgr_out[pixel_idx * 3 + 1] = p4;
                bgr_out[pixel_idx * 3 + 2] = (p3 + p5) >> 1;
                break;
            case PixelCfaTypes::B:
                bgr_out[pixel_idx * 3 + 0] = p4;
                bgr_out[pixel_idx * 3 + 1] = (p1 + p3 + p5 + p7) >> 2;
                bgr_out[pixel_idx * 3 + 2] = (p0 + p2 + p6 + p8) >> 2;
                break;
            default:
                break;
            }

            ClipMinMax<int32_t>(bgr_out[pixel_idx * 3 + 0], (int32_t)isp_prm->info.max_val, 0);
            ClipMinMax<int32_t>(bgr_out[pixel_idx * 3 + 1], (int32_t)isp_prm->info.max_val, 0);
            ClipMinMax<int32_t>(bgr_out[pixel_idx * 3 + 2], (int32_t)isp_prm->info.max_val, 0);
        }
    }

    SwapMem<void>((frame->data.bgr_s32_o), (frame->data.bgr_s32_i));

    return 0;
}

void RegisterDemoasicMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::RAW;
    mod.out_domain = ColorDomains::BGR;

    mod.name = MOD_NAME;
    mod.version = "001";
    mod.run_function = Demoasic;

    RegisterIspModule(mod);
}