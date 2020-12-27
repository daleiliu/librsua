/**
 * @file reg.h
 * @brief Register client
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAREG_H_INCLUDED
#define UAREG_H_INCLUDED

#include "rsua-re/re.h"


#ifndef UAMODAPI_USE		/* Internal API */

struct reg;
struct ua;

int  reg_add(struct list *lst, struct ua *ua, int regid);
int  reg_register(struct reg *reg, const char *reg_uri,
		    const char *params, uint32_t regint, const char *outbound);
void reg_unregister(struct reg *reg);
bool reg_isok(const struct reg *reg);
bool reg_failed(const struct reg *reg);
int  reg_debug(struct re_printf *pf, const struct reg *reg);
int  reg_json_api(struct odict *od, const struct reg *reg);
int  reg_status(struct re_printf *pf, const struct reg *reg);
int  reg_af(const struct reg *reg);

#endif /* ifndef UAMODAPI_USE */

#endif /* UAREG_H_INCLUDED */
