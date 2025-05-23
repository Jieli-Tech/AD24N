#ifndef __APP_MODULES_H__
#define __APP_MODULES_H__
//  此文件在在服务器编译库时会自动生成，源文件位于app/bsp/common/app_modules_h.c
//  作者: 刘杰
//  日期:2022年11月22日
//  设计目的: 用于在应用代码中控制各种算法模块的使用
//  注意事项：不要在库文件中包含


//  A格式解码
#define DECODER_A_EN 1

//  标准MP3格式解码
#define DECODER_MP3_ST_EN 0

//  WAV格式解码
#define DECODER_WAV_EN 0

//  ANS功能使能
#define ANS_EN 0

//  F1A格式解码
#define DECODER_F1A_EN 1
#define MAX_F1A_CHANNEL 2

//  UMP3格式解码
#define DECODER_UMP3_EN 1

//  MIDI格式解码
#define DECODER_MIDI_EN 1

//  MIDI琴格式解码
#define DECODER_MIDI_KEYBOARD_EN 1

//  mp3格式压缩
#define ENCODER_MP3_EN 1

//  ump3格式压缩
#define ENCODER_UMP3_EN 1

//  a格式压缩
#define ENCODER_A_EN 1

//  MIO功能使能
#define HAS_MIO_EN 0

//  SRC变采样功能使能
#define HAS_HW_SRC_EN 1 //  硬件SRC

//  解码SPEED功能使能
#define AUDIO_SPEED_EN 1

//  陷波/移频啸叫抑制使能
#define NOTCH_HOWLING_EN 1
#define PITCHSHIFT_HOWLING_EN 1

//  变声功能使能
#define VO_PITCH_EN 0

//  动物变声功能使能
#define VO_CHANGER_EN 0

//  ECHO混响功能使能
#define ECHO_EN 1

//  EQ功能使能
#define PCM_EQ_EN 0

//  APP应用使能
#define RECORD_MODE_EN 1 //  录音应用模式
#define LINEIN_MODE_EN 1 //  Linein应用模式
#define SIMPLE_DEC_EN 1 //  简单解码应用模式
#define LOUDSPEAKER_EN 1 //  扩音应用模式

//  驱动使能


#define CONFIG_FLASH_DTR_EN 0
#define CONFIG_FLASH_WPS_EN 0
#define CONFIG_VOTP_SIZE 256

#endif


// ****************************************************************
//  此文件用于管理不同CPU间有差异的宏，源文件为app_modules_cpu_h.c
// ****************************************************************
#ifndef __APP_MODULES_CPU_H__
#define __APP_MODULES_CPU_H__

//  驱动使能
#define EXT_FLASH_EN 1 //  外挂资源flash使能
#define FLASH_CACHE_ENABLE 1

#define TFG_SD_EN 1 //  SDMMC驱动使能

//  文件系统
#define NORFS_EN 1 //  NORFS文件系统使能
#define FATFS_EN 1 //  FATFS文件系统使能
#define DUFAULT_FAT "fat"
#define FREE_FS_EN 0 //  FREEFS文件系统使能
#endif
