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

    auto width = isp_prms.info.width;
    auto height = isp_prms.info.height;

    Frame frame(isp_prms.info);

    ret = frame.ReadFileToFrame(isp_prms.raw_file, width * height * isp_prms.info.bpp / 8);
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
        std::string bmp_path = isp_prms.out_file_path + "isp_result.bmp";
        //std::string raw_path = isp_prms.out_file_path + "isp_result_bgr.raw";
        if (!WriteBgrMemToBmp(bmp_path.c_str(), (char *)frame.data.bgr_u8_o, width, height, 24))
        {
            LOG(ERROR) << "failed to dump output bmp: " << bmp_path;
            return -1;
        }
        //WriteMemToFile(raw_path, frame.data.bgr_u8_o, width * height * 3);

        uint32_t crc_val = ComputeFileCrc32(bmp_path);
        const uint32_t kGoldenCrc = 0xEC90E952;

        if (crc_val == kGoldenCrc) {
            LOG(INFO) << "CRC validation passed. Output matches golden reference.";
            LOG(INFO) << "  Computed CRC: 0x" << std::hex << std::uppercase << crc_val;
            LOG(INFO) << "  Golden CRC:   0x" << std::hex << std::uppercase << kGoldenCrc;
            LOG(INFO) << "success";
        } else {
            LOG(ERROR) << "CRC validation failed. Output does not match golden reference.";
            LOG(ERROR) << "  Computed CRC: 0x" << std::hex << std::uppercase << crc_val;
            LOG(ERROR) << "  Golden CRC:   0x" << std::hex << std::uppercase << kGoldenCrc;
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
