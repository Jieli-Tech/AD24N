#ifndef __PMU_FLAG_H__
#define __PMU_FLAG_H__

//p33 soft flag
enum soft_flag_io_stage {
    SOFTFLAG_HIGH_RESISTANCE,
    SOFTFLAG_PU10K,
    SOFTFLAG_PD10K,

    SOFTFLAG_OUT0,
    SOFTFLAG_OUT0_HD0,
    SOFTFLAG_OUT0_HD,
    SOFTFLAG_OUT0_HD0_HD,

    SOFTFLAG_OUT1,
    SOFTFLAG_OUT1_HD0,
    SOFTFLAG_OUT1_HD,
    SOFTFLAG_OUT1_HD0_HD,

    SOFTFLAG_PU100K,
    SOFTFLAG_PU1M,
    SOFTFLAG_PD100K,
    SOFTFLAG_PD1M,

    SOFTFLAG_SKIP_IO_RECOVER,
};

struct app_soft_flag_t {
    u8 sfc_fast_boot;
    u8 sfc_flash_stable_delay_sel;
    u8 flash_stable_delay_sel;
    u8 usbdp;
    u8 usbdm;
    u8 pa7;
    u8 pb3;
};

//*********************************************************************************//
//                                                                                 //
//                               end of this module                                //
//                                                                                 //
//*********************************************************************************//
#endif
