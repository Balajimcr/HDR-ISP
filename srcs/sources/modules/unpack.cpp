/**
 * @file unpack.cpp
 * @author Balaji R (balajimcr@gmail.com)
 * @brief MIPI raw data unpack
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

#define MOD_NAME "unpack"

/**
 * @brief  mipi raw10 to raw16
 * [p1 9:2][p2 9:2][p3 9:2][p4 9:2][(p1 1:0)(p2 1:0)(p3 1:0)(p4 1:0)]
 * @param raw
 * @param unpack_raw16
 * @param width
 * @param height
 */
static void UnpackRaw10ToRaw16(uint8_t *raw, uint16_t *unpack_raw16, int width, int height)
{
    const int pixel_count = width * height;
    const int full_groups = pixel_count / 4;

    for (int g = 0; g < full_groups; ++g)
    {
        uint8_t *raw10_packed_in = raw + g * 5;
        uint16_t *raw16_unpacked_out = unpack_raw16 + g * 4;
        raw16_unpacked_out[0] = ((uint16_t)raw10_packed_in[0] << 2) | (((uint16_t)raw10_packed_in[4] >> 6) & 0x03);
        raw16_unpacked_out[1] = ((uint16_t)raw10_packed_in[1] << 2) | (((uint16_t)raw10_packed_in[4] >> 4) & 0x03);
        raw16_unpacked_out[2] = ((uint16_t)raw10_packed_in[2] << 2) | (((uint16_t)raw10_packed_in[4] >> 2) & 0x03);
        raw16_unpacked_out[3] = ((uint16_t)raw10_packed_in[3] << 2) | (((uint16_t)raw10_packed_in[4] >> 0) & 0x03);
    }

    const int tail = pixel_count % 4;
    if (tail > 0)
    {
        LOG(WARNING) << "RAW10 unpack skipped trailing " << tail << " pixel(s) due to non-4-aligned image size";
    }
}
/**
 * @brief  mipi raw12 to raw16
 * [p1 11:4][p2 11:4][(p1 3:0)(p1 3:0)]
 * @param raw
 * @param unpack_raw16
 * @param width
 * @param height
 */
static void UnpackRaw12ToRaw16(uint8_t *raw, uint16_t *unpack_raw16, int width, int height)
{
    const int pixel_count = width * height;
    const int full_groups = pixel_count / 2;

    for (int g = 0; g < full_groups; ++g)
    {
        uint8_t *raw12_packed_in = raw + g * 3;
        uint16_t *raw16_unpacked_out = unpack_raw16 + g * 2;
        raw16_unpacked_out[0] = (raw12_packed_in[0] << 4) | ((raw12_packed_in[2] >> 4) & 0x0f);
        raw16_unpacked_out[1] = (raw12_packed_in[1] << 4) | ((raw12_packed_in[2] >> 0) & 0x0f);
    }

    const int tail = pixel_count % 2;
    if (tail > 0)
    {
        LOG(WARNING) << "RAW12 unpack skipped trailing " << tail << " pixel(s) due to non-2-aligned image size";
    }
}

/**
 * @brief  mipi raw16 to raw16
 * [p1 15: 8][p1 7 : 0]
 * @param raw
 * @param unpack_raw16
 * @param width
 * @param height
 */
static void UnpackRaw16ToRaw16(uint8_t *raw, uint16_t *unpack_raw16, int width, int height)
{
    uint8_t *raw16_packed_in = raw;
    uint16_t *raw16_unpacked_out = unpack_raw16;

    memcpy(raw16_unpacked_out, raw16_packed_in, width * height * 2);
}

static int MipiDataUnpack(Frame *frame, const IspPrms *prms)
{
    if ((frame == nullptr) || (prms == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }

    switch (frame->info.dt)
    {
    case RawDataTypes::RAW10:
        UnpackRaw10ToRaw16((uint8_t *)frame->data.raw_u8_i, (uint16_t *)frame->data.raw_u16_o, frame->info.width, frame->info.height);
        break;
    case RawDataTypes::RAW12:
        UnpackRaw12ToRaw16((uint8_t *)frame->data.raw_u8_i, (uint16_t *)frame->data.raw_u16_o, frame->info.width, frame->info.height);
        break;
    case RawDataTypes::RAW16:
        UnpackRaw16ToRaw16((uint8_t *)frame->data.raw_u8_i, (uint16_t *)frame->data.raw_u16_o, frame->info.width, frame->info.height);
        break;
    default:
        break;
    }

    SwapMem<void>(frame->data.raw_u16_o, frame->data.raw_u16_i);

    return 0;
}

void RegisterUnpackMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_UINT8;
    mod.out_type = DataPtrTypes::TYPE_UINT16;

    mod.in_domain = ColorDomains::RAW;
    mod.out_domain = ColorDomains::RAW;

    mod.name = MOD_NAME;
    mod.version = "001";

    mod.run_function = MipiDataUnpack;

    RegisterIspModule(mod);
}
