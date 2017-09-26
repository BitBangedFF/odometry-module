/**
 * @file config.h
 * @brief TODO.
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

// task priorities, higher number == higher priority
#define SYSTEM_TASK_PRI (2)

// task stack sizes
#define SYSTEM_TASK_STACKSIZE (2* configMINIMAL_STACK_SIZE)

// task names
#define SYSTEM_TASK_NAME "SYSTEM"

#endif /* CONFIG_H */
