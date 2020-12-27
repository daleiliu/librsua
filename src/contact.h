/**
 * @file contact.h
 * @brief Contact
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UACONTACT_H_INCLUDED
#define UACONTACT_H_INCLUDED

#include "rsua-re/re.h"

enum presence_status {
	PRESENCE_UNKNOWN,
	PRESENCE_OPEN,
	PRESENCE_CLOSED,
	PRESENCE_BUSY
};


struct contact;
typedef void (contact_update_h)(struct contact *c, bool removed, void *arg);

struct contacts;


int  contact_init(struct contacts **contactsp);
int  contact_add(struct contacts *contacts,
		 struct contact **contactp, const struct pl *addr);
void contact_remove(struct contacts *contacts, struct contact *c);
void contacts_enable_presence(struct contacts *contacts, bool enabled);
void contact_set_update_handler(struct contacts *contacs,
				contact_update_h *updateh, void *arg);
int  contact_print(struct re_printf *pf, const struct contact *cnt);
int  contacts_print(struct re_printf *pf, const struct contacts *contacts);
enum presence_status contact_presence(const struct contact *c);
void contact_set_presence(struct contact *c, enum presence_status status);
bool contact_block_access(const struct contacts *contacts, const char *uri);
struct contact  *contact_find(const struct contacts *contacts,
			      const char *uri);
struct sip_addr *contact_addr(const struct contact *c);
struct list     *contact_list(const struct contacts *contacts);
const char      *contact_str(const struct contact *c);
const char      *contact_uri(const struct contact *c);
const char      *contact_presence_str(enum presence_status status);
struct le       *contact_le(struct contact *cnt);
void contacts_set_current(struct contacts *contacts, struct contact *cnt);
struct contact *contacts_current(const struct contacts *contacts);

#endif /* UACONTACT_H_INCLUDED */
