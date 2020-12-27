/**
 * @file audio.h
 * @brief Audio stream
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAAUDIO_H_INCLUDED
#define UAAUDIO_H_INCLUDED

#include "rsua-re/re.h"

struct account;
struct aucodec;
struct audio;
struct config;
struct media_ctx;
struct mnat;
struct mnat_sess;
struct menc;
struct menc_sess;
struct stream_param;

typedef void (audio_event_h)(int key, bool end, void *arg);
typedef void (audio_level_h)(bool tx, double lvl, void *arg);
typedef void (audio_err_h)(int err, const char *str, void *arg);

int audio_alloc(struct audio **ap, struct list *streaml,
		const struct stream_param *stream_prm,
		const struct config *cfg,
		struct account *acc, struct sdp_session *sdp_sess, int label,
		const struct mnat *mnat, struct mnat_sess *mnat_sess,
		const struct menc *menc, struct menc_sess *menc_sess,
		uint32_t ptime, const struct list *aucodecl, bool offerer,
		audio_event_h *eventh, audio_level_h *levelh,
		audio_err_h *errh, void *arg);
void audio_mute(struct audio *a, bool muted);
bool audio_ismuted(const struct audio *a);
int  audio_set_devicename(struct audio *a, const char *src, const char *play);
int  audio_set_source(struct audio *au, const char *mod, const char *device);
int  audio_set_player(struct audio *au, const char *mod, const char *device);
void audio_level_put(const struct audio *au, bool tx, double lvl);
int  audio_level_get(const struct audio *au, double *level);
int  audio_debug(struct re_printf *pf, const struct audio *a);
struct stream *audio_strm(const struct audio *au);
uint64_t audio_jb_current_value(const struct audio *au);
int  audio_set_bitrate(struct audio *au, uint32_t bitrate);
bool audio_rxaubuf_started(const struct audio *au);
int  audio_start(struct audio *a);
int  audio_start_source(struct audio *a, struct list *ausrcl,
			struct list *aufiltl);
void audio_stop(struct audio *a);
bool audio_started(const struct audio *a);
void audio_set_hold(struct audio *au, bool hold);
int  audio_encoder_set(struct audio *a, const struct aucodec *ac,
		       int pt_tx, const char *params);
int  audio_decoder_set(struct audio *a, const struct aucodec *ac,
		       int pt_rx, const char *params);
const struct aucodec *audio_codec(const struct audio *au, bool tx);
struct config_audio *audio_config(struct audio *au);
void audio_set_media_context(struct audio *au, struct media_ctx **ctx);


#ifndef UAMODAPI_USE		/* Internal API */

int  audio_send_digit(struct audio *a, char key);
void audio_sdp_attr_decode(struct audio *a);

#endif /* ifndef UAMODAPI_USE */

#endif /* UAAUDIO_H_INCLUDED */
