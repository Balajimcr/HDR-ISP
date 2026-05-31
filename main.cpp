/**
 * @file main.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief
 * @version 0.1
 * @date 2023-07-28
 *
 * Copyright (c) of ADAS_EYES 2023
 *
 */

#include "easylogging++.h"
#include "EasyBMP.h"
#include "common/pipeline.h"
#include "common/common.h"
#include <iomanip>
#include <filesystem>

//#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>

INITIALIZE_EASYLOGGINGPP

extern int ParseIspCfgFile(const std::string cfg_file_path, IspPrms &isp_prm);

static void SetupLogging()
{
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%time %level %msg");
}


int main(int argc, char *argv[])
{
    SetupLogging();
    LOG(INFO) << "APP Start Running";

    if (argc < 2)
    {
        LOG(ERROR) << "usage :\n\t ./app isp_cfg.json\n";
        return -1;
    }

    IspPipeline pipeline;
    IspPrms isp_prms;


    auto ret = ParseIspCfgFile(argv[1], isp_prms);
    if (ret)
    {
        LOG(ERROR) << argv[1] << " parse failed\n";
        return -1;
    }

    const size_t width = static_cast<size_t>(isp_prms.info.width);
    const size_t height = static_cast<size_t>(isp_prms.info.height);
    const size_t bpp = static_cast<size_t>(isp_prms.info.bpp);
    const size_t raw_buffer_size = (width * height * bpp) / 8;

    Frame frame(isp_prms.info);

    ret = frame.ReadFileToFrame(isp_prms.raw_file, raw_buffer_size);
    if (ret != 0)
    {
        LOG(ERROR) << "failed to load raw file " << isp_prms.raw_file;
        return -1;
    }

    ret = pipeline.MakePipe(isp_prms.pipe);
    if (ret != 0)
    {
        LOG(ERROR) << "failed to create pipeline";
        return -1;
    }
    ret = pipeline.PrintPipe();
    if (ret != 0)
    {
        LOG(ERROR) << "failed to print pipeline";
        return -1;
    }

    ret = pipeline.RunPipe(&frame, &isp_prms);

    if (!ret)
    {
        //cv::Mat isp_result(height, width, CV_8UC3, frame.data.bgr_u8_o);
        //cv::imwrite(isp_prms.out_file_path + "isp_result.png", isp_result);
        std::filesystem::path out_dir(isp_prms.out_file_path);
        std::string bmp_path = (out_dir / "isp_result.bmp").string();
        //std::string raw_path = (out_dir / "isp_result_bgr.raw").string();
        if (!WriteBgrMemToBmp(bmp_path.c_str(), (char *)frame.data.bgr_u8_o, static_cast<int>(width), static_cast<int>(height), 24))
        {
            LOG(ERROR) << "failed to dump output bmp: " << bmp_path;
            return -1;
        }
        //WriteMemToFile(raw_path, frame.data.bgr_u8_o, width * height * 3);

        // Compute CRC in-memory before disk serialization to avoid I/O overhead
        uint32_t crc_val = ComputeMemCrc32(frame.data.bgr_u8_o, width * height * 3);
        const uint32_t kGoldenCrc = isp_prms.expected_crc;

        char crc_computed_str[16];
        char crc_golden_str[16];
        snprintf(crc_computed_str, sizeof(crc_computed_str), "%08X", crc_val);
        snprintf(crc_golden_str, sizeof(crc_golden_str), "%08X", kGoldenCrc);

        if (crc_val == kGoldenCrc) {
            LOG(INFO) << "CRC validation passed. Output matches golden reference.";
            LOG(INFO) << "  Computed CRC: 0x" << crc_computed_str;
            LOG(INFO) << "  Golden CRC:   0x" << crc_golden_str;
            LOG(INFO) << "success";
        } else {
            LOG(ERROR) << "CRC validation failed. Output does not match golden reference.";
            LOG(ERROR) << "  Computed CRC: 0x" << crc_computed_str;
            LOG(ERROR) << "  Golden CRC:   0x" << crc_golden_str;
        }

        LOG(INFO) << "Output dump: isp_result | " << width << "x" << height
                  << " | BGR(uint8) | " << bmp_path;
        //LOG(INFO) << "Output dump: isp_result_bgr | " << width * height * 3 << " bytes | " << raw_path;
        LOG(INFO) << "APP Common Exit";
    }
    else
    {
        LOG(ERROR) << "Pipe run Error Exit";
    }
    return 0;
}
