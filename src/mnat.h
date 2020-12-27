/**
 * @file mnat.h
 * @brief Media NAT
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAMNAT_H_INCLUDED
#define UAMNAT_H_INCLUDED

#include "rsua-re/re.h"

struct mnat;
struct mnat_sess;
struct mnat_media;
struct stun_uri;

typedef void (mnat_estab_h)(int err, uint16_t scode, const char *reason,
	void *arg);

typedef void (mnat_connected_h)(const struct sa *raddr1,
	const struct sa *raddr2, void *arg);


typedef int (mnat_sess_h)(struct mnat_sess **sessp,
	const struct mnat *mnat, struct dnsc *dnsc,
	int af, const struct stun_uri *srv,
	const char *user, const char *pass,
	struct sdp_session *sdp, bool offerer,
	mnat_estab_h *estabh, void *arg);

typedef int (mnat_media_h)(struct mnat_media **mp, struct mnat_sess *sess,
	struct udp_sock *sock1, struct udp_sock *sock2,
	struct sdp_media *sdpm,
	mnat_connected_h *connh, void *arg);

typedef int (mnat_update_h)(struct mnat_sess *sess);

struct mnat {
	struct le le;
	const char *id;
	const char *ftag;
	bool wait_connected;
	mnat_sess_h *sessh;
	mnat_media_h *mediah;
	mnat_update_h *updateh;
};

void mnat_register(struct list *mnatl, struct mnat *mnat);
void mnat_unregister(struct mnat *mnat);
const struct mnat *mnat_find(const struct list *mnatl, const char *id);

#endif /* UAMNAT_H_INCLUDED */
