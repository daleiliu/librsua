/**
 * @file video.h
 * @brief Video stream
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAVIDEO_H_INCLUDED
#define UAVIDEO_H_INCLUDED

#include "rsua-re/re.h"

struct config;
struct mnat;
struct mnat_sess;
struct media_ctx;
struct menc;
struct menc_sess;
struct stream_param;
struct vidcodec;
struct video;

typedef void (video_err_h)(int err, const char *str, void *arg);

int  video_alloc(struct video **vp, struct list *streaml,
		 const struct stream_param *stream_prm,
		 const struct config *cfg,
		 struct sdp_session *sdp_sess, int label,
		 const struct mnat *mnat, struct mnat_sess *mnat_sess,
		 const struct menc *menc, struct menc_sess *menc_sess,
		 const char *content, const struct list *vidcodecl,
		 const struct list *vidfiltl, bool offerer,
		 video_err_h *errh, void *arg);
int  video_encoder_set(struct video *v, struct vidcodec *vc,
		       int pt_tx, const char *params);
int  video_update(struct video *v, struct media_ctx **ctx, const char *peer);
int  video_start_source(struct video *v, struct media_ctx **ctx);
int  video_start_display(struct video *v, const char *peer);
void video_stop(struct video *v);
int   video_set_fullscreen(struct video *v, bool fs);
void  video_vidsrc_set_device(struct video *v, const char *dev);
int   video_set_source(struct video *v, const char *name, const char *dev);
void  video_set_devicename(struct video *v, const char *src, const char *disp);
int   video_debug(struct re_printf *pf, const struct video *v);
struct stream *video_strm(const struct video *v);
const struct vidcodec *video_codec(const struct video *vid, bool tx);
void video_sdp_attr_decode(struct video *v);


#ifndef UAMODAPI_USE		/* Internal API */

int  video_decoder_set(struct video *v, struct vidcodec *vc, int pt_rx,
		       const char *fmtp);
void video_update_picture(struct video *v);
int  video_print(struct re_printf *pf, const struct video *v);

#endif /* ifndef UAMODAPI_USE */

#endif /* UAVIDEO_H_INCLUDED */
