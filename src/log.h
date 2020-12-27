/**
 * @file log.h
 * @brief Log
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UALOG_H_INCLUDED
#define UALOG_H_INCLUDED

#include "rsua-re/re.h"

enum log_level {
	LEVEL_DEBUG = 0,
	LEVEL_INFO,
	LEVEL_WARN,
	LEVEL_ERROR,
};

typedef void (log_h)(uint32_t level, const char *msg);

struct log {
	struct le le;
	log_h *h;
};

void log_register_handler(struct log *logh);
void log_unregister_handler(struct log *logh);
void log_level_set(enum log_level level);
enum log_level log_level_get(void);
const char *log_level_name(enum log_level level);
void log_enable_debug(bool enable);
void log_enable_info(bool enable);
void log_enable_stdout(bool enable);
void vlog(enum log_level level, const char *fmt, va_list ap);
void loglv(enum log_level level, const char *fmt, ...);
void debug(const char *fmt, ...);
void info(const char *fmt, ...);
void warning(const char *fmt, ...);

#endif /* UALOG_H_INCLUDED */
