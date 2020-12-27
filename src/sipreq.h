/**
 * @file sipreq.h
 * @brief SIP Request with Authentication
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UASIPREQ_H_INCLUDED
#define UASIPREQ_H_INCLUDED

#include "rsua-re/re.h"

struct ua;

int sip_req_send(struct ua *ua, const char *method, const char *uri,
		 sip_resp_h *resph, void *arg, const char *fmt, ...);

#endif /* SIPREQ_H_INCLUDED */
