/**
 * @file common.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-28
 * 
 * Copyright (c) of ADAS_EYES 2023
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "common/common.h"


size_t ReadFileToMem(std::string file_name, void* mem, int size)
{
    FILE* fp;

    fp = fopen(file_name.c_str(), "rb");

    if (!fp) {
        LOG(ERROR) << file_name << " open failed";
        return -1;
    }

    auto rd = fread(mem, size, 1, fp);

    fclose(fp);

    return rd;
}


size_t WriteMemToFile(std::string file_name, void* mem, int size)
{
    FILE* fp;

    fp = fopen(file_name.c_str(), "wb+");

    if (!fp) {
        LOG(ERROR) << file_name << " open failed";
        return -1;
    }

    int rd = fwrite(mem, size, 1, fp);

    fclose(fp);

    return rd;
}

uint32_t ComputeFileCrc32(const std::string &file_path)
{
    FILE *fp = fopen(file_path.c_str(), "rb");
    if (!fp) {
        LOG(ERROR) << file_path << " open failed for CRC";
        return 0;
    }

    uint32_t crc = 0xFFFFFFFF;
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        crc ^= static_cast<uint32_t>(ch);
        for (int i = 0; i < 8; ++i) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }
    fclose(fp);
    return ~crc;
}