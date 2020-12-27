/**
 * @file message.h
 * @brief SIP MESSAGE
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAMESSAGE_H_INCLUDED
#define UAMESSAGE_H_INCLUDED

#include "rsua-re/re.h"

struct message;
struct ua;

typedef void (message_recv_h)(struct ua *ua, const struct pl *peer,
			      const struct pl *ctype,
			      struct mbuf *body, void *arg);

int  message_init(struct message **messagep);
int  message_listen(struct message *message,
		    message_recv_h *h, void *arg);
void message_unlisten(struct message *message, message_recv_h *recvh);
int  message_send(struct ua *ua, const char *peer, const char *msg,
		  sip_resp_h *resph, void *arg);

#endif /* UAMESSAGE_H_INCLUDED */
