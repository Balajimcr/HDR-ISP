# HDR-ISP Agent Guide

This file is intended for AI coding agents working on the HDR-ISP project. The project is an Image Signal Processing (ISP) pipeline written in C++ for HDR CMOS image sensors.

---

## Project Overview

HDR-ISP is a modular, CPU-based ISP pipeline targeting embedded deployment. The core ISP algorithm modules are written in C style for portability, while the framework (pipeline orchestration, configuration parsing, and I/O) is written in C++.

- **Repository**: `https://github.com/JokerEyeAdas/HDR-ISP`
- **Language**: C++ (framework), C-style (ISP modules)
- **License**: Copyright (c) of ADAS_EYES 2023 (see source headers)
- **No external library dependencies** — all third-party code is vendored under `thirdparty/`.

### Supported ISP Modules

| Domain | Modules |
|--------|---------|
| Raw | `unpack` (MIPI raw unpack), `depwl` (PWL decode), `dpc` (dynamic pixel correct), `lsc` (lens shading correct), `blc` (black level correct), `wbgain`, `demoasic` (demosaicing) |
| RGB | `ltm` (local tone mapping), `rgbgamma`, `ccm` (color correct matrix), `rgb2yuv` |
| YUV | `ygamma`, `contrast`, `sharpen` (USM), `cns` (chroma noise filter), `saturation`, `yuv2rgb` |

> **Note**: `rns` (raw noise filter) is listed in the README but is **not yet implemented**.

---

## Technology Stack

- **Build System**: CMake (minimum version 3.0)
- **Compiler**: C++11 or later (tested with VS2019 on Windows, GCC on Linux)
- **Logging**: [easylogging++](https://github.com/amrayn/easyloggingpp) (vendored in `thirdparty/easylog`)
- **Image I/O**: [EasyBMP](https://easybmp.sourceforge.net/) (vendored in `thirdparty/easybmp`) — outputs `.bmp` and raw BGR files
- **JSON Parsing**: [nlohmann/json](https://github.com/nlohmann/json) (single-header, vendored in `thirdparty/json/nlohmann`)
- **OpenCV**: Previously used but **removed**; references remain commented out in `CMakeLists.txt`, `main.cpp`, and `parse.cpp`. Do not re-introduce OpenCV unless explicitly requested.

---

## Directory Structure

```
HDR-ISP/
├── CMakeLists.txt          # Root CMake: builds executable, links easylog, easybmp, modules
├── main.cpp                # Entry point: loads config, builds Frame & Pipeline, runs ISP
├── parse.cpp               # JSON config parser (nlohmann/json)
├── srcs/
│   ├── CMakeLists.txt      # Builds the `modules` static library
│   ├── include/
│   │   ├── common/         # Framework headers
│   │   │   ├── common.h    # Macros (FOR_ITER, GET_PIXEL_INDEX, ClipMinMax, SwapMem), file I/O
│   │   │   ├── types.h     # Enums (CfaTypes, ColorDomains, RawDataTypes, etc.) and parameter structs
│   │   │   ├── frame.h     # Frame class: memory allocation, raw data loading
│   │   │   └── pipeline.h  # IspPipeline class: module chain builder and runner
│   │   └── modules/
│   │       └── modules.h   # Module registry (IspModule, IspPrms) and registration function declarations
│   └── sources/
│       ├── common/         # Framework implementations
│       │   ├── common.cpp  # ReadFileToMem / WriteMemToFile
│       │   ├── frame.cpp   # Frame memory management (double-buffered allocations)
│       │   ├── modules.cpp # Global module registry (std::map<std::string, IspModule>)
│       │   └── pipeline.cpp# Pipeline construction, validation, and execution with timing logs
│       └── modules/        # Individual ISP module implementations
│           ├── unpack.cpp
│           ├── depwl.cpp
│           ├── blc.cpp
│           ├── lsc.cpp
│           ├── dpc.cpp
│           ├── wbgain.cpp
│           ├── demoasic.cpp
│           ├── ccm.cpp
│           ├── ltm.cpp
│           ├── rgbgamma.cpp
│           ├── rgb2yuv.cpp
│           ├── ygamma.cpp
│           ├── contrast.cpp
│           ├── sharpen.cpp
│           ├── cns.cpp
│           ├── saturation.cpp
│           ├── yuv2rgb.cpp
│           └── clipout.cpp
├── thirdparty/
│   ├── easylog/            # easylogging++
│   ├── easybmp/            # EasyBMP library + sample
│   └── json/nlohmann/      # nlohmann/json single-header library
├── cfgs/                   # JSON configuration files
│   ├── isp_config_cannon.json
│   └── isp_config_dsc.json
├── data/                   # Sample raw image files (.raw)
├── docs/                   # Documentation, images, Chinese readme
└── build/                  # CMake build output (user-created)
```

---

## Build Instructions

### Linux

```bash
# Dependencies: cmake only (no OpenCV required)
sudo apt update
sudo apt install cmake

# Build
cd HDR-ISP/
mkdir build && cd build
cmake ..
make -j12
```

### Windows (x64)

- IDE: VS Code
- Tools: CMake, Visual Studio 2019 C++ build tools (x64 compiler)
- In VS Code, select `Debug` or `Release`, compiler `xxx-amd64`, then **Build All**.

### CMake Targets

| Target | Type | Description |
|--------|------|-------------|
| `HDR_ISP` | Executable | Main application |
| `modules` | Static Library | All framework + ISP module object files |
| `easylog` | Static Library | Logging library |
| `easybmp` | Static Library | BMP I/O library |

---

## Running the Application

After building, copy the config and data directories into the build directory (or run from the project root with relative paths):

```bash
cd build
cp -r ../data/ ./
cp -r ../cfgs/ ./
./HDR_ISP ./cfgs/isp_config_cannon.json
```

### Outputs

On success, the application writes:
- `isp_result.bmp` — final BGR image (via EasyBMP)
- `isp_result_bgr.raw` — raw BGR pixel data
- `myeasylog.log` — execution log (via easylogging++)

---

## Configuration (JSON)

The ISP pipeline and all module parameters are defined in a JSON file. Key sections:

- `raw_file`: Path to input RAW image
- `out_file_path`: Directory for output files
- `info`: Sensor metadata (`sensor_name`, `cfa`, `data_type`, `bpp`, `max_bit`, `width`, `height`, `mipi_packed`)
- `pipe`: Pipeline string, e.g. `unpack|depwl|blc|lsc|dpc|wbgain|demoasic|ccm|ltm|rgbgamma|rgb2yuv|ygamma|contrast|sharpen|cns|saturation|yuv2rgb`
- Per-module parameters: `blc`, `lsc`, `depwl`, `wb_gain`, `ccm`, `ltm`, `rgbgamma`, `rgb2yuv`, `ygamma`, `saturation`, `contrast`, `sharpen`, `dpc`

**Important constraints enforced by the parser:**
- `lsc.mesh_width_nums` must equal `11` (`kLscMeshPointHNums`)
- `lsc.mesh_height_nums` must equal `10` (`kLscMeshPointVNums`)
- `wb_gain` arrays must have exactly 4 elements (R, G, G, B)
- `ccm` matrices are 3x3 but only `d65_ccm` is actively parsed in `parse.cpp` (the others are present in JSON for future use)

---

## Code Architecture

### Module Pattern

Every ISP module follows a strict registration pattern:

1. **Implementation**: A static function with signature `int ModuleName(Frame *frame, const IspPrms *isp_prm)`
2. **Registration**: A `void RegisterModuleNameMod()` function that constructs an `IspModule` descriptor and calls `RegisterIspModule(mod)`

The `IspModule` struct declares:
- `name`: String identifier used in the JSON `pipe` field
- `in_type` / `out_type`: `DataPtrTypes` enum (e.g., `TYPE_UINT8`, `TYPE_INT32`)
- `in_domain` / `out_domain`: `ColorDomains` enum (`RAW`, `BGR`, `YUV`)
- `run_function`: `std::function<int(Frame *, const IspPrms *)>`

### Pipeline Execution

1. `IspPipeline::MakePipe()` parses the `pipe` string into a `std::list<IspModule>`.
2. It validates that each module's `in_type` and `in_domain` match the previous module's `out_type` and `out_domain`.
3. `IspPipeline::RunPipe()` iterates the list, calling each `run_function`, and logs per-module execution time.

### Memory Model & Double Buffering

`Frame` allocates separate buffers for every stage to avoid in-place corruption:
- Raw domain: `raw_u8_i`, `raw_u16_i`, `raw_u16_o`, `raw_s32_i`, `raw_s32_o`
- RGB domain: `bgr_s32_i`, `bgr_s32_o`, `bgr_u8_o`
- YUV domain: `yuv_f32_i/o`, `yuv_u8_i/o`

Buffers are padded to `(width + 8) * (height + 8)` pixels. Modules typically read from an "input" buffer and write to an "output" buffer, then call `SwapMem()` to flip the pointers for the next module.

### Color Domain Transitions

The default pipeline transitions across domains as follows:
- **Raw** → `demoasic` → **BGR** → `rgb2yuv` → **YUV** → `yuv2rgb` → **BGR**

Modules must declare the correct domain transition in their registration descriptor.

---

## Code Style Guidelines

- **File headers**: Every `.cpp` and `.h` file should include the standard Doxygen-style header:
  ```cpp
  /**
   * @file filename.cpp
   * @author joker.mao (joker_mao@163.com)
   * @brief Brief description
   * @version 0.1
   * @date YYYY-MM-DD
   *
   * Copyright (c) of ADAS_EYES 2023
   *
   */
  ```
- **Macros**: Use project macros for iteration (`FOR_ITER`), pixel indexing (`GET_PIXEL_INDEX`), clipping (`ClipMinMax`), and pointer swapping (`SwapMem`).
- **Null checks**: Every module function must validate `frame` and `isp_prm` at entry.
- **Logging**: Use `LOG(INFO)` / `LOG(ERROR)` / `LOG(WARNING)` from easylogging++. Do not use `std::cout` in production code.
- **Memory**: The project uses raw `new` / `delete[]` (managed inside `Frame`). Do not introduce `std::vector` or other STL containers for pixel buffers unless the architecture is explicitly refactored.
- **Type casts**: Use `reinterpret_cast<>` when converting `void*` frame buffers to typed pointers.
- **Module name**: Define `#define MOD_NAME "module_name"` at the top of each module file and use it during registration.

---

## Testing Strategy

**Current state**: There is **no formal test suite** (no unit tests, integration tests, or CI configuration).

Validation is done manually:
1. Build the project.
2. Run with a known config + raw file (e.g., `isp_config_dsc.json` + `DSC_1339_768x512_rggb.raw`).
3. Inspect `isp_result.bmp` visually and check `myeasylog.log` for timing and errors.

If you add new modules or modify existing ones:
- Test with both provided configs (`isp_config_cannon.json`, `isp_config_dsc.json`).
- Verify the pipeline string in JSON still matches the module's registered `name`.
- Check that `in_type`/`in_domain` and `out_type`/`out_domain` compatibility is maintained with neighbors in the pipeline.
- Ensure no memory leaks or buffer overruns (buffer size is `(width+8)*(height+8)`).

---

## Adding a New ISP Module

1. Create `srcs/sources/modules/<mod_name>.cpp`.
2. Implement `static int ModName(Frame *frame, const IspPrms *isp_prm)`.
3. Implement `void RegisterModNameMod()` with correct `in_type`, `out_type`, `in_domain`, `out_domain`, and `MOD_NAME`.
4. Add the registration function declaration to `srcs/include/modules/modules.h`.
5. Call the registration function inside `IspInit()` in `srcs/sources/common/pipeline.cpp`.
6. Add the corresponding JSON parsing logic in `parse.cpp` if the module requires new parameters.
7. Add the parameter struct to `srcs/include/common/types.h` and extend `IspPrms` in `srcs/include/modules/modules.h`.
8. Update the JSON config file(s) in `cfgs/` with the new parameters.

---

## Security Considerations

- The project reads raw binary files and JSON configs from disk using `fopen` / `fread` and `std::ifstream`. There is **no input sanitization** beyond size checks in `Frame::RawMemToFrame`.
- File paths come directly from the JSON config (`raw_file`, `out_file_path`). Ensure paths are trustworthy.
- Memory allocation in `Frame::FrameDateMalloc` uses `new[]` with sizes derived from `width * height`. Very large dimensions could cause `std::bad_alloc`.
- No cryptography, network I/O, or privilege escalation paths exist in this codebase.

---

## Common Issues

- **"pipeline is not vailed"**: A module's output type/domain does not match the next module's input type/domain. Check registration descriptors.
- **"parse failed"**: JSON missing a required field or array size mismatch (e.g., `lsc` mesh dimensions, `wb_gain` length). Check `myeasylog.log` for the exact error.
- **OpenCV references**: Old commented-out OpenCV code remains in `main.cpp` and `parse.cpp`. Do not uncomment unless you intend to restore the dependency.
