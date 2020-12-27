/**
 * @file conf.c  Configuration utils and Core Configuration
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#define _DEFAULT_SOURCE 1
#define _BSD_SOURCE 1
#include "conf.h"
#include <fcntl.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdio.h>
#include <sys/stat.h>
#ifdef HAVE_IO_H
#include <io.h>
#endif
#include <string.h>
#include <dirent.h>

#define DEBUG_MODULE ""
#define DEBUG_LEVEL 0
#include "rsua-re/re_dbg.h"

#include "rsua-rem/rem.h"
#include "data.h"
#include "module.h"
#include "log.h"


#ifdef WIN32
#define open _open
#define read _read
#define close _close
#endif


#if defined (WIN32)
#define DIR_SEP "\\"
#else
#define DIR_SEP "/"
#endif


#ifndef PREFIX
#define PREFIX "/usr"
#endif


static const char *conf_path = NULL;
static struct conf *conf_obj;


/**
 * Check if a file exists
 *
 * @param path Filename
 *
 * @return True if exist, False if not
 */
bool conf_fileexist(const char *path)
{
	struct stat st;

	if (!path)
		 return false;

	if (stat(path, &st) < 0)
		 return false;

	if ((st.st_mode & S_IFMT) != S_IFREG)
		 return false;

	return true;
}


static void print_populated(const char *what, uint32_t n)
{
	info("Populated %u %s%s\n", n, what, 1==n ? "" : "s");
}


/**
 * Parse a config file, calling handler for each line
 *
 * @param filename Config file
 * @param ch       Line handler
 * @param arg      Handler argument
 *
 * @return 0 if success, otherwise errorcode
 */
int conf_parse(const char *filename, confline_h *ch, void *arg)
{
	struct pl pl, val;
	struct mbuf *mb;
	int err = 0, fd = open(filename, O_RDONLY);
	if (fd < 0)
		return errno;

	mb = mbuf_alloc(1024);
	if (!mb) {
		err = ENOMEM;
		goto out;
	}

	for (;;) {
		uint8_t buf[1024];

		const ssize_t n = read(fd, (void *)buf, sizeof(buf));
		if (n < 0) {
			err = errno;
			break;
		}
		else if (n == 0)
			break;

		err |= mbuf_write_mem(mb, buf, n);
	}

	pl.p = (const char *)mb->buf;
	pl.l = mb->end;

	while (pl.p < ((const char *)mb->buf + mb->end) && !err) {
		const char *lb = pl_strchr(&pl, '\n');

		val.p = pl.p;
		val.l = lb ? (uint32_t)(lb - pl.p) : pl.l;
		pl_advance(&pl, val.l + 1);

		if (!val.l || val.p[0] == '#')
			continue;

		err = ch(&val, arg);
	}

 out:
	mem_deref(mb);
	(void)close(fd);

	return err;
}


/**
 * Set the path to configuration files
 *
 * @param path Configuration path
 */
void conf_path_set(const char *path)
{
	conf_path = path;
}


/**
 * Get the path to configuration files
 *
 * @param path Buffer to write path
 * @param sz   Size of path buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int conf_path_get(char *path, size_t sz)
{
	char buf[FS_PATH_MAX];
	int err;

	/* Use explicit conf path */
	if (conf_path) {
		if (re_snprintf(path, sz, "%s", conf_path) < 0)
			return ENOMEM;
		return 0;
	}

#ifdef CONFIG_PATH
	str_ncpy(buf, CONFIG_PATH, sizeof(buf));
	(void)err;
#else
	err = fs_gethome(buf, sizeof(buf));
	if (err)
		return err;
#endif

	if (re_snprintf(path, sz, "%s" DIR_SEP ".baresip", buf) < 0)
		return ENOMEM;

	return 0;
}


int conf_get_range(const struct conf *conf, const char *name,
		   struct range *rng)
{
	struct pl r, min, max;
	uint32_t v;
	int err;

	err = conf_get(conf, name, &r);
	if (err)
		return err;

	err = re_regex(r.p, r.l, "[0-9]+-[0-9]+", &min, &max);
	if (err) {
		/* fallback to non-range numeric value */
		err = conf_get_u32(conf, name, &v);
		if (err) {
			warning("conf: %s: could not parse range: (%r)\n",
				name, &r);
			return err;
		}

		rng->min = rng->max = v;

		return err;
	}

	rng->min = pl_u32(&min);
	rng->max = pl_u32(&max);

	if (rng->min > rng->max) {
		warning("conf: %s: invalid range (%u - %u)\n",
			name, rng->min, rng->max);
		return EINVAL;
	}

	return 0;
}


int conf_get_csv(const struct conf *conf, const char *name,
		 char *str1, size_t sz1, char *str2, size_t sz2)
{
	struct pl r, pl1, pl2 = pl_null;
	int err;

	err = conf_get(conf, name, &r);
	if (err)
		return err;

	/* note: second value may be quoted */
	err = re_regex(r.p, r.l, "[^,]+,[~]*", &pl1, &pl2);
	if (err)
		return err;

	(void)pl_strcpy(&pl1, str1, sz1);
	if (pl_isset(&pl2))
		(void)pl_strcpy(&pl2, str2, sz2);

	return 0;
}


/**
 * Get the video size of a configuration item
 *
 * @param conf Configuration object
 * @param name Name of config item key
 * @param sz   Returned video size of config item, if present
 *
 * @return 0 if success, otherwise errorcode
 */
int conf_get_vidsz(const struct conf *conf, const char *name, struct vidsz *sz)
{
	struct pl r, w, h;
	int err;

	err = conf_get(conf, name, &r);
	if (err)
		return err;

	w.l = h.l = 0;
	err = re_regex(r.p, r.l, "[0-9]+x[0-9]+", &w, &h);
	if (err)
		return err;

	if (pl_isset(&w) && pl_isset(&h)) {
		sz->w = pl_u32(&w);
		sz->h = pl_u32(&h);
	}

	/* check resolution */
	if (sz->w & 0x1 || sz->h & 0x1) {
		warning("conf: %s: should be multiple of 2 (%u x %u)\n",
			name, sz->w, sz->h);
		return EINVAL;
	}

	return 0;
}


/**
 * Get the socket address of a configuration item
 *
 * @param conf Configuration object
 * @param name Name of config item key
 * @param sa   Returned socket address of config item, if present
 *
 * @return 0 if success, otherwise errorcode
 */
int conf_get_sa(const struct conf *conf, const char *name, struct sa *sa)
{
	struct pl opt;
	int err;

	if (!conf || !name || !sa)
		return EINVAL;

	err = conf_get(conf, name, &opt);
	if (err)
		return err;

	return sa_decode(sa, opt.p, opt.l);
}


int conf_get_float(const struct conf *conf, const char *name, double *val)
{
	struct pl opt;
	int err;

	if (!conf || !name || !val)
		return EINVAL;

	err = conf_get(conf, name, &opt);
	if (err)
		return err;

	*val = pl_float(&opt);

	return 0;
}


/**
 * Configure the system with default settings
 *
 * @return 0 if success, otherwise errorcode
 */
int conf_configure(void)
{
	char path[FS_PATH_MAX], file[FS_PATH_MAX];
	int err;

#if defined (WIN32)
	dbg_init(DBG_INFO, DBG_NONE);
#endif

	err = conf_path_get(path, sizeof(path));
	if (err) {
		warning("conf: could not get config path: %m\n", err);
		return err;
	}

	if (re_snprintf(file, sizeof(file), "%s/config", path) < 0)
		return ENOMEM;

	if (!conf_fileexist(file)) {

		(void)fs_mkdir(path, 0700);

		err = config_write_template(file, data_config());
		if (err)
			goto out;
	}

	conf_obj = mem_deref(conf_obj);
	err = conf_alloc(&conf_obj, file);
	if (err)
		goto out;

	err = config_parse_conf(data_config(), conf_obj);
	if (err)
		goto out;

 out:
	return err;
}


/**
 * Configure the system from a buffer
 *
 * @param buf Buffer with config
 * @param sz  Size of buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int conf_configure_buf(const uint8_t *buf, size_t sz)
{
	int err;

	if (!buf || !sz)
		return EINVAL;

	conf_obj = mem_deref(conf_obj);

	err = conf_alloc_buf(&conf_obj, buf, sz);
	if (err)
		return err;

	return 0;
}


/**
 * Load all modules from config file
 *
 * @return 0 if success, otherwise errorcode
 *
 * @note conf_configure must be called first
 */
int conf_modules(void)
{
	int err;

	err = module_init(conf_obj);
	if (err) {
		warning("conf: configure module parse error (%m)\n", err);
		goto out;
	}

	print_populated("audio codec",  list_count(data_aucodecl()));
	print_populated("audio filter", list_count(data_aufiltl()));
	print_populated("video codec",  list_count(data_vidcodecl()));
	print_populated("video filter", list_count(data_vidfiltl()));

 out:
	return err;
}


/**
 * Get the current configuration object
 *
 * @return Config object
 *
 * @note It is only available after init and before conf_close()
 */
struct conf *conf_cur(void)
{
	if (!conf_obj) {
		warning("conf: no config object\n");
	}
	return conf_obj;
}


/**
 * Close the current configuration object
 */
void conf_close(void)
{
	conf_obj = mem_deref(conf_obj);
}


static int range_print(struct re_printf *pf, const struct range *rng)
{
	if (!rng)
		return 0;

	return re_hprintf(pf, "%u-%u", rng->min, rng->max);
}


static int dns_handler(const struct pl *pl, void *arg, bool fallback)
{
	struct config_net *cfg = arg;
	const size_t max_count = ARRAY_SIZE(cfg->nsv);
	int err;

	if (cfg->nsc >= max_count) {
		warning("config: too many DNS nameservers (max %zu)\n",
			max_count);
		return EOVERFLOW;
	}

	/* Append dns_server to the network config */
	err = pl_strcpy(pl, cfg->nsv[cfg->nsc].addr,
			sizeof(cfg->nsv[0].addr));

	cfg->nsv[cfg->nsc].fallback = fallback;

	if (err) {
		warning("config: dns_server: could not copy string (%r)\n",
			pl);
		return err;
	}

	++cfg->nsc;

	return 0;
}


static int dns_server_handler(const struct pl *pl, void *arg)
{
	int err;

	err = dns_handler(pl, arg, false);

	return err;
}


static int dns_fallback_handler(const struct pl *pl, void *arg)
{
	int err;

	err = dns_handler(pl, arg, true);

	return err;
}


static enum aufmt resolve_aufmt(const struct pl *fmt)
{
	if (0 == pl_strcasecmp(fmt, "s16"))     return AUFMT_S16LE;
	if (0 == pl_strcasecmp(fmt, "s16le"))   return AUFMT_S16LE;
	if (0 == pl_strcasecmp(fmt, "float"))   return AUFMT_FLOAT;
	if (0 == pl_strcasecmp(fmt, "s24_3le")) return AUFMT_S24_3LE;

	return (enum aufmt)-1;
}


static int conf_get_aufmt(const struct conf *conf, const char *name,
			  int *fmtp)
{
	struct pl pl;
	int fmt;
	int err;

	err = conf_get(conf, name, &pl);
	if (err)
		return err;

	fmt = resolve_aufmt(&pl);
	if (fmt == -1) {
		warning("config: %s: sample format not supported"
			" (%r)\n", name, &pl);
		return EINVAL;
	}

	*fmtp = fmt;

	return 0;
}


static int conf_get_vidfmt(const struct conf *conf, const char *name,
			   int *fmtp)
{
	struct pl pl;
	int fmt;
	int err;

	err = conf_get(conf, name, &pl);
	if (err)
		return err;

	for (fmt=0; fmt<VID_FMT_N; fmt++) {

		const char *str = vidfmt_name(fmt);

		if (0 == pl_strcasecmp(&pl, str)) {

			*fmtp = fmt;
			return 0;
		}
	}

	warning("config: %s: pixel format not supported (%r)\n", name, &pl);

	return ENOENT;
}


static const char *jbuf_type_str(enum jbuf_type jbtype)
{
	switch (jbtype) {
	case JBUF_OFF:
		return "off";
	case JBUF_FIXED:
		return "fixed";
	case JBUF_ADAPTIVE:
		return "adaptive";
	}

	return "?";
}


static enum jbuf_type resolve_jbuf_type(const struct pl *pl)
{
	if (0 == pl_strcasecmp(pl, "off"))      return JBUF_OFF;
	if (0 == pl_strcasecmp(pl, "fixed"))    return JBUF_FIXED;
	if (0 == pl_strcasecmp(pl, "adaptive")) return JBUF_ADAPTIVE;

	warning("unsupported jitter buffer type (%r)\n", pl);
	return JBUF_FIXED;
}


/**
 * Parse the core configuration file and update baresip core config
 *
 * @param cfg  Baresip core config to update
 * @param conf Configuration file to parse
 *
 * @return 0 if success, otherwise errorcode
 */
int config_parse_conf(struct config *cfg, const struct conf *conf)
{
	struct pl pollm;
	enum poll_method method;
	struct vidsz size = {0, 0};
	struct pl txmode;
	struct pl jbtype;
	uint32_t v;
	int err = 0;

	if (!cfg || !conf)
		return EINVAL;

	/* Core */
	if (0 == conf_get(conf, "poll_method", &pollm)) {
		if (0 == poll_method_type(&method, &pollm)) {
			err = poll_method_set(method);
			if (err) {
				warning("config: poll method (%r) set: %m\n",
					&pollm, err);
			}
		}
		else {
			warning("config: unknown poll method (%r)\n", &pollm);
		}
	}

	/* SIP */
	(void)conf_get_str(conf, "sip_listen", cfg->sip.local,
			   sizeof(cfg->sip.local));
	(void)conf_get_str(conf, "sip_certificate", cfg->sip.cert,
			   sizeof(cfg->sip.cert));
	(void)conf_get_str(conf, "sip_cafile", cfg->sip.cafile,
			   sizeof(cfg->sip.cafile));

	/* Call */
	(void)conf_get_u32(conf, "call_local_timeout",
			   &cfg->call.local_timeout);
	(void)conf_get_u32(conf, "call_max_calls",
			   &cfg->call.max_calls);

	/* Audio */
	(void)conf_get_str(conf, "audio_path", cfg->audio.audio_path,
			   sizeof(cfg->audio.audio_path));
	(void)conf_get_csv(conf, "audio_player",
			   cfg->audio.play_mod,
			   sizeof(cfg->audio.play_mod),
			   cfg->audio.play_dev,
			   sizeof(cfg->audio.play_dev));

	(void)conf_get_csv(conf, "audio_source",
			   cfg->audio.src_mod, sizeof(cfg->audio.src_mod),
			   cfg->audio.src_dev, sizeof(cfg->audio.src_dev));

	(void)conf_get_csv(conf, "audio_alert",
			   cfg->audio.alert_mod,
			   sizeof(cfg->audio.alert_mod),
			   cfg->audio.alert_dev,
			   sizeof(cfg->audio.alert_dev));

	(void)conf_get_u32(conf, "ausrc_srate", &cfg->audio.srate_src);
	(void)conf_get_u32(conf, "auplay_srate", &cfg->audio.srate_play);
	(void)conf_get_u32(conf, "ausrc_channels", &cfg->audio.channels_src);
	(void)conf_get_u32(conf, "auplay_channels", &cfg->audio.channels_play);

	if (0 == conf_get(conf, "audio_txmode", &txmode)) {

		if (0 == pl_strcasecmp(&txmode, "poll"))
			cfg->audio.txmode = AUDIO_MODE_POLL;
		else if (0 == pl_strcasecmp(&txmode, "thread"))
			cfg->audio.txmode = AUDIO_MODE_THREAD;
		else {
			warning("unsupported audio txmode (%r)\n", &txmode);
		}
	}

	(void)conf_get_bool(conf, "audio_level", &cfg->audio.level);

	conf_get_aufmt(conf, "ausrc_format", &cfg->audio.src_fmt);
	conf_get_aufmt(conf, "auplay_format", &cfg->audio.play_fmt);
	conf_get_aufmt(conf, "auenc_format", &cfg->audio.enc_fmt);
	conf_get_aufmt(conf, "audec_format", &cfg->audio.dec_fmt);

	conf_get_range(conf, "audio_buffer", &cfg->audio.buffer);
	if (!cfg->audio.buffer.min || !cfg->audio.buffer.max) {
		warning("config: audio_buffer cannot be zero\n");
		return EINVAL;
	}

	/* Video */
	(void)conf_get_csv(conf, "video_source",
			   cfg->video.src_mod, sizeof(cfg->video.src_mod),
			   cfg->video.src_dev, sizeof(cfg->video.src_dev));
	(void)conf_get_csv(conf, "video_display",
			   cfg->video.disp_mod, sizeof(cfg->video.disp_mod),
			   cfg->video.disp_dev, sizeof(cfg->video.disp_dev));
	if (0 == conf_get_vidsz(conf, "video_size", &size)) {
		cfg->video.width  = size.w;
		cfg->video.height = size.h;
	}
	(void)conf_get_u32(conf, "video_bitrate", &cfg->video.bitrate);
	(void)conf_get_float(conf, "video_fps", &cfg->video.fps);
	(void)conf_get_bool(conf, "video_fullscreen", &cfg->video.fullscreen);

	conf_get_vidfmt(conf, "videnc_format", &cfg->video.enc_fmt);

	/* AVT - Audio/Video Transport */
	if (0 == conf_get_u32(conf, "rtp_tos", &v))
		cfg->avt.rtp_tos = v;
	(void)conf_get_range(conf, "rtp_ports", &cfg->avt.rtp_ports);
	if (0 == conf_get_range(conf, "rtp_bandwidth",
				&cfg->avt.rtp_bw)) {
		cfg->avt.rtp_bw.min *= 1000;
		cfg->avt.rtp_bw.max *= 1000;
	}

	(void)conf_get_bool(conf, "rtcp_mux", &cfg->avt.rtcp_mux);
	if (0 == conf_get(conf, "jitter_buffer_type", &jbtype))
		cfg->avt.jbtype = resolve_jbuf_type(&jbtype);

	(void)conf_get_range(conf, "jitter_buffer_delay",
			     &cfg->avt.jbuf_del);
	(void)conf_get_u32(conf, "jitter_buffer_wish",
			     &cfg->avt.jbuf_wish);
	(void)conf_get_bool(conf, "rtp_stats", &cfg->avt.rtp_stats);
	(void)conf_get_u32(conf, "rtp_timeout", &cfg->avt.rtp_timeout);

	if (err) {
		warning("config: configure parse error (%m)\n", err);
	}

	/* Network */
	(void)conf_apply(conf, "dns_server", dns_server_handler, &cfg->net);
	(void)conf_apply(conf, "dns_fallback",
			   dns_fallback_handler, &cfg->net);
	(void)conf_get_str(conf, "net_interface",
			   cfg->net.ifname, sizeof(cfg->net.ifname));

	return err;
}


/**
 * Print the baresip core config
 *
 * @param pf  Print function
 * @param cfg Baresip core config
 *
 * @return 0 if success, otherwise errorcode
 */
int config_print(struct re_printf *pf, const struct config *cfg)
{
	int err;

	if (!cfg)
		return 0;

	err = re_hprintf(pf,
			 "\n"
			 "# SIP\n"
			 "sip_listen\t\t%s\n"
			 "sip_certificate\t%s\n"
			 "sip_cafile\t\t%s\n"
			 "\n"
			 "# Call\n"
			 "call_local_timeout\t%u\n"
			 "call_max_calls\t\t%u\n"
			 "\n"
			 "# Audio\n"
			 "audio_path\t\t%s\n"
			 "audio_player\t\t%s,%s\n"
			 "audio_source\t\t%s,%s\n"
			 "audio_alert\t\t%s,%s\n"
			 "auplay_srate\t\t%u\n"
			 "ausrc_srate\t\t%u\n"
			 "auplay_channels\t\t%u\n"
			 "ausrc_channels\t\t%u\n"
			 "audio_level\t\t%s\n"
			 "\n"
			 "# Video\n"
			 "video_source\t\t%s,%s\n"
			 "video_display\t\t%s,%s\n"
			 "video_size\t\t\"%ux%u\"\n"
			 "video_bitrate\t\t%u\n"
			 "video_fps\t\t%.2f\n"
			 "video_fullscreen\t%s\n"
			 "videnc_format\t\t%s\n"
			 "\n"
			 "# AVT\n"
			 "rtp_tos\t\t\t%u\n"
			 "rtp_ports\t\t%H\n"
			 "rtp_bandwidth\t\t%H\n"
			 "rtcp_mux\t\t%s\n"
			 "jitter_buffer_type\t%s\n"
			 "jitter_buffer_delay\t%H\n"
			 "jitter_buffer_wish\t%u\n"
			 "rtp_stats\t\t%s\n"
			 "rtp_timeout\t\t%u # in seconds\n"
			 "\n"
			 "# Network\n"
			 "net_interface\t\t%s\n"
			 "\n"
			 ,

			 cfg->sip.local, cfg->sip.cert, cfg->sip.cafile,

			 cfg->call.local_timeout,
			 cfg->call.max_calls,

			 cfg->audio.audio_path,
			 cfg->audio.play_mod,  cfg->audio.play_dev,
			 cfg->audio.src_mod,   cfg->audio.src_dev,
			 cfg->audio.alert_mod, cfg->audio.alert_dev,
			 cfg->audio.srate_play, cfg->audio.srate_src,
			 cfg->audio.channels_play, cfg->audio.channels_src,
			 cfg->audio.level ? "yes" : "no",

			 cfg->video.src_mod, cfg->video.src_dev,
			 cfg->video.disp_mod, cfg->video.disp_dev,
			 cfg->video.width, cfg->video.height,
			 cfg->video.bitrate, cfg->video.fps,
			 cfg->video.fullscreen ? "yes" : "no",
			 vidfmt_name(cfg->video.enc_fmt),

			 cfg->avt.rtp_tos,
			 range_print, &cfg->avt.rtp_ports,
			 range_print, &cfg->avt.rtp_bw,
			 cfg->avt.rtcp_mux ? "yes" : "no",
			 jbuf_type_str(cfg->avt.jbtype),
			 range_print, &cfg->avt.jbuf_del,
			 cfg->avt.jbuf_wish,
			 cfg->avt.rtp_stats ? "yes" : "no",
			 cfg->avt.rtp_timeout,

			 cfg->net.ifname
		   );

	return err;
}


static const char *default_cafile(void)
{
#if defined (DEFAULT_CAFILE)
	return DEFAULT_CAFILE;
#elif defined (DARWIN)
	return "/etc/ssl/cert.pem";
#else
	return "/etc/ssl/certs/ca-certificates.crt";
#endif
}


static const char *default_audio_device(void)
{
#if defined (DEFAULT_AUDIO_DEVICE)
	return DEFAULT_AUDIO_DEVICE;
#elif defined (ANDROID)
	return "opensles,nil";
#elif defined (DARWIN)
	return "coreaudio,default";
#elif defined (FREEBSD)
	return "oss,/dev/dsp";
#elif defined (OPENBSD)
	return "sndio,default";
#elif defined (WIN32)
	return "winwave,nil";
#else
	return "alsa,default";
#endif
}


static const char *default_video_device(void)
{
#ifdef DARWIN
	return "avcapture,nil";
#elif defined (WIN32)
	return "dshow,nil";
#else
	return "v4l2,/dev/video0";
#endif
}


static const char *default_video_display(void)
{
#ifdef DARWIN
	return "sdl,nil";
#elif defined (WIN32)
	return "sdl,nil";
#else
	return "x11,nil";
#endif
}


static const char *default_avcodec_hwaccel(void)
{
#if defined (LINUX)
	return "vaapi";
#elif defined (DARWIN)
	return "videotoolbox";
#else
	return "none";
#endif

}


static int default_interface_print(struct re_printf *pf, void *unused)
{
	char ifname[64];
	(void)unused;

	if (0 == net_rt_default_get(AF_INET, ifname, sizeof(ifname)))
		return re_hprintf(pf, "%s", ifname);
	else
		return re_hprintf(pf, "eth0");
}


static int core_config_template(struct re_printf *pf, const struct config *cfg)
{
	int err = 0;

	if (!cfg)
		return 0;

	err |= re_hprintf(pf,
			  "\n# Core\n"
			  "poll_method\t\t%s\t\t# poll, select"
#ifdef HAVE_EPOLL
				", epoll .."
#endif
#ifdef HAVE_KQUEUE
				", kqueue .."
#endif
				"\n"
			  "\n# SIP\n"
			  "#sip_listen\t\t0.0.0.0:5060\n"
			  "#sip_certificate\tcert.pem\n"
			  "#sip_cafile\t\t%s\n"
			  "\n"
			  "# Call\n"
			  "call_local_timeout\t%u\n"
			  "call_max_calls\t\t%u\n"
			  "\n"
			  "# Audio\n"
#if defined (SHARE_PATH)
			  "#audio_path\t\t" SHARE_PATH "\n"
#elif defined (PREFIX)
			  "#audio_path\t\t" PREFIX "/share/baresip\n"
#else
			  "#audio_path\t\t/usr/share/baresip\n"
#endif
			  "audio_player\t\t%s\n"
			  "audio_source\t\t%s\n"
			  "audio_alert\t\t%s\n"
			  "#ausrc_srate\t\t48000\n"
			  "#auplay_srate\t\t48000\n"
			  "#ausrc_channels\t\t0\n"
			  "#auplay_channels\t0\n"
			  "#audio_txmode\t\tpoll\t\t# poll, thread\n"
			  "audio_level\t\tno\n"
			  "ausrc_format\t\ts16\t\t# s16, float, ..\n"
			  "auplay_format\t\ts16\t\t# s16, float, ..\n"
			  "auenc_format\t\ts16\t\t# s16, float, ..\n"
			  "audec_format\t\ts16\t\t# s16, float, ..\n"
			  "audio_buffer\t\t%H\t\t# ms\n"
			  ,
			  poll_method_name(poll_method_best()),
			  default_cafile(),
			  cfg->call.local_timeout,
			  cfg->call.max_calls,
			  default_audio_device(),
			  default_audio_device(),
			  default_audio_device(),
			  range_print, &cfg->audio.buffer);

	err |= re_hprintf(pf,
			  "\n# Video\n"
			  "#video_source\t\t%s\n"
			  "#video_display\t\t%s\n"
			  "video_size\t\t%dx%d\n"
			  "video_bitrate\t\t%u\n"
			  "video_fps\t\t%.2f\n"
			  "video_fullscreen\tno\n"
			  "videnc_format\t\t%s\n"
			  ,
			  default_video_device(),
			  default_video_display(),
			  cfg->video.width, cfg->video.height,
			  cfg->video.bitrate, cfg->video.fps,
			  vidfmt_name(cfg->video.enc_fmt));

	err |= re_hprintf(pf,
			  "\n# AVT - Audio/Video Transport\n"
			  "rtp_tos\t\t\t184\n"
			  "#rtp_ports\t\t10000-20000\n"
			  "#rtp_bandwidth\t\t512-1024 # [kbit/s]\n"
			  "rtcp_mux\t\tno\n"
			  "jitter_buffer_type\tfixed\t\t# off, fixed,"
				" adaptive\n"
			  "jitter_buffer_delay\t%u-%u\t\t# frames\n"
			  "#jitter_buffer_wish\t%u\t\t# frames for start\n"
			  "rtp_stats\t\tno\n"
			  "#rtp_timeout\t\t60\n"
			  "\n# Network\n"
			  "#dns_server\t\t1.1.1.1:53\n"
			  "#dns_server\t\t1.0.0.1:53\n"
			  "#dns_fallback\t\t8.8.8.8:53\n"
			  "#net_interface\t\t%H\n"
			  "\n"
			  "# Play tones\n"
			  "#file_ausrc\t\taufile\n"
			  "#file_srate\t\t16000\n"
			  "#file_channels\t\t1\n",
			  cfg->avt.jbuf_del.min, cfg->avt.jbuf_del.max,
			  cfg->avt.jbuf_del.min + 1,
			  default_interface_print, NULL);

	return err;
}


static uint32_t count_modules(const char *path)
{
	DIR *dirp;
	struct dirent *dp;
	uint32_t n = 0;

	dirp = opendir(path);
	if (!dirp)
		return 0;

	while ((dp = readdir(dirp)) != NULL) {

		size_t len = strlen(dp->d_name);
		const size_t x = sizeof(MOD_EXT)-1;

		if (len <= x)
			continue;

		if (0==memcmp(&dp->d_name[len-x], MOD_EXT, x))
			++n;
	}

	(void)closedir(dirp);

	return n;
}


static const char *detect_module_path(bool *valid)
{
	static const char * const pathv[] = {
#if defined (MOD_PATH)
		MOD_PATH,
#elif defined (PREFIX)
		"" PREFIX "/lib/baresip/modules",
#else
		"/usr/local/lib/baresip/modules",
		"/usr/lib/baresip/modules",
#endif
	};
	const char *current = pathv[0];
	uint32_t nmax = 0;
	size_t i;

	for (i=0; i<ARRAY_SIZE(pathv); i++) {

		uint32_t n = count_modules(pathv[i]);

		info("%s: detected %u modules\n", pathv[i], n);

		if (n > nmax) {
			nmax = n;
			current = pathv[i];
		}
	}

	if (nmax > 0)
		*valid = true;

	return current;
}


/**
 * Write the baresip core config template to a file
 *
 * @param file Filename of output file
 * @param cfg  Baresip core config
 *
 * @return 0 if success, otherwise errorcode
 */
int config_write_template(const char *file, const struct config *cfg)
{
	FILE *f = NULL;
	int err = 0;
	const char *modpath;
	bool modpath_valid = false;

	if (!file || !cfg)
		return EINVAL;

	info("config: creating config template %s\n", file);

	f = fopen(file, "w");
	if (!f) {
		warning("config: writing %s: %m\n", file, errno);
		return errno;
	}

	(void)re_fprintf(f,
			 "#\n"
			 "# baresip configuration\n"
			 "#\n"
			 "\n"
			 "#------------------------------------"
			 "------------------------------------------\n");

	(void)re_fprintf(f, "%H", core_config_template, cfg);

	(void)re_fprintf(f,
			 "\n#------------------------------------"
			 "------------------------------------------\n"
			 "# Modules\n"
			 "\n");

	modpath = detect_module_path(&modpath_valid);
	(void)re_fprintf(f, "%smodule_path\t\t%s\n",
			 modpath_valid ? "" : "#", modpath);

	(void)re_fprintf(f, "\n# UI Modules\n");
#if defined (WIN32)
	(void)re_fprintf(f, "module\t\t\t" "wincons" MOD_EXT "\n");
#else
	(void)re_fprintf(f, "module\t\t\t" "stdio" MOD_EXT "\n");
#endif
	(void)re_fprintf(f, "#module\t\t\t" "cons" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "evdev" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "httpd" MOD_EXT "\n");

	(void)re_fprintf(f, "\n# Audio codec Modules (in order)\n");
	(void)re_fprintf(f, "#module\t\t\t" "opus" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "amr" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "g7221" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "g722" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "g726" MOD_EXT "\n");
	(void)re_fprintf(f, "module\t\t\t" "g711" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "gsm" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "l16" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "mpa" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "codec2" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "ilbc" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "isac" MOD_EXT "\n");

	(void)re_fprintf(f, "\n# Audio filter Modules (in encoding order)\n");
	(void)re_fprintf(f, "#module\t\t\t" "vumeter" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "sndfile" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "speex_pp" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "plc" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "webrtc_aec" MOD_EXT "\n");

	(void)re_fprintf(f, "\n# Audio driver Modules\n");
#if defined (ANDROID)
	(void)re_fprintf(f, "module\t\t\t" "opensles" MOD_EXT "\n");
#elif defined (DARWIN)
	(void)re_fprintf(f, "module\t\t\t" "coreaudio" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "audiounit" MOD_EXT "\n");
#elif defined (FREEBSD)
	(void)re_fprintf(f, "module\t\t\t" "oss" MOD_EXT "\n");
#elif defined (OPENBSD)
	(void)re_fprintf(f, "module\t\t\t" "sndio" MOD_EXT "\n");
#elif defined (WIN32)
	(void)re_fprintf(f, "module\t\t\t" "winwave" MOD_EXT "\n");
#else
	if (!strncmp(default_audio_device(), "pulse", 5)) {
		(void)re_fprintf(f, "#module\t\t\t" "alsa" MOD_EXT "\n");
		(void)re_fprintf(f, "module\t\t\t" "pulse" MOD_EXT "\n");
	}
	else {
		(void)re_fprintf(f, "module\t\t\t" "alsa" MOD_EXT "\n");
		(void)re_fprintf(f, "#module\t\t\t" "pulse" MOD_EXT "\n");
	}
#endif
	(void)re_fprintf(f, "#module\t\t\t" "jack" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "portaudio" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "aubridge" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "aufile" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "ausine" MOD_EXT "\n");


	(void)re_fprintf(f, "\n# Video codec Modules (in order)\n");
	(void)re_fprintf(f, "#module\t\t\t" "avcodec" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "vp8" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "vp9" MOD_EXT "\n");

	(void)re_fprintf(f, "\n# Video filter Modules (in encoding order)\n");
	(void)re_fprintf(f, "#module\t\t\t" "selfview" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "snapshot" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "swscale" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "vidinfo" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "avfilter" MOD_EXT "\n");

	(void)re_fprintf(f, "\n# Video source modules\n");
#if defined (DARWIN)

	(void)re_fprintf(f, "module\t\t\t" "avcapture" MOD_EXT "\n");

#elif defined (WIN32)
	(void)re_fprintf(f, "module\t\t\t" "dshow" MOD_EXT "\n");

#else
	(void)re_fprintf(f, "#module\t\t\t" "v4l2" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "v4l2_codec" MOD_EXT "\n");
#endif
	(void)re_fprintf(f, "#module\t\t\t" "x11grab" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "cairo" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "vidbridge" MOD_EXT "\n");

	(void)re_fprintf(f, "\n# Video display modules\n");
#ifdef LINUX
	(void)re_fprintf(f, "#module\t\t\t" "directfb" MOD_EXT "\n");
#endif
	(void)re_fprintf(f, "#module\t\t\t" "x11" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "sdl" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "fakevideo" MOD_EXT "\n");


	(void)re_fprintf(f, "\n# Audio/Video source modules\n");
	(void)re_fprintf(f, "#module\t\t\t" "avformat" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "rst" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "gst" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "gst_video" MOD_EXT "\n");

	(void)re_fprintf(f, "\n# Compatibility modules\n");
	(void)re_fprintf(f, "#module\t\t\t" "ebuacip" MOD_EXT "\n");

	(void)re_fprintf(f, "\n# Media NAT modules\n");
	(void)re_fprintf(f, "module\t\t\t" "stun" MOD_EXT "\n");
	(void)re_fprintf(f, "module\t\t\t" "turn" MOD_EXT "\n");
	(void)re_fprintf(f, "module\t\t\t" "ice" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "natpmp" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "pcp" MOD_EXT "\n");

	(void)re_fprintf(f, "\n# Media encryption modules\n");
	(void)re_fprintf(f, "#module\t\t\t" "srtp" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "dtls_srtp" MOD_EXT "\n");
	(void)re_fprintf(f, "#module\t\t\t" "zrtp" MOD_EXT "\n");
	(void)re_fprintf(f, "\n");

	(void)re_fprintf(f, "\n#------------------------------------"
			 "------------------------------------------\n");
	(void)re_fprintf(f, "# Temporary Modules (loaded then unloaded)\n");
	(void)re_fprintf(f, "\n");
	(void)re_fprintf(f, "module_tmp\t\t" "uuid" MOD_EXT "\n");
	(void)re_fprintf(f, "module_tmp\t\t" "account" MOD_EXT "\n");
	(void)re_fprintf(f, "\n");

	(void)re_fprintf(f, "\n#------------------------------------"
			 "------------------------------------------\n");
	(void)re_fprintf(f, "# Application Modules\n");
	(void)re_fprintf(f, "\n");
	(void)re_fprintf(f, "module_app\t\t" "auloop"MOD_EXT"\n");
	(void)re_fprintf(f, "#module_app\t\t" "b2bua"MOD_EXT"\n");
	(void)re_fprintf(f, "module_app\t\t"  "contact"MOD_EXT"\n");
	(void)re_fprintf(f, "module_app\t\t"  "debug_cmd"MOD_EXT"\n");
	(void)re_fprintf(f, "#module_app\t\t"  "echo"MOD_EXT"\n");
	(void)re_fprintf(f, "#module_app\t\t" "gtk" MOD_EXT "\n");
	(void)re_fprintf(f, "module_app\t\t"  "menu"MOD_EXT"\n");
	(void)re_fprintf(f, "#module_app\t\t"  "mwi"MOD_EXT"\n");
	(void)re_fprintf(f, "#module_app\t\t" "presence"MOD_EXT"\n");
	(void)re_fprintf(f, "#module_app\t\t" "syslog"MOD_EXT"\n");
	(void)re_fprintf(f, "#module_app\t\t" "mqtt" MOD_EXT "\n");
	(void)re_fprintf(f, "#module_app\t\t" "ctrl_tcp" MOD_EXT "\n");
	(void)re_fprintf(f, "module_app\t\t" "vidloop"MOD_EXT"\n");
	(void)re_fprintf(f, "#module_app\t\t" "httpreq"MOD_EXT"\n");
	(void)re_fprintf(f, "\n");

	(void)re_fprintf(f, "\n#------------------------------------"
			 "------------------------------------------\n");
	(void)re_fprintf(f, "# Module parameters\n");
	(void)re_fprintf(f, "\n");

	(void)re_fprintf(f, "\n# UI Modules parameters\n");
	(void)re_fprintf(f, "cons_listen\t\t0.0.0.0:5555 # cons\n");

	(void)re_fprintf(f, "\n");
	(void)re_fprintf(f, "http_listen\t\t0.0.0.0:8000 # httpd - server\n");

	(void)re_fprintf(f, "\n");
	(void)re_fprintf(f, "ctrl_tcp_listen\t\t0.0.0.0:4444 # ctrl_tcp\n");

	(void)re_fprintf(f, "\n");
	(void)re_fprintf(f, "evdev_device\t\t/dev/input/event0\n");

	(void)re_fprintf(f, "\n# Opus codec parameters\n");
	(void)re_fprintf(f, "opus_bitrate\t\t28000 # 6000-510000\n");
	(void)re_fprintf(f, "#opus_stereo\t\tyes\n");
	(void)re_fprintf(f, "#opus_sprop_stereo\tyes\n");
	(void)re_fprintf(f, "#opus_cbr\t\tno\n");
	(void)re_fprintf(f, "#opus_inbandfec\t\tno\n");
	(void)re_fprintf(f, "#opus_dtx\t\tno\n");
	(void)re_fprintf(f, "#opus_mirror\t\tno\n");
	(void)re_fprintf(f, "#opus_complexity\t10\n");
	(void)re_fprintf(f, "#opus_application\taudio\t# {voip,audio}\n");
	(void)re_fprintf(f, "#opus_samplerate\t48000\n");
	(void)re_fprintf(f, "#opus_packet_loss\t10\t# 0-100 percent\n");

	(void)re_fprintf(f, "\n# Opus Multistream codec parameters\n");
	(void)re_fprintf(f,
			 "#opus_ms_channels\t2\t#total channels (2 or 4)\n");
	(void)re_fprintf(f,
			 "#opus_ms_streams\t\t2\t#number of streams\n");
	(void)re_fprintf(f,
			"#opus_ms_c_streams\t2\t#number of coupled streams\n");

	(void)re_fprintf(f, "\n");
	(void)re_fprintf(f, "vumeter_stderr\t\tyes\n");

	(void)re_fprintf(f, "\n");
	(void)re_fprintf(f, "#jack_connect_ports\tyes\n");

	(void)re_fprintf(f,
			"\n# Selfview\n"
			"video_selfview\t\twindow # {window,pip}\n"
			"#selfview_size\t\t64x64\n");

	(void)re_fprintf(f,
			"\n# ZRTP\n"
			"#zrtp_hash\t\tno  # Disable SDP zrtp-hash "
			"(not recommended)\n");

	(void)re_fprintf(f,
			"\n# Menu\n"
			"#menu_bell\t\tyes\n"
			"#redial_attempts\t0 # Num or <inf>\n"
			"#redial_delay\t\t5 # Delay in seconds\n"
			"#ringback_disabled\tno\n"
			"#statmode_default\toff\n"
			"#menu_clean_number\tno\n"
			"#ring_aufile\t\tring.wav\n"
			"#callwaiting_aufile\tcallwaiting.wav\n"
			"#ringback_aufile\tringback.wav\n"
			"#notfound_aufile\tnotfound.wav\n"
			"#busy_aufile\t\tbusy.wav\n"
			"#error_aufile\t\terror.wav\n"
			);

	(void)re_fprintf(f,
			"\n# avcodec\n"
			"#avcodec_h264enc\tlibx264\n"
			"#avcodec_h264dec\th264\n"
			"#avcodec_h265enc\tlibx265\n"
			"#avcodec_h265dec\thevc\n"
			"#avcodec_hwaccel\t%s\n",
			default_avcodec_hwaccel());

	(void)re_fprintf(f,
			 "\n# mqtt\n"
			 "#mqtt_broker_host\t127.0.0.1\n"
			 "#mqtt_broker_port\t1883\n"
			 "#mqtt_broker_cafile\t/path/to/broker-ca.crt\n"
			 "#mqtt_broker_clientid\tbaresip01\n"
			 "#mqtt_broker_user\tuser\n"
			 "#mqtt_broker_password\tpass\n"
			 "#mqtt_basetopic\t\tbaresip/01\n");

	(void)re_fprintf(f,
			 "\n# sndfile\n"
			 "#snd_path\t\t/tmp\n");

	(void)re_fprintf(f,
			 "\n# EBU ACIP\n"
			 "#ebuacip_jb_type\tfixed\t# auto,fixed\n");

	(void)re_fprintf(f,
			 "\n# HTTP request module\n"
			 "# httpreq_ca\t\ttrusted1.pem\n"
			 "# httpreq_ca\t\ttrusted2.pem\n"
			 "# httpreq_dns\t\t1.1.1.1\n"
			 "# httpreq_dns\t\t8.8.8.8\n"
			 "# httpreq_hostname\tmyserver\n"
			 "# httpreq_cert\t\tcert.pem\n"
			 "# httpreq_key\t\tkey.pem\n"
			 );

	if (f)
		(void)fclose(f);

	return err;
}
