/**
 * @file udpserver.h
 * @brief TODO.
 *
 */

#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <stdint.h>
#include <stdbool.h>

void udpserver_start(void);

bool udpserver_set_item(
        const uint16_t offset,
        const uint8_t * const data,
        const uint8_t size);

#endif /* UDPSERVER_H */
