/*****************************************************************************
 *  Copyright (C) RJ Brands 2021. All rights reserved.
 *****************************************************************************/

/**
 * @file scheduler.h
 * @brief
 *
 */

#ifndef __SCHEDULER_H
#define __SCHEDULER_H

/*****************************************************************************
 *	Public Includes
 *****************************************************************************/
#include <stdbool.h>
#include <stdint.h>
/*****************************************************************************
 *	Public Defines & Macros
 *****************************************************************************/
#define SCHEDULER_PERIOD_NONE  0
#define SCHEDULER_PERIOD_1MS   1
#define SCHEDULER_PERIOD_5MS   5
#define SCHEDULER_PERIOD_10MS  10
#define SCHEDULER_PERIOD_50MS  50
#define SCHEDULER_PERIOD_100MS 100
#define SCHEDULER_PERIOD_200MS 200
#define SCHEDULER_PERIOD_500MS 500
#define SCHEDULER_PERIOD_750MS 750
#define SCHEDULER_PERIOD_1S    1000
#define SCHEDULER_PERIOD_2S    2000
#define SCHEDULER_PERIOD_10S   10000

/*****************************************************************************
 *	Public Typedefs & Enumerations
 *****************************************************************************/
typedef enum
{
    SCHEDULER_OK = 0,
    SCHEDULER_ERROR,
} scheduler_err_t;

typedef struct
{
    void (*task_init)(void);
    void (*task_run)(void);
    uint16_t time_ms;
} scheduler_task_config_t;

typedef struct
{
    bool                    semaphore;
    bool                    halt;
    uint16_t                reload_ticks;
    volatile uint16_t       tick_counter;
    scheduler_task_config_t config;
    bool                    registered;
} scheduler_task_t;

typedef scheduler_task_t* scheduler_task_handle_t;

typedef struct
{
    scheduler_task_handle_t* task_table;
    uint32_t                 task_table_size;
    uint8_t                  hal_systimer_ch;
    uint32_t                 millisec_per_tick;
} scheduler_config_t;
/*****************************************************************************
 *	Public Function Prototypes
 *****************************************************************************/
scheduler_err_t scheduler_init(scheduler_config_t* config);
void            scheduler_process(void);
scheduler_err_t scheduler_register_task(scheduler_task_handle_t task_handle, scheduler_task_config_t* config);
void            scheduler_set_semaphore(scheduler_task_handle_t task_handle);
void            scheduler_halt_periodic_task(scheduler_task_handle_t task_handle);
void            scheduler_restart_periodic_task(scheduler_task_handle_t task_handle);
void            scheduler_set_task_interval(scheduler_task_handle_t task_handle, uint16_t time_ms);
uint64_t        scheduler_get_tick_count(void);
uint64_t        scheduler_get_time_ms(void);
uint64_t        scheduler_get_time_lapse_ms(uint64_t from_time_ms);
void            scheduler_delay_ms(uint64_t delay_time_ms);
#endif /* __SCHEDULER_H */
