// *INDENT-OFF*
#include  "maskrom_stubs.ld"
//config

_BOOT_RAM_BEGIN = _MASK_EXPORT_MEM_BEGIN - 0x24;
MEMORY
{
    app_code(rx)        : ORIGIN = 0xC000100,            LENGTH = 64M-0x100
    ram0(rw)            : ORIGIN = _RAM_LIMIT_L,         LENGTH = _BOOT_RAM_BEGIN - _RAM_LIMIT_L
    boot_ram(rw)        : ORIGIN = _BOOT_RAM_BEGIN,     LENGTH = 0x24
}

ENTRY(_start)

SECTIONS
{
    /* L1 memory sections */

    . = ORIGIN(boot_ram);
    .boot_data ALIGN(4) : SUBALIGN(4)
    {
         *(.boot_info)
    } > boot_ram

    . = ORIGIN(ram0);
    .data ALIGN(4) : SUBALIGN(4)
    {
        PROVIDE(data_buf_start = .);
        *(.data*)
        *(.*.data)

        cache_Lx_code_text_begin = .;
        *(.common)
        *(.mic_capless_tab)

        *(.*.text.cache.L1)
        *(.*.text.cache.L2)
        *(.*.text.cache.L3)
        . = ALIGN(4);
    } > ram0

    .lowpower_overlay ALIGN(4) : SUBALIGN(4)
    {
        lowpower_buf_start = .;
        *(.power_driver.data.overlay);
        *(.power_driver.text.cache.L1.overlay);
        *(.power_driver.data.bss.overlay);
        lowpower_buf_end = .;
    } > ram0
    cache_Lx_code_text_end = .;

    .ans_data_sec ALIGN(4):
    {
        ans_data_start = .;
        *(.aec_data)
        *(.ns_data)
        *(.nlp_data)
        *(.fft_data)
        *(.noisegate_data)
        ans_data_end = .;
    } > ram0

    .debug_data ALIGN(4) : SUBALIGN(4)
    {
        PROVIDE(debug_buf_start = .);
        *(.debug_bss)
        *(.debug_data)
    } > ram0


    .bss ALIGN(4) (NOLOAD) : SUBALIGN(4)
    {
        PROVIDE(bss_buf_start = .);
        . = ALIGN(4);
        _cpu0_sstack_begin = .;
        . = ALIGN(4);
        *(.intr_stack)
        . = ALIGN(4);
        *(.stack_magic);
        . = ALIGN(4);
        *(.stack)
        . = ALIGN(4);
        *(.stack_magic0);
        . = ALIGN(4);
        _cpu0_sstack_end = .;
        . = ALIGN(4);
        _system_data_begin = .;
        *(.bss)
        *(.*.data.bss)
        . = ALIGN(4);
        _system_data_end = .;
        *(.usb_h_dma)
        *(.DAC_BUFFER)
        *(.AUDIO_ADC_BUFFER)
    } > ram0

    .ans_bss_sec ALIGN(4):
    {
        ans_bss_start = .;
        . = ALIGN(32);
        *(.aec_bss)
        *(.ns_bss)
        *(.nlp_bss)
        *(.fft_bss)
        ans_bss_end = .;
    } > ram0

    /* OVERLAY : */
    .effect_buf ALIGN(4) : SUBALIGN(4)
    {
        PROVIDE(effect_buf_start = .);
        /* . = ALIGN(4); */
        /* *(.sp_data) */
        . = ALIGN(4);
        *(.rs_data)
        /* .d_sp {  *(.sp_data) } */
        /* .d_rs { *(.rs_data) } */
    } > ram0
    /* _ram0_end = .; */
    /* .dec_buf ALIGN(32): */
    /* { */
        /* PROVIDE(a_buf_start = .); */
        /* *(.a_data); */
        /* PROVIDE(a_buf_end = .); */
    /* } > ram0 */

    /* . = ORIGIN(ram1); */
    OVERLAY : AT(0x200000)
    {
        .d_toy_music
        {
            PROVIDE(toy_music_buf_start = .);
            *(.toy_music_data);
            PROVIDE(toy_music_buf_end = .);
        }
        d_speed
        {
            . = toy_music_buf_end;
            PROVIDE(speed_buf_start = .);
            . = ALIGN(4);
            *(.sp_data)
            PROVIDE(speed_buf_end = .);
        }
        .d_a
        {
            . = speed_buf_end;
            PROVIDE(a_buf_start = .);
            *(.a_data);
            PROVIDE(a_buf_end = .);
        }

        .d_midi
        {
            . = a_buf_end;
            PROVIDE(midi_buf_start = .);
            *(.midi_buf);
            PROVIDE(midi_buf_end = .);
            PROVIDE(midi_ctrl_buf_start = .);
            *(.midi_ctrl_buf);
            PROVIDE(midi_ctrl_buf_end = .);
        }
        .d_ump3
        {
            . = a_buf_end;
            PROVIDE(ump3_buf_start = .);
            *(.ump3_data);
            PROVIDE(ump3_buf_end = .);
        }
        .d_mp3_st
        {
            . = a_buf_end;
            PROVIDE(mp3_st_buf_start = .);
            *(.mp3_st_data);
            PROVIDE(mp3_st_buf_end = .);
        }
        .d_wav
        {
            . = a_buf_end;
            PROVIDE(wav_buf_start = .);
            *(.wav_data);
            PROVIDE(wav_buf_end = .);
        }
        .d_f1a
        {
            . = a_buf_end;
            PROVIDE(f1a_1_buf_start = .);
            *(.f1a_1_buf);
            PROVIDE(f1a_1_buf_end = .);
           PROVIDE(f1a_2_buf_start = .);
            *(.f1a_2_buf);
           PROVIDE(f1a_2_buf_end = .);
        }
        .d_rec
        {
            *(.rec_data)
            *(.ans_data)
            rec_data_end = .;
        }
        .d_enc_ima
        {
            . = rec_data_end;
            *(.enc_a_data)
        }
        .d_enc_mp3
        {
            . = rec_data_end;
            *(.enc_mp3_data)
        }
        .d_aux
        {
            /* . = aux_data_end; */
            *(.aux_data)
            aux_data_end = .;
        }
        .d_vp_data0
        {
            /* . = aux_data_end; */
            *(.speaker_data)
            speaker_data_end = .;
            . = ALIGN(4);
            *(.howling_data)
            . = ALIGN(4);
            *(.notch_howling_data)
            . = ALIGN(4);
            *(.notch_howling_lib)
            howling_data_end = .;
        }

        .d_vp_data1
        {
            . = howling_data_end;
            . = ALIGN(4);
            *(.vp_data);
        }

        .d_vc_data
        {
            . = howling_data_end;
            . = ALIGN(4);
            *(.voicechanger_data);
        }
        .d_echo_data
        {

            . = howling_data_end;
            . = ALIGN(4);
            *(.echo_data);
        }

        .d_pcm_eq_data
        {
            . = howling_data_end;
            . = ALIGN(4);
            *(.pcm_eq_data);
        }

        .pc_buffer
        {
            *(.usb_msd_dma)
            *(.usb_hid_dma )
            *(.usb_config_var)
            *(.mass_storage)
            *(.usb_iso_dma)
            *(.uac_var)
            *(.uac_rx)
            pc_data_end = .;
        }
        .norflash_cache
        {
            . = pc_data_end;
            *(.norflash_cache_buf)
        }
        /* .d_ima {  *(.a_data) } */
    } > ram0

    d_dec_0 = MAX(midi_ctrl_buf_end,ump3_buf_end);
    d_dec_1 = MAX(d_dec_0 ,mp3_st_buf_end);
    d_dec_2 = MAX(d_dec_1 ,wav_buf_end);
    d_dec_max = MAX(d_dec_2 ,f1a_2_buf_end);


    .heap_buf ALIGN(4) : SUBALIGN(4)
    {
        PROVIDE(_free_start = .);
        . = LENGTH(ram0) + ORIGIN(ram0) - 1;
        PROVIDE(_free_end = .);
    } > ram0


    _ram_end = .;


    . = ORIGIN(app_code);
    .app_code ALIGN(4) : SUBALIGN(4)
    {
        app_code_text_begin = .;
        *startup.o(.text)
        . = ALIGN(4);
        *(*.f1a_code)
        _VERSION_BEGIN = .;
        KEEP(*(.version))
        _VERSION_END = .;
        *(.debug)
        *(.debug_const)
        *(.debug_code)
        *(.debug_string)
        . = ALIGN(4);
        *(.fft_const)
        *(.fft_code)
        *(.*.text.const)
        *(.text.*)
        /* *memset.o(.text .rodata*) */
        /* *memcmp.o(.text .rodata*) */
        *(*.text.const)
        *(*.text)
        *(.text)
        *(.app_root)
        *(.vm)
        . = ALIGN(4);
        _SPI_CODE_START = .;
        *(.spi_code)
        . = ALIGN(4);
        _SPI_CODE_END = .;

		. = ALIGN(4);
        _SFC_DTR_CODE_START = . ;
        *(.sfc_dtr_code)
		. = ALIGN(4);
        _SFC_DTR_CODE_END = . ;

        *(.rodata*)
        *(.ins)
        app_code_text_end = .;

        /* *(.poweroff_text) */
        . = ALIGN(4);
        loop_detect_handler_begin = .;
            KEEP(*(.loop_detect_region))
        loop_detect_handler_end = .;

        . = ALIGN(4);
        device_node_begin = .;
        PROVIDE(device_node_begin = .);
        KEEP(*(.device))
        _device_node_end = .;
        PROVIDE(device_node_end = .);

        vfs_ops_begin = .;
        KEEP(*(.vfs_operations))
        vfs_ops_end = .;

        . = ALIGN(4);
        lp_target_begin = .;
        PROVIDE(lp_target_begin = .);
        KEEP(*(.lp_target))
        lp_target_end = .;
        PROVIDE(lp_target_end = .);

        . = ALIGN(4);
        lp_request_begin = .;
        PROVIDE(lp_request_begin = .);
        KEEP(*(.lp_request))
        lp_request_end = .;
        PROVIDE(lp_request_end = .);

        . = ALIGN(4);
        deepsleep_target_begin = .;
        PROVIDE(deepsleep_target_begin = .);
        KEEP(*(.deepsleep_target))
        deepsleep_target_end = .;
        PROVIDE(deepsleep_target_end = .);

        . = ALIGN(4);
        p2m_msg_handler_begin = .;
        PROVIDE(p2m_msg_handler_begin = .);
        KEEP(*(.p2m_msg_handler))
        PROVIDE(p2m_msg_handler_end = .);
        p2m_msg_handler_end = .;

        . = ALIGN(4);
        phw_begin = .;
        PROVIDE(phw_begin = .);
        KEEP(*(.phw_operation))
        PROVIDE(phw_end = .);
        phw_end = .;

		hsb_critical_handler_begin = .;
		KEEP(*(.hsb_critical_txt))
		hsb_critical_handler_end = .;

		lsb_critical_handler_begin = .;
		KEEP(*(.lsb_critical_txt))
		lsb_critical_handler_end = .;

        app_size = .;
        /* . = LENGTH(app_code) - LENGTH_OF_ENTRY_LIST; */
        /* *(.sec.array_maskrom_export) */
        . = ALIGN(4);
        text_end = .;
    } >app_code


    bss_begin       = ADDR(.bss);
    bss_size        = SIZEOF(.bss);

    /*除堆栈外的bss区*/
    bss_size1       = _system_data_end - _system_data_begin;
    bss_begin1      = _system_data_begin;

    data_addr  = ADDR(.data) ;
    data_begin = text_end ;
    data_size =  SIZEOF(.data) + SIZEOF(.debug_data);

    text_size       = SIZEOF(.app_code);

    _sdk_text_addr = ADDR(.app_code);
    _sdk_text_size = text_size;
    _sdk_text_addr_end = _sdk_text_addr + _sdk_text_size;

    _sdk_data_size = data_size;

    ASSERT((bss_size1 % 4) == 0, "bss size not 4")
    ASSERT((bss_begin1 % 4) == 0, "data size not 4")

    ASSERT((data_size % 4) == 0, "data size not 4")
    ASSERT((data_addr % 4) == 0, "data_addr size not 4")
    ASSERT((data_begin % 4) == 0, "data_begin size not 4")

}

lowpower_overlay_addr       = lowpower_buf_start;
lowpower_overlay_begin      = data_begin + data_size;
lowpower_overlay_size       = lowpower_buf_end - lowpower_buf_start;

