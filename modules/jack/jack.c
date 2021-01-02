/**
 * @file jack.c  JACK audio driver
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2021 Dalei Liu
 */
#include "rsua-mod/modapi.h"
#include "mod_jack.h"


static struct auplay *auplay;
static struct ausrc *ausrc;


static int module_init(void)
{
	int err = 0;

	err |= auplay_register(&auplay, data_auplayl(),
			       "jack", jack_play_alloc);
	err |= ausrc_register(&ausrc, data_ausrcl(),
			      "jack", jack_src_alloc);

	return err;
}


static int module_close(void)
{
	auplay = mem_deref(auplay);
	ausrc  = mem_deref(ausrc);

	return 0;
}


const struct mod_export DECL_EXPORTS(jack) = {
	"jack",
	"sound",
	module_init,
	module_close
};
