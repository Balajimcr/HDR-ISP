#ifndef ADAS_ISP_FRAME_H
#define ADAS_ISP_FRAME_H

#include "common.h"

#define SAFE_FREE(arr)     \
    do                     \
    {                      \
        if (arr)           \
        {                  \
            delete[] arr;  \
            arr = nullptr; \
        }                  \
    } while (0)

class Frame
{
private:
    int FrameDateMalloc(int width, int height);
public:
    ImageInfo info;
    ImageMem data;

public:
    Frame(const ImageInfo &img_info);
    Frame(const Frame &) = delete;
    Frame &operator=(const Frame &) = delete;
    Frame(Frame &&) = delete;
    Frame &operator=(Frame &&) = delete;

    ~Frame();

    int ReadFileToFrame(const std::string &file, size_t size);

    int RawMemToFrame(void *src, size_t len);
};

#endif // !ADAS_ISP_FRAME_H
