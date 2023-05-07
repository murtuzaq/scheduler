/**
 * @file scheduler.c
 *
 */

/*****************************************************************************
 *	Private Includes
 *****************************************************************************/
#include "scheduler.h"
#include "hal_systimer.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
/*****************************************************************************
 *	Private External References
 *****************************************************************************/

/*****************************************************************************
 *	Private Defines & Macros
 *****************************************************************************/

/*****************************************************************************
 *	Private Typedefs & Enumerations
 *****************************************************************************/
struct scheduler_t
{
    volatile uint64_t     ticks;
    struct hal_systimer_t hal_systimer;          /*< Hardware abstraction layer timer used by the scheduler */
    uint32_t              registered_task_count; /*< Number of registered tasks in the scheduler */
    scheduler_config_t    config;
    bool                  initialized;
};
/*****************************************************************************
 *	Private Variables
 *****************************************************************************/
static struct scheduler_t scheduler;
/*****************************************************************************
 *	Private Function Prototypes
 *****************************************************************************/
static void scheduler_update_count_tick(void* param);
static bool is_task_ready_to_run(scheduler_task_handle_t task_handle);
static void process_task(scheduler_task_handle_t task_handle);
static void count_down_task_timer(scheduler_task_handle_t task_handle);
/*****************************************************************************
 *	Public Functions
 *****************************************************************************/
scheduler_err_t scheduler_init(scheduler_config_t* config)
{
    if (config == NULL)
    {
        return SCHEDULER_ERROR;
    }

    if (config->task_table == NULL)
    {
        return SCHEDULER_ERROR;
    }

    scheduler.ticks = 0;

    struct hal_systimer_config_t timer_config = {
        .channel = config->hal_systimer_ch,
        .cb =
            {
                .isr_cb    = scheduler_update_count_tick,
                .param     = NULL,
                .period_ms = config->millisec_per_tick,
            },
    };

    if (hal_systimer_init(&scheduler.hal_systimer, &timer_config) != HAL_SYSTIMER_OK)
    {
        return SCHEDULER_ERROR;
    }

    scheduler.config = *config;

    scheduler.registered_task_count = 0;

    scheduler.initialized = true;

    return SCHEDULER_OK;
}

void scheduler_process(void)
{
    if (scheduler.initialized == false)
    {
        return;
    }

    for (uint32_t i = 0; i < scheduler.registered_task_count; i++)
    {
        scheduler_task_handle_t task_handle = scheduler.config.task_table[i];
        process_task(task_handle);
    }
}

scheduler_err_t scheduler_register_task(scheduler_task_handle_t task_handle, scheduler_task_config_t* config)
{
    if (task_handle == NULL || config == NULL || config->task_cb == NULL)
    {
        return SCHEDULER_ERROR;
    }

    if (scheduler.initialized == false)
    {
        return SCHEDULER_ERROR;
    }

    if (task_handle->registered == true)
    {
        return SCHEDULER_ERROR;
    }

    if (scheduler.registered_task_count >= scheduler.config.task_table_size)
    {
        return SCHEDULER_ERROR;
    }

    task_handle->config = *config;

    scheduler.config.task_table[scheduler.registered_task_count] = task_handle;
    scheduler.registered_task_count++;

    task_handle->reload_ticks = config->time_ms / scheduler.config.millisec_per_tick;
    task_handle->tick_counter = task_handle->reload_ticks;

    task_handle->registered = true;

    return SCHEDULER_OK;
}

void scheduler_set_semaphore(scheduler_task_handle_t task_handle)
{
    if (task_handle == NULL)
    {
        return;
    }

    task_handle->semaphore = true;

    return;
}

void scheduler_halt_periodic_task(scheduler_task_handle_t task_handle)
{
    if (task_handle == NULL)
    {
        return;
    }

    task_handle->halt = true;

    return;
}

void scheduler_restart_periodic_task(scheduler_task_handle_t task_handle)
{
    if (task_handle == NULL)
    {
        return;
    }

    task_handle->halt         = false;
    task_handle->tick_counter = task_handle->reload_ticks;
    return;
}

void scheduler_set_task_interval(scheduler_task_handle_t task_handle, uint16_t time_ms)
{
    if (task_handle == NULL)
    {
        return;
    }

    task_handle->reload_ticks = time_ms / scheduler.config.millisec_per_tick;
    task_handle->tick_counter = task_handle->reload_ticks;

    return;
}

/*****************************************************************************
 *	Private Functions
 *****************************************************************************/
static void scheduler_update_count_tick(void* param)
{
    for (uint32_t i = 0; i < scheduler.config.task_table_size; i++)
    {
        scheduler_task_t* task = scheduler.config.task_table[i];
        count_down_task_timer(task);
    }

    scheduler.ticks++;
}

static bool is_task_ready_to_run(scheduler_task_handle_t task_handle)
{
    if (task_handle->semaphore == true)
    {
        return true;
    }

    if (task_handle->halt == true)
    {
        return false;
    }

    if (task_handle->tick_counter == 0)
    {
        return true;
    }

    return false;
}

static void process_task(scheduler_task_handle_t task_handle)
{
    if (task_handle == NULL)
    {
        return;
    }

    if (task_handle->config.task_cb == NULL)
    {
        return;
    }

    if (is_task_ready_to_run(task_handle) == false)
    {
        return;
    }

    task_handle->semaphore    = false;
    task_handle->tick_counter = task_handle->reload_ticks;

    void (*task_cb)(void*) = task_handle->config.task_cb;
    void* param            = task_handle->config.param;
    task_cb(param);

    return;
}

static void count_down_task_timer(scheduler_task_handle_t task_handle)
{
    if (task_handle == NULL)
    {
        return;
    }

    if (task_handle->tick_counter <= 0)
    {
        return;
    }

    task_handle->tick_counter--;

    return;
}