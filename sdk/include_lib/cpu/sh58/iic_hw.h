#ifndef __IIC_HW_H__
#define __IIC_HW_H__

#define CONFIG_ENABLE_IIC_OS_SEM  0

#define MAX_HW_IIC_NUM                  1
#define P11_HW_IIC_NUM                  0 //p11 iic使能,及锁使能
// typedef enum {
//     HW_IIC_0,
//     // HW_IIC_1,
// } hw_iic_dev;

enum {
    HW_IIC_0,
    // HW_IIC_1,
#if defined(P11_HW_IIC_NUM)&&P11_HW_IIC_NUM
    HW_P11_IIC_0,//p11 init,pb
#endif

};


#if CONFIG_ENABLE_IIC_OS_SEM
typedef OS_SEM IIC_Semaphore ;
static inline int IIC_OSSemCreate(IIC_Semaphore *sem, int count)
{
    return os_sem_create(sem, count);
}
static inline int IIC_OSSemPost(IIC_Semaphore *sem)
{
    return os_sem_post(sem);
}
static inline int IIC_OSSemPend(IIC_Semaphore *sem, int timeout)
{
    return os_sem_pend(sem, timeout);
}
static inline int IIC_OSSemSet(IIC_Semaphore *sem, u16 count)
{
    return os_sem_set(sem, count);
}
static inline int IIC_OSSemClose(IIC_Semaphore *sem, int block)
{
    return os_sem_del(sem, block);
}
static inline void iic_sleep()
{
    os_time_dly(1);
}

typedef OS_MUTEX IIC_mutex;
static inline int IIC_OSMutexCreate(IIC_mutex *mutex)
{
    return os_mutex_create(mutex);
}
static inline int IIC_OSMutexPost(IIC_mutex *mutex)
{
    return os_mutex_post(mutex);
}
static inline int IIC_OSMutexPend(IIC_mutex *mutex, u32 timeout)
{
    return os_mutex_pend(mutex, timeout);
}
static inline int IIC_OSMutexClose(IIC_mutex *mutex, u32 block)
{
    return os_mutex_del(mutex, block);
}

#else
#include "jiffies.h"

static inline u32 iic_get_jiffies(void)
{
#if 1
    return jiffies;
#endif
#if 0
    return Jtime_updata_jiffies();
#endif
}
static inline u32 iic_msecs_to_jiffies(u32 msecs)
{
    if (msecs >= 10) {
        msecs /= 10;
    } else if (msecs) {
        msecs = 1;
    }
    return msecs;
}

typedef volatile u8 IIC_Semaphore;
extern void wdt_clear();
static inline int IIC_OSSemCreate(IIC_Semaphore *sem, int count)
{
    *sem = count;
    return 0;
}
static inline int IIC_OSSemPost(IIC_Semaphore *sem)
{
    (*sem)++;
    return 0;
}
static inline int IIC_OSSemPend(IIC_Semaphore *sem, int timeout)
{
    u32 timeout_tmp = iic_msecs_to_jiffies(timeout);
    u32 _timeout = timeout_tmp + iic_get_jiffies();
    while (1) {
        if (*sem) {
            (*sem) --;
            break;
        }
        if (timeout < 0) {
            return -2;
        }
        if ((timeout != 0) && time_before(_timeout, iic_get_jiffies())) {
            return -1;
        }
        wdt_clear();
    }
    return 0;
}
static inline int IIC_OSSemSet(IIC_Semaphore *sem, u16 count)
{
    *sem = count;
    return 0;
}
static inline int IIC_OSSemClose(IIC_Semaphore *sem, int block)
{

    return 0;
}
static inline void iic_sleep()
{
    wdt_clear();
}

typedef volatile u8 IIC_mutex;
static inline int IIC_OSMutexCreate(IIC_mutex *mutex)
{
    *mutex = 1;
    return 0;
}
static inline int IIC_OSMutexPost(IIC_mutex *mutex)
{
    (*mutex) = 1;
    return 0;
}
static inline int IIC_OSMutexPend(IIC_mutex *mutex, u32 timeout)
{
    timeout = iic_msecs_to_jiffies(timeout);
    u32 _timeout = timeout + iic_get_jiffies();
    while (1) {
        if (*mutex) {
            (*mutex) = 0;
            break;
        }
        if ((timeout != 0) && time_before(_timeout, iic_get_jiffies())) {
            return -1;
        }
        wdt_clear();
        asm("idle");
    }
    return 0;
}
static inline int IIC_OSMutexClose(IIC_mutex *mutex, u32 block)
{
    return 0;
}
#endif

#endif

