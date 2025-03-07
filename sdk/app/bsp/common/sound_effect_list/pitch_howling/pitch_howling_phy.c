#include "typedef.h"
#include "howling_pitchshifter_api.h"
#include "decoder_mge.h"
#include "config.h"
#include "sound_effect_api.h"
#include "my_malloc.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

typedef struct _HOWLING_HDL {
    EFFECT_OBJ obj;//必须在第一个
    sound_in_obj si;
    HOWL_PS_IO_CONTEXT io;
    HOWLING_PITCHSHIFT_PARM parm;
} HOWLING_HDL;

HOWLING_HDL howling_hdl_save AT(.howling_data);


static int howing_run(void *hld, short *inbuf, int len)
{
    HOWLING_PITCHSHIFT_FUNC_API *ops;
    int res = 0;
    sound_in_obj *p_si = hld;
    ops = (HOWLING_PITCHSHIFT_FUNC_API *)p_si->ops;
    res = ops->run(p_si->p_dbuf, inbuf, len);
    return res;
}


void howling_parm_debug(HOWLING_HDL *howling_hdl)
{
    log_info("howling->parm.ps_parm %d\n", howling_hdl->parm.ps_parm);
    log_info("howling->parm.fe_parm %d\n", howling_hdl->parm.fs_parm);
    log_info("howling->parm.effect_v %d\n", howling_hdl->parm.effect_v);
}


void *howling_phy(void *obuf, void *dbuf, HOWLING_PITCHSHIFT_PARM *parm, u32 sr, void **ppsound)
{
    u32 buf_len, i;
    HOWLING_PITCHSHIFT_FUNC_API *ops;
    EFFECT_OBJ *howling_obj;
    sound_in_obj *howling_si;
    if (!parm) {
        log_error("howling parm NULL\n");
        return NULL;
    }
    ops = (HOWLING_PITCHSHIFT_FUNC_API *)get_howling_ps_func_api(); //接口获取

    memset(&howling_hdl_save, 0x0, sizeof(HOWLING_HDL));
    HOWLING_HDL *howling_hdl = &howling_hdl_save;
    if (howling_hdl && parm) {
        memcpy(&howling_hdl->parm, parm, sizeof(HOWLING_PITCHSHIFT_PARM));
        howling_parm_debug(howling_hdl);
    }

    unsigned int *howling_hdl_ptr = (unsigned int *)dbuf;
    log_info("howling_hdl_ptr %x\n", howling_hdl_ptr);
    /* if (sizeof(dbuf) < buf_len) { */
    /*     log_error("howing work buf less %d, need len %d", sizeof(dbuf), buf_len); */
    /*     return NULL; */
    /* } */
    howling_obj = &howling_hdl->obj;
    howling_hdl->io.priv = &howling_obj->sound;
    howling_hdl->io.output = sound_output;

    /*************************************************/
    howling_si =  &howling_hdl->si;
    howling_si->ops = ops;
    howling_si->p_dbuf = howling_hdl_ptr;
    /*************************************************/
    howling_obj->p_si = howling_si;
    howling_obj->run = howing_run;
    howling_obj->sound.p_obuf = obuf;
    *ppsound = &howling_obj->sound;

    ops->open(howling_hdl_ptr, sr, &howling_hdl->parm, &howling_hdl->io);
    return howling_obj;
}

void update_howling_parm_fs(void *dbuf, u32 sr, s16 new_fs)
{

    HOWLING_PITCHSHIFT_FUNC_API *ops;
    ops = (HOWLING_PITCHSHIFT_FUNC_API *)get_howling_ps_func_api(); //接口获取

    HOWLING_HDL *howling_hdl = &howling_hdl_save;
    howling_hdl->parm.fs_parm = new_fs;

    unsigned int *howling_hdl_ptr = (unsigned int *)dbuf;
    ops->open(howling_hdl_ptr, sr, &howling_hdl->parm, NULL);
}


