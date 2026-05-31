/**
 * @file pipeline.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief
 * @version 0.1
 * @date 2023-07-27
 *
 * Copyright (c) of ADAS_EYES 2023
 *
 */
#include "modules/modules.h"
#include "common/pipeline.h"
#include "EasyBMP.h"
#include <cstdio>
#include <cstring>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

static void EnsureDirectory(const std::string &path)
{
#ifdef _WIN32
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0777);
#endif
}

static const char *DomainToString(ColorDomains domain)
{
    switch (domain)
    {
    case ColorDomains::RAW:
        return "RAW";
    case ColorDomains::BGR:
        return "BGR";
    case ColorDomains::YUV:
        return "YUV";
    case ColorDomains::HSV:
        return "HSV";
    default:
        return "UNKNOWN";
    }
}

static const char *TypeToString(DataPtrTypes type)
{
    switch (type)
    {
    case DataPtrTypes::TYPE_INT8:
        return "int8";
    case DataPtrTypes::TYPE_UINT8:
        return "uint8";
    case DataPtrTypes::TYPE_UINT16:
        return "uint16";
    case DataPtrTypes::TYPE_INT16:
        return "int16";
    case DataPtrTypes::TYPE_INT32:
        return "int32";
    case DataPtrTypes::TYPE_UINT32:
        return "uint32";
    case DataPtrTypes::TYPE_FLOAT32:
        return "float32";
    default:
        return "unknown";
    }
}

static int DumpStageOutput(int stage_index, const std::string &stage_name, const std::string &stage_ver,
                           Frame *frame, const IspPrms *prms, ColorDomains domain, DataPtrTypes type)
{
    if (!prms->dump_stages)
    {
        return 0;
    }

    std::string dir = prms->blocks_output_path;
    if (dir.empty())
    {
        dir = prms->out_file_path;
        if (!dir.empty() && dir.back() != '/' && dir.back() != '\\')
        {
            dir += '/';
        }
        dir += "BlocksOutput";
    }
    EnsureDirectory(dir);

    if (!dir.empty() && dir.back() != '/' && dir.back() != '\\')
    {
        dir += '/';
    }
    char index_str[8];
    snprintf(index_str, sizeof(index_str), "%02d", stage_index);
    std::string filepath = dir + index_str + "_" + stage_name + "_v" + stage_ver + ".bmp";

    int width = frame->info.width;
    int height = frame->info.height;
    int pixel_count = width * height;

    std::vector<uint8_t> bgr_u8(pixel_count * 3);

    bool has_range = false;
    int32_t dbg_min = 0;
    int32_t dbg_max = 0;

    if (domain == ColorDomains::RAW)
    {
        if (type == DataPtrTypes::TYPE_UINT8)
        {
            uint8_t *src = reinterpret_cast<uint8_t *>(frame->data.raw_u8_i);
            int min_v = src[0];
            int max_v = src[0];
            FOR_ITER(i, pixel_count)
            {
                if (src[i] < min_v) min_v = src[i];
                if (src[i] > max_v) max_v = src[i];
            }
            int range = max_v - min_v;
            if (range == 0) range = 1;
            FOR_ITER(i, pixel_count)
            {
                uint8_t val = static_cast<uint8_t>((src[i] - min_v) * 255 / range);
                bgr_u8[i * 3 + 0] = val;
                bgr_u8[i * 3 + 1] = val;
                bgr_u8[i * 3 + 2] = val;
            }
            has_range = true;
            dbg_min = min_v;
            dbg_max = max_v;
        }
        else if (type == DataPtrTypes::TYPE_UINT16)
        {
            uint16_t *src = reinterpret_cast<uint16_t *>(frame->data.raw_u16_i);
            int min_v = src[0];
            int max_v = src[0];
            FOR_ITER(i, pixel_count)
            {
                if (src[i] < min_v) min_v = src[i];
                if (src[i] > max_v) max_v = src[i];
            }
            int range = max_v - min_v;
            if (range == 0) range = 1;
            FOR_ITER(i, pixel_count)
            {
                uint8_t val = static_cast<uint8_t>((src[i] - min_v) * 255 / range);
                bgr_u8[i * 3 + 0] = val;
                bgr_u8[i * 3 + 1] = val;
                bgr_u8[i * 3 + 2] = val;
            }
            has_range = true;
            dbg_min = min_v;
            dbg_max = max_v;
        }
        else if (type == DataPtrTypes::TYPE_INT32)
        {
            int32_t *src = reinterpret_cast<int32_t *>(frame->data.raw_s32_i);
            int32_t min_v = src[0];
            int32_t max_v = src[0];
            FOR_ITER(i, pixel_count)
            {
                if (src[i] < min_v) min_v = src[i];
                if (src[i] > max_v) max_v = src[i];
            }
            int32_t range = max_v - min_v;
            if (range == 0) range = 1;
            FOR_ITER(i, pixel_count)
            {
                uint8_t val = static_cast<uint8_t>((src[i] - min_v) * 255 / range);
                bgr_u8[i * 3 + 0] = val;
                bgr_u8[i * 3 + 1] = val;
                bgr_u8[i * 3 + 2] = val;
            }
            has_range = true;
            dbg_min = min_v;
            dbg_max = max_v;
        }
        else
        {
            LOG(WARNING) << "Unsupported raw type for stage dump: " << stage_name;
            return -1;
        }
    }
    else if (domain == ColorDomains::BGR)
    {
        if (stage_name == "yuv2rgb")
        {
            uint8_t *src = reinterpret_cast<uint8_t *>(frame->data.bgr_u8_o);
            memcpy(bgr_u8.data(), src, pixel_count * 3);
        }
        else if (type == DataPtrTypes::TYPE_INT32)
        {
            int32_t *src = reinterpret_cast<int32_t *>(frame->data.bgr_s32_i);
            int32_t min_v = src[0];
            int32_t max_v = src[0];
            FOR_ITER(i, pixel_count * 3)
            {
                if (src[i] < min_v) min_v = src[i];
                if (src[i] > max_v) max_v = src[i];
            }
            int32_t range = max_v - min_v;
            if (range == 0) range = 1;
            FOR_ITER(i, pixel_count * 3)
            {
                bgr_u8[i] = static_cast<uint8_t>((src[i] - min_v) * 255 / range);
            }
            has_range = true;
            dbg_min = min_v;
            dbg_max = max_v;
        }
        else
        {
            LOG(WARNING) << "Unsupported bgr type for stage dump: " << stage_name;
            return -1;
        }
    }
    else if (domain == ColorDomains::YUV)
    {
        uint8_t *y = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.y);
        uint8_t *u = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.u);
        uint8_t *v = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.v);

        FOR_ITER(h, height)
        {
            FOR_ITER(w, width)
            {
                int idx = GET_PIXEL_INDEX(w, h, width);
                int r_tmp = y[idx] + 0 + 1.114 * (v[idx] - 128);
                int g_tmp = y[idx] - 0.395 * (u[idx] - 128) - 0.581 * (v[idx] - 128);
                int b_tmp = y[idx] + 2.032 * (u[idx] - 128) + 0;

                ClipMinMax(r_tmp, 255, 0);
                ClipMinMax(g_tmp, 255, 0);
                ClipMinMax(b_tmp, 255, 0);

                bgr_u8[idx * 3 + 0] = static_cast<uint8_t>(b_tmp);
                bgr_u8[idx * 3 + 1] = static_cast<uint8_t>(g_tmp);
                bgr_u8[idx * 3 + 2] = static_cast<uint8_t>(r_tmp);
            }
        }
    }
    else
    {
        LOG(WARNING) << "Unsupported domain for stage dump: " << stage_name;
        return -1;
    }

    bool ok = WriteBgrMemToBmp(filepath.c_str(), reinterpret_cast<char *>(bgr_u8.data()), width, height, 24);

    if (!ok)
    {
        LOG(ERROR) << "Failed to write stage dump: " << filepath;
        return -1;
    }

    if (has_range)
    {
        LOG(INFO) << "Stage dump [" << stage_index << "]: " << stage_name << "(v" << stage_ver << ")"
                  << " | " << width << "x" << height
                  << " | " << DomainToString(domain) << "(" << TypeToString(type) << ")"
                  << " | range[" << dbg_min << "-" << dbg_max << "]"
                  << " | " << filepath;
    }
    else
    {
        LOG(INFO) << "Stage dump [" << stage_index << "]: " << stage_name << "(v" << stage_ver << ")"
                  << " | " << width << "x" << height
                  << " | " << DomainToString(domain) << "(" << TypeToString(type) << ")"
                  << " | " << filepath;
    }
    return 0;
}

void IspInit()
{
    RegisterUnpackMod();
    RegisterDePwlMod();
    RegisterBlcMod();
    RegisterDemoasicMod();
    RegisterCcmMod();
    RegisterYGammaMod();
    RegisterWbGaincMod();
    RegisterLtmMod();
    RegisterRgbGammaMod();
    RegisterYuv2RgbMod();
    RegisterRgb2YuvMod();
    RegisterSaturationMod();
    RegisterContrastMod();
    RegisterSharpenMod();
    RegisterLscMod();
    RegisterDpcMod();
    RegisterCnsMod();
}

IspPipeline::IspPipeline()
{
    pipe_.clear();
    IspInit();
    ShowAllIspModules();
}
IspPipeline::~IspPipeline()
{
    pipe_.clear();
}

IspPipeline::IspPipeline(std::list<std::string> pipeline) : IspPipeline() { MakePipe(pipeline); }

int IspPipeline::MakePipe(const std::list<std::string> &pipeline_str)
{
    pipe_.clear();
    is_pipe_vaild_ = false;

    if (pipeline_str.empty())
    {
        LOG(ERROR) << "pipeline is empty";
        return -1;
    }

    IspModule mod;
    IspModule last_mod;
    for (const auto &item : pipeline_str)
    {
        if (0 == GetIspModuleFromName(item, mod))
        {
            if (pipe_.size() > 0)
            {
                if ((mod.in_type != last_mod.out_type) || (mod.in_domain != last_mod.out_domain))
                {
                    if ((mod.in_domain != last_mod.out_domain))
                        LOG(ERROR) << "mod " << mod.name << " domain is not equal wait " << last_mod.name;
                    if (mod.in_type != last_mod.out_type)
                        LOG(ERROR) << "mod " << mod.name << " in bit is not equal wait " << last_mod.name;
                    is_pipe_vaild_ = false;
                    return -1;
                }
            }
            pipe_.push_back(mod);
            last_mod = mod;
        }
        else
        {
            LOG(ERROR) << item << " find failed";
            return -1;
        }
    }
    is_pipe_vaild_ = true;
    return 0;
}

int IspPipeline::RunPipe(Frame *frame, const IspPrms *prms)
{
    if (frame == nullptr || prms == nullptr)
    {
        LOG(ERROR) << "RunPipe invalid input";
        return -1;
    }

    if (!is_pipe_vaild_)
    {
        LOG(ERROR) << "pipeline is not vailed..";
        return -1;
    }
    // uint64_t start_tick, end_tick;
    LOG(INFO) << "============= user pipeline running ==============";
    int stage_index = 1;
    for (const auto &isp_mod : pipe_)
    {
        auto start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch());
        if (isp_mod.run_function(frame, prms) != 0)
        {
            LOG(ERROR) << "pipeline run failed, mod " << isp_mod.name;
            return -1;
        }
        if (prms->dump_stages)
        {
            DumpStageOutput(stage_index, isp_mod.name, isp_mod.version, frame, prms, isp_mod.out_domain, isp_mod.out_type);
        }
        ++stage_index;
        auto end_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch());
        LOG(INFO) << "mod " << isp_mod.name << "(v" << isp_mod.version << ")\t time: " << (end_ms - start_ms).count() << "ms";
    }
    LOG(INFO) << "============= user pipeline running end ==============";
    return 0;
}

int IspPipeline::PrintPipe()
{
    if (!is_pipe_vaild_)
    {
        LOG(ERROR) << "pipeline is not vailed..";
        return -1;
    }
    int index = 0;
    LOG(INFO) << "============= user pipeline print start ==============";
    for (const auto &isp_mod : pipe_)
    {
        LOG(INFO) << "mod[" << index++ << "] -> " << isp_mod.name;
    }
    LOG(INFO) << "============= user pipeline print end ==============";
    return 0;
}
