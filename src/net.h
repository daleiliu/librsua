/**
 * @file net.h
 * @brief Networking
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UANET_H_INCLUDED
#define UANET_H_INCLUDED

#include "rsua-re/re.h"

struct config_net;
struct network;

typedef void (net_change_h)(void *arg);

int  net_alloc(struct network **netp, const struct config_net *cfg);
int  net_use_nameserver(struct network *net,
			const struct sa *srvv, size_t srvc);
int  net_set_address(struct network *net, const struct sa *ip);
void net_change(struct network *net, uint32_t interval,
		net_change_h *ch, void *arg);
void net_force_change(struct network *net);
bool net_check(struct network *net);
bool net_af_enabled(const struct network *net, int af);
int  net_set_af(struct network *net, int af);
int  net_dns_debug(struct re_printf *pf, const struct network *net);
int  net_debug(struct re_printf *pf, const struct network *net);
const struct sa *net_laddr_af(const struct network *net, int af);
struct dnsc     *net_dnsc(const struct network *net);

#endif /* UANET_H_INCLUDED */
