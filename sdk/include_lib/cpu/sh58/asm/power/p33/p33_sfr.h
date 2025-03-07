#ifndef __P33_SFR__
#define __P33_SFR__

#ifdef PMU_SYSTEM
#define P33_ACCESS(x) (*(volatile u32 *)(0xc000 + x*4))
#else
#define P33_ACCESS(x) (*(volatile u32 *)(0xf20000 + 0xc000 + x*4))
#endif


//==============================================================================


//............. 0X0000 - 0X000F............         for reset0
#define P3_RST_FLAG                   P33_ACCESS(0x000)
#define P3_RST_CON0                   P33_ACCESS(0x001)
#define P3_RST_CON1                   P33_ACCESS(0x002)
#define P3_RST_CON2                   P33_ACCESS(0x003)
//#define P3_RST_CON4                   P33_ACCESS(0x004)
//#define P3_RST_CON5                   P33_ACCESS(0x005)
#define P3_RST_SRC0                   P33_ACCESS(0x006)
#define P3_RST_SRC1                   P33_ACCESS(0x007)

//............. 0X0010 - 0X001F............         for reset1
#define P3_VLVD_CON0                  P33_ACCESS(0x010)
#define P3_VLVD_CON1                  P33_ACCESS(0x011)
#define P3_VLVD_FLT                   P33_ACCESS(0x012)
#define P3_PR_PWR                     P33_ACCESS(0x013)
#define P3_WDT_CON                    P33_ACCESS(0x014)

#define P3_PINR_CON                   P33_ACCESS(0x018)
#define P3_PINR_CON1                  P33_ACCESS(0x019)
#define P3_PINR_SAFE                  P33_ACCESS(0x01a)
#define P3_PINR_SAFE1                 P33_ACCESS(0x01b)
#define P3_PINR_PND1                  P33_ACCESS(0x01c)

//............. 0X0020 - 0X002F............         for clock0
#define P3_CLK_CON0                   P33_ACCESS(0x020)
#define P3_CLK_CON1                   P33_ACCESS(0x021)
//#define P3_CLK_CON2                   P33_ACCESS(0x022)
//#define P3_CLK_CON3                   P33_ACCESS(0x023)
#define P3_VLD_KEEP                   P33_ACCESS(0x024)

#define P3_LRC_CON0                   P33_ACCESS(0x028)
#define P3_LRC_CON1                   P33_ACCESS(0x029)

#define P3_OSL_CON                    P33_ACCESS(0x02f)

//............. 0X0030 - 0X003F............         for clock1

//............. 0X0040 - 0X004F............         for analog flow
#define P3_ANA_FLOW0                  P33_ACCESS(0x040)
#define P3_ANA_FLOW1                  P33_ACCESS(0x041)
#define P3_ANA_FLOW2                  P33_ACCESS(0x042)
#define P3_ANA_FLOW3                  P33_ACCESS(0x043)

#define P3_ANA_KEEP0                  P33_ACCESS(0x048)
#define P3_ANA_KEEP1                  P33_ACCESS(0x049)
#define P3_ANA_KEEP2                  P33_ACCESS(0x04a)
#define P3_ANA_KEEP3                  P33_ACCESS(0x04b)

//............. 0X0050 - 0X005F............         for analog control0
#define P3_VPWR_CON0                  P33_ACCESS(0x050)
#define P3_VPWR_CON1                  P33_ACCESS(0x051)
#define P3_PSW_CON0                   P33_ACCESS(0x052)
#define P3_PSW_CON1                   P33_ACCESS(0x053)
#define P3_PSW_CON2                   P33_ACCESS(0x054)
#define P3_PMU_ADC0                   P33_ACCESS(0x055)
#define P3_PMU_ADC1                   P33_ACCESS(0x056)
#define P3_VBG_CON0                   P33_ACCESS(0x057)
#define P3_VBG_CON1                   P33_ACCESS(0x058)
#define P3_IOV_CON0                   P33_ACCESS(0x059)
#define P3_IOV_CON1                   P33_ACCESS(0x05a)
#define P3_PAVD_CON0                  P33_ACCESS(0x05b)
#define P3_PVD_CON0                   P33_ACCESS(0x05c)
#define P3_WVD_CON0                   P33_ACCESS(0x05d)
#define P3_EVD_CON0                   P33_ACCESS(0x05e)

//............. 0X0060 - 0X006F............         for analog control1
#define P3_DVD_CON0                   P33_ACCESS(0x060)
#define P3_DVD2_CON0                  P33_ACCESS(0x061)
#define P3_RVD_CON0                   P33_ACCESS(0x062)
#define P3_RVD_CON1                   P33_ACCESS(0x063)
#define P3_RVD2_CON0                  P33_ACCESS(0x064)
#define P3_ANA_READ                   P33_ACCESS(0x065)
#define P3_OCP_CON0                   P33_ACCESS(0x066)
#define P3_DRPG_CON0                  P33_ACCESS(0x067)
#define P3_DRPG_CON1                  P33_ACCESS(0x068)
#define P3_SDPG_CON                   P33_ACCESS(0x069)
#define P3_VBAT_TYPE                  P33_ACCESS(0x06a)
#define P3_RTC_ADC0                   P33_ACCESS(0x06b)
#define P3_DCV_CON0                   P33_ACCESS(0x06c)
#define P3_DCV_CON1                   P33_ACCESS(0x06d)

//............. 0X0070 - 0X007F............         for analog control2
#define P3_CHG_CON0                   P33_ACCESS(0x070)
#define P3_CHG_CON1                   P33_ACCESS(0x071)
#define P3_CHG_CON2                   P33_ACCESS(0x072)
#define P3_CHG_CON3                   P33_ACCESS(0x073)
#define P3_CHG_CON4                   P33_ACCESS(0x074)

//............. 0X0080 - 0X008F............         for analog control3
#define P3_ANA_MFIX                   P33_ACCESS(0x080)
#define P3_MFIX_OPT                   P33_ACCESS(0x081)

//............. 0X0090 - 0X009F............         for analog control4

//............. 0X00A0 - 0X00AF............         for buck circuit0
//#define P3_BUCK1_CON0                 P33_ACCESS(0x0a0)
//#define P3_BUCK1_CON1                 P33_ACCESS(0x0a1)
//#define P3_BUCK1_CON2                 P33_ACCESS(0x0a2)
//#define P3_BUCK1_CON3                 P33_ACCESS(0x0a3)
//#define P3_BUCK1_CON4                 P33_ACCESS(0x0a4)
//#define P3_BUCK1_CON5                 P33_ACCESS(0x0a5)
//#define P3_BUCK1_CON6                 P33_ACCESS(0x0a6)
//#define P3_BUCK1_CON7                 P33_ACCESS(0x0a7)
#define P3_BUCK2_CON0                 P33_ACCESS(0x0a8)
#define P3_BUCK2_CON1                 P33_ACCESS(0x0a9)
#define P3_BUCK2_CON2                 P33_ACCESS(0x0aa)
#define P3_BUCK2_CON3                 P33_ACCESS(0x0ab)
#define P3_BUCK2_CON4                 P33_ACCESS(0x0ac)
#define P3_BUCK2_CON5                 P33_ACCESS(0x0ad)
#define P3_BUCK2_CON6                 P33_ACCESS(0x0ae)
#define P3_BUCK2_CON7                 P33_ACCESS(0x0af)

//............. 0X00B0 - 0X00BF............         for buck circuit1
//#define P3_BUCK3_CON0                 P33_ACCESS(0x0b0)
//#define P3_BUCK3_CON1                 P33_ACCESS(0x0b1)
//#define P3_BUCK3_CON2                 P33_ACCESS(0x0b2)
//#define P3_BUCK3_CON3                 P33_ACCESS(0x0b3)
//#define P3_BUCK3_CON4                 P33_ACCESS(0x0b4)
//#define P3_BUCK3_CON5                 P33_ACCESS(0x0b5)
//#define P3_BUCK3_CON6                 P33_ACCESS(0x0b6)
//#define P3_BUCK3_CON7                 P33_ACCESS(0x0b7)

//............. 0X00C0 - 0X00CF............         for low pwr flow0
#define P3_IVS_RD                     P33_ACCESS(0x0c0)
#define P3_IVS_SET                    P33_ACCESS(0x0c1)
#define P3_IVS_CLR                    P33_ACCESS(0x0c2)
//#define P3_WKUP_DLY                   P33_ACCESS(0x0c3)
#define P3_PMU_CON0                   P33_ACCESS(0x0c4)
//#define P3_PMU_CON1                   P33_ACCESS(0x0c5)
//#define P3_PMU_CON2                   P33_ACCESS(0x0c6)
//#define P3_PMU_CON3                   P33_ACCESS(0x0c7)
//#define P3_PMU_CON4                   P33_ACCESS(0x0c8)
//#define P3_PMU_CON5                   P33_ACCESS(0x0c9)
#define P3_P11_CPU                    P33_ACCESS(0x0ca)
#define P3_LS_P11                     P33_ACCESS(0x0cb)
#define P3_LS_EN                      P33_ACCESS(0x0cc)

//............. 0X00D0 - 0X00DF............         for low pwr flow1
#define P3_MLP_CTL0                   P33_ACCESS(0x0d0)
#define P3_MLP_CFG0                   P33_ACCESS(0x0d1)
#define P3_MLP_CFG1                   P33_ACCESS(0x0d2)
#define P3_MLP_CNT0                   P33_ACCESS(0x0d3)
#define P3_MLP_CNT1                   P33_ACCESS(0x0d4)

#define P3_PLP_CTL0                   P33_ACCESS(0x0d8)
#define P3_PLP_CFG0                   P33_ACCESS(0x0d9)
#define P3_PLP_CFG1                   P33_ACCESS(0x0da)
#define P3_PLP_CNT0                   P33_ACCESS(0x0db)
#define P3_PLP_CNT1                   P33_ACCESS(0x0dc)
#define P3_PLP_CNT2                   P33_ACCESS(0x0dd)

//............. 0X00E0 - 0X00EF............         for low pwr flow2
#define P3_WVDD_AUTO0                 P33_ACCESS(0x0e0)
#define P3_WVDD_AUTO1                 P33_ACCESS(0x0e1)
#define P3_PVDD_AUTO0                 P33_ACCESS(0x0e2)
#define P3_PVDD_AUTO1                 P33_ACCESS(0x0e3)

//............. 0X00F0 - 0X00FF............         for low pwr flow3

//............. 0X0100 - 0X010F............         for low pwr flow4
#define P3_DBG_CON0                   P33_ACCESS(0x100)
#define P3_DBG_CON1                   P33_ACCESS(0x101)
#define P3_FUNC_EN                    P33_ACCESS(0x102)
#define P3_FUNC_CTL0                  P33_ACCESS(0x103)
#define P3_FUNC_CTL1                  P33_ACCESS(0x104)
#define P3_FUNC_CTL2                  P33_ACCESS(0x105)

//............. 0X0110 - 0X011F............         for port wake up
#define P3_WKUP_FLT_EN0               P33_ACCESS(0x110)
#define P3_WKUP_P_IE0                 P33_ACCESS(0x111)
#define P3_WKUP_N_IE0                 P33_ACCESS(0x112)
#define P3_WKUP_LEVEL0                P33_ACCESS(0x113)
#define P3_WKUP_P_CPND0               P33_ACCESS(0x114)
#define P3_WKUP_N_CPND0               P33_ACCESS(0x115)
#define P3_WKUP_P_PND0                P33_ACCESS(0x116)
#define P3_WKUP_N_PND0                P33_ACCESS(0x117)
#define P3_WKUP_FLT_EN1               P33_ACCESS(0x118)
#define P3_WKUP_P_IE1                 P33_ACCESS(0x119)
#define P3_WKUP_N_IE1                 P33_ACCESS(0x11a)
#define P3_WKUP_LEVEL1                P33_ACCESS(0x11b)
#define P3_WKUP_P_CPND1               P33_ACCESS(0x11c)
#define P3_WKUP_N_CPND1               P33_ACCESS(0x11d)
#define P3_WKUP_P_PND1                P33_ACCESS(0x11e)
#define P3_WKUP_N_PND1                P33_ACCESS(0x11f)

//............. 0X0120 - 0X012F............         for analog wake up
#define P3_AWKUP_FLT_EN               P33_ACCESS(0x120)
#define P3_AWKUP_P_IE                 P33_ACCESS(0x121)
#define P3_AWKUP_N_IE                 P33_ACCESS(0x122)
#define P3_AWKUP_LEVEL                P33_ACCESS(0x123)
#define P3_AWKUP_P_PND                P33_ACCESS(0x124)
#define P3_AWKUP_N_PND                P33_ACCESS(0x125)
#define P3_AWKUP_P_CPND               P33_ACCESS(0x126)
#define P3_AWKUP_N_CPND               P33_ACCESS(0x127)
#define P3_WKUP_CLK_SEL               P33_ACCESS(0x128)
#define P3_AWKUP_CLK_SEL              P33_ACCESS(0x129)

//............. 0X0130 - 0X013F............         for wake up cfg0
#define P3_PCNT_FLT                   P33_ACCESS(0x130)
#define P3_PCNT_CON                   P33_ACCESS(0x131)
#define P3_PCNT_SET0                  P33_ACCESS(0x132)
#define P3_PCNT_SET1                  P33_ACCESS(0x133)
#define P3_PCNT_DAT0                  P33_ACCESS(0x134)
#define P3_PCNT_DAT1                  P33_ACCESS(0x135)

//............. 0X0140 - 0X014F............         for wake up cfg1
#define P3_WKUP_SRC0                  P33_ACCESS(0x140)
#define P3_WKUP_SRC1                  P33_ACCESS(0x141)

//............. 0X0150 - 0X015F............         for memory cfg0
#define P3_MNV_PWR0                   P33_ACCESS(0x150)
#define P3_MNV_PWR1                   P33_ACCESS(0x151)
#define P3_MNV_PWR2                   P33_ACCESS(0x152)
#define P3_MNV_PWR3                   P33_ACCESS(0x153)
#define P3_MNV_PWR4                   P33_ACCESS(0x154)
#define P3_MNV_PWR5                   P33_ACCESS(0x155)
#define P3_MNV_PWR6                   P33_ACCESS(0x156)
#define P3_MNV_PWR7                   P33_ACCESS(0x157)
#define P3_MNV_PWR8                   P33_ACCESS(0x158)
#define P3_MNV_PWR9                   P33_ACCESS(0x159)
#define P3_MNV_PWRA                   P33_ACCESS(0x15a)
#define P3_MNV_PWRB                   P33_ACCESS(0x15b)
#define P3_MNV_PWRC                   P33_ACCESS(0x15c)
#define P3_MNV_PWRD                   P33_ACCESS(0x15d)
#define P3_MNV_PWRE                   P33_ACCESS(0x15e)
#define P3_MNV_PWRF                   P33_ACCESS(0x15f)

//............. 0X0160 - 0X016F............         for memory cfg1
#define P3_MNV_CFG                    P33_ACCESS(0x160)
#define P3_PNV_CFG                    P33_ACCESS(0x161)

#define P3_MNV_SEL0                   P33_ACCESS(0x164)
#define P3_MNV_SEL1                   P33_ACCESS(0x165)
#define P3_PNV_SEL0                   P33_ACCESS(0x166)
#define P3_PNV_SEL1                   P33_ACCESS(0x167)

#define P3_PNV_PWR0                   P33_ACCESS(0x168)
#define P3_PNV_PWR1                   P33_ACCESS(0x169)
#define P3_PNV_PWR2                   P33_ACCESS(0x16a)
#define P3_PNV_PWR3                   P33_ACCESS(0x16b)
#define P3_PNV_PWR4                   P33_ACCESS(0x16c)
#define P3_PNV_PWR5                   P33_ACCESS(0x16d)
#define P3_PNV_PWR6                   P33_ACCESS(0x16e)
#define P3_PNV_PWR7                   P33_ACCESS(0x16f)

//............. 0X0170 - 0X017F............         for memory cfg2
#define P3_EFUSE_CON0                 P33_ACCESS(0x170)
#define P3_EFUSE_CON1                 P33_ACCESS(0x171)
#define P3_EFUSE_CON2                 P33_ACCESS(0x172)
#define P3_EFUSE_RDAT                 P33_ACCESS(0x173)
#define P3_EFUSE_PU_DAT0              P33_ACCESS(0x174)
#define P3_EFUSE_PU_DAT1              P33_ACCESS(0x175)
#define P3_EFUSE_PU_DAT2              P33_ACCESS(0x176)
#define P3_EFUSE_PU_DAT3              P33_ACCESS(0x177)
#define P3_EFUSE_ANA0                 P33_ACCESS(0x178)

//............. 0X0180 - 0X018F............         for soft flag
#define P3_SFLAG0                     P33_ACCESS(0x180)
#define P3_SFLAG1                     P33_ACCESS(0x181)
#define P3_SFLAG2                     P33_ACCESS(0x182)
#define P3_SFLAG3                     P33_ACCESS(0x183)
#define P3_SFLAG4                     P33_ACCESS(0x184)
#define P3_SFLAG5                     P33_ACCESS(0x185)
#define P3_SFLAG6                     P33_ACCESS(0x186)
#define P3_SFLAG7                     P33_ACCESS(0x187)
#define P3_SFLAG8                     P33_ACCESS(0x188)
#define P3_SFLAG9                     P33_ACCESS(0x189)
#define P3_SFLAGA                     P33_ACCESS(0x18a)
#define P3_SFLAGB                     P33_ACCESS(0x18b)
#define P3_SFLAGC                     P33_ACCESS(0x18c)
#define P3_SFLAGD                     P33_ACCESS(0x18d)
#define P3_SFLAGE                     P33_ACCESS(0x18e)
#define P3_SFLAGF                     P33_ACCESS(0x18f)

//............. 0X0190 - 0X019F............         for port input select
#define P3_PORT_SEL0                  P33_ACCESS(0x190)
#define P3_PORT_SEL1                  P33_ACCESS(0x191)
#define P3_PORT_SEL2                  P33_ACCESS(0x192)
#define P3_PORT_SEL3                  P33_ACCESS(0x193)
#define P3_PORT_SEL4                  P33_ACCESS(0x194)
#define P3_PORT_SEL5                  P33_ACCESS(0x195)
#define P3_PORT_SEL6                  P33_ACCESS(0x196)
#define P3_PORT_SEL7                  P33_ACCESS(0x197)
#define P3_PORT_SEL8                  P33_ACCESS(0x198)
#define P3_PORT_SEL9                  P33_ACCESS(0x199)
#define P3_PORT_SEL10                 P33_ACCESS(0x19a)
#define P3_PORT_SEL11                 P33_ACCESS(0x19b)
#define P3_PORT_SEL12                 P33_ACCESS(0x19c)
#define P3_PORT_SEL13                 P33_ACCESS(0x19d)
#define P3_PORT_SEL14                 P33_ACCESS(0x19e)
#define P3_PORT_SEL15                 P33_ACCESS(0x19f)

//............. 0X01A0 - 0X01AF............         for ls cfg
#define P3_LS_IO_USR                  P33_ACCESS(0x1a0)
#define P3_LS_IO_ROM                  P33_ACCESS(0x1a1)
#define P3_LS_IO_PINR                 P33_ACCESS(0x1a2)
#define P3_LS_PLL                     P33_ACCESS(0x1a3)
#define P3_LS_CON4                    P33_ACCESS(0x1a4)
#define P3_LS_CON5                    P33_ACCESS(0x1a5)
#define P3_LS_CON6                    P33_ACCESS(0x1a6)
#define P3_LS_CON7                    P33_ACCESS(0x1a7)
#define P3_LS_CON8                    P33_ACCESS(0x1a8)
#define P3_LS_CON9                    P33_ACCESS(0x1a9)
#define P3_LS_CONA                    P33_ACCESS(0x1aa)
#define P3_LS_CONB                    P33_ACCESS(0x1ab)
#define P3_LS_CONC                    P33_ACCESS(0x1ac)
#define P3_LS_COND                    P33_ACCESS(0x1ad)
#define P3_LS_CONE                    P33_ACCESS(0x1ae)
#define P3_LS_CONF                    P33_ACCESS(0x1af)

//............. 0X01B0 - 0X01BF............         for p33 lp timer cfg0
#define P3_LP_RSC00                   P33_ACCESS(0x1b0)
#define P3_LP_RSC01                   P33_ACCESS(0x1b1)
#define P3_LP_RSC02                   P33_ACCESS(0x1b2)
#define P3_LP_RSC03                   P33_ACCESS(0x1b3)
#define P3_LP_RSC04                   P33_ACCESS(0x1b4)
#define P3_LP_RSC05                   P33_ACCESS(0x1b5)
#define P3_LP_PRD00                   P33_ACCESS(0x1b6)
#define P3_LP_PRD01                   P33_ACCESS(0x1b7)
#define P3_LP_PRD02                   P33_ACCESS(0x1b8)
#define P3_LP_PRD03                   P33_ACCESS(0x1b9)
#define P3_LP_PRD04                   P33_ACCESS(0x1ba)
#define P3_LP_PRD05                   P33_ACCESS(0x1bb)
#define P3_LP_TMR0_CLK                P33_ACCESS(0x1bc)
#define P3_LP_TMR0_CON                P33_ACCESS(0x1bd)
#define P3_LP_TMR0_CFG                P33_ACCESS(0x1be)

//............. 0X01C0 - 0X01CF............         for p33 lp timer cfg1
#define P3_LP_RSC10                   P33_ACCESS(0x1c0)
#define P3_LP_RSC11                   P33_ACCESS(0x1c1)
#define P3_LP_RSC12                   P33_ACCESS(0x1c2)
#define P3_LP_RSC13                   P33_ACCESS(0x1c3)
#define P3_LP_RSC14                   P33_ACCESS(0x1c4)
#define P3_LP_RSC15                   P33_ACCESS(0x1c5)
#define P3_LP_PRD10                   P33_ACCESS(0x1c6)
#define P3_LP_PRD11                   P33_ACCESS(0x1c7)
#define P3_LP_PRD12                   P33_ACCESS(0x1c8)
#define P3_LP_PRD13                   P33_ACCESS(0x1c9)
#define P3_LP_PRD14                   P33_ACCESS(0x1ca)
#define P3_LP_PRD15                   P33_ACCESS(0x1cb)
#define P3_LP_TMR1_CLK                P33_ACCESS(0x1cc)
#define P3_LP_TMR1_CON                P33_ACCESS(0x1cd)
#define P3_LP_TMR1_CFG                P33_ACCESS(0x1ce)

//............. 0X01D0 - 0X01DF............         for p33 lp timer cfg2
#define P3_LP_CNT0                    P33_ACCESS(0x1d0)
#define P3_LP_CNT1                    P33_ACCESS(0x1d1)
#define P3_LP_CNT2                    P33_ACCESS(0x1d2)
#define P3_LP_CNT3                    P33_ACCESS(0x1d3)
#define P3_LP_CNT4                    P33_ACCESS(0x1d4)
#define P3_LP_CNT5                    P33_ACCESS(0x1d5)
#define P3_LP_CNTRD0                  P33_ACCESS(0x1d6)

//............. 0X01F0 - 0X01FF............         for reserved





#endif
