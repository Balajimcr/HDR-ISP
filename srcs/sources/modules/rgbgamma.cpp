/**
 * @file rgbgamma.cpp
 * @author Balaji R (balajimcr@gmail.com)
 * @brief RGB gamma correction
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

#define MOD_NAME "rgbgamma"

static int RgbGamma(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;

    const auto &gamma_prm = isp_prm->rgb_gamma;

    float step_coff = (float)(gamma_prm.nums - 1) / (1 << gamma_prm.in_bits);
    float out_max = (1 << gamma_prm.out_bits) - 1;

    int32_t *bgr_i = reinterpret_cast<int32_t *>(frame->data.bgr_s32_i);
    int32_t *bgr_o = reinterpret_cast<int32_t *>(frame->data.bgr_s32_o);

    FOR_ITER(h, frame->info.height)
    {
        FOR_ITER(w, frame->info.width)
        {
            pixel_idx = h * frame->info.width + w;

            FOR_ITER(color_idx, 3)
            {
                auto color = bgr_i[3 * pixel_idx + color_idx];

                float cuvre_id_f = color * step_coff;
                int curve_id = static_cast<int>(cuvre_id_f);
                if (curve_id < 0) {
                    curve_id = 0;
                } else if (curve_id >= (gamma_prm.nums - 1)) {
                    curve_id = gamma_prm.nums - 2;
                }
                // scale to 0~1
                float scale = (cuvre_id_f - curve_id) * (gamma_prm.curve[curve_id + 1] - gamma_prm.curve[curve_id]) + gamma_prm.curve[curve_id];
                // get scale value
                color = out_max * scale;
                // ClipMinMax(color, isp_prm->info.max_val, 0);
                bgr_o[3 * pixel_idx + color_idx] = color;
            }
        }
    }

    SwapMem<void>(frame->data.bgr_s32_i, frame->data.bgr_s32_o);

    return 0;
}

void RegisterRgbGammaMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::BGR;
    mod.out_domain = ColorDomains::BGR;

    mod.name = MOD_NAME;
    mod.version = "001";
    mod.run_function = RgbGamma;

    RegisterIspModule(mod);
}
