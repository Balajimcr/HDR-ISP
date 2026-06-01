#ifndef ISP_PARSE_H
#define ISP_PARSE_H

#include <string>
#include "modules/modules.h"

int ParseIspCfgFile(const std::string& cfg_file_path, IspPrms &isp_prm);

#endif // ISP_PARSE_H
