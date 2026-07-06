[tag download]:https://github.com/Jieli-Tech/AD24N/tags
[tag_badgen]:https://img.shields.io/github/v/tag/Jieli-Tech/AD24N?style=plastic&labelColor=ffffff&color=informational&label=Tag&

# fw-AD24N_GP-MCU_SDK  [![tag][tag_badgen]][tag download]

<div align="center">

**Jieli AD24N Series Universal MCU SDK Firmware**

[中文](./README.md) · [Documentation Center](https://doc.zh-jieli.com/AD24/zh-cn/master/index.html) · [SDK Release History](doc/AD24N_SDK_发布版本信息.pdf) · [Report an Issue](https://github.com/Jieli-Tech/fw-AD24N/issues)

</div>

---

## Table of Contents

- [1. Overview](#1-overview)
- [2. Supported Chips and Platforms](#2-supported-chips-and-platforms)
- [3. Environment Setup](#3-environment-setup)
- [4. Quick Start](#4-quick-start)
- [5. Project Structure](#5-project-structure)
- [6. Applications and Examples](#6-applications-and-examples)
- [7. Build Guide](#7-build-guide)
- [8. Flashing and Upgrade](#8-flashing-and-upgrade)
- [9. Configuration](#9-configuration)
- [10. FAQ](#10-faq)
- [11. Community and Support](#11-community-and-support)
- [12. Disclaimer](#12-disclaimer)

---

## 1. Overview

`fw-AD24N_GP-MCU_SDK` is a universal MCU SDK development package provided by Jieli Technology for the AD24N series chips. These chips are primarily targeted at the following application scenarios:

| Application Type | Typical Products |
|---------|---------|
| **Voice Toys** | Storytelling machines, learning devices, voice-controlled toys, MIDI instruments, voice-controlled lights |
| **Mini Speakers** | Music players, loudspeakers |
| **General MCU** | Smart control, sensor acquisition, general peripheral applications |

### Core Features

- **Dual-Issue DSP**: 32-bit dual-issue DSP @ 240 MHz
- **Audio Decoding**: Supports .a/.b/.e, .f1a/.f1b/.f1c, UMP3, and other audio formats
- **MIDI Playback**: Supports MIDI synthesis and playback
- **Triple-stream Decoding**: .a/.b/.e + .f1a/.f1b/.f1c + .f1a/.f1b/.f1c three simultaneous audio decode streams
- **Variable Speed/Pitch**: Supports variable-speed and pitch-shifted audio playback (requires system clock ≥ 100 MHz)
- **Hardware Resampling**: Built-in hardware SRC
- **Built-in Audio Codec**: 16-bit DAC + 16-bit ADC, supports 8K–96K sample rates
- **Class-D Amplifier**: Direct speaker drive
- **Audio Effects**: ANS noise reduction, variable speed, ECHO reverb, voice pitch, voice changer, PCM EQ
- **Low Power**: Shutdown current as low as 2 µA+, power down 19 µA+
- **Multiple Storage Options**: Supports internal/external FLASH
- **I2S Interface**: Digital audio input/output on select models

This repository contains SDK release versions and sample projects. Compilation requires the corresponding library files (`lib.a`) that follow the naming convention.

---

## 2. Supported Chips and Platforms

### 2.1 SoC Families

| Chip Series | Application Domain |
|---------|---------|
| AD242A | Voice Toys / Audio player (SOP16) |
| AD245A | Voice Toys / General MCU (QSOP24, I2S) |
| AD246A | Voice Toys / General MCU (QFN32, max GPIO, I2S) |
| AD248A | Voice Toys / Voice-controlled lights (SOP8, no DAC, smallest package) |
| AD248B | Voice Toys / Voice-controlled lights (SOP8, no Class-D) |

> For chip models, datasheets, and schematic resources, see: [doc/ directory](doc/)

### 2.2 MCU Hardware/Software Parameter Differences

![Chip](jl_ad_chip.png)

---

## 3. Environment Setup

### 3.1 Prerequisites

| System | Description |
|------|------|
| **Windows** | Code::Blocks IDE recommended for compilation |
| **Linux** | Command-line compilation via Makefile (requires rewriting `download_bat.c` for Linux compatibility) |
| **macOS** | Cross-compilation toolchain must be configured manually |

### 3.2 Install the Build Toolchain

1. Download and install the **Jieli Build Toolchain**: [Download Link](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/dev_env/index.html)
2. Linux users can download from: [pkgman.jieliapp.com](http://pkgman.jieliapp.com/doc/all)
   - Extract to `/opt/jieli` after downloading
   - Ensure `/opt/jieli/pi32/bin/clang` exists
3. Verify installation:

```bash
# Verify toolchain installation
clang --version
```

### 3.3 Install Flashing Tools

| Tool | Purpose | How to Obtain |
|------|------|---------|
| **USB Upgrade Tool** | Flash firmware to the target board | [Application Link](https://item.taobao.com/item.htm?id=620295020803) · [User Guide](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/forced_upgrade/index.html) |
| **Mass Production Burner** | Mass production / bare-die programming | **From distributor** · [User Guide](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/burner_1tuo2/index.html) |

---

### 3.4 Audio Tools

Universal audio tools for packaging, audio file conversion, MIDI, etc.: [Download Link](https://pan.baidu.com/s/1ajzBF4BFeiRFpDF558ER9w#list/path=%2F) Password: `3jey`

---

## 4. Quick Start

### 4.1 Clone the Repository

```bash
git clone https://github.com/Jieli-Tech/AD24N.git
cd AD24N/sdk
```

### 4.2 Project Entry Points

The SDK includes the following application projects, located in the `sdk/` root directory:

| Project File | Chip | Application Type |
|---------|------|---------|
| `AD24N_voice_toy.cbp` | AD24N All Series | Voice Toys |
| `AD24N_voice_enhanced.cbp` | AD24N All Series | Loudspeaker |

### 4.3 Application Code Entry Points

```
sdk/app/src/
├── voice_toy/          # Voice toy application
├── voice_func/         # Voice function modules
└── voice_enhanced/     # Loudspeaker application
```

### 4.4 Build and Flash

**Method 1: Code::Blocks (Recommended for Windows users)**

1. Double-click the corresponding `.cbp` project file
2. Click **Build → Build** (Ctrl+F9)
3. After a successful build, use the USB Upgrade Tool to flash the generated firmware

**Method 2: Makefile (Command Line)**

```bash
# Windows users
Double-click sdk/make_prompt.bat to open the command-line environment

# Build voice toy
make ad24n_voice_toy -j4

# Build loudspeaker
make ad24n_voice_enhanced -j4
```

> **💡 Tip**: Before building, ensure the USB Upgrade Tool is properly connected and the target board has entered programming mode.

**Method 3: VS Code Build**

The repository comes pre-configured with VS Code tasks. Press `Ctrl+Shift+B` to select a build target.

---

## 5. Project Structure

```
fw-AD24N/
├── sdk/                           # SDK root directory
│   ├── app/                       # Application layer code
│   │   ├── src/                   #   Application entry source code
│   │   │   ├── voice_toy/         #     Voice toy application
│   │   │   ├── voice_func/        #     Voice function modules
│   │   │   └── voice_enhanced/    #     Loudspeaker application
│   │   ├── bsp/                   #   Board Support Package (BSP)
│   │   └── post_build/            #   Post-build scripts and tools
│   ├── include_lib/               # Headers and precompiled libraries
│   │   ├── cpu/                   #   CPU platform headers
│   │   ├── decoder/               #   Decoder API headers
│   │   ├── encoder/               #   Encoder API headers
│   │   ├── audio/                 #   Audio API headers
│   │   ├── device/                #   Device driver headers
│   │   ├── dev_mg/                #   Device management headers
│   │   ├── common/                #   Common headers
│   │   ├── fs/                    #   File system headers
│   │   ├── msg/                   #   Message mechanism
│   │   ├── ans/                   #   ANS noise reduction
│   │   ├── vo_changer/            #   Voice changer
│   │   ├── vo_pitch/              #   Voice pitch
│   │   ├── update/                #   Firmware upgrade
│   │   └── liba/                  #   Precompiled libraries (.a)
│   ├── tools/                     # Build tools and scripts
│   │   ├── make_prompt.bat        #   Windows build command-line launcher
│   │   └── utils/                 #   Utilities (make, rm, etc.)
│   ├── Makefile                   # Top-level Makefile
│   └── *.cbp                      # Code::Blocks project files
├── doc/                           # Documentation
│   ├── ad24n硬件文档/              #   Hardware docs (datasheets / schematics / dev board)
│   ├── stuff/                     #   Miscellaneous (DingTalk group, flashing tool docs, etc.)
│   ├── AD24N_SDK手册_v1.1.pdf     #   SDK manual
│   ├── AD24N_SDK_发布版本信息.pdf  #   SDK release notes
│   ├── AD24N用户手册V1.2.pdf       #   Chip user manual
│   └── 杰理科技32位AD系列语音MCU选型表.pdf  # Chip selection table
└── README.md                      # This file
```

---

## 6. Applications and Examples

### 6.1 Voice Toy Application (`app/src/voice_toy/`)

| Feature | Description |
|-------|------|
| **Music Playback** | Local/external FLASH file playback |
| **MIDI Performance** | MIDI synthesis and playback |
| **Recording** | Audio encoding and recording |
| **LINEIN** | Line input |
| **Loudspeaker** | Loudspeaker / voice amplifier |
| **USB Device** | USB slave device |

### 6.2 Loudspeaker Application (`app/src/voice_enhanced/`)

| Feature | Description |
|-------|------|
| **Enhanced Effects** | Advanced audio processing and enhancement solutions |
| **Multi-mode** | Flexible switching between multiple operating modes |

Target domains: voice toys, storytelling machines, learning devices, MIDI instruments, voice-controlled lights, etc.

---

## 7. Build Guide

### 7.1 Build Command Quick Reference

Run the following commands from the `sdk/` directory:

| Target | Command |
|------|------|
| **Voice Toy** | `make ad24n_voice_toy -j4` |
| **Loudspeaker** | `make ad24n_voice_enhanced -j4` |
| **Build All** | `make all` |
| **Clean All** | `make clean` |

### 7.2 Code::Blocks Build (Recommended for Windows users)

1. Ensure the Jieli build toolchain is installed
2. Double-click the corresponding `.cbp` project file to open Code::Blocks
3. Click **Build → Build** (Ctrl+F9)
4. After a successful build, the firmware will be generated in the `post_build/` directory

### 7.3 Makefile Build

```bash
# Windows users
Double-click sdk/make_prompt.bat to open the command-line environment
make ad24n_voice_toy -j4

# Linux users (requires modifying download_bat.c for Linux compatibility)
cd sdk
make ad24n_voice_toy -j`nproc`
```

### 7.4 Common Build Errors

| Error Message | Solution |
|---------|---------|
| `clang: command not found` | Jieli build toolchain is not installed, or environment variables are not configured |
| `cannot find -lxxx` | Missing corresponding `.a` library file; check the `include_lib/liba/` directory |
| `make: command not found` | On Windows, use `tools/make_prompt.bat` to open the build command environment |
| Link errors | Verify that the Makefile target matches the current chip model |

---

## 8. Flashing and Upgrade

### 8.1 First-Time Flashing

1. **Connect Hardware**: Connect the development board to the PC via **USB** or **USB Upgrade Tool**
2. **Enter Programming Mode**:
    - Method 1 (USB): Hold the flash button on the development board, then reset or power-cycle
    - Method 2 (USB/UART): Use the USB Upgrade Tool to enter programming mode
3. **Launch USB Upgrade Tool**: Start the flashing host software
4. **Select Firmware**: Choose the compiled firmware file
5. **Start Flashing**: Click the download button and wait for completion

> **Note**: Before flashing, ensure the USB Upgrade Tool is properly connected and the target board has entered programming mode. For details on ISD_CONFIG.INI, see [ISD Configuration Guide](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/toolchains/ini_cfg.html).

### 8.2 Mass Production Flashing

For mass production scenarios, use Jieli's mass production burner (one-to-two / one-to-eight), which supports bare-die programming. See [One-to-Two Burner User Guide](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/burner_1tuo2/index.html) · [One-to-Eight Burner User Guide](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/burner_1tuo8/index.html)

### 8.3 OTA Upgrade

Supports dual-bank firmware upgrade (dual_bank).

---

## 9. Configuration

- Edit `sdk/app/src/<application>/app_config.h` to configure feature toggles for the target application
- Select different application types and chip models via Makefile targets

---

## 10. FAQ

### 10.1 Development Workflow

**Q: How do I create a new project?**
A: Start from an existing `.cbp` project and the application code in `app/src/`, then configure the corresponding application examples.

**Q: How do I switch between different chip models?**
A: Select the corresponding chip model in the configuration. The SDK provides unified build entries for all chips in the series.

### 10.2 Build Issues

**Q: On Windows, `make` is reported as an invalid command?**
A: Use `sdk/make_prompt.bat` to enter the pre-configured command-line environment, which sets up all environment variables and the `make` path.

**Q: How can I speed up compilation?**
A: Use the `-j` flag for parallel compilation, e.g. `make -j4` (the number specifies the parallel job count).

### 10.3 Debugging Tips

- **UART Logging**: Debug logs can be output via UART
- **GPIO Debug**: Use idle GPIO pins to output debug waveforms for timing measurement

---

## 11. Community and Support

### Technical Discussion

| Platform | Group / Link | Status |
|------|-----------|------|
| **DingTalk Tech Group** | See [Group QR Code](doc/stuff/dingtalk.jpg) | ✅ Joinable |

### Resource Links

| Resource | Link |
|------|------|
| 📖 **Online Documentation Center** | [doc.zh-jieli.com/AD24](https://doc.zh-jieli.com/AD24/zh-cn/master/index.html) |
| 📚 **SDK Release History** | [SDK Release Notes](doc/AD24N_SDK_发布版本信息.pdf) |
| 🔧 **SDK Quick Start** | [SDK Manual](doc/AD24N_SDK手册_v1.1.pdf) |
| 📖 **Chip User Manual** | [AD24N User Manual](doc/AD24N用户手册V1.2.pdf) |
| 📄 **Chip Selection Guide** | [Selection Table](doc/杰理科技32位AD系列语音MCU选型表.pdf) |
| 🎬 **Video Tutorials** | [Bilibili Homepage](https://space.bilibili.com/3493277347088769/dynamic) |
| 🎵 **MIDI Development Manual** | [MIDI Application Development Guide](https://doc.zh-jieli.com/MIDI/zh-cn/master/index.html) |
| 📦 **FAE Support** | [FAE Support Repository](https://gitee.com/jieli-tech_fae/fw-jl) |
| 🛒 **Dev Board / Flashing Tool Purchase** | [Jieli Official Store](https://shop321455197.taobao.com/) |
| 🐛 **Issue Tracker** | [Github Issues](https://github.com/Jieli-Tech/fw-AD24N/issues) |

---

## 12. Disclaimer

`fw-AD24N_GP-MCU_SDK` supports development for the AD24N series chips. These chips support common general-purpose MCU applications and may be used for development, evaluation, sampling, and mass production. For the corresponding SDK version, please refer to [SDK Release History](doc/AD24N_SDK_发布版本信息.pdf).

---

<div align="center">
  <sub>Copyright &copy; Zhuhai Jieli Technology Co., Ltd. All rights reserved.</sub>
</div>
