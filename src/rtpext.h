/**
 * @file rtpext.h
 * @brief RTP Header Extensions
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UARTPEXT_H_INCLUDED
#define UARTPEXT_H_INCLUDED

#include "rsua-re/re.h"


#ifndef UAMODAPI_USE		/* Internal API */

#define RTPEXT_HDR_SIZE        4
#define RTPEXT_TYPE_MAGIC 0xbede

enum {
	RTPEXT_ID_MIN  =  1,
	RTPEXT_ID_MAX  = 14,
};

enum {
	RTPEXT_LEN_MIN =  1,
	RTPEXT_LEN_MAX = 16,
};

struct rtpext {
	unsigned id:4;
	unsigned len:4;
	uint8_t data[RTPEXT_LEN_MAX];
};


int rtpext_hdr_encode(struct mbuf *mb, size_t num_bytes);
int rtpext_encode(struct mbuf *mb, unsigned id, unsigned len,
		  const uint8_t *data);
int rtpext_decode(struct rtpext *ext, struct mbuf *mb);

#endif /* ifndef UAMODAPI_USE */

#endif /* UARTPEXT_H_INCLUDED */
