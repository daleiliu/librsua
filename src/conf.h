/**
 * @file conf.h
 * @brief Conf (utils) and Config (core configuration)
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UACONF_H_INCLUDED
#define UACONF_H_INCLUDED

#include "rsua-re/re.h"

struct config;
struct range;
struct vidsz;

/** Defines the configuration line handler */
typedef int (confline_h)(const struct pl *addr, void *arg);

int  conf_configure(void);
int  conf_configure_buf(const uint8_t *buf, size_t sz);
int  conf_modules(void);
void conf_path_set(const char *path);
int  conf_path_get(char *path, size_t sz);
int  conf_parse(const char *filename, confline_h *ch, void *arg);
int  conf_get_vidsz(const struct conf *conf, const char *name,
		    struct vidsz *sz);
int  conf_get_sa(const struct conf *conf, const char *name, struct sa *sa);
bool conf_fileexist(const char *path);
void conf_close(void);
struct conf *conf_cur(void);


int config_parse_conf(struct config *cfg, const struct conf *conf);
int config_print(struct re_printf *pf, const struct config *cfg);
int config_write_template(const char *file, const struct config *cfg);


#ifndef UAMODAPI_USE		/* Internal API */

int conf_get_range(const struct conf *conf, const char *name,
		   struct range *rng);
int conf_get_csv(const struct conf *conf, const char *name,
		 char *str1, size_t sz1, char *str2, size_t sz2);
int conf_get_float(const struct conf *conf, const char *name, double *val);

#endif /* ifndef UAMODAPI_USE */

#endif /* UACONF_H_INCLUDED */
