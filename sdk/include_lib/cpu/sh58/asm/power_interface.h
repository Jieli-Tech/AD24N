#ifndef __POWER_INTERFACE_H__
#define __POWER_INTERFACE_H__

//#include "generic/typedef.h"
#include "typedef.h"
#include "cpu/gpio.h"

//-------------------------------------------------------
/* p33
 */
#include "power/p33/p33_sfr.h"
#include "power/p33/p33_access.h"
#include "power/p33/charge_hw.h"
#include "power/p33/p33_api.h"
#include "power/p33/wdt.h"
/* #include "power/p33/p33_interface.h" */

//-------------------------------------------------------
/* p11
 */
//#include "power/p11/p11_csfr.h"
#include "power/p11_sfr.h"
//#include "power/p11/p11_mmap.h"
//#include "power/p11/p11_api.h"
//#include "power/p11/lp_ipc.h"


//-------------------------------------------------------
/* power
 */
#include "power/power_api.h"
#include "power/power_wakeup.h"
#include "power/power_reset.h"
#include "power/power_port.h"
#include "power/vbat_det_trim.h"
#include "power/power_trim.h"

//-------------------------------------------------------
/* app
 */
#include "power/power_app.h"
#include "power/power_compat.h"

#endif
