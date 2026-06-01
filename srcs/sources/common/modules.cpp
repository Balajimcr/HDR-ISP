/**
 * @file modules.cpp
 * @author Balaji R (balajimcr@gmail.com)
 * @brief Module registry implementation
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
#include <map>

static std::map<std::string, IspModule> s_isp_mode_map;

int RegisterIspModule(IspModule mod)
{
    if (mod.name.empty()) {
        return -1;
    }
    s_isp_mode_map[mod.name] = mod;

    return 0;
}

int GetIspModuleFromName(std::string name, IspModule& mod)
{
    if (s_isp_mode_map.count(name) > 0) {
		mod = s_isp_mode_map[name];
        return 0;
    }
    return -1;
}

int ShowAllIspModules()
{
    LOG(INFO) << "============= default pipeline mods show start ==============";
    for (auto iter = s_isp_mode_map.rbegin(); iter != s_isp_mode_map.rend(); ++iter) {
		LOG(INFO) << "isp module-> [" << iter->first << "\t] | ver [" << iter->second.version << "] | in bits [" \
                 << std::to_string((int)iter->second.in_type) << "] | out bits [" << std::to_string((int)iter->second.out_type) << "]";
    }
    LOG(INFO) << "============= default pipeline mods show end ==============";
    return 0;
}