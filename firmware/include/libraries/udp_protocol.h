/**
 * @file udp_protocol.h
 * @brief TODO.
 *
 */

#ifndef UDP_PROTOCOL_H
#define UDP_PROTOCOL_H

#include <stdint.h>

#define UP_VERSION_MAJOR (0x0000)
#define UP_VERSION_MINOR (0x0001)

#define UP_MSG_TYPE_INVALID (0x0000)
#define UP_MSG_TYPE_DATA (0x0020)

#define UP_HEADER_SIZE (8)
#define UP_MSG_DATA_SIZE (128)
#define UP_MSG_DATA_ITEMS_LEN (32)

typedef struct __attribute__((packed))
{
    uint16_t version_major;
    uint16_t version_minor;
    uint16_t msg_type;
    uint16_t msg_size;
    uint8_t msg_data[0];
} up_header;

typedef struct __attribute__((packed))
{
    uint32_t data_items[UP_MSG_DATA_ITEMS_LEN];
} up_msg_data;

#endif /* UDP_PROTOCOL_H */
