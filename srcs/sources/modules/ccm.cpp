/**
 * @file ccm.cpp
 * @author Balaji R (balajimcr@gmail.com)
 * @brief Color correction matrix
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

#define MOD_NAME "ccm"

static int Ccm(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;

    int32_t *bgr_in = reinterpret_cast<int32_t *>(frame->data.bgr_s32_i);
    int32_t *bgr_out = reinterpret_cast<int32_t *>(frame->data.bgr_s32_o);

    FOR_ITER(h, frame->info.height)
    {
        FOR_ITER(w, frame->info.width)
        {
            pixel_idx = h * frame->info.width + w;

            int32_t _r = bgr_in[pixel_idx * 3 + 2];
            int32_t _g = bgr_in[pixel_idx * 3 + 1];
            int32_t _b = bgr_in[pixel_idx * 3 + 0];

            bgr_out[pixel_idx * 3 + 2] = (int32_t)(_r * isp_prm->ccm_prms.ccm[0][0] + _g * isp_prm->ccm_prms.ccm[0][1] + _b * isp_prm->ccm_prms.ccm[0][2]);
            bgr_out[pixel_idx * 3 + 1] = (int32_t)(_r * isp_prm->ccm_prms.ccm[1][0] + _g * isp_prm->ccm_prms.ccm[1][1] + _b * isp_prm->ccm_prms.ccm[1][2]);bgr_out[pixel_idx * 3 + 0] = (int32_t)(_r * isp_prm->ccm_prms.ccm[2][0] + _g * isp_prm->ccm_prms.ccm[2][1] + _b * isp_prm->ccm_prms.ccm[2][2]);

            ClipMinMax<int32_t>(bgr_out[pixel_idx * 3 + 0], (int32_t)isp_prm->info.max_val, 0);
            ClipMinMax<int32_t>(bgr_out[pixel_idx * 3 + 1], (int32_t)isp_prm->info.max_val, 0);
            ClipMinMax<int32_t>(bgr_out[pixel_idx * 3 + 2], (int32_t)isp_prm->info.max_val, 0);
        }
    }

    SwapMem<void>(frame->data.bgr_s32_i, frame->data.bgr_s32_o);

    return 0;
}

void RegisterCcmMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::BGR;
    mod.out_domain = ColorDomains::BGR;

    mod.name = MOD_NAME;
    mod.version = "001";
    mod.run_function = Ccm;

    RegisterIspModule(mod);
}