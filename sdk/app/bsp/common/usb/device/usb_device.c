#include "usb/device/usb_stack.h"
#include "usb_config.h"
#include "usb/device/msd.h"
#include "usb/scsi.h"
#include "usb/device/hid.h"
#include "usb/device/cdc.h"
#include "usb/device/uac_audio.h"
#include "irq.h"
#include "gpio.h"
#include "usb_common_def.h"
#include "app_config.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[USB]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE

#include "log.h"
static void usb_device_init(const usb_dev usb_id)
{
    usb_config(usb_id);
    usb_g_sie_init(usb_id);
    usb_slave_init(usb_id);
    usb_set_dma_raddr(usb_id, 0, usb_get_ep_buffer(usb_id, 0));
    usb_set_dma_raddr(usb_id, 1, usb_get_ep_buffer(usb_id, 0));
    usb_set_dma_raddr(usb_id, 2, usb_get_ep_buffer(usb_id, 0));
    usb_set_dma_raddr(usb_id, 3, usb_get_ep_buffer(usb_id, 0));
    usb_set_dma_raddr(usb_id, 4, usb_get_ep_buffer(usb_id, 0));

    usb_write_intr_usbe(usb_id, INTRUSB_RESET_BABBLE | INTRUSB_SUSPEND);
    usb_clr_intr_txe(usb_id, -1);
    usb_clr_intr_rxe(usb_id, -1);
    usb_set_intr_txe(usb_id, 0);
    usb_set_intr_rxe(usb_id, 0);
    usb_g_isr_reg(usb_id, IRQ_USB_IP, 0);
    /* usb_sof_isr_reg(usb_id,3,0); */
    /* usb_sofie_enable(usb_id); */
    /* USB_DEBUG_PRINTF("ep0 addr %x %x\n", usb_get_dma_taddr(0), ep0_dma_buffer); */
}
static void usb_device_hold(const usb_dev usb_id)
{

    usb_g_hold(usb_id);
    usb_release(usb_id);
}


int usb_device_mode(const usb_dev usb_id, const u32 class)
{
    /* usb_device_set_class(CLASS_CONFIG); */
    u8 class_index = 0;
    if (class == 0) {
        gpio_set_mode(IO_PORT_SPILT(IO_PORT_DM), PORT_HIGHZ);
        gpio_set_mode(IO_PORT_SPILT(IO_PORT_DP), PORT_HIGHZ);

        os_time_dly(20);

        gpio_set_mode(IO_PORT_SPILT(IO_PORT_DM), PORT_INPUT_FLOATING);
        gpio_set_mode(IO_PORT_SPILT(IO_PORT_DP), PORT_INPUT_FLOATING);

#if USB_DEVICE_CLASS_CONFIG & MASSSTORAGE_CLASS
        msd_release(usb_id);
#endif
#if USB_DEVICE_CLASS_CONFIG & AUDIO_CLASS
        uac_release(usb_id);
#endif
        usb_device_hold(usb_id);
        return 0;
    }

    usb_add_desc_config(usb_id, MAX_INTERFACE_NUM, NULL);

#if USB_DEVICE_CLASS_CONFIG & AUDIO_CLASS
    if ((class & AUDIO_CLASS) == AUDIO_CLASS) {
        usb_add_desc_config(usb_id, class_index++, uac_audio_desc_config);
        log_info("add desc audio");
    } else if ((class & SPEAKER_CLASS) == SPEAKER_CLASS) {
        usb_add_desc_config(usb_id, class_index++, uac_spk_desc_config);
        log_info("add desc speaker");
    } else if ((class & MIC_CLASS) == MIC_CLASS) {
        usb_add_desc_config(usb_id, class_index++, uac_mic_desc_config);
        log_info("add desc mic");
    }
#endif

#if USB_DEVICE_CLASS_CONFIG & HID_CLASS
    if ((class & HID_CLASS) == HID_CLASS) {
        usb_add_desc_config(usb_id, class_index++, hid_desc_config);
        log_info("add desc std hid");
    }
#endif
#if USB_DEVICE_CLASS_CONFIG & MASSSTORAGE_CLASS
    if ((class & MASSSTORAGE_CLASS) == MASSSTORAGE_CLASS) {
        msd_register(usb_id);
        usb_add_desc_config(usb_id, class_index++, msd_desc_config);
        log_info("add desc msd");
    }
#endif
#if USB_DEVICE_CLASS_CONFIG & CDC_CLASS
    if ((class & CDC_CLASS) == CDC_CLASS) {
        cdc_register(usb_id);
        usb_add_desc_config(usb_id, class_index++, cdc_desc_config);
        log_info("add desc std cdc");
    }
#endif
#if USB_DEVICE_CLASS_CONFIG & MIDI_CLASS
    extern u32 midi_desc_config(const usb_dev usb_id, u8 * ptr, u32 * cur_itf_num);
    if ((class & MIDI_CLASS) == MIDI_CLASS) {
        msd_register(usb_id);
        usb_add_desc_config(usb_id, class_index++, midi_desc_config);
        log_info(">>>>> add desc midi <<<<<");
    }
#endif
    usb_device_init(usb_id);
    user_setup_filter_install(usb_id2device(usb_id));
    return 0;
}
void usb_otg_sof_check_init(const usb_dev id)
{
    u32 ep = 0;
    u8 *ep_buffer = usb_get_ep_buffer(id, ep);

    usb_g_sie_init(id);

    usb_set_dma_raddr(id, ep, ep_buffer);

    for (ep = 1; ep < USB_MAX_HW_EPNUM; ep++) {
        usb_disable_ep(id, ep);
    }
    usb_sof_clr_pnd(id);
}
/* module_initcall(usb_device_mode); */
