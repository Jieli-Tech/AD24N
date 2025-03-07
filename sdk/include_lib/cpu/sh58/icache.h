#ifndef __J32CPU_ICACHE__
#define __J32CPU_ICACHE__

//*********************************************************************************//
// Module name : icache.h                                                          //
// Description : j32CPU icache control head file                                   //
// By Designer : zequan_liu                                                        //
// Dat changed :                                                                   //
//*********************************************************************************//

//   ------  ------       ------  ------        ------  ------
//   | c0 |  | c1 |       | c0 |  | c1 |        | c0 |  | c1 |
//   ------  ------       ------  ------        ------  ------
//     |       |            |       |             |       |
//   ------  ------       ------  ------        ------  ------
//   | L1 |  | L1 |       | L1 |  | L1 |        | L1 |  | L1 |
//   ------  ------       ------  ------        ------  ------
//     |_______|            |_______|             |_______|
//         |                    |                     |
//     ---------            ---------             ---------
//     | flash |            |  L2i  |             |  L2c  |
//     ---------            ---------             ---------
//                              |                     |
//                          ---------             ---------
//                          | flash |             | flash |
//                          ---------             ---------
//
//   INCLUDE_L2I==0      INCLUDE_L2I==1        INCLUDE_L2I==0
//   INCLUDE_L2C==0      INCLUDE_L2C==0        INCLUDE_L2C==1
#include "typedef.h"

#define INCLUDE_ICU_RPT      0
#define INCLUDE_ICU_EMU      0
#define INCLUDE_L2I          0   // L2i & L2d separate
#define INCLUDE_L2C          0   // L2i & L2d together

//------------------------------------------------------//
// icache level 1 function
//------------------------------------------------------//

void IcuEnable(void);
void IcuDisable(void);
void IcuInitial(void);
void IcuWaitIdle(void);
void IcuSetWayNum(u32 way);

void IcuFlushinvAll(void);
void IcuFlushinvRegion(u32 *beg, u32 len);            // note len!=0
void IcuUnlockAll(void);
void IcuUnlockRegion(u32 *beg, u32 len);              // note len!=0
void IcuLockRegion(u32 *beg, u32 len);                // note len!=0
void IcuPfetchRegion(u32 *beg, u32 len);              // note len!=0

void IcuReportEnable(void);
void IcuReportDisable(void);
void IcuReportPrintf(void);
void IcuReportClear(void);

void IcuEmuEnable(void);
void IcuEmuDisable(void);
void IcuEmuMessage(void);

//------------------------------------------------------//
// icache level 2 function
//------------------------------------------------------//

#if (INCLUDE_L2I)

void L2iEnable(void);
void L2iDisable(void);
void L2iInitial(void);
void L2iSetWayNum(u32 way);

void L2iFlushinvAll(void);
void L2iFlushinvRegion(u32 *beg, u32 len);        // note len!=0
void L2iUnlockAll(void);
void L2iUnlockRegion(u32 *beg, u32 len);          // note len!=0
void L2iLockRegion(u32 *beg, u32 len);            // note len!=0
void L2iPfetchRegion(u32 *beg, u32 len);          // note len!=0

void L2iReportEnable(void);
void L2iReportDisable(void);
void L2iReportPrintf(void);
void L2iReportClear(void);

void L2iEmuEnable(void);
void L2iEmuDisable(void);
void L2iEmuMessage(void);

#endif

//*********************************************************************************//
//                                                                                 //
//                               end of this module                                //
//                                                                                 //
//*********************************************************************************//
#endif

