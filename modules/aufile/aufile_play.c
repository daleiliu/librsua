/**
 * @file aufile.c WAV Audio Player
 *
 * Copyright (C) 2020 commend.com - Christian Spielberger
 * Copyright (C) 2020 Dalei Liu
 */
#define _DEFAULT_SOURCE 1
#define _BSD_SOURCE 1
#include <pthread.h>
#include <string.h>
#include "rsua-mod/modapi.h"
#include "aufile.h"


struct auplay_st {
	const struct auplay *ap;  /* pointer to base-class (inheritance) */
	struct aufile *auf;
	struct auplay_prm prm;

	pthread_t thread;
	volatile bool run;
	void *sampv;
	size_t sampc;
	size_t num_bytes;
	auplay_write_h *wh;
	void *arg;
};


static void auplay_destructor(void *arg)
{
	struct auplay_st *st = arg;
	/* Wait for termination of other thread */
	if (st->run) {
		debug("aufile: stopping playback thread\n");
		st->run = false;
		(void)pthread_join(st->thread, NULL);
	}

	mem_deref(st->auf);
	mem_deref(st->sampv);
}


static void *write_thread(void *arg)
{
	struct auplay_st *st = arg;
	uint64_t t;
	int dt;
	int err;
	uint32_t ptime = st->prm.ptime;

	t = tmr_jiffies();
	st->run = true;
	while (st->run) {
		st->wh(st->sampv, st->sampc, st->arg);

		err = aufile_write(st->auf, st->sampv, st->num_bytes);
		if (err)
			break;

		t += ptime;
		dt = t - tmr_jiffies();
		if (dt <= 2)
			continue;

		sys_msleep(dt);
	}

	st->run = false;
	return NULL;
}


int play_alloc(struct auplay_st **stp, const struct auplay *ap,
		    struct auplay_prm *prm, const char *device,
		    auplay_write_h *wh, void *arg)
{
	struct auplay_st *st;
	const char *file;
	struct aufile_prm aufprm;
	int err;

	if (!prm || !wh)
		return EINVAL;

	if (!prm->ch || !prm->srate || !prm->ptime)
		return EINVAL;

	st = mem_zalloc(sizeof(*st), auplay_destructor);
	if (!st)
		return ENOMEM;

	file = "speaker.wav";
	if (str_isset(device))
		file = device;

	aufprm.srate    = prm->srate;
	aufprm.channels = prm->ch;
	aufprm.fmt      = prm->fmt;
	err = aufile_open(&st->auf, &aufprm, file, AUFILE_WRITE);
	if (err) {
		warning("aufile: could not open %s for writing\n", file);
		return err;
	}

	st->ap  = ap;
	st->wh = wh;
	st->arg = arg;
	st->prm = *prm;
	st->sampc = st->prm.ch * st->prm.srate * st->prm.ptime / 1000;
	st->num_bytes = st->sampc * aufmt_sample_size(prm->fmt);
	st->sampv = mem_alloc(st->num_bytes, NULL);

	info("aufile: writing speaker audio to %s\n", file);
	err = pthread_create(&st->thread, NULL, write_thread, st);
	if (err) {
		st->run = false;
		goto out;
	}

out:
	if (err)
		mem_deref(st);
	else if (stp)
		*stp = st;

	return err;
}
