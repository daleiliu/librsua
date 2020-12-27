/**
 * @file ev.h
 * @brief Generic event
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAEV_H_INCLUDED
#define UAEV_H_INCLUDED

#include "rsua-re/re.h"

struct call;
struct ua;

/** Events from User-Agent */
enum ua_event {
	UA_EVENT_REGISTERING = 0,
	UA_EVENT_REGISTER_OK,
	UA_EVENT_REGISTER_FAIL,
	UA_EVENT_UNREGISTERING,
	UA_EVENT_FALLBACK_OK,
	UA_EVENT_FALLBACK_FAIL,
	UA_EVENT_MWI_NOTIFY,
	UA_EVENT_SHUTDOWN,
	UA_EVENT_EXIT,

	UA_EVENT_CALL_INCOMING,
	UA_EVENT_CALL_RINGING,
	UA_EVENT_CALL_PROGRESS,
	UA_EVENT_CALL_ESTABLISHED,
	UA_EVENT_CALL_CLOSED,
	UA_EVENT_CALL_TRANSFER,
	UA_EVENT_CALL_TRANSFER_FAILED,
	UA_EVENT_CALL_DTMF_START,
	UA_EVENT_CALL_DTMF_END,
	UA_EVENT_CALL_RTPESTAB,
	UA_EVENT_CALL_RTCP,
	UA_EVENT_CALL_MENC,
	UA_EVENT_VU_TX,
	UA_EVENT_VU_RX,
	UA_EVENT_AUDIO_ERROR,
	UA_EVENT_CALL_LOCAL_SDP,      /**< param: offer or answer */
	UA_EVENT_CALL_REMOTE_SDP,     /**< param: offer or answer */

	UA_EVENT_MAX,
};

/** Defines the User-Agent event handler */
typedef void (ua_event_h)(struct ua *ua, enum ua_event ev,
			  struct call *call, const char *prm, void *arg);


int event_encode_dict(struct odict *od, struct ua *ua, enum ua_event ev,
		      struct call *call, const char *prm);
int event_add_au_jb_stat(struct odict *od_parent, const struct call *call);
int  uag_event_register(ua_event_h *eh, void *arg);
void uag_event_unregister(ua_event_h *eh);
void ua_event(struct ua *ua, enum ua_event ev, struct call *call,
	      const char *fmt, ...);
const char  *uag_event_str(enum ua_event ev);

#endif /* UAEV_H_INCLUDED */
