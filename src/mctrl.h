/**
 * @file mctrl.h
 * @brief Media control
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAMCTRL_H_INCLUDED
#define UAMCTRL_H_INCLUDED

#include "rsua-re/re.h"


#ifndef UAMODAPI_USE		/* Internal API */

int mctrl_handle_media_control(struct pl *body, bool *pfu);

#endif /* ifndef UAMODAPI_USE */

#endif /* UAMCTRL_H_INCLUDED */
