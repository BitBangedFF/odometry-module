/**
 * @file config.h
 * @brief TODO.
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

// task priorities, higher number == higher priority
#define SYSTEM_TASK_PRI (2)
#define UDPSERVER_TASK_PRI (1)

// task stack sizes
#define SYSTEM_TASK_STACKSIZE (2 * configMINIMAL_STACK_SIZE)
#define UDPSERVER_TASK_STACKSIZE (5 * configMINIMAL_STACK_SIZE)

// task names
#define SYSTEM_TASK_NAME "SYSTEM"
#define UDPSERVER_TASK_NAME "UDPSERVER"

#endif /* CONFIG_H */
