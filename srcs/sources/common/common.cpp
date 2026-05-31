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


int ReadFileToMem(std::string file_name, void* mem, size_t size)
{
    if (!mem || size == 0) {
        LOG(ERROR) << "ReadFileToMem invalid input";
        return -1;
    }

    FILE* fp;

    fp = fopen(file_name.c_str(), "rb");

    if (!fp) {
        LOG(ERROR) << file_name << " open failed";
        return -1;
    }

    size_t rd = fread(mem, size, 1, fp);

    fclose(fp);

    if (rd != 1) {
        LOG(ERROR) << file_name << " read size mismatch, expected " << size << " bytes";
        return -1;
    }
    return 0;
}


int WriteMemToFile(std::string file_name, void* mem, size_t size)
{
    if (!mem || size == 0) {
        LOG(ERROR) << "WriteMemToFile invalid input";
        return -1;
    }

    FILE* fp;

    fp = fopen(file_name.c_str(), "wb+");

    if (!fp) {
        LOG(ERROR) << file_name << " open failed";
        return -1;
    }

    size_t rd = fwrite(mem, size, 1, fp);

    fclose(fp);

    if (rd != 1) {
        LOG(ERROR) << file_name << " write size mismatch, expected " << size << " bytes";
        return -1;
    }
    return 0;
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

uint32_t ComputeMemCrc32(const void *data, size_t len)
{
    if (!data || len == 0) {
        return 0;
    }

    uint32_t crc = 0xFFFFFFFF;
    const uint8_t *bytes = static_cast<const uint8_t *>(data);
    for (size_t j = 0; j < len; ++j) {
        crc ^= bytes[j];
        for (int i = 0; i < 8; ++i) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }
    return ~crc;
}
