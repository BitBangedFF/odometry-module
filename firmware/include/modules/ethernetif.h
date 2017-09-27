/**
 * @file ethernetif.h
 * @brief TODO.
 *
 */

#ifndef ETHERNETIF_H
#define ETHERNETIF_H

#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);

#endif /* ETHERNETIF_H */
