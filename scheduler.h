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
    void (*task_cb)(void*);
    void*    param;
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
#endif /* __SCHEDULER_H */
