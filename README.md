# Scheduler
The Scheduler Module is a software library that allows users to schedule and execute periodic tasks in embedded systems. It provides an easy-to-use API for registering and configuring tasks, as well as setting intervals and controlling task execution. This module is useful in any system that requires periodic execution of tasks, such as sensor sampling, data logging, or system monitoring.

## Usage

1. To use the Scheduler Module in your project, you should include the scheduler.h header file and link against the scheduler.c source file. The module provides several functions for configuring and controlling tasks, as well as a main function for processing scheduled tasks.

2. The basic workflow for using the Scheduler Module is as follows:

3. Initialize the Scheduler Module by calling the scheduler_init() function with a scheduler_config_t struct containing the configuration parameters for the module.

4. Register tasks using the scheduler_register_task() function with a scheduler_task_handle_t handle and a scheduler_task_config_t struct containing the task callback function, task parameter, and initial time interval.
    
Start the Scheduler Module by calling the scheduler_process() function in a main loop.
    
Control task execution using the various control functions, such as scheduler_set_semaphore(), scheduler_halt_periodic_task(), scheduler_restart_periodic_task(), and scheduler_set_task_interval().

## Example
Here's an example of how to use the Scheduler Module to schedule a simple periodic task:
``` C
#include "scheduler.h"

void my_task(void *param)
{
    // Do something here
}

int main()
{
    // Initialize Scheduler Module with a 10ms tick interval
    scheduler_config_t config = {
        .task_table_size = 10,
        .millisec_per_tick = 10
    };
    scheduler_init(&config);

    // Register my_task to run every 100ms
    scheduler_task_handle_t task_handle;
    scheduler_task_config_t task_config = {
        .task_cb = my_task,
        .param = NULL,
        .time_ms = 100
    };
    scheduler_register_task(&task_handle, &task_config);

    // Start Scheduler Module
    while (1) {
        scheduler_process();
    }

    return 0;
}
```
This example initializes the Scheduler Module with a tick interval of 10ms, registers a task to run my_task() every 100ms, and starts the Scheduler Module in a main loop. In a real application, my_task() would be replaced with the user's own task function.