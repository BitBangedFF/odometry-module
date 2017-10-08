/**
 * @file xsmessage.h
 * @brief TODO.
 *
 */

#ifndef XSMESSAGE_H
#define XSMESSAGE_H

#include <stdint.h>
#include <stdbool.h>

#define XS_PREAMBLE             (0xFA)
#define XS_EXTLENCODE           (0xFF)

#define XS_LEN_MSGHEADER        (4)
#define XS_LEN_MSGEXTHEADER     (6)
#define XS_LEN_MSGHEADERCS      (5)
#define XS_LEN_MSGEXTHEADERCS   (7)
#define XS_LEN_CHECKSUM         (1)
#define XS_LEN_UNSIGSHORT       (2)
#define XS_LEN_UNSIGINT         (4)
#define XS_LEN_FLOAT            (4)

#define XS_MAXDATALEN           (8192-XS_LEN_MSGEXTHEADERCS)
#define XS_MAXSHORTDATALEN      (254)
#define XS_MAXMSGLEN            (XS_MAXDATALEN+XS_LEN_MSGEXTHEADERCS)
#define XS_MAXSHORTMSGLEN       (XS_MAXSHORTDATALEN+XS_LEN_MSGHEADERCS)
#define XS_MAXGARBAGE           (XS_MAXMSGLEN+1)

#endif /* XSMESSAGE_H */
