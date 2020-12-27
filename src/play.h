/**
 * @file play.h
 * @brief audio file player
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAPLAY_H_INCLUDED
#define UAPLAY_H_INCLUDED

#include "rsua-re/re.h"

struct play;
struct player;

int  play_file(struct play **playp, struct player *player,
	       const char *filename, int repeat,
	       const char *play_mod, const char *play_dev);
int  play_tone(struct play **playp, struct player *player,
	       struct mbuf *tone,
	       uint32_t srate, uint8_t ch, int repeat,
	       const char *play_mod, const char *play_dev);
int  play_init(struct player **playerp);
void play_set_path(struct player *player, const char *path);

#endif /* UAPLAY_H_INCLUDED */
