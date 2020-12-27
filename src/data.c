/**
 * @file data.c
 * @brief librsua shared data
 *
 * Copyright (C) 2010 - 2016 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#include "data.h"
#include "rsua-rem/rem.h"
#include <string.h>


/* global variables */

/** Core Run-time Configuration - populated from config file */
static struct config core_config = {

	/** SIP User-Agent */
	{
		"",
		"",
		"",
		""
	},

	/** Call config */
	{
		120,
		4
	},

	/** Audio */
	{
		SHARE_PATH,
		"","",
		"","",
		"","",
		0,
		0,
		0,
		0,
		AUDIO_MODE_POLL,
		false,
		AUFMT_S16LE,
		AUFMT_S16LE,
		AUFMT_S16LE,
		AUFMT_S16LE,
		{20, 160},
	},

	/** Video */
	{
		"", "",
		"", "",
		352, 288,
		500000,
		25,
		true,
		VID_FMT_YUV420P,
	},

	/** Audio/Video Transport */
	{
		0xb8,
		{1024, 49152},
		{0, 0},
		false,
		JBUF_FIXED,
		{5, 10},
		0,
		false,
		0
	},

	/* Network */
	{
		AF_UNSPEC,
		"",
		{ {"",0} },
		0
	},
};


/* Top-level struct that holds all other subsystems */
static struct alldata {
	struct data_subsys subsys;
	struct list mnatl;
	struct list mencl;
	struct list aucodecl;
	struct list ausrcl;
	struct list auplayl;
	struct list aufiltl;
	struct list vidcodecl;
	struct list vidsrcl;
	struct list vidispl;
	struct list vidfiltl;
} all;


int data_init(struct data_subsys *subsys)
{
	memset(&all, 0, sizeof(all));

	all.subsys = *subsys;
	list_init(&all.mnatl);
	list_init(&all.mencl);
	list_init(&all.aucodecl);
	list_init(&all.ausrcl);
	list_init(&all.auplayl);
	list_init(&all.aufiltl);
	list_init(&all.vidcodecl);
	list_init(&all.vidsrcl);
	list_init(&all.vidispl);
	list_init(&all.vidfiltl);

	return 0;
}


void data_close(void)
{
	/* all lists will be flushed by modules/components */

	all.subsys.uis = mem_deref(all.subsys.uis);
	all.subsys.message = mem_deref(all.subsys.message);
	all.subsys.player = mem_deref(all.subsys.player);
	all.subsys.commands = mem_deref(all.subsys.commands);
	all.subsys.contacts = mem_deref(all.subsys.contacts);
	all.subsys.net = mem_deref(all.subsys.net);
}


/**
 * Get the baresip core config
 *
 * @return Core config
 */
struct config *data_config(void)
{
	return &core_config;
}


/**
 * Get the network subsystem
 *
 * @return Network subsystem
 */
struct network *data_network(void)
{
	return all.subsys.net;
}


/**
 * Get the contacts subsystem
 *
 * @return Contacts subsystem
 */
struct contacts *data_contacts(void)
{
	return all.subsys.contacts;
}


/**
 * Get the commands subsystem
 *
 * @return Commands subsystem
 */
struct commands *data_commands(void)
{
	return all.subsys.commands;
}


/**
 * Get the audio player
 *
 * @return Audio player
 */
struct player *data_player(void)
{
	return all.subsys.player;
}


/**
 * Get the Message subsystem
 *
 * @return Message subsystem
 */
struct message *data_message(void)
{
	return all.subsys.message;
}


/**
 * Get the User Interface (UI) subsystem
 *
 * @return User Interface (UI) subsystem
 */
struct ui_sub *data_uis(void)
{
	return all.subsys.uis;
}


/**
 * Get the list of Media NATs
 *
 * @return List of Media NATs
 */
struct list *data_mnatl(void)
{
	return &all.mnatl;
}


/**
 * Get the list of Media encryptions
 *
 * @return List of Media encryptions
 */
struct list *data_mencl(void)
{
	return &all.mencl;
}


/**
 * Get the list of Audio Codecs
 *
 * @return List of audio-codecs
 */
struct list *data_aucodecl(void)
{
	return &all.aucodecl;
}


/**
 * Get the list of Audio Sources
 *
 * @return List of audio-sources
 */
struct list *data_ausrcl(void)
{
	return &all.ausrcl;
}


/**
 * Get the list of Audio Players
 *
 * @return List of audio-players
 */
struct list *data_auplayl(void)
{
	return &all.auplayl;
}


/**
 * Get the list of Audio Filters
 *
 * @return List of audio-filters
 */
struct list *data_aufiltl(void)
{
	return &all.aufiltl;
}


/**
 * Get the list of Video codecs
 *
 * @return List of video-codecs
 */
struct list *data_vidcodecl(void)
{
	return &all.vidcodecl;
}


/**
 * Get the list of Video sources
 *
 * @return List of video-sources
 */
struct list *data_vidsrcl(void)
{
	return &all.vidsrcl;
}


/**
 * Get the list of Video displays
 *
 * @return List of video-displays
 */
struct list *data_vidispl(void)
{
	return &all.vidispl;
}


/**
 * Get the list of Video filters
 *
 * @return List of video-filters
 */
struct list *data_vidfiltl(void)
{
	return &all.vidfiltl;
}
