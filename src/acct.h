/**
 * @file acct.h
 * @brief SIP Account
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAACCT_H_INCLUDED
#define UAACCT_H_INCLUDED

#include "rsua-re/re.h"

/** Defines the answermodes */
enum answermode {
	ANSWERMODE_MANUAL = 0,
	ANSWERMODE_EARLY,
	ANSWERMODE_AUTO
};

struct account;

int account_alloc(struct account **accp, const char *sipaddr);
int account_debug(struct re_printf *pf, const struct account *acc);
int account_json_api(struct odict *odacc, struct odict *odcfg,
		 const struct account *acc);
int account_set_auth_user(struct account *acc, const char *user);
int account_set_auth_pass(struct account *acc, const char *pass);
int account_set_outbound(struct account *acc, const char *ob, unsigned ix);
int account_set_sipnat(struct account *acc, const char *sipnat);
int account_set_answermode(struct account *acc, enum answermode mode);
int account_set_display_name(struct account *acc, const char *dname);
int account_set_regint(struct account *acc, uint32_t regint);
int account_set_stun_uri(struct account *acc, const char *uri);
int account_set_stun_host(struct account *acc, const char *host);
int account_set_stun_port(struct account *acc, uint16_t port);
int account_set_stun_user(struct account *acc, const char *user);
int account_set_stun_pass(struct account *acc, const char *pass);
int account_set_mediaenc(struct account *acc, const char *mediaenc);
int account_set_medianat(struct account *acc, const char *medianat);
int account_set_audio_codecs(struct account *acc, const char *codecs);
int account_set_video_codecs(struct account *acc, const char *codecs);
int account_set_mwi(struct account *acc, const char *value);
int account_set_call_transfer(struct account *acc, const char *value);
int account_auth(const struct account *acc, char **username, char **password,
		 const char *realm);
struct list *account_aucodecl(const struct account *acc);
struct list *account_vidcodecl(const struct account *acc);
struct sip_addr *account_laddr(const struct account *acc);
struct uri *account_luri(const struct account *acc);
uint32_t account_regint(const struct account *acc);
uint32_t account_fbregint(const struct account *acc);
uint32_t account_pubint(const struct account *acc);
uint32_t account_ptime(const struct account *acc);
uint32_t account_prio(const struct account *acc);
enum answermode account_answermode(const struct account *acc);
const char *account_display_name(const struct account *acc);
const char *account_aor(const struct account *acc);
const char *account_auth_user(const struct account *acc);
const char *account_auth_pass(const struct account *acc);
const char *account_outbound(const struct account *acc, unsigned ix);
const char *account_sipnat(const struct account *acc);
const char *account_stun_user(const struct account *acc);
const char *account_stun_pass(const struct account *acc);
const char *account_stun_host(const struct account *acc);
const struct stun_uri *account_stun_uri(const struct account *acc);
uint16_t account_stun_port(const struct account *acc);
const char *account_mediaenc(const struct account *acc);
const char *account_medianat(const struct account *acc);
const char *account_mwi(const struct account *acc);
const char *account_call_transfer(const struct account *acc);
const char *account_extra(const struct account *acc);


#ifndef UAMODAPI_USE		/* Internal API */

struct account {
	char *buf;                   /**< Buffer for the SIP address         */
	struct sip_addr laddr;       /**< Decoded SIP address                */
	struct uri luri;             /**< Decoded AOR uri                    */
	char *dispname;              /**< Display name                       */
	char *aor;                   /**< Local SIP uri                      */

	/* parameters: */
	enum answermode answermode;  /**< Answermode for incoming calls      */
	struct le acv[16];           /**< List elements for aucodecl         */
	struct list aucodecl;        /**< List of preferred audio-codecs     */
	char *auth_user;             /**< Authentication username            */
	char *auth_pass;             /**< Authentication password            */
	char *mnatid;                /**< Media NAT handling                 */
	char *mencid;                /**< Media encryption type              */
	const struct mnat *mnat;     /**< MNAT module                        */
	const struct menc *menc;     /**< MENC module                        */
	char *outboundv[2];          /**< Optional SIP outbound proxies      */
	uint32_t ptime;              /**< Configured packet time in [ms]     */
	uint32_t regint;             /**< Registration interval in [seconds] */
	uint32_t fbregint;           /**< Fallback R. interval in [seconds]  */
	uint32_t rwait;              /**< R. Int. in [%] from proxy expiry   */
	uint32_t pubint;             /**< Publication interval in [seconds]  */
	uint32_t prio;               /**< Prio for serial registration       */
	char *regq;                  /**< Registration Q-value               */
	char *sipnat;                /**< SIP Nat mechanism                  */
	char *stun_user;             /**< STUN Username                      */
	char *stun_pass;             /**< STUN Password                      */
	struct stun_uri *stun_host;  /**< STUN Server                        */
	struct le vcv[4];            /**< List elements for vidcodecl        */
	struct list vidcodecl;       /**< List of preferred video-codecs     */
	bool mwi;                    /**< MWI on/off                         */
	bool refer;                  /**< REFER method on/off                */
	char *ausrc_mod;
	char *ausrc_dev;
	char *auplay_mod;
	char *auplay_dev;
	char *extra;                 /**< Extra parameters                   */
};

#endif /* ifndef UAMODAPI_USE */

#endif /* UAACCT_H_INCLUDED */
