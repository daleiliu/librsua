/**
 * @file call.h
 * @brief Call
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UACALL_H_INCLUDED
#define UACALL_H_INCLUDED

#include "rsua-re/re.h"
#include "rsua-mod/ept.h"

enum call_event {
	CALL_EVENT_INCOMING,
	CALL_EVENT_RINGING,
	CALL_EVENT_PROGRESS,
	CALL_EVENT_ESTABLISHED,
	CALL_EVENT_CLOSED,
	CALL_EVENT_TRANSFER,
	CALL_EVENT_TRANSFER_FAILED,
	CALL_EVENT_MENC,
};

/** Call States */
enum call_state {
	CALL_STATE_IDLE = 0,
	CALL_STATE_INCOMING,
	CALL_STATE_OUTGOING,
	CALL_STATE_RINGING,
	CALL_STATE_EARLY,
	CALL_STATE_ESTABLISHED,
	CALL_STATE_TERMINATED,
	CALL_STATE_UNKNOWN
};

struct account;
struct call;

typedef void (call_event_h)(struct call *call, enum call_event ev,
			    const char *str, void *arg);
typedef void (call_dtmf_h)(struct call *call, char key, void *arg);

int  call_connect(struct call *call, const struct pl *paddr);
int  call_answer(struct call *call, uint16_t scode, enum vidmode vmode);
int  call_progress(struct call *call);
void call_hangup(struct call *call, uint16_t scode, const char *reason);
int  call_modify(struct call *call);
int  call_hold(struct call *call, bool hold);
int  call_set_video_dir(struct call *call, enum sdp_dir dir);
int  call_send_digit(struct call *call, char key);
bool call_has_audio(const struct call *call);
bool call_has_video(const struct call *call);
int  call_transfer(struct call *call, const char *uri);
int  call_status(struct re_printf *pf, const struct call *call);
int  call_debug(struct re_printf *pf, const struct call *call);
int  call_notify_sipfrag(struct call *call, uint16_t scode,
			 const char *reason, ...);
void call_set_handlers(struct call *call, call_event_h *eh,
		       call_dtmf_h *dtmfh, void *arg);
uint16_t      call_scode(const struct call *call);
enum call_state call_state(const struct call *call);
uint32_t      call_duration(const struct call *call);
uint32_t      call_setup_duration(const struct call *call);
const char   *call_id(const struct call *call);
const char   *call_peeruri(const struct call *call);
const char   *call_peername(const struct call *call);
const char   *call_localuri(const struct call *call);
struct audio *call_audio(const struct call *call);
struct video *call_video(const struct call *call);
struct list  *call_streaml(const struct call *call);
struct ua    *call_get_ua(const struct call *call);
bool          call_is_onhold(const struct call *call);
bool          call_is_outgoing(const struct call *call);
void          call_enable_rtp_timeout(struct call *call, uint32_t timeout_ms);
uint32_t      call_linenum(const struct call *call);
struct call  *call_find_linenum(const struct list *calls, uint32_t linenum);
struct call  *call_find_id(const struct list *calls, const char *id);
void call_set_current(struct list *calls, struct call *call);
const struct list *call_get_custom_hdrs(const struct call *call);
int call_set_media_direction(struct call *call, enum sdp_dir a,
			     enum sdp_dir v);


#ifndef UAMODAPI_USE		/* Internal API */

enum {
	CALL_LINENUM_MIN  =   1,
	CALL_LINENUM_MAX  = 256
};

struct config;
struct call;

/** Call parameters */
struct call_prm {
	struct sa laddr;
	enum vidmode vidmode;
	int af;
	bool use_rtp;
};

int  call_alloc(struct call **callp, const struct config *cfg,
		struct list *lst,
		const char *local_name, const char *local_uri,
		struct account *acc, struct ua *ua, const struct call_prm *prm,
		const struct sip_msg *msg, struct call *xcall,
		struct dnsc *dnsc,
		call_event_h *eh, void *arg);
int  call_accept(struct call *call, struct sipsess_sock *sess_sock,
		 const struct sip_msg *msg);
int  call_sdp_get(const struct call *call, struct mbuf **descp, bool offer);
int  call_jbuf_stat(struct re_printf *pf, const struct call *call);
int  call_info(struct re_printf *pf, const struct call *call);
int  call_reset_transp(struct call *call, const struct sa *laddr);
int  call_af(const struct call *call);
void call_set_xrtpstat(struct call *call);
struct account *call_account(const struct call *call);
void call_set_custom_hdrs(struct call *call, const struct list *hdrs);

#endif /* ifndef UAMODAPI_USE */

#endif /* UACALL_H_INCLUDED */
