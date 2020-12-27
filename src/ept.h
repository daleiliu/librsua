/**
 * @file ept.h
 * @brief SIP endpoint for an account
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAEPT_H_INCLUDED
#define UAEPT_H_INCLUDED

#include "rsua-re/re.h"
#include "rsua-mod/contact.h"

struct ua;
struct call;

/** Video mode */
enum vidmode {
	VIDMODE_OFF = 0,    /**< Video disabled                */
	VIDMODE_ON,         /**< Video enabled                 */
};

typedef void (options_resp_h)(int err, const struct sip_msg *msg, void *arg);

typedef void (ua_exit_h)(void *arg);


/* Multiple instances */
int  ua_alloc(struct ua **uap, const char *aor);
int  ua_connect(struct ua *ua, struct call **callp,
		const char *from_uri, const char *req_uri,
		enum vidmode vmode);
int  ua_connect_dir(struct ua *ua, struct call **callp,
		    const char *from_uri, const char *req_uri,
		    enum vidmode vmode, enum sdp_dir adir, enum sdp_dir vdir);
void ua_hangup(struct ua *ua, struct call *call,
	       uint16_t scode, const char *reason);
int  ua_answer(struct ua *ua, struct call *call, enum vidmode vmode);
int  ua_hold_answer(struct ua *ua, struct call *call, enum vidmode vmode);
int  ua_options_send(struct ua *ua, const char *uri,
		     options_resp_h *resph, void *arg);
int  ua_debug(struct re_printf *pf, const struct ua *ua);
int  ua_state_json_api(struct odict *od, const struct ua *ua);
int  ua_print_calls(struct re_printf *pf, const struct ua *ua);
int  ua_print_status(struct re_printf *pf, const struct ua *ua);
int  ua_print_supported(struct re_printf *pf, const struct ua *ua);
int  ua_update_account(struct ua *ua);
int  ua_register(struct ua *ua);
int  ua_fallback(struct ua *ua);
void ua_unregister(struct ua *ua);
bool ua_isregistered(const struct ua *ua);
bool ua_regfailed(const struct ua *ua);
unsigned ua_destroy(struct ua *ua);
void ua_pub_gruu_set(struct ua *ua, const struct pl *pval);
const char     *ua_aor(const struct ua *ua);
const char     *ua_cuser(const struct ua *ua);
const char     *ua_local_cuser(const struct ua *ua);
struct account *ua_account(const struct ua *ua);
const char     *ua_outbound(const struct ua *ua);
struct call    *ua_call(const struct ua *ua);
struct call    *ua_prev_call(const struct ua *ua);
struct list    *ua_calls(const struct ua *ua);
enum presence_status ua_presence_status(const struct ua *ua);
void ua_presence_status_set(struct ua *ua, const enum presence_status status);
void ua_set_media_af(struct ua *ua, int af_media);
void ua_set_catchall(struct ua *ua, bool enabled);
int ua_add_xhdr_filter(struct ua *ua, const char *hdr_name);
int  ua_set_custom_hdrs(struct ua *ua, struct list *custom_hdrs);
int  ua_uri_complete(struct ua *ua, struct mbuf *buf, const char *uri);
int  ua_call_alloc(struct call **callp, struct ua *ua,
		   enum vidmode vidmode, const struct sip_msg *msg,
		   struct call *xcall, const char *local_uri,
		   bool use_rtp);


/* One instance */
int  ua_init(const char *software, bool udp, bool tcp, bool tls);
void ua_close(void);
void ua_stop_all(bool forced);
void uag_set_exit_handler(ua_exit_h *exith, void *arg);
void uag_enable_sip_trace(bool enable);
int  uag_reset_transp(bool reg, bool reinvite);
void uag_set_sub_handler(sip_msg_h *subh);
int  uag_set_extra_params(const char *eprm);
struct ua   *uag_find(const struct pl *cuser);
struct ua   *uag_find_aor(const char *aor);
struct ua   *uag_find_param(const char *name, const char *val);
struct sip  *uag_sip(void);
struct list *uag_list(void);
uint32_t     uag_call_count(void);
void         uag_current_set(struct ua *ua);
struct ua   *uag_current(void);
struct tls  *uag_tls(void);
struct sipsess_sock  *uag_sipsess_sock(void);
struct sipevent_sock *uag_sipevent_sock(void);


#ifndef UAMODAPI_USE		/* Internal API */

void         ua_printf(const struct ua *ua, const char *fmt, ...);

int ua_print_allowed(struct re_printf *pf, const struct ua *ua);

#endif /* ifndef UAMODAPI_USE */

#endif /* UAEPT_H_INCLUDED */
