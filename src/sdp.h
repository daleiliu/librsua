/**
 * @file sdp.h
 * @brief SDP
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UASDP_H_INCLUDED
#define UASDP_H_INCLUDED

#include "rsua-re/re.h"

bool sdp_media_has_media(const struct sdp_media *m);
int  sdp_fingerprint_decode(const char *attr, struct pl *hash,
			    uint8_t *md, size_t *sz);


#ifndef UAMODAPI_USE		/* Internal API */

int sdp_decode_multipart(const struct pl *ctype_prm, struct mbuf *mb);

#endif /* ifndef UAMODAPI_USE */

#endif /* UASDP_H_INCLUDED */
