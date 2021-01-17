/**
 * @file module.h
 * @brief Modules
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 - 2021 Dalei Liu
 */

#ifndef UAMODULE_H_INCLUDED
#define UAMODULE_H_INCLUDED

#include "rsua-re/re.h"

int  module_preload(const char *module);
int  module_load(const char *path, const char *name);
void module_unload(const char *name);
void module_app_unload(void);


#ifndef UAMODAPI_USE		/* Internal API */

int module_load_fromconf(const struct conf *conf);

void module_set_path(const char *path);

#endif /* ifndef UAMODAPI_USE */

#endif /* UAMODULE_H_INCLUDED */
