/**
 * @file ethernetif.h
 * @brief TODO.
 *
 */

#ifndef ETHERNETIF_H
#define ETHERNETIF_H

#include <stdint.h>
#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);

void ethernetif_update_config(struct netif *netif);

void ethernetif_notify_conn_changed(struct netif *netif);

uint8_t ethernetif_link_status(void);

#endif /* ETHERNETIF_H */
