//*********************************************************************************//
// Module name : csfr.h                                                            //
// Description : j32CPU core sfr define                                            //
// By Designer : zequan_liu                                                        //
// Dat changed :                                                                   //
//*********************************************************************************//

#ifndef __J32CPU_CSFR__
#define __J32CPU_CSFR__

#define __RW         volatile       // read write
#define __RO         volatile const // only read
#define __WO         volatile       // only write

#define __u8         unsigned int   // u8  to u32 special for struct
#define __u16        unsigned int   // u16 to u32 special for struct
#define __u32        unsigned int

#define csfr_base    0xff0000
#define j32CPU_num            1
//*********************************************************************************
//
// hcore_sfr
//
//*********************************************************************************

//............. 0x0000 - 0x00ff............
typedef struct {
    __RW __u32 CON0;
    __RW __u32 FTMAX;
} JL_CMNG_TypeDef;

#define JL_CMNG_BASE                   (csfr_base + map_adr(0x00, 0x00))
#define JL_CMNG                        ((JL_CMNG_TypeDef *)JL_CMNG_BASE)

//............. 0x0100 - 0x01ff............
//typedef struct {
//    __RW __u32 CON;
//    __RW __u32 KEY;
//} JL_SDTAP_TypeDef;

//#define JL_SDTAP_BASE                  (csfr_base + map_adr(0x01, 0x00))
//#define JL_SDTAP                       ((JL_SDTAP_TypeDef *)JL_SDTAP_BASE)

//............. 0x0200 - 0x02ff............
typedef struct {
    __RW __u32 WREN;
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 MSG0;
    __RW __u32 MSG1;
    __RW __u32 MSG2;
    __RW __u32 MSG3;
    __RO __u32 ID;
} JL_CEMU_TypeDef;

#define JL_CEMU_BASE                   (csfr_base + map_adr(0x02, 0x00))
#define JL_CEMU                        ((JL_CEMU_TypeDef *)JL_CEMU_BASE)

//............. 0x0300 - 0x03ff............

//#define MPU_INV                        (1<<31)
//#define MPU_PWEN                       (1<<16)
//#define MPU_PREN                       (1<<8)
//#define MPU_PEN                        (MPU_PWEN | MPU_PREN)
//#define MPU_XEN                        (1<<2)
//#define MPU_WEN                        (1<<1)
//#define MPU_REN                        (1<<0)

//#define MPU_IDx_cfg(n, id)             (id<<(n*8))
//#define MPU_IDx_pen(n, pr, pw)         ((pr<<(9+n)) | (pw<<(17+n)))

//typedef struct {
//    __RW __u32 CON[15]; // 0-1 used in sh58
//    __RO __u32 REV0;
//    __RW __u32 CID[15]; // 0-1 used in sh58
//    __RO __u32 REV1;
//    __RW __u32 BEG[15]; // 0-1 used in sh58
//    __RO __u32 REV2;
//    __RW __u32 END[15]; // 0-1 used in sh58
//    __RW __u32 WREN;
//} JL_MPU_TypeDef;

//#define JL_MPU_BASE                   (csfr_base + map_adr(0x03, 0x00))
//#define JL_MPU                        ((JL_MPU_TypeDef *)JL_MPU_BASE)

//............. 0x0400 - 0x04ff............
/*
  typedef struct {
      __RW __u32 CON;
      __RW __u32 TLB1_BEG;
      __RW __u32 TLB1_END;
  } JL_MMU_TypeDef;

  #define JL_MMU_BASE                   (csfr_base + map_adr(0x04, 0x00))
  #define JL_MMU                        ((JL_MMU_TypeDef *)JL_MMU_BASE)

  typedef struct {
      short page:14;
      short vld:1;
      short reserved:1;
  } JL_MMU_TLB1_TypeDef;

  #define JL_MMU_TLB1                   ((JL_MMU_TLB1_TypeDef *)(JL_MMU->TLB1_BEG))
*/
//............. 0x0500 - 0x05ff............
#define JL_TypeDef_L1P                JL_TypeDef_j32CPU_ICU
//#define JL_TypeDef_L1P                JL_TypeDef_j32CPU_DCU
#define JL_L1P_BASE                   (csfr_base + map_adr(0x05, 0x00))
#define JL_L1P                        ((JL_TypeDef_L1P *)JL_L1P_BASE)

//............. 0x0600 - 0x06ff............
#define JL_TypeDef_L2I                JL_TypeDef_j32CPU_ICU
#define JL_L2I_BASE                   (csfr_base + map_adr(0x06, 0x00))
#define JL_L2I                        ((JL_TypeDef_L2I *)JL_L2I_BASE)

//............. 0x0700 - 0x07ff............
#define JL_TypeDef_L2D                JL_TypeDef_j32CPU_DCU
#define JL_L2D_BASE                   (csfr_base + map_adr(0x07, 0x00))
#define JL_L2D                        ((JL_TypeDef_L2D *)JL_L2D_BASE)

//............. 0x0800 - 0x08ff............
typedef struct {
    __RO __u32 CHIP_ID;
    __RO __u32 CHIP_VER;
} JL_SYSTEM_TypeDef;

#define JL_SYSTEM_BASE                (csfr_base + map_adr(0x08, 0x00))
#define JL_SYSTEM                     ((JL_SYSTEM_TypeDef   *)JL_SYSTEM_BASE)

//............. 0x0900 - 0x09ff............
typedef struct {
    __RW __u32 CON;
    __RW __u32 BEG;
    __RW __u32 END;
    __RW __u32 DAT_VLD0;
    __RW __u32 DAT_VLD1;
    __RW __u32 DAT_VLD2;
    __RW __u32 DAT_VLD3;
    __RO __u32 ROM_CRC;
    __RW __u32 MCFG0_SEL;
    __RW __u32 MCFG1_SEL;
    __RW __u32 MCFG0_RF1P;
    __RW __u32 MCFG0_RF2P;
    __RW __u32 MCFG0_RM1P;
    __RW __u32 MCFG0_RM2P;
    __RW __u32 MCFG0_VROM;
    __RW __u32 MCFG1_RM1P;
    __RW __u32 MCFG0_CON[4];
    __RW __u32 MCFG1_CON[4];
} JL_MBIST_TypeDef;

#define JL_MBIST_BASE                 (csfr_base + map_adr(0x09, 0x00))
#define JL_MBIST                      ((JL_MBIST_TypeDef *)JL_MBIST_BASE)

//............. 0x0a00 - 0x0aff............
//typedef struct {
//    __RW __u32 CON;
//    __RW __u32 CADR;
//    __RW __u32 ACC0L;
//    __RW __u32 ACC0H;
//    __RW __u32 ACC1L;
//    __RW __u32 ACC1H;
//    __RW __u32 CONST;
//    __RW __u32 TEST1;
//} JL_FFT_TypeDef;
//
//#define JL_FFT_BASE                   (csfr_base + map_adr(0x0a, 0x00))
//#define JL_FFT                        ((JL_FFT_TypeDef    *)JL_FFT_BASE)

//............. 0x0b00 - 0x0bff............
//typedef struct {
///* 00 */  __RW __u32 CON0;
///* 01 */  __RW __u32 LPEN_CON;
///* 02 */  __RW __u32 LSEN_CON;
///* 03 */  __RO __u32 LS_PND;
///* 04 */  __RO __u32 CE_PND;
///* 05 */  __RW __u32 LS_PRD_32K;
///* 06 */  __RW __u32 CE_PRD_32K;
///* 07 */  __RW __u32 LS_PRD_64K;
///* 08 */  __RW __u32 CE_PRD_64K;
///* 09 */  __RW __u32 LS_PRD_48K;
///* 0a */  __RW __u32 CE_PRD_48K;
//} JL_ATOLP_TypeDef;

//#define JL_ATOLP_BASE                 (csfr_base + map_adr(0x0b, 0x00))
//#define JL_ATOLP                      ((JL_ATOLP_TypeDef  *)JL_ATOLP_BASE)

//*********************************************************************************
//
// j32CPU_sfr
//
//*********************************************************************************

//---------------------------------------------//
// j32CPU define
//---------------------------------------------//

#define j32CPU_sfr_offset   0x000800
#define j32CPU_sfr_base    (csfr_base + 0xe000)

#define j32CPU_cpu_base    (j32CPU_sfr_base + 0x0000)
#define j32CPU_icu_base    (j32CPU_sfr_base + 0x0400)

#define j32CPU(n)          ((JL_TypeDef_j32CPU     *)(j32CPU_sfr_base + j32CPU_sfr_offset*n))
#define j32CPU_icu(n)      ((JL_TypeDef_j32CPU_ICU *)(j32CPU_icu_base + j32CPU_sfr_offset*n))

//---------------------------------------------//
// j32CPU core sfr
//---------------------------------------------//

typedef struct {
    /* 00 */  __RO __u32 DR00;
    /* 01 */  __RO __u32 DR01;
    /* 02 */  __RO __u32 DR02;
    /* 03 */  __RO __u32 DR03;
    /* 04 */  __RO __u32 DR04;
    /* 05 */  __RO __u32 DR05;
    /* 06 */  __RO __u32 DR06;
    /* 07 */  __RO __u32 DR07;
    /* 08 */  __RO __u32 DR08;
    /* 09 */  __RO __u32 DR09;
    /* 0a */  __RO __u32 DR10;
    /* 0b */  __RO __u32 DR11;
    /* 0c */  __RO __u32 DR12;
    /* 0d */  __RO __u32 DR13;
    /* 0e */  __RO __u32 DR14;
    /* 0f */  __RO __u32 SP;

    /* 10 */  __RO __u32 RETI;
    /* 11 */  __RO __u32 RETE;
    /* 12 */  __RO __u32 SR02;
    /* 13 */  __RO __u32 SR03;
    /* 14 */  __RO __u32 MACL;
    /* 15 */  __RO __u32 MACH;
    /* 16 */  __RO __u32 RETS;
    /* 17 */  __RO __u32 PSR;
    /* 18 */  __RO __u32 SR08;
    /* 19 */  __RO __u32 SR09;
    /* 1a */  __RO __u32 SR10;
    /* 1b */  __RO __u32 SSP;
    /* 1c */  __RO __u32 CNUM;
    /* 1d */  __RO __u32 ICFG;
    /* 1e */  __RO __u32 PCRS;
    /* 1f */  __RO __u32 USP;

    /* 20 */  __RW __u32 BPCON;
    /* 21 */  __RW __u32 BSP;
    /* 22 */  __RW __u32 BP0;
    /*    */  __RO __u32 REV22[0x26 - 0x22 - 1];
    /* 26 */  __WO __u32 CMD_PAUSE;
    /*    */  __RO __u32 REV2a[0x30 - 0x26 - 1];

    /* 30 */  __RW __u32 PMU_CON0;
    /* 31 */  __RW __u32 PMU_CON1;
    /*    */  __RO __u32 REV30[0x3b - 0x31 - 1];
    /* 3b */  __RW __u8  TTMR_CON;
    /* 3c */  __RW __u32 TTMR_CNT;
    /* 3d */  __RW __u32 TTMR_PRD;
    /* 3e */  __RW __u32 BANK_CON;
    /* 3f */  __RW __u32 BANK_NUM;

    /* 40 */  __RW __u32 ICFG00;
    /* 41 */  __RW __u32 ICFG01;
    /* 42 */  __RW __u32 ICFG02;
    /* 43 */  __RW __u32 ICFG03;
    /* 44 */  __RW __u32 ICFG04;
    /* 45 */  __RW __u32 ICFG05;
    /* 46 */  __RW __u32 ICFG06;
    /* 47 */  __RW __u32 ICFG07;
    /* 48 */  __RW __u32 ICFG08;
    /* 49 */  __RW __u32 ICFG09;
    /* 4a */  __RW __u32 ICFG10;
    /* 4b */  __RW __u32 ICFG11;
    /* 4c */  __RW __u32 ICFG12;
    /* 4d */  __RW __u32 ICFG13;
    /* 4e */  __RW __u32 ICFG14;
    /* 4f */  __RW __u32 ICFG15;

    /* 50 */  __RW __u32 ICFG16;
    /* 51 */  __RW __u32 ICFG17;
    /* 52 */  __RW __u32 ICFG18;
    /* 53 */  __RW __u32 ICFG19;
    /* 54 */  __RW __u32 ICFG20;
    /* 55 */  __RW __u32 ICFG21;
    /* 56 */  __RW __u32 ICFG22;
    /* 57 */  __RW __u32 ICFG23;
    /* 58 */  __RW __u32 ICFG24;
    /* 59 */  __RW __u32 ICFG25;
    /* 5a */  __RW __u32 ICFG26;
    /* 5b */  __RW __u32 ICFG27;
    /* 5c */  __RW __u32 ICFG28;
    /* 5d */  __RW __u32 ICFG29;
    /* 5e */  __RW __u32 ICFG30;
    /* 5f */  __RW __u32 ICFG31;

    /* 60 */  __RO __u32 IPND0;
    /* 61 */  __RO __u32 IPND1;
    /* 62 */  __RO __u32 IPND2;
    /* 63 */  __RO __u32 IPND3;
    /* 64 */  __RO __u32 IPND4;
    /* 65 */  __RO __u32 IPND5;
    /* 66 */  __RO __u32 IPND6;
    /* 67 */  __RO __u32 IPND7;
    /* 68 */  __WO __u32 ILAT_SET;
    /* 69 */  __WO __u32 ILAT_CLR;
    /* 6a */  __RW __u32 IPMASK;
    /* 6b */  __RW __u32 GIEMASK;
    /* 6c */  __RW __u32 IWKUP_NUM;
    /*    */  __RO __u32 REV6c[0x80 - 0x6c - 1];

    /* 80 */  __RW __u32 EMU_CON;
    /* 81 */  __RW __u32 EMU_MSG;
    /* 82 */  __RW __u32 EMU_SSP_H;
    /* 83 */  __RW __u32 EMU_SSP_L;
    /* 84 */  __RW __u32 EMU_USP_H;
    /* 85 */  __RW __u32 EMU_USP_L;
    /* 86 */  __RW __u32 LIM_PC0_H;
    /* 87 */  __RW __u32 LIM_PC0_L;
    /* 88 */  __RW __u32 LIM_PC1_H;
    /* 89 */  __RW __u32 LIM_PC1_L;
    /* 8a */  __RW __u32 LIM_PC2_H;
    /* 8b */  __RW __u32 LIM_PC2_L;
    /*    */  __RO __u32 REV8b[0x90 - 0x8b - 1];

    /* 90 */  __RW __u32 ESU_CON;
    /* 91 */  __RO __u32 CNT_CHIT;
    /* 92 */  __RO __u32 CNT_CMIS;
    /* 93 */  __RO __u32 CNT_FILL;
    /* 94 */  __RO __u32 CNT_IHIT;
    /* 95 */  __RO __u32 CNT_IMIS;
    /* 96 */  __RO __u32 CNT_RHIT;
    /* 97 */  __RO __u32 CNT_RMIS;
    /* 98 */  __RO __u32 CNT_WHIT;
    /* 99 */  __RO __u32 CNT_WMIS;
} JL_TypeDef_j32CPU;

//---------------------------------------------//
// j32CPU icache sfr
//---------------------------------------------//

typedef struct {
    __RW __u32 CON;
    __RW __u32 EMU_CON;
    __RW __u32 EMU_MSG;
    __RW __u32 EMU_ID;
    __RW __u32 CMD_CON;
    __RW __u32 CMD_BEG;
    __RW __u32 CMD_END;
    __RW __u32 CNT_RACK;
    __RW __u32 CNT_RNAK;
    __RW __u32 MBIST_SEL;
    __RW __u32 MCFG0_CON[4];
} JL_TypeDef_j32CPU_ICU;

#undef __RW
#undef __RO
#undef __WO

#undef __u8
#undef __u16
#undef __u32

//*********************************************************************************//
//                                                                                 //
//                               end of this module                                //
//                                                                                 //
//*********************************************************************************//
#endif

