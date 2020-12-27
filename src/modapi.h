/**
 * @file modapi.h
 * @brief Module interface
 *
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAMODAPI_H_INCLUDED
#define UAMODAPI_H_INCLUDED

#include "rsua-re/re.h"
#ifdef UAMODAPI_FIX_SPANDSP	/* for modules: g722, g726, plc */
#include "rsua-rem/rem_au.h"
#define REM_H__			/* exclude "rem.h" from all headers */
#else
#include "rsua-rem/rem.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


#define UAMODAPI_USE		1
#include "rsua-mod/acct.h"
#include "rsua-mod/aucodec.h"
#include "rsua-mod/audio.h"
#include "rsua-mod/aufilt.h"
#include "rsua-mod/auframe.h"
#include "rsua-mod/aulevel.h"
#include "rsua-mod/auplay.h"
#include "rsua-mod/ausrc.h"
#include "rsua-mod/call.h"
#include "rsua-mod/cmd.h"
#include "rsua-mod/conf.h"
#include "rsua-mod/data.h"
#include "rsua-mod/ept.h"
#include "rsua-mod/ev.h"
#include "rsua-mod/h264.h"
#include "rsua-mod/log.h"
#include "rsua-mod/mediadev.h"
#include "rsua-mod/menc.h"
#include "rsua-mod/message.h"
#include "rsua-mod/mnat.h"
#include "rsua-mod/net.h"
#include "rsua-mod/play.h"
#include "rsua-mod/sdp.h"
#include "rsua-mod/sipreq.h"
#include "rsua-mod/stream.h"
#include "rsua-mod/stunuri.h"
#include "rsua-mod/ui.h"
#include "rsua-mod/vidcodec.h"
#include "rsua-mod/video.h"
#include "rsua-mod/vidfilt.h"
#include "rsua-mod/vidisp.h"
#include "rsua-mod/vidsrc.h"
#include "rsua-mod/vidutil.h"

#ifdef STATIC
#define DECL_EXPORTS(name) exports_ ##name
#else
#define DECL_EXPORTS(name) exports
#endif

/** Media Context */
struct media_ctx {
	const char *id;  /**< Media Context identifier */
};


#ifdef __cplusplus
}
#endif


#endif /* UAMODAPI_H_INCLUDED */
