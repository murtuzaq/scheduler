/**
 * @file scheduler.h
 * @brief This header file defines the public interface for a task scheduler module.
 *
 * The module allows the user to register tasks and specify the time interval at which
 * they should be executed.
 *
 * The module processes these tasks in a time-sliced manner by
 * calling their associated callback functions.
 *
 * The scheduler uses a system timer (e.g., SysTick, RTC) implemented in hal_systimer, to
 * keep track of time intervals.
 *
 * The timer period and number of tasks to be registered are specified in the
 * scheduler_config_t structure when initializing the module.
 *
 * A task is defined by the scheduler_task_t structure, which contains information about the
 * task's callback function, associated parameters, and time interval for execution.
 *
 */
#ifndef __SCHEDULER_H
#define __SCHEDULER_H

/*****************************************************************************
 *	Public Includes
 *****************************************************************************/
#include "hal_systimer.h"
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
    void (*task_cb)(void*); /**< A function pointer to the task callback function. */
    void*    param;         /**< A pointer to the parameter that will be passed to the task callback function. */
    uint16_t time_ms;       /**< The time interval at which the task should be executed, in milliseconds. */
} scheduler_task_config_t;

typedef struct
{
    bool                    semaphore; /**< A flag indicating whether the task should be executed. */
    bool                    halt; /**< A flag indicating whether the periodic execution of the task should be paused. */
    uint16_t                reload_ticks; /**< The number of ticks until the task is executed again. */
    volatile uint16_t       tick_counter; /**< The number of ticks elapsed since the last task execution. */
    scheduler_task_config_t config;       /**< The configuration of the task. */
    bool registered; /**< A flag indicating whether the task has been registered with the scheduler. */
} scheduler_task_t;

typedef scheduler_task_t* scheduler_task_handle_t;

typedef struct
{
    scheduler_task_handle_t* task_table; /**< An array of task handles for the tasks to be executed by the scheduler. */
    uint32_t                 task_table_size;   /**< The size of the task table array. */
    hal_systimer_channel_t   hal_systimer_ch;   /**< The channel of the hardware system timer used by the scheduler. */
    uint32_t                 millisec_per_tick; /**< The duration of a single tick, in milliseconds. */
} scheduler_config_t;

/*****************************************************************************
 *	Public Function Prototypes
 *****************************************************************************/
/**
 * @brief Initializes the scheduler with the given configuration.
 *
 * Initializes the system timer and task table.
 *
 * @param[in] config A pointer to the configuration of the scheduler.
 * @return A scheduler error code.
 */
scheduler_err_t scheduler_init(scheduler_config_t* config);

/**
 * @brief Processes the tasks handled by the scheduler.
 *
 * Executes all registered tasks whose interval time has elapsed. Must be called periodically
 * to allow the scheduler to check if any tasks are due for execution.  When a task's interval has
 * elapsed, the scheduler will call the task's associated callback function with the parameters
 * provided during registration
 */
void scheduler_process(void);

/**
 * @brief Registers a new task with the scheduler
 *
 * This function is used to register a new task with the scheduler. The task configuration is
 * passed via the `config` parameter. The `task_handle` parameter is used to return the handle to the
 * newly registered task.
 *
 * @param[in] task_handle Pointer to the task handle to be returned by the function
 * @param[in] config Pointer to the task configuration data structure
 *
 * @return Returns SCHEDULER_OK if the task was registered successfully, otherwise SCHEDULER_ERROR
 */
scheduler_err_t scheduler_register_task(scheduler_task_handle_t task_handle, scheduler_task_config_t* config);

/**
 * @brief Sets a semaphore for the specified task
 *
 * This function is used to set a semaphore for the specified task. When the task is processed by the
 * scheduler, it checks whether a semaphore has been set for the task. If a semaphore has been set, the
 * task callback function is executed immediately, bypassing the task's normal interval.
 *
 * @param[in] task_handle Pointer to the task handle
 */
void scheduler_set_semaphore(scheduler_task_handle_t task_handle);

/**
 * @brief Halts a periodic task
 *
 * This function is used to halt a periodic task. When a periodic task is halted, it will no longer be
 * executed by the scheduler until it is restarted using the `scheduler_restart_periodic_task` function.
 *
 * @param[in] task_handle Pointer to the task handle
 */
void scheduler_halt_periodic_task(scheduler_task_handle_t task_handle);

/**
 * @brief Restarts a periodic task
 *
 * This function is used to restart a periodic task that was previously halted using the
 * `scheduler_halt_periodic_task` function. Once a periodic task is restarted, it will be processed
 * by the scheduler as normal, at the task's specified interval.
 *
 * @param[in] task_handle Pointer to the task handle
 */
void scheduler_restart_periodic_task(scheduler_task_handle_t task_handle);

/**
 * @brief Sets the interval for a periodic task
 *
 * This function is used to set the interval (in milliseconds) for a periodic task. The interval
 * determines how often the task will be executed by the scheduler.
 *
 * @param[in] task_handle Pointer to the task handle
 * @param[in] time_ms The new interval for the task (in milliseconds)
 */
void scheduler_set_task_interval(scheduler_task_handle_t task_handle, uint16_t time_ms);
#endif /* __SCHEDULER_H */
