/**
 * @file rsua_rt.c
 * @brief librsua runtime functions
 *
 * Copyright (C) 2010 - 2016 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#include "rsua.h"
#include <stdlib.h>
#include <pthread.h>
#include "rsua-re/re.h"
#include "data.h"
#include "ept.h"
#include "net.h"
#include "log.h"
#include "module.h"
#include "cmd.h"
#include "play.h"
#include "contact.h"
#include "message.h"
#include "conf.h"
#include "ui.h"

static struct tmr tmr_quit;

/* functions */

static void signal_handler(int sig)
{
	static bool term = false;

	if (term) {
		mod_close();
		exit(0);
	}

	term = true;

	info("terminated by signal %d\n", sig);

	ua_stop_all(false);
}


static void net_change_handler(void *arg)
{
	(void)arg;

	info("IP-address changed: %j\n",
	     net_laddr_af(data_network(), AF_INET));

	(void)uag_reset_transp(true, true);
}


static void ua_exit_handler(void *arg)
{
	(void)arg;
	debug("ua exited -- stopping main runloop\n");

	/* The main run-loop can be stopped now */
	re_cancel();
}


static void tmr_quit_handler(void *arg)
{
	(void)arg;

	ua_stop_all(false);
}

static int cmd_quit(struct re_printf *pf, void *unused)
{
	int err;

	(void)unused;

	err = re_hprintf(pf, "Quit\n");

	ua_stop_all(false);

	return err;
}


static int insmod_handler(struct re_printf *pf, void *arg)
{
       const struct cmd_arg *carg = arg;
       char path[256];
       int err;

       if (conf_get_str(conf_cur(), "module_path", path, sizeof(path)))
	       str_ncpy(path, ".", sizeof(path));

       err = module_load(path, carg->prm);
       if (err) {
               return re_hprintf(pf, "insmod: ERROR: could not load module"
                                 " '%s': %m\n", carg->prm, err);
       }

       return re_hprintf(pf, "loaded module %s\n", carg->prm);
}


static int rmmod_handler(struct re_printf *pf, void *arg)
{
       const struct cmd_arg *carg = arg;
       (void)pf;

       module_unload(carg->prm);

       return 0;
}


static const struct cmd corecmdv[] = {
	{"quit", 'q', 0, "Quit",                     cmd_quit             },
	{"insmod", 0, CMD_PRM, "Load module",        insmod_handler       },
	{"rmmod",  0, CMD_PRM, "Unload module",      rmmod_handler        },
};


int rsua_start(struct rsua_opts *opts)
{
	int err;
	struct data_subsys subsys;
	int i;

	err = libre_init();
	if (err)
		goto out;

	/* Initialise Network */
	err = net_alloc(&subsys.net, &data_config()->net);
	if (err) {
		warning("ua: network init failed: %m\n", err);
		return err;
	}

	err = contact_init(&subsys.contacts);
	if (err)
		return err;

	err = cmd_init(&subsys.commands);
	if (err)
		return err;

	err = play_init(&subsys.player);
	if (err)
		return err;

	err = message_init(&subsys.message);
	if (err) {
		warning("rsua_rt: message init failed: %m\n", err);
		return err;
	}

	err = ui_init(&subsys.uis);
	if (err)
		return err;

	err = data_init(&subsys);
	if (err)
		goto out;

	err = cmd_register(data_commands(), corecmdv, ARRAY_SIZE(corecmdv));
	if (err)
		return err;

	tmr_init(&tmr_quit);

	/* Set audio path preferring the one given in -p argument (if any) */
	if (opts->audio_path)
		play_set_path(data_player(), opts->audio_path);
	else if (str_isset(data_config()->audio.audio_path)) {
		play_set_path(data_player(),
			      data_config()->audio.audio_path);
	}

	/* NOTE: must be done after all arguments are processed */
	if (opts->modc) {

		info("pre-loading modules: %zu\n", opts->modc);

		for (i=0; i < opts->modc; i++) {

			err = module_preload(opts->modv[i]);
			if (err) {
				re_fprintf(stderr,
					   "could not pre-load module"
					   " '%s' (%m)\n", opts->modv[i], err);
			}
		}
	}

	/* Initialise User Agents */
	err = ua_init("rsua v" RSUA_VERSION " (" ARCH "/" OS ")",
		      true, true, true);
	if (err)
		goto out;

	net_change(data_network(), 60, net_change_handler, NULL);

	uag_set_exit_handler(ua_exit_handler, NULL);

	if (opts->ua_eprm) {
		err = uag_set_extra_params(opts->ua_eprm);
		if (err)
			goto out;
	}

	if (opts->sip_trace)
		uag_enable_sip_trace(true);

	/* Load modules */
	err = conf_modules();
	if (err)
		goto out;

	if (opts->run_daemon) {
		err = sys_daemon();
		if (err)
			goto out;

		log_enable_stdout(false);
	}

	info("rsua is ready.\n");

	/* Execute any commands from input arguments */
	for (i=0; i < opts->execmdc; i++) {
		ui_input_str(opts->execmdv[i]);
	}

	if (opts->tmo) {
		tmr_start(&tmr_quit, opts->tmo * 1000, tmr_quit_handler, NULL);
	}

	/* Main loop */
	err = re_main(opts->handle_signal ? signal_handler : NULL);

 out:
	if (err)
		ua_stop_all(true);

	return err;
}

void rsua_stop(void)
{
	tmr_cancel(&tmr_quit);

	ua_close();

	/* note: must be done before mod_close() */
	module_app_unload();

	conf_close();

	cmd_unregister(data_commands(), corecmdv);

	ui_reset(data_uis());

	/* NOTE: modules must be unloaded after all application
	 *       activity has stopped.
	 */
	debug("main: unloading modules..\n");
	mod_close();

	data_close();
	libre_close();

	/* Check for memory leaks */
	tmr_debug();
	mem_debug();
}
