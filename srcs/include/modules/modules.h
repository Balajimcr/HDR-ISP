/**
 * @file modules.h
 * @author Balaji R (balajimcr@gmail.com)
 * @brief ISP module registry and parameter structures
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
#ifndef ISP_MODS_H
#define ISP_MODS_H

#include <list>
#include <functional>
#include "common/frame.h"
#include "common/common.h"
#include <cmath>

struct IspPrms
{
    std::string raw_file;
    std::string out_file_path;
    std::string blocks_output_path;
    bool dump_stages = false;
    uint32_t expected_crc = 0;

    std::string sensor_name;
    int blc = 0;
    bool data_packed = false;
    std::list<std::string> pipe;
    ImageInfo info;
    DePwlPrms depwl_prm;
    CcmPrms ccm_prms;
    WbGain wb_gains;
    GammmaCurve y_gamma;
    GammmaCurve rgb_gamma;
    LtmPrms ltm_prms;
    SaturationPrms sat_prms;
    ContrastPrms contrast_prms;
    SharpenPrms sharpen_prms;
    LscPrms lsc_prms;
    DpcPrms dpc_prms;
};

struct IspModule
{
    std::string name;
    std::string version;

    DataPtrTypes in_type;
    DataPtrTypes out_type;
    ColorDomains in_domain;
    ColorDomains out_domain;

    std::function<int(Frame *, const IspPrms *)> run_function;
};

int RegisterIspModule(IspModule mod);
int GetIspModuleFromName(std::string, IspModule &mod);
int ShowAllIspModules();

/**
 * @brief register isp module instance
 */

void RegisterUnpackMod();
void RegisterDePwlMod();
void RegisterBlcMod();
void RegisterDemoasicMod();
void RegisterCcmMod();
void RegisterYGammaMod();
void RegisterWbGaincMod();
void RegisterLtmMod();
void RegisterRgbGammaMod();
void RegisterYuv2RgbMod();
void RegisterRgb2YuvMod();
void RegisterSaturationMod();
void RegisterContrastMod();
void RegisterSharpenMod();
void RegisterLscMod();
void RegisterDpcMod();
void RegisterCnsMod();

#endif
