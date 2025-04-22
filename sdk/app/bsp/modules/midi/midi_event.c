﻿#include "midi_dec.h"

const int smpl_rate_tab[9] = {
    48000,
    44100,
    32000,
    24000,
    22050,
    16000,
    12000,
    11025,
    8000
};

const int inv_smpl_rate_tab[9] = {
    2731, 2972, 4096, 5461, 5944, 8192, 10923, 11889, 16384
};

static int midi_setTempo(MIDI_DECODE_VAR *mid_dec_obj, int lval)
{
    long long tmp64;
    mid_dec_obj->srTicks = (lval / 15625) * (smpl_rate_tab[mid_dec_obj->sample_rate] * (1 << (MIDI_RESAMPLE_SHIFT - 6)) / (mid_dec_obj->tmDiv));
    MULSI(mid_dec_obj->now_srTicks, tmp64, mid_dec_obj->srTicks, mid_dec_obj->midi_tempo_v, 10);
    return 0;
}

int MetaEvent(MIDI_DECODE_VAR *mid_dec_obj, short trkNum, int block_len)
{
    u16 meta, tmpVar;
    u32 metaLen = 0;
    int lval;

    meta = midi_inputdata(mid_dec_obj, 1);
    block_len--;

    do {
        tmpVar = midi_inputdata(mid_dec_obj, 1);
        metaLen = (metaLen << 7) + (tmpVar & 0x7f);
        block_len--;
    } while (tmpVar & 0x80);

    switch (meta) {
    case MIDI_META_TMPO: {
        lval = DATA_REVERSE(midi_inputdata(mid_dec_obj, 3), 3);
        block_len -= 3;
        midi_setTempo(mid_dec_obj, lval);
    }
    break;
    case MIDI_META_TMSG: {
        lval = midi_inputdata(mid_dec_obj, 4);
        mid_dec_obj->tmDiv_up_dest = lval & 0xff;
        lval = lval >> 8;
        mid_dec_obj->several_notes = 1 << (lval & 0xff);
        block_len -= 4;

        if (mid_dec_obj->div_first_cnt) {
            mid_dec_obj->div_first_cnt = 0;
            if (mid_dec_obj->tmDiv_enable) {
                mid_dec_obj->tmDiv_trig.timeDiv_trigger(mid_dec_obj->tmDiv_trig.priv);
            }
            if (mid_dec_obj->beat_enable) {
                mid_dec_obj->beat_trig.beat_trigger(mid_dec_obj->beat_trig.priv, mid_dec_obj->tmDiv_up_dest, mid_dec_obj->several_notes);
            }
        }
    }
    break;
    case 0x06:
    case 0x07: {
        u8 *o_ptr = (u8 *)mid_dec_obj->out_channel;
        u8 ptr_i = 0;
        for (ptr_i = 0; ptr_i < metaLen; ptr_i++) {
            o_ptr[ptr_i] = midi_inputdata(mid_dec_obj, 1);
        }

        mid_dec_obj->midi_mark_info.cur_mark_pos = mid_dec_obj->midi_mark_info.cur_mark_pos + 1;
        if (mid_dec_obj->triggered) {
            mid_dec_obj->mark_trig.mark_trigger(mid_dec_obj->mark_trig.priv, o_ptr, metaLen);
        }

        if (mid_dec_obj->midi_mark_info.cur_mark_pos == mid_dec_obj->midi_mark_info.mark_start) {
            memcpy(&mid_dec_obj->mark_array[1], &mid_dec_obj->mark_array[2], sizeof(MIDI_SAVE_POS_STRUCT));
            mid_dec_obj->midi_mark_info.cur_mark_store_pos = mid_dec_obj->midi_mark_info.cur_mark_pos;
        }

        block_len -= metaLen;
    }
    break;
    default: {
        midi_skipdata(mid_dec_obj, metaLen);
        block_len -= metaLen;
    }
    break;

    }
    return block_len;
}

int SysCommon(MIDI_DECODE_VAR *mid_dec_obj, unsigned short msg, int block_len)
{
    unsigned int dataLen, tmpVar;
    switch (msg) {
    case MIDI_SYSEX:
        dataLen = 0;
        {
            do {
                tmpVar = midi_inputdata(mid_dec_obj, 1);
                dataLen = (dataLen << 7) + (tmpVar & 0x7f);
                block_len = block_len - 1;
            } while (tmpVar & 0x80);
        }
        midi_skipdata(mid_dec_obj, dataLen);
        block_len = block_len - dataLen;
        break;
    case MIDI_SNGPOS:
        midi_inputdata(mid_dec_obj, 2);
        block_len = block_len - 2;
        break;
    case MIDI_TMCODE:
    case MIDI_SNGSEL:
        midi_inputdata(mid_dec_obj, 1);
        block_len = block_len - 1;
        break;
    case MIDI_TUNREQ:
    case MIDI_ENDEX:
    case MIDI_TMCLK:
    case MIDI_START:
    case MIDI_CONT:
    case MIDI_STOP:
    case MIDI_ACTSNS:
        break;
    default:
        break;
    }
    return block_len;
}


static void control_volume(MIDI_DECODE_VAR *mid_dec_obj, WaveInfo_t *waveInfo, short chn)
{
    int ctrlAtten;
    ctrlAtten = mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_EXPR_CC];
    ctrlAtten = ctrlAtten * mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_VOL_CC];
    waveInfo->vol_atten = ((int)((u16)waveInfo->initAtten) * (int)(mkey_vol_tab[ctrlAtten >> 7])) >> 9;

    if (mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_SFT_ON_CC] > MIDI_SwitchLevel_CC) {
        waveInfo->vol_atten = ((int)waveInfo->vol_atten * 46396) >> 16;
    }
    waveInfo->vol_hold_new = waveInfo->vol_hold << 14;
}

int control_stop(MIDI_DECODE_VAR *mid_dec_obj, WaveInfo_t *temp_t, short chn)
{
    long long tmp64;
    temp_t->ison = SEQ_AE_REL;
    if ((mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_SUS_ON_CC]) ||
        (!temp_t->sostenuto && (mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_SOS_ON_CC]))) {
        temp_t->pendingStop = 1;
        return 0;
    }
    if (temp_t->mode == 3) {
        temp_t->mode = 0;
    }

    temp_t->pendingStop = 0;

    temp_t->vol_dec_now = temp_t->vol_rel;
    temp_t->vol_dec_now_bit = 16;
    temp_t->vol_hold = 0;
    temp_t->vol_hold_new = 0;

    temp_t->vol_cnt = 1;
    if (temp_t->env_info.env_use) {
        MULSI(temp_t->vol_now, tmp64, temp_t->vol_now, temp_t->env_info.vol_f, 30);
        temp_t->env_info.env_use = 0;
    }
    return 1;
}

static void control_pan(MIDI_DECODE_VAR *mid_dec_obj, WaveInfo_t *waveInfo, short chn)
{
    int pan_t;
    pan_t = mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_PAN_CC] - 64;
    pan_t += waveInfo->pan;
    if (pan_t < -63) {
        pan_t = -63;
    } else if (pan_t > 63) {
        pan_t = 63;
    }
    waveInfo->panLft = pan_tab[(64 - pan_t)];
    waveInfo->panRgt = pan_tab[(64 + pan_t)];
};

static  void  sr_renew_waveinfo(WaveInfo_t *waveInfo, int loopstart_n, int loopend_n)
{
    if (waveInfo->mode) {
        int  indexval = 8192 * loopend_n / waveInfo->loopLen;
        indexval = indexval;
        waveInfo->loopLen = loopend_n - 1;
        waveInfo->loopStart = loopstart_n;
        waveInfo->loopEnd = waveInfo->loopLen + waveInfo->loopStart;
        waveInfo->tableEnd = waveInfo->loopEnd + 1;
        waveInfo->voice.indexIncr = (waveInfo->voice.indexIncr * indexval) >> 13;
    } else {
        int len_n;
        len_n = loopstart_n;
        int  indexval = 8192 * len_n / (waveInfo->tableEnd - 1);
        waveInfo->voice.indexIncr = (waveInfo->voice.indexIncr * indexval) >> 13;
        waveInfo->tableEnd = len_n;
    }
}

void control_note_on(MIDI_DECODE_VAR *mid_dec_obj, short chn, short trackn, int val1, int val2)
{
    WaveInfo_t *waveInfo;
    long long tmp64;
    mid_dec_obj->peekOK = 0;

    int maintrackval = trackn;

    if (MAINTRACK_USE_CHN) {
        maintrackval = chn;
    }

    if ((mid_dec_obj->crtl_mode) && (mid_dec_obj->crtl_mode != CMD_MIDI_CTRL_MODE_W2S)) {
        mid_dec_obj->flag_t = 1;

        if (mid_dec_obj->okon_mode == CMD_MIDI_OKON_MODE_1) {
            if ((mid_dec_obj->note_on == 1) && mid_dec_obj->note_on_flag) {
                mid_dec_obj->note_on = 2;
                mid_dec_obj->okon_wr_cnt = 1;
                return;
            }
            mid_dec_obj->midi_player_on |= BIT(mid_dec_obj->player_index);
            mid_dec_obj->note_on = 1;
        }

        if (mid_dec_obj->okon_mode == CMD_MIDI_OKON_MODE_0) {
            if (maintrackval == mid_dec_obj->tracknV) {
                if (mid_dec_obj->melody_enable) {
                    if ((mid_dec_obj->note_on == 1) && mid_dec_obj->note_on_flag) {
                        mid_dec_obj->note_on = 2;
                        mid_dec_obj->okon_wr_cnt = 1;
                        return;
                    }
                    mid_dec_obj->note_on = 1;
                    mid_dec_obj->midi_player_on |= BIT(mid_dec_obj->player_index);
                } else {
                    return;
                }
            } else {
                mid_dec_obj->midi_player_on |= BIT(mid_dec_obj->player_index);
            }
        }
        if (mid_dec_obj->okon_mode == CMD_MIDI_OKON_MODE_2) {
            if (maintrackval == mid_dec_obj->tracknV) {
                if (mid_dec_obj->melody_enable) {
                    if ((mid_dec_obj->note_on == 1) && mid_dec_obj->note_on_flag) {
                        mid_dec_obj->note_on = 2;
                        mid_dec_obj->okon_wr_cnt = 1;
                        return;
                    }
                    mid_dec_obj->note_on = 1;
                    mid_dec_obj->midi_player_on |= BIT(mid_dec_obj->player_index);
                } else {
                    return;
                }
            }
        }
    } else {
        mid_dec_obj->midi_player_on |= BIT(mid_dec_obj->player_index);
    }


    mid_dec_obj->keydown_cnt++;


    mid_dec_obj->midi_channel_on |= BIT(chn);

    waveInfo = &mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info;
    memset(waveInfo, 0, sizeof(WaveInfo_t));

    if ((mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_W2S) && (maintrackval == mid_dec_obj->tracknV)) {

        MIDI_W2S_INFO *w2s_obj = (MIDI_W2S_INFO *)(&mid_dec_obj->w2s_obj);

        mid_dec_obj->zone.sampleMap = w2s_obj->data_pos[w2s_obj->word_cnti];
        mid_dec_obj->zone.tableEnd = w2s_obj->data_len[w2s_obj->word_cnti];

        mid_dec_obj->zone.vol_dec = 65527;
        mid_dec_obj->zone.vol_rel = 0;
        mid_dec_obj->zone.vol_incr = 3140;
        mid_dec_obj->zone.cents = 4096;
        mid_dec_obj->zone.mode = 0;
        mid_dec_obj->zone.initAtten = 65535;
        mid_dec_obj->zone.loopStart = 0;
        mid_dec_obj->zone.vol_hold = 16384;

        w2s_obj->word_cnti++;
        if (w2s_obj->word_cnti >= w2s_obj->word_cnt) {
            w2s_obj->word_cnti = 0;
        }

        if (mid_dec_obj->key_set == 0) {
            mid_dec_obj->zone.keyNum = w2s_obj->key_diff + val1;
            if (mid_dec_obj->zone.keyNum < 0) {
                mid_dec_obj->zone.keyNum = val1;
            } else if (mid_dec_obj->zone.keyNum > 127) {
                mid_dec_obj->zone.keyNum = val1;
            }
            mid_dec_obj->key_set = mid_dec_obj->zone.keyNum;
        } else {
            mid_dec_obj->zone.keyNum = mid_dec_obj->key_set;
        }

    }

    waveInfo->trk_v = trackn;

    waveInfo->noKey = val1;
    waveInfo->noVel = val2;
    waveInfo->chnl = chn;

    if (mid_dec_obj->pitch_enable) {
        val1 = val1 + mid_dec_obj->semitone_ctrl.key_diff[chn];
        if (val1 > 127) {
            val1 = 127;
        }
        if (val1 < 0) {
            val1 = 0;
        }
    }

    waveInfo->sostenuto = (mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_SOS_ON_CC] > MIDI_SwitchLevel_CC);

    waveInfo->initAtten = mid_dec_obj->zone.initAtten;
    waveInfo->initAtten = (waveInfo->initAtten * (int)mkey_vol_tab[val2]) >> 8;

    {
        s8 test_key_i;
        u32 shift_v;
        u8 shift_i;
        test_key_i = val1 - mid_dec_obj->zone.keyNum;

        shift_i = 13;

        if (test_key_i < 0) {
            while (test_key_i < 0) {
                test_key_i += 12;
                shift_i++;
            }
            shift_v = (pow2VALTab_12[(int)test_key_i]);
        } else {
            while (test_key_i >= 12) {
                test_key_i = test_key_i - 12;
                shift_i--;
            }
            shift_v = (pow2VALTab_12[(int)test_key_i]);
        }

        shift_v = (shift_v * inv_smpl_rate_tab[mid_dec_obj->sample_rate]) >> 13;

        waveInfo->voice.indexIncr = (unsigned int)shift_v * (unsigned int)mid_dec_obj->zone.cents >> shift_i;
    }

    waveInfo->sampleMap = mid_dec_obj->zone.sampleMap;

    if (mid_dec_obj->zone.vol_incr == 16385) {
        waveInfo->vol_dec_now = 65536;
        waveInfo->vol_rel = 65536;
    } else {
        waveInfo->vol_dec_now = mid_dec_obj->zone.vol_dec;
        waveInfo->vol_rel = mid_dec_obj->zone.vol_rel;
        {
            int tmpsr = mid_dec_obj->sample_rate;
            while (tmpsr >= 3) {
                tmpsr = tmpsr - 3;
                MULRS(waveInfo->vol_dec_now, tmp64, waveInfo->vol_dec_now, waveInfo->vol_dec_now, 16);
                MULRS(waveInfo->vol_rel, tmp64, waveInfo->vol_rel, waveInfo->vol_rel, 16);
            }

            if (tmpsr) {
                MULRS(waveInfo->vol_dec_now, tmp64, waveInfo->vol_dec_now, 65535, 16);
                MULRS(waveInfo->vol_rel, tmp64, waveInfo->vol_rel, 65535, 16);
            }

            MULRS(waveInfo->vol_dec_now, tmp64, waveInfo->vol_dec_now, mid_dec_obj->decay_speed[chn], 15);
            MULRS(waveInfo->vol_rel, tmp64, waveInfo->vol_rel, mid_dec_obj->decay_speed[chn], 15);
        }
    }
    waveInfo->vol_dec_now_bit = 16;
    waveInfo->vol_hold = mid_dec_obj->zone.vol_hold;
    waveInfo->vol_hold_flag = mid_dec_obj->zone.vol_hold_flag;
    waveInfo->tableEnd = mid_dec_obj->zone.tableEnd;
    waveInfo->loopLen = mid_dec_obj->zone.loopLen;
    waveInfo->loopStart = mid_dec_obj->zone.loopStart;
    waveInfo->loopEnd = waveInfo->loopStart + waveInfo->loopLen;
    waveInfo->mode = mid_dec_obj->zone.mode;

    waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap]);

    if ((mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_W2S) && (maintrackval == mid_dec_obj->tracknV)) {
        waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_posbk[mid_dec_obj->zone.sampleMap]);
    }

    if ((mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_W2S) && (maintrackval == mid_dec_obj->tracknV)) {
        waveInfo->prognum = 0;
    } else {
        if (mid_dec_obj->compressIN) {
            waveInfo->prognum = waveInfo->voice.wavebuf[0];
            waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap + 1]);

            if (waveInfo->prognum == PCM_KIND) {
                waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap + 2]);
            } else if (waveInfo->prognum >= PCM_KIND_DOWN) {
                unsigned char *sptr = (unsigned char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap + 2]);
                short loopstart_n = sptr[1] << 8 | sptr[0];
                short loopend_n = sptr[3] << 8 | sptr[2];
                if (waveInfo->mode) {
                    waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap + 6]);
                    sr_renew_waveinfo(waveInfo, loopstart_n, loopend_n);
                } else {
                    waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap + 4]);
                    sr_renew_waveinfo(waveInfo, loopstart_n, loopend_n);
                }
                waveInfo->prognum = waveInfo->prognum - PCM_KIND_DOWN;
            }
        }
    }
    waveInfo->voice.index = 0;

    control_pan(mid_dec_obj, waveInfo, chn);
    control_volume(mid_dec_obj, waveInfo, chn);

    waveInfo->note_on_off_dest = 100;

    if (mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) {
        mid_dec_obj->peekOK = 1;//midi_peek_value(mid_dec_obj,val1);
    }

    waveInfo->voice.isend = 0;
    waveInfo->pendingStop = 0;
    waveInfo->ison = SEQ_AE_ON;
    waveInfo->flags = SEQ_AE_TM;

    waveInfo->attack_incr = (mid_dec_obj->zone.vol_incr << 16);

    if (mid_dec_obj->zone.exclNote) {
        waveInfo->exclIndex = (chn << 9) | (mid_dec_obj->zone.exclNote & 0xf);
        waveInfo->exclOn = 1;
    }

    waveInfo->initAtten = waveInfo->voice.indexIncr;
    waveInfo->voice.indexIncr = (waveInfo->initAtten * mid_dec_obj->pitchBend_v[chn]) >> 8;
    if (mid_dec_obj->zone.have_array) {
        ENV *env_info = &waveInfo->env_info;
        env_info->env_use = 1;
        u32 tz = mid_dec_obj->zone.vol_hold_flag;
        tz = (tz << 16) | mid_dec_obj->zone.vol_hold;
        env_info->env_infov = &mid_dec_obj->midi_spi_pos[tz];
        env_info->vol_f = 1073741824;
        waveInfo->vol_hold_flag = 0;

        int valget = 0;

        unsigned char *env_infov = env_info->env_infov;
        {
            short tmpshift = 7;
            u8 readval = env_infov[env_info->envi++];
            valget = (readval & 0x7f);
            while (!(readval & 0x80)) {
                readval = env_infov[env_info->envi++];
                valget = ((readval & 0x7f) << tmpshift) | valget;
                tmpshift += 7;
            }
        }

        env_info->env_points = valget;

        if (env_info->env_points > 0) {
            valget = 0;
            int doPoints = smpl_rate_tab[mid_dec_obj->sample_rate] * 5 / 1000;

            u8 readval = env_infov[env_info->envi++];
            valget = (readval & 0x7f);

            short tmpshift = 7;
            while (!(readval & 0x80)) {
                readval = env_infov[env_info->envi++];
                valget = ((readval & 0x7f) << tmpshift) | valget;
                tmpshift += 7;
            }
            int stepcnt = valget;

            readval = env_infov[env_info->envi++];
            valget = env_infov[env_info->envi++];
            valget = (valget << 8) | readval;
            short tmpste = (short)(valget & 0xffff);

            int tmpste1 = tmpste;
            env_info->stepval = (tmpste1 << 15) / doPoints;
            env_info->stepcnt = stepcnt * doPoints;

            env_info->env_points--;
        }

    } else {
        waveInfo->env_info.env_use = 0;
    }
    waveInfo->active_off_flag = 1;

}


static void control_message_handle(MIDI_DECODE_VAR *mid_dec_obj, short chn, int val1, int val2)
{
    int play_cnt = 0;
    int tmp_i;
    unsigned char o_v[32];
    MIDI_CHANNEL_CONTROL *st = &(mid_dec_obj->channel_mixer[chn]);

    switch (val1) {
    case  MIDI_CTRL_SOS_ON:
    case  MIDI_CTRL_SUS_ON:
    case  MIDI_CTRL_EXPR:
    case  MIDI_CTRL_VOL:
    case  MIDI_CTRL_SFT_ON:
    case MIDI_CTRL_PAN:
        for (tmp_i = 0; tmp_i < mid_dec_obj->MAX_PLAYER_CNTt; tmp_i++) {
            if (mid_dec_obj->midi_players[tmp_i].wave_info.chnl == chn) {
                if (mid_dec_obj->midi_player_on & BIT(tmp_i)) {
                    o_v[play_cnt++] = tmp_i;
                }
            }
        }
        break;
    default:
        return;
    }

    switch (val1) {
    case  MIDI_CTRL_SOS_ON: {
        st->cc[MIDI_CTRL_SOS_ON_CC] = val2;
        for (tmp_i = 0; tmp_i < play_cnt; tmp_i++) {
            WaveInfo_t *wv_info = &mid_dec_obj->midi_players[o_v[tmp_i]].wave_info;
            if (wv_info->pendingStop) {
                control_stop(mid_dec_obj, wv_info, chn);
            }
        };

    }
    break;              //sustain

    case  MIDI_CTRL_SUS_ON: {
        st->cc[MIDI_CTRL_SUS_ON_CC] = val2;
        for (tmp_i = 0; tmp_i < play_cnt; tmp_i++) {
            WaveInfo_t *wv_info = &mid_dec_obj->midi_players[o_v[tmp_i]].wave_info;
            if (wv_info->pendingStop) {
                control_stop(mid_dec_obj, wv_info, chn);
            }
        };
    }
    break;
    case  MIDI_CTRL_EXPR: {
        st->cc[MIDI_CTRL_EXPR_CC] = val2;

    }
    break;
    case  MIDI_CTRL_VOL: {
        st->cc[MIDI_CTRL_VOL_CC] = val2 & 0x7f;

    }
    break;
    case  MIDI_CTRL_SFT_ON: {
        st->cc[MIDI_CTRL_SFT_ON_CC] = val2;
    }
    break;                //vol

    case MIDI_CTRL_PAN: {
        st->cc[MIDI_CTRL_PAN_CC] = val2;
    }
    break;
    default:
        break;
    }
}

int ChnlMessage(MIDI_DECODE_VAR *mid_dec_obj, short trkNum, unsigned short msg, int block_len)
{
    unsigned char chn = msg & MIDI_CHNMSK;
    unsigned char mevent = msg & MIDI_EVTMSK;
    WaveInfo_t *waveInfo;
    u16 val1, val2;

    switch (mevent) {
    case MIDI_NOTEON:

        if (mid_dec_obj->saved_rd) {
            val1 = mid_dec_obj->value_rd;
            mid_dec_obj->saved_rd = 0;
            block_len += 1;
        } else {
            val1 = midi_inputdata(mid_dec_obj, 1);
        }
        val2 = (midi_inputdata(mid_dec_obj, 1) & 0x7f);
        block_len -= 2;

        if (mid_dec_obj->midi_mark_info.mark_enable) {
            if ((mid_dec_obj->midi_mark_info.cur_mark_pos < mid_dec_obj->midi_mark_info.mark_start) || (mid_dec_obj->midi_mark_info.cur_mark_pos >= mid_dec_obj->midi_mark_info.mark_end)) {
                mid_dec_obj->wdt_count = mid_dec_obj->wdt_count + 1;
                if (val2 == 0) {
                    goto midi_note_off;
                }
                return block_len;
            }
        }

        if (mid_dec_obj->track_set == 0) {
            if (MAINTRACK_USE_CHN) {
                mid_dec_obj->tracknV = chn;
            } else {
                mid_dec_obj->tracknV = trkNum;
            }
            mid_dec_obj->track_set = 1;
        }

        int maintrackval = trkNum;

        if (MAINTRACK_USE_CHN) {
            maintrackval = chn;
        }
        if (mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) {
            if ((mid_dec_obj->okon_mode == CMD_MIDI_OKON_MODE_2) && (maintrackval != mid_dec_obj->tracknV)) {
                mid_dec_obj->wdt_count = mid_dec_obj->wdt_count + 1;
                return block_len;
            }
        }

        if (val2 == 0) {
            goto midi_note_off;
        }

        {
            int main_track_flag = 0;
            int i, player_OK = 0;
            u16 key_val_t;
            u16 instr_val_t;

            for (i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
                if ((mid_dec_obj->midi_player_on & BIT(i))) {
                    WaveInfo_t *twaveinfo = &mid_dec_obj->midi_players[i].wave_info;
                    if ((mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) && (mid_dec_obj->okon_mode == CMD_MIDI_OKON_MODE_2)) {
                        int trackOrchn = twaveinfo->trk_v;
                        if (MAINTRACK_USE_CHN) {
                            trackOrchn = twaveinfo->chnl;
                        }
                        if ((mid_dec_obj->main_interrupt_enable) && (trackOrchn == maintrackval)) {
                            control_stop(mid_dec_obj, twaveinfo, chn);
                        }
                    } else if ((twaveinfo->chnl == chn) && (twaveinfo->noKey == val1) && (twaveinfo->trk_v == trkNum)) {
                        control_stop(mid_dec_obj, twaveinfo, chn);
                    }
                } else {
                    if (!player_OK) {
                        mid_dec_obj->player_index = i;
                        player_OK = 1;
                    }
                }
            }

            {
                key_val_t = mid_dec_obj->instr_key_map[chn][val1];
                instr_val_t = mid_dec_obj->instr_start_index[chn] + mid_dec_obj->instr_key_map[chn][val1] - 1;
                if ((mid_dec_obj->ins_set) && (mid_dec_obj->track_set)) {

                    if (mid_dec_obj->tracknV == maintrackval) {
                        key_val_t = mid_dec_obj->instr_key_map_set[val1];
                        instr_val_t = mid_dec_obj->instr_start_index_set + key_val_t - 1;

                        main_track_flag = 1;
                    }
                }

                if ((mid_dec_obj->ins_set) && (mid_dec_obj->replace_mode == 1)) {
                    key_val_t = mid_dec_obj->instr_key_map_set[val1];
                    instr_val_t = mid_dec_obj->instr_start_index_set + key_val_t - 1;

                    main_track_flag = 1;
                }

                if (mid_dec_obj->instr_key_chnl_enable[chn]) {
                    key_val_t = mid_dec_obj->instr_key_chnl_map[chn][val1];
                    instr_val_t = mid_dec_obj->instr_start_chnl_index[chn] + key_val_t - 1;
                }


                if (key_val_t != 0) {
                    unsigned char *zone_ptr;

                    zone_ptr = mid_dec_obj->spi_zone_start + (instr_val_t) * sizeof(Zone_t);
                    memcpy(&(mid_dec_obj->zone), zone_ptr, sizeof(Zone_t));
                }
            }


            if (player_OK == 0) {
                for (i = mid_dec_obj->MAX_PLAYER_CNTt - 1; i >= 0; i--) {
                    WaveInfo_t *waveInfo_tmp = &mid_dec_obj->midi_players[i].wave_info;
                    if (waveInfo_tmp->ison == SEQ_AE_REL) {
                        player_OK = 1;
                        mid_dec_obj->player_index = i;

                        if (mid_dec_obj->melody_stop) {
                            if ((mid_dec_obj->melody_stop_trig.main_chn_enable) && (mid_dec_obj->midi_main_player_on & BIT(i))) {
                                mid_dec_obj->midi_main_player_on &= ~BIT(i);
                                mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, mid_dec_obj->main_key[i], waveInfo_tmp->chnl);
                            } else if (mid_dec_obj->melody_stop_trig.main_chn_enable == 0) {
                                if (mid_dec_obj->midi_main_player_on & BIT(i)) {
                                    mid_dec_obj->midi_main_player_on &= ~BIT(i);
                                }
                                mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo_tmp->noKey, waveInfo_tmp->chnl);
                            }
                        }
                        break;
                    }
                }
            }

            if (player_OK == 1) {
                if (!mid_dec_obj->mute_enable) {
                    control_note_on(mid_dec_obj, chn, trkNum, val1, val2);
                    if ((mid_dec_obj->note_on == 2) && (mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1)) {
                        if (mid_dec_obj->key_mode == CMD_MIDI_MELODY_KEY_1) {
                            if (mid_dec_obj->melody_on) {
                                if (maintrackval == mid_dec_obj->tracknV) {
                                    mid_dec_obj->melody_trig.melody_trigger(mid_dec_obj->melody_trig.priv, (val1 & 0x7f), (val2 & 0x7f));
                                }
                            }
                        }
                        break;
                    }

                    if (main_track_flag) {
                        int tmpvol;
                        waveInfo = &mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info;
                        tmpvol = ((int)waveInfo->vol_atten * mid_dec_obj->set_chvol_in) >> 10;
                        if (tmpvol > 65535) {
                            tmpvol = 65535;
                        }
                        waveInfo->vol_atten = tmpvol;
                    }

                }



                if ((mid_dec_obj->crtl_mode != CMD_MIDI_CTRL_MODE_1) || (mid_dec_obj->key_mode != CMD_MIDI_MELODY_KEY_1)) {
                    if (mid_dec_obj->melody_on) {
                        if (maintrackval == mid_dec_obj->tracknV) {
                            mid_dec_obj->melody_trig.melody_trigger(mid_dec_obj->melody_trig.priv, (val1 & 0x7f), (val2 & 0x7f));
                        }
                    }
                }

                if (mid_dec_obj->melody_stop) {
                    if (maintrackval == mid_dec_obj->tracknV) {
                        mid_dec_obj->midi_main_player_on |= BIT(mid_dec_obj->player_index);
                        mid_dec_obj->main_key[mid_dec_obj->player_index] = val1;
                    }
                }
            }
        }
        break;


    case MIDI_NOTEOFF: {
        if (mid_dec_obj->saved_rd) {
            val1 = mid_dec_obj->value_rd;
            mid_dec_obj->saved_rd = 0;
            block_len += 1;
        } else {
            val1 = midi_inputdata(mid_dec_obj, 1);
        }
        val2 = midi_inputdata(mid_dec_obj, 1);
        block_len -= 2;

        int maintrackval = trkNum;

        if (MAINTRACK_USE_CHN) {
            maintrackval = chn;
        }
        if (mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) {
            if ((mid_dec_obj->okon_mode == CMD_MIDI_OKON_MODE_2) && (maintrackval != mid_dec_obj->tracknV)) {
                return block_len;
            }
        }

midi_note_off: {
            WaveInfo_t *temp_t;
            int i;

            for (i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
                if (mid_dec_obj->midi_player_on & BIT(i)) {
                    temp_t = &(mid_dec_obj->midi_players[i].wave_info);
                    if ((temp_t->chnl == chn) && (temp_t->noKey == val1) && (temp_t->trk_v == trkNum)) {
                        if (!control_stop(mid_dec_obj, temp_t, chn)) {
                            return block_len;
                        }
                    }
                }
            }
        }

    }
    break;

    case MIDI_KEYAT: {
        if (mid_dec_obj->saved_rd) {
            val1 = mid_dec_obj->value_rd;
            mid_dec_obj->saved_rd = 0;
            block_len += 1;
        } else {
            val1 = midi_inputdata(mid_dec_obj, 1);
        }
        val2 = midi_inputdata(mid_dec_obj, 1);
        block_len -= 2;

    }
    break;
    case MIDI_CTLCHG: {
        if (mid_dec_obj->saved_rd) {
            val1 = mid_dec_obj->value_rd;
            mid_dec_obj->saved_rd = 0;
            block_len += 1;
        } else {
            val1 = midi_inputdata(mid_dec_obj, 1);
        }
        val2 = midi_inputdata(mid_dec_obj, 1);
        block_len -= 2;

        control_message_handle(mid_dec_obj, chn, val1, val2);

    }
    break;
    case MIDI_PWCHG: {
        if (mid_dec_obj->saved_rd) {
            val1 = mid_dec_obj->value_rd;
            mid_dec_obj->saved_rd = 0;
            block_len += 1;
        } else {
            val1 = midi_inputdata(mid_dec_obj, 1);
        }

        val2 = midi_inputdata(mid_dec_obj, 1);
        block_len -= 2;

        {
            u8 tmp_i;
            u16 shift_v = midi_inputdata(mid_dec_obj, 2);
            mid_dec_obj->pitchBend_v[chn] = shift_v;


            for (tmp_i = 0; tmp_i < mid_dec_obj->MAX_PLAYER_CNTt; tmp_i++) {
                if (mid_dec_obj->midi_player_on & BIT(tmp_i)) {
                    if (mid_dec_obj->midi_players[tmp_i].wave_info.chnl == chn) {
                        waveInfo = &mid_dec_obj->midi_players[tmp_i].wave_info;
                        waveInfo->voice.indexIncr = (waveInfo->initAtten * shift_v) >> 8;
                    }
                }
            }
        }
    }
    break;
    case MIDI_PRGCHG: { //smf->ProgChange(chan, val1, trkNum);
        if (mid_dec_obj->saved_rd) {
            val1 = mid_dec_obj->value_rd;
            mid_dec_obj->saved_rd = 0;
            block_len += 1;
        } else {
            val1 = midi_inputdata(mid_dec_obj, 1);
        }
        val2 = 0;
        block_len--;

        {
            int tbank = val1;
            if (chn == 9) {
                tbank = 128 + val1;
            }

            if (mid_dec_obj->instr_on[chn] != tbank) {
                u16 zone_key_v;
                mid_dec_obj->instr_on[chn] = tbank;
                zone_key_v = mid_dec_obj->midi_spi_pos[2 + tbank];
                mid_dec_obj->instr_start_index[chn] = mid_dec_obj->instr_map[zone_key_v];
                mid_dec_obj->instr_key_map[chn] = (char *)(&mid_dec_obj->spi_key_start[zone_key_v * 128]);
            }
        }
    }
    break;
    case MIDI_CHNAT: { //smf->ChannelAfterTouch(chan, val1, trkNum);
        if (mid_dec_obj->saved_rd) {
            val1 = mid_dec_obj->value_rd;
            mid_dec_obj->saved_rd = 0;
            block_len += 1;
        } else {
            val1 = midi_inputdata(mid_dec_obj, 1);
        }
        val2 = 0;
        block_len--;
    }
    break;
    default:
        break;
    }
    return block_len;
}

void player_control_note_on(MIDI_DECODE_VAR *mid_dec_obj, short chn, short trackn, int val1, int val2)
{
    WaveInfo_t *waveInfo;
    long long tmp64;

    mid_dec_obj->peekOK = 0;
    mid_dec_obj->midi_player_on &= ~BIT(mid_dec_obj->player_index);

    int maintrackval = trackn;

    if (MAINTRACK_USE_CHN) {
        maintrackval = chn;
    }

    mid_dec_obj->keydown_cnt++;


    mid_dec_obj->midi_channel_on |= BIT(chn);

    waveInfo = &mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info;
    memset(waveInfo, 0, sizeof(WaveInfo_t));

    if ((mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_W2S) && (maintrackval == mid_dec_obj->tracknV)) {

        MIDI_W2S_INFO *w2s_obj = (MIDI_W2S_INFO *)(&mid_dec_obj->w2s_obj);

        mid_dec_obj->zone.sampleMap = w2s_obj->data_pos[w2s_obj->word_cnti];
        mid_dec_obj->zone.tableEnd = w2s_obj->data_len[w2s_obj->word_cnti];

        mid_dec_obj->zone.vol_dec = 65527;
        mid_dec_obj->zone.vol_rel = 0;
        mid_dec_obj->zone.vol_incr = 3140;
        mid_dec_obj->zone.cents = 4096;
        mid_dec_obj->zone.mode = 0;
        mid_dec_obj->zone.initAtten = 65535;
        mid_dec_obj->zone.loopStart = 0;
        mid_dec_obj->zone.vol_hold = 16384;

        w2s_obj->word_cnti++;
        if (w2s_obj->word_cnti >= w2s_obj->word_cnt) {
            w2s_obj->word_cnti = 0;
        }

        if (mid_dec_obj->key_set == 0) {
            mid_dec_obj->zone.keyNum = w2s_obj->key_diff + val1;
            if (mid_dec_obj->zone.keyNum < 0) {
                mid_dec_obj->zone.keyNum = val1;
            } else if (mid_dec_obj->zone.keyNum > 127) {
                mid_dec_obj->zone.keyNum = val1;
            }
            mid_dec_obj->key_set = mid_dec_obj->zone.keyNum;
        } else {
            mid_dec_obj->zone.keyNum = mid_dec_obj->key_set;
        }

    }

    waveInfo->trk_v = trackn;

    waveInfo->noKey = val1;
    waveInfo->noVel = val2;
    waveInfo->chnl = chn;

    if (mid_dec_obj->pitch_enable) {
        val1 = val1 + mid_dec_obj->semitone_ctrl.key_diff[chn];
        if (val1 > 127) {
            val1 = 127;
        }
        if (val1 < 0) {
            val1 = 0;
        }
    }

    waveInfo->sostenuto = (mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_SOS_ON_CC] > MIDI_SwitchLevel_CC);

    waveInfo->initAtten = mid_dec_obj->zone.initAtten;
    waveInfo->initAtten = (waveInfo->initAtten * (int)mkey_vol_tab[val2]) >> 8;

    {
        s8 test_key_i;
        u32 shift_v;
        u8 shift_i;
        test_key_i = val1 - mid_dec_obj->zone.keyNum;

        shift_i = 13;

        if (test_key_i < 0) {
            while (test_key_i < 0) {
                test_key_i += 12;
                shift_i++;
            }
            shift_v = (pow2VALTab_12[(int)test_key_i]);
        } else {
            while (test_key_i >= 12) {
                test_key_i = test_key_i - 12;
                shift_i--;
            }
            shift_v = (pow2VALTab_12[(int)test_key_i]);
        }

        shift_v = (shift_v * inv_smpl_rate_tab[mid_dec_obj->sample_rate]) >> 13;

        waveInfo->voice.indexIncr = (unsigned int)shift_v * (unsigned int)mid_dec_obj->zone.cents >> shift_i;
    }

    waveInfo->sampleMap = mid_dec_obj->zone.sampleMap;

    if (mid_dec_obj->zone.vol_incr == 16385) {
        waveInfo->vol_dec_now = 65536;
        waveInfo->vol_rel = 65536;
    } else {
        waveInfo->vol_dec_now = mid_dec_obj->zone.vol_dec;
        waveInfo->vol_rel = mid_dec_obj->zone.vol_rel;

        {
            int tmpsr = mid_dec_obj->sample_rate;
            while (tmpsr >= 3) {
                tmpsr = tmpsr - 3;
                MULRS(waveInfo->vol_dec_now, tmp64, waveInfo->vol_dec_now, waveInfo->vol_dec_now, 16);
                MULRS(waveInfo->vol_rel, tmp64, waveInfo->vol_rel, waveInfo->vol_rel, 16);
            }

            if (tmpsr) {
                MULRS(waveInfo->vol_dec_now, tmp64, waveInfo->vol_dec_now, 65535, 16);
                MULRS(waveInfo->vol_rel, tmp64, waveInfo->vol_rel, 65535, 16);
            }

            MULRS(waveInfo->vol_dec_now, tmp64, waveInfo->vol_dec_now, mid_dec_obj->decay_speed[chn], 15);
            MULRS(waveInfo->vol_rel, tmp64, waveInfo->vol_rel, mid_dec_obj->decay_speed[chn], 15);
        }
    }
    waveInfo->vol_dec_now_bit = 16;
    waveInfo->vol_hold = mid_dec_obj->zone.vol_hold;
    waveInfo->vol_hold_flag = mid_dec_obj->zone.vol_hold_flag;

    waveInfo->tableEnd = mid_dec_obj->zone.tableEnd;
    waveInfo->loopLen = mid_dec_obj->zone.loopLen;
    waveInfo->loopStart = mid_dec_obj->zone.loopStart;
    waveInfo->loopEnd = waveInfo->loopStart + waveInfo->loopLen;
    waveInfo->mode = mid_dec_obj->zone.mode;

    waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap]);
    if ((mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_W2S) && (maintrackval == mid_dec_obj->tracknV)) {
        waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_posbk[mid_dec_obj->zone.sampleMap]);
        waveInfo->prognum = 0;
    } else {
        if (mid_dec_obj->compressIN) {
            waveInfo->prognum = waveInfo->voice.wavebuf[0];
            waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap + 1]);

            if (waveInfo->prognum == PCM_KIND) {
                waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap + 2]);
            } else if (waveInfo->prognum >= PCM_KIND_DOWN) {
                unsigned char *sptr = (unsigned char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap + 2]);
                short loopstart_n = sptr[1] << 8 | sptr[0];
                short loopend_n = sptr[3] << 8 | sptr[2];
                if (waveInfo->mode) {
                    waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap + 6]);
                    sr_renew_waveinfo(waveInfo, loopstart_n, loopend_n);
                } else {
                    waveInfo->voice.wavebuf = (char *)(&mid_dec_obj->midi_spi_pos[mid_dec_obj->zone.sampleMap + 4]);
                    sr_renew_waveinfo(waveInfo, loopstart_n, loopend_n);
                }
                waveInfo->prognum = waveInfo->prognum - PCM_KIND_DOWN;
            }
        }
    }
    waveInfo->voice.index = 0;

    control_pan(mid_dec_obj, waveInfo, chn);
    control_volume(mid_dec_obj, waveInfo, chn);

    waveInfo->note_on_off_dest = 100;

    if (mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) {
        mid_dec_obj->peekOK = 1;//midi_peek_value(mid_dec_obj,val1);
    }

    waveInfo->voice.isend = 0;
    waveInfo->pendingStop = 0;
    waveInfo->ison = SEQ_AE_ON;
    waveInfo->flags = SEQ_AE_TM;

    waveInfo->attack_incr = (mid_dec_obj->zone.vol_incr << 16);             //�����ٶ�

    if (mid_dec_obj->zone.exclNote) {
        waveInfo->exclIndex = (chn << 9) | (mid_dec_obj->zone.exclNote & 0xf);
        waveInfo->exclOn = 1;
    }

    waveInfo->initAtten = waveInfo->voice.indexIncr;
    waveInfo->voice.indexIncr = (waveInfo->initAtten * mid_dec_obj->pitchBend_v[chn]) >> 8;
    if (mid_dec_obj->zone.have_array) {
        ENV *env_info = &waveInfo->env_info;
        env_info->env_use = 1;
        u32 tz = mid_dec_obj->zone.vol_hold_flag;
        tz = (tz << 16) | mid_dec_obj->zone.vol_hold;
        env_info->env_infov = &mid_dec_obj->midi_spi_pos[tz];
        env_info->vol_f = 1073741824;
        waveInfo->vol_hold_flag = 0;

        int valget = 0;

        unsigned char *env_infov = env_info->env_infov;
        {
            short tmpshift = 7;
            u8 readval = env_infov[env_info->envi++];
            valget = (readval & 0x7f);
            while (!(readval & 0x80)) {
                readval = env_infov[env_info->envi++];
                valget = ((readval & 0x7f) << tmpshift) | valget;
                tmpshift += 7;
            }
        }

        env_info->env_points = valget;   // ��ȡ�ڵ���

        if (env_info->env_points > 0) {
            valget = 0;
            int doPoints = smpl_rate_tab[mid_dec_obj->sample_rate] * 5 / 1000;

            u8 readval = env_infov[env_info->envi++];
            valget = (readval & 0x7f);

            short tmpshift = 7;
            while (!(readval & 0x80)) {
                readval = env_infov[env_info->envi++];
                valget = ((readval & 0x7f) << tmpshift) | valget;
                tmpshift += 7;
            }
            int stepcnt = valget;

            readval = env_infov[env_info->envi++];
            valget = env_infov[env_info->envi++];
            valget = (valget << 8) | readval;
            short tmpste = (short)(valget & 0xffff);

            int tmpste1 = tmpste;
            env_info->stepval = (tmpste1 << 15) / doPoints;
            env_info->stepcnt = stepcnt * doPoints;

            env_info->env_points--;
        }

    } else {
        waveInfo->env_info.env_use = 0;
    }
    waveInfo->active_off_flag = 1;
    mid_dec_obj->midi_player_on |= BIT(mid_dec_obj->player_index);

}


int midi_player_Message(MIDI_DECODE_VAR *mid_dec_obj, short trkNum, unsigned short msg, u8 val1, u8 val2, u16 time)
{
    unsigned char chn = msg & MIDI_CHNMSK;
    unsigned char mevent = msg & MIDI_EVTMSK;
    WaveInfo_t *waveInfo;

    switch (mevent) {
    case MIDI_NOTEON:
        if (val2 == 0) {
            goto midi_note_off;
        }
        {
            int i, player_OK = 0;
            int t_instr_res;

            for (i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
                if ((mid_dec_obj->midi_player_on & BIT(i))) {
                    WaveInfo_t *twaveinfo = &mid_dec_obj->midi_players[i].wave_info;
                    if ((twaveinfo->chnl == chn) && (twaveinfo->noKey == val1)) {
                        control_stop(mid_dec_obj, twaveinfo, chn);
                    }

                } else {
                    if (!player_OK) {
                        mid_dec_obj->player_index = i;
                        player_OK = 1;
                    }
                }
            }

            t_instr_res = mid_dec_obj->instr_key_map[chn][val1];
            if (t_instr_res == 0) {
                unsigned char *t_instr_k = (unsigned char *)mid_dec_obj->instr_key_map[chn];
                int t_instr_i;

                for (t_instr_i = val1 - 1; t_instr_i >= 0; t_instr_i--) {
                    if (t_instr_k[t_instr_i]) {
                        t_instr_res = t_instr_k[t_instr_i];
                        break;
                    }
                }
                if (!t_instr_res) {
                    for (t_instr_i = val1; t_instr_i < 128; t_instr_i++) {
                        if (t_instr_res < t_instr_k[t_instr_i]) {
                            t_instr_res = t_instr_k[t_instr_i];
                            break;
                        }
                    }
                }

            }
            if (t_instr_res != 0) {
                unsigned char *zone_ptr;

                if (mid_dec_obj->crtl_mode != CMD_MIDI_CTRL_MODE_W2S) {
                    zone_ptr = mid_dec_obj->spi_zone_start + (mid_dec_obj->instr_start_index[chn] + t_instr_res - 1) * sizeof(Zone_t);
                    memcpy(&(mid_dec_obj->zone), zone_ptr, sizeof(Zone_t));
                } else {

                }
            }


            if (player_OK == 0) {
                for (i = mid_dec_obj->MAX_PLAYER_CNTt - 1; i >= 0; i--) {
                    if (mid_dec_obj->midi_players[i].wave_info.ison == SEQ_AE_REL) {
                        player_OK = 1;
                        mid_dec_obj->player_index = i;
                        break;
                    }
                }
            }

            if (player_OK == 1) {
                control_note_on(mid_dec_obj, chn, trkNum, val1, val2);

                if (mid_dec_obj->melody_on) {
                    mid_dec_obj->melody_trig.melody_trigger(mid_dec_obj->melody_trig.priv, (val1 & 0x7f), (val2 & 0x7f));
                }
                if (mid_dec_obj->melody_stop) {
                    mid_dec_obj->main_key[mid_dec_obj->player_index] = val1;
                }
            }
        }
        break;


    case MIDI_NOTEOFF: {
midi_note_off: {
            WaveInfo_t *temp_t;
            int i;
            for (i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
                if (mid_dec_obj->midi_player_on & BIT(i)) {
                    temp_t = &(mid_dec_obj->midi_players[i].wave_info);
                    if ((temp_t->chnl == chn) && (temp_t->noKey == val1)) {
                        if (!control_stop(mid_dec_obj, temp_t, chn)) {
                            return 0;
                        }
                        if (time) {
                            SetKeyDecay(temp_t, time * smpl_rate_tab[mid_dec_obj->sample_rate] / 1000);
                        } else {
                            if (NOTE_OFF_TRIGGER) {
                                temp_t->active_off_flag = 0;
                            }
                        }
                    }
                }
            }
        }

    }
    break;
    case MIDI_CTLCHG: {
        control_message_handle(mid_dec_obj, chn, val1, val2);

    }
    break;
    case MIDI_PWCHG: {
        u16 shift_v;
        u8 tmp_i;

        shift_v = val1 | (val2 << 8);
        mid_dec_obj->pitchBend_v[chn] = shift_v;
        for (tmp_i = 0; tmp_i < mid_dec_obj->MAX_PLAYER_CNTt; tmp_i++) {
            if (mid_dec_obj->midi_player_on & BIT(tmp_i)) {
                if (mid_dec_obj->midi_players[tmp_i].wave_info.chnl == chn) {
                    waveInfo = &mid_dec_obj->midi_players[tmp_i].wave_info;
                    waveInfo->voice.indexIncr = (waveInfo->initAtten * shift_v) >> 8;
                }
            }
        }
    }
    break;

    case MIDI_PRGCHG: {
        u16 zone_key_v = 0;
        int tbank = val1;
        if (chn == 9) {
            tbank = 128 + val1;
        }
        if (mid_dec_obj->instr_on[chn] != tbank) {
            mid_dec_obj->instr_on[chn] = tbank;
            zone_key_v = mid_dec_obj->midi_spi_pos[2 + tbank];
            mid_dec_obj->instr_start_index[chn] = (unsigned short)mid_dec_obj->instr_map[zone_key_v];
            mid_dec_obj->instr_key_map[chn] = (char *)(&mid_dec_obj->spi_key_start[zone_key_v * 128]);
        }

    }
    break;

    case MIDI_PLAY_VELVIB: {
        unsigned short *modulatep = (unsigned short *)mid_dec_obj->track_len;
        WaveInfo_t *temp_t;
        int i;
        for (i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
            if (mid_dec_obj->midi_player_on & BIT(i)) {
                temp_t = &(mid_dec_obj->midi_players[i].wave_info);
                if ((temp_t->chnl == chn) && (temp_t->noKey == val1)) {
                    temp_t->modulate_vel = val2;
                    modulatep[i] = 8192;
                }
            }
        }
    }
    break;

    default:
        break;
    }
    return 0;
}





