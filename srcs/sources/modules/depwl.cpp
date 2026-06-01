/**
 * @file depwl.cpp
 * @author Balaji R (balajimcr@gmail.com)
 * @brief Piecewise linear decode
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

#define MOD_NAME "depwl"

static int Depwl(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;
    int pwl_idx = 0;

    uint16_t* raw16_in = reinterpret_cast<uint16_t *>(frame->data.raw_u16_i);
    int32_t*  raw32_out = reinterpret_cast<int32_t *>(frame->data.raw_s32_o);

    const DePwlPrms *pwl_prm = &(isp_prm->depwl_prm);

    FOR_ITER(h, frame->info.height)
    {
        FOR_ITER(w, frame->info.width)
        {
            pixel_idx = h * frame->info.width + w;
            for(int index = 1; index < pwl_prm->pwl_nums; ++index){
                if (raw16_in[pixel_idx] <= pwl_prm->x_cood[index]) {
                    pwl_idx = index;
                    break;
                }
                pwl_idx = index;
            }
            if (pwl_idx == 0) {
                return -1;
            }
            //y = slope * (Xn - Xn-1) + Yn-1
            raw32_out[pixel_idx] = (int32_t)((raw16_in[pixel_idx] - pwl_prm->x_cood[pwl_idx - 1]) * pwl_prm->slope[pwl_idx] \
                                 + pwl_prm->y_cood[pwl_idx - 1]);

            ClipMinMax(raw32_out[pixel_idx], isp_prm->info.max_val, 0);
        }
    }

    SwapMem<void>(frame->data.raw_s32_o, frame->data.raw_s32_i);

    return 0;
}

void RegisterDePwlMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_UINT16;
    mod.out_type = DataPtrTypes::TYPE_INT32;


    mod.in_domain = ColorDomains::RAW;
    mod.out_domain = ColorDomains::RAW;

    mod.name = MOD_NAME;
    mod.version = "001";
    mod.run_function = Depwl;

    RegisterIspModule(mod);
}