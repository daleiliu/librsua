/**
 * @file stunuri.h
 * @brief STUN URI
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UASTUNURI_H_INCLUDED
#define UASTUNURI_H_INCLUDED

#include "rsua-re/re.h"

/** Defines the STUN uri scheme */
enum stun_scheme {
	STUN_SCHEME_STUN,  /**< STUN scheme        */
	STUN_SCHEME_STUNS, /**< Secure STUN scheme */
	STUN_SCHEME_TURN,  /**< TURN scheme        */
	STUN_SCHEME_TURNS, /**< Secure TURN scheme */
};

/** Defines a STUN/TURN uri */
struct stun_uri {
	enum stun_scheme scheme;  /**< STUN Scheme            */
	char *host;               /**< Hostname or IP-address */
	uint16_t port;            /**< Port number            */
};

int stunuri_decode(struct stun_uri **sup, const struct pl *pl);
int stunuri_set_host(struct stun_uri *su, const char *host);
int stunuri_set_port(struct stun_uri *su, uint16_t port);
int stunuri_print(struct re_printf *pf, const struct stun_uri *su);
const char *stunuri_scheme_name(enum stun_scheme scheme);

#endif /* UASTUNURI_H_INCLUDED */
