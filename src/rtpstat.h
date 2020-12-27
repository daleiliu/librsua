/**
 * @file rtpstat.h
 * @brief RTP Stats
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UARTPSTAT_H_INCLUDED
#define UARTPSTAT_H_INCLUDED

#include "rsua-re/re.h"


#ifndef UAMODAPI_USE		/* Internal API */

struct call;

int rtpstat_print(struct re_printf *pf, const struct call *call);

#endif /* ifndef UAMODAPI_USE */

#endif /* UARTPSTAT_H_INCLUDED */
