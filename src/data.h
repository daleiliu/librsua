/**
 * @file data.h
 * @brief Shared data
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UADATA_H_INCLUDED
#define UADATA_H_INCLUDED

#include "rsua-re/re.h"

#ifndef NET_MAX_NS
#define NET_MAX_NS (4)
#endif

/*
 * Clock-rate for audio timestamp
 */
#define AUDIO_TIMEBASE 1000000U

/*
 * Clock-rate for video timestamp
 */
#define VIDEO_TIMEBASE 1000000U


/** A range of numbers */
struct range {
	uint32_t min;  /**< Minimum number */
	uint32_t max;  /**< Maximum number */
};

static inline bool in_range(const struct range *rng, uint32_t val)
{
	return rng ? (val >= rng->min && val <= rng->max) : false;
}

/** Audio transmit mode */
enum audio_mode {
	AUDIO_MODE_POLL = 0,         /**< Polling mode                  */
	AUDIO_MODE_THREAD,           /**< Use dedicated thread          */
};

/** SIP User-Agent */
struct config_sip {
	char uuid[64];          /**< Universally Unique Identifier  */
	char local[64];         /**< Local SIP Address              */
	char cert[256];         /**< SIP Certificate                */
	char cafile[256];       /**< SIP CA-file                    */
};

/** Call config */
struct config_call {
	uint32_t local_timeout; /**< Incoming call timeout [sec] 0=off    */
	uint32_t max_calls;     /**< Maximum number of calls, 0=unlimited */
};

/** Audio */
struct config_audio {
	char audio_path[256];   /**< Audio file directory           */
	char src_mod[16];       /**< Audio source module            */
	char src_dev[128];      /**< Audio source device            */
	char play_mod[16];      /**< Audio playback module          */
	char play_dev[128];     /**< Audio playback device          */
	char alert_mod[16];     /**< Audio alert module             */
	char alert_dev[128];    /**< Audio alert device             */
	uint32_t srate_play;    /**< Opt. sampling rate for player  */
	uint32_t srate_src;     /**< Opt. sampling rate for source  */
	uint32_t channels_play; /**< Opt. channels for player       */
	uint32_t channels_src;  /**< Opt. channels for source       */
	enum audio_mode txmode; /**< Audio transmit mode            */
	bool level;             /**< Enable audio level indication  */
	int src_fmt;            /**< Audio source sample format     */
	int play_fmt;           /**< Audio playback sample format   */
	int enc_fmt;            /**< Audio encoder sample format    */
	int dec_fmt;            /**< Audio decoder sample format    */
	struct range buffer;    /**< Audio receive buffer in [ms]   */
};

/** Video */
struct config_video {
	char src_mod[16];       /**< Video source module            */
	char src_dev[128];      /**< Video source device            */
	char disp_mod[16];      /**< Video display module           */
	char disp_dev[128];     /**< Video display device           */
	unsigned width, height; /**< Video resolution               */
	uint32_t bitrate;       /**< Encoder bitrate in [bit/s]     */
	double fps;             /**< Video framerate                */
	bool fullscreen;        /**< Enable fullscreen display      */
	int enc_fmt;            /**< Encoder pixelfmt (enum vidfmt) */
};

/** Audio/Video Transport */
struct config_avt {
	uint8_t rtp_tos;        /**< Type-of-Service for outg. RTP  */
	struct range rtp_ports; /**< RTP port range                 */
	struct range rtp_bw;    /**< RTP Bandwidth range [bit/s]    */
	bool rtcp_mux;          /**< RTP/RTCP multiplexing          */
	enum jbuf_type jbtype;  /**< Jitter buffer type             */
	struct range jbuf_del;  /**< Delay, number of frames        */
	uint32_t jbuf_wish;     /**< Startup wish delay of frames   */
	bool rtp_stats;         /**< Enable RTP statistics          */
	uint32_t rtp_timeout;   /**< RTP Timeout in seconds (0=off) */
};

/** Network Configuration */
struct config_net {
	int af;                 /**< AF_UNSPEC, AF_INET or AF_INET6 */
	char ifname[64];        /**< Bind to interface (optional)   */
	struct {
		char addr[64];
		bool fallback;
	} nsv[NET_MAX_NS];      /**< Configured DNS nameservers     */
	size_t nsc;             /**< Number of DNS nameservers      */
};


/** Core configuration */
struct config {

	struct config_sip sip;

	struct config_call call;

	struct config_audio audio;

	struct config_video video;
	struct config_avt avt;

	struct config_net net;
};

struct config *data_config(void);

struct network *data_network(void);
struct contacts *data_contacts(void);
struct commands *data_commands(void);
struct player *data_player(void);
struct message *data_message(void);
struct ui_sub *data_uis(void);
struct list   *data_mnatl(void);
struct list   *data_mencl(void);
struct list   *data_aucodecl(void);
struct list   *data_ausrcl(void);
struct list   *data_auplayl(void);
struct list   *data_aufiltl(void);
struct list   *data_vidcodecl(void);
struct list   *data_vidsrcl(void);
struct list   *data_vidispl(void);
struct list   *data_vidfiltl(void);


#ifndef UAMODAPI_USE		/* Internal API */

struct network;
struct contacts;
struct commands;
struct player;
struct message;
struct ui_sub;

struct data_subsys {
	struct network *net;
	struct contacts *contacts;
	struct commands *commands;
	struct player *player;
	struct message *message;
	struct ui_sub *uis;
};

int data_init(struct data_subsys *subsys);
void data_close(void);


#include <limits.h>

/* max bytes in pathname */
#if defined (PATH_MAX)
#define FS_PATH_MAX PATH_MAX
#elif defined (_POSIX_PATH_MAX)
#define FS_PATH_MAX _POSIX_PATH_MAX
#else
#define FS_PATH_MAX 512
#endif


/**
 * RFC 3551:
 *
 *    0 -  95  Static payload types
 *   96 - 127  Dynamic payload types
 */
enum {
	PT_CN       = 13,
};


/** Media constants */
enum {
	AUDIO_BANDWIDTH = 128000,  /**< Bandwidth for audio in bits/s      */
	VIDEO_SRATE     =  90000,  /**< Sampling rate for video            */
};

#endif /* ifndef UAMODAPI_USE */

#endif /* UADATA_H_INCLUDED */
