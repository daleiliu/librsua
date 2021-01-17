/**
 * @file rsua_cfg.c
 * @brief librsua initialization and configuration functions
 *
 * Copyright (C) 2010 - 2016 Creytiv.com
 * Copyright (C) 2020 - 2021 Dalei Liu
 */

#include "rsua.h"
#include "rsua-re/re.h"
#include "module.h"
#include "data.h"
#include "acct.h"
#include "conf.h"
#include "log.h"

/* public api functions */

int rsua_init_fromopts(struct rsua_opts *opts)
{
	int err;

	if (!opts)
		return EINVAL;

	if (opts->verbose)
		log_enable_debug(true);

	if (opts->coredump)
		sys_coredump_set(true);

	if (opts->conf_path)
		conf_path_set(opts->conf_path);

	if (opts->module_path)
		module_set_path(opts->module_path);

	if (opts->use_conf) {
		err = conf_configure();
		if (err) {
			warning("main: configure failed: %m\n", err);
			return err;
		}
	}

	/*
	 * Set the network interface before initializing the config
	 */
	if (opts->net_interface) {
		struct config *theconf = data_config();

		str_ncpy(theconf->net.ifname, opts->net_interface,
			 sizeof(theconf->net.ifname));
	}

	/*
	 * Set prefer_ipv6 preferring the one given in -6 argument (if any)
	 */
	if (opts->af != AF_UNSPEC)
		data_config()->net.af = opts->af;

	return 0;
}

void rsua_delete(void)
{
}
