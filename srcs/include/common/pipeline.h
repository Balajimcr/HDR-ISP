/**
 * @file pipeline.h
 * @author Balaji R (balajimcr@gmail.com)
 * @brief ISP pipeline class declaration
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
#ifndef ISP_PIPE_H
#define ISP_PIPE_H

#include <list>
#include <functional>

#include "modules/modules.h"
#include <chrono>

class IspPipeline
{
private:
    /* data */
    std::list<IspModule> pipe_;
    bool is_pipe_vaild_ = false;

public:
    IspPipeline();
    ~IspPipeline();

    IspPipeline(std::list<std::string> pipeline);

    int MakePipe(const std::list<std::string> &pipeline_str);
    int RunPipe(Frame *frame, const IspPrms *prms);
    int PrintPipe();
};


#endif // ! ISP_PIPE_H