/* Copyright (C) 2020 Dalei Liu */

/**
 * @file rsua.h
 * @brief librsua: re/rem/baresip-based SIP user agent library
 *
 * General startup/shutdown steps from an application:
 \verbatim
	rsua_init_*();
	rsua_set_*();		// optional
	rsua_start(...);	// main loop in sync mode
	// or app main loop in async mode

	rsua_stop();
	rsua_delete();
 \endverbatim
 *
 */

#ifndef RSUA_H_INCLUDED
#define RSUA_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RSUA_CONTROL rsua_control
 * @brief Control functions
 * @{
 */

/** startup options (baresip compatibility) */
struct rsua_opts {
	int coredump;
	int af;
	int run_daemon;
	int execmdc;
	char *execmdv[16];
	int use_conf;
	char *conf_path;
	int modc;
	char *modv[16];
	char *audio_path;
	int sip_trace;
	int tmo;
	char *net_interface;
	char *ua_eprm;
	int verbose;
	int handle_signal;
};

/**
 * Initialize the library from options (baresip compatibility).
 * @param opts Options.
 * @return 0, OK; otherwise, error code.
 */
int rsua_init_fromopts(struct rsua_opts *opts);

/**
 * Clean up the library.
 */
void rsua_delete(void);

/**
 * Start running the library.
 * @param opts Options (baresip compatibility).
 * @return 0, OK; otherwise, error code.
 */
int rsua_start(struct rsua_opts *opts);

/**
 * Shut down the library.
 */
void rsua_stop(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* RSUA_H_INCLUDED */
