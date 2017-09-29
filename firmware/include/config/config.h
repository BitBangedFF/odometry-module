/**
 * @file config.h
 * @brief TODO.
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

// task priorities, higher number == higher priority
#define SYSTEM_TASK_PRI (2)
#define UDPSERVER_INIT_TASK_PRI (1)

// task stack sizes
#define SYSTEM_TASK_STACKSIZE (2 * configMINIMAL_STACK_SIZE)
#define UDPSERVER_INIT_TASK_STACKSIZE (5 * configMINIMAL_STACK_SIZE)

// task names
#define SYSTEM_TASK_NAME "SYSTEM"
#define UDPSERVER_INIT_TASK_NAME "UDPSERVER-INIT"

#define IP_ADDR0 (192)
#define IP_ADDR1 (168)
#define IP_ADDR2 (1)
#define IP_ADDR3 (39)

#define NETMASK_ADDR0 (255)
#define NETMASK_ADDR1 (255)
#define NETMASK_ADDR2 (255)
#define NETMASK_ADDR3 (0)

#define GW_ADDR0 (192)
#define GW_ADDR1 (168)
#define GW_ADDR2 (1)
#define GW_ADDR3 (1)

#define UDPSERVER_PORT (57319)

#endif /* CONFIG_H */
