/**
 * @file stream.h
 * @brief Generic stream
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UASTREAM_H_INCLUDED
#define UASTREAM_H_INCLUDED

#include "rsua-re/re.h"

struct mnat;
struct mnat_sess;
struct rtpext;
struct stream;

/** Common parameters for media stream */
struct stream_param {
	bool use_rtp;       /**< Enable or disable RTP */
	int af;             /**< Wanted address family */
	const char *cname;  /**< Canonical name        */
};

typedef void (stream_mnatconn_h)(struct stream *strm, void *arg);
typedef void (stream_rtpestab_h)(struct stream *strm, void *arg);
typedef void (stream_rtcp_h)(struct stream *strm,
			     struct rtcp_msg *msg, void *arg);
typedef void (stream_error_h)(struct stream *strm, int err, void *arg);

void stream_update(struct stream *s);
const struct rtcp_stats *stream_rtcp_stats(const struct stream *strm);
struct sdp_media *stream_sdpmedia(const struct stream *s);
uint32_t stream_metric_get_tx_n_packets(const struct stream *strm);
uint32_t stream_metric_get_tx_n_bytes(const struct stream *strm);
uint32_t stream_metric_get_tx_n_err(const struct stream *strm);
uint32_t stream_metric_get_rx_n_packets(const struct stream *strm);
uint32_t stream_metric_get_rx_n_bytes(const struct stream *strm);
uint32_t stream_metric_get_rx_n_err(const struct stream *strm);
void stream_set_secure(struct stream *strm, bool secure);
bool stream_is_secure(const struct stream *strm);
int  stream_start_mediaenc(struct stream *strm);
int  stream_start(const struct stream *strm);
int stream_open_natpinhole(const struct stream *strm);
void stream_set_session_handlers(struct stream *strm,
				 stream_mnatconn_h *mnatconnh,
				 stream_rtpestab_h *rtpestabh,
				 stream_rtcp_h *rtcph,
				 stream_error_h *errorh, void *arg);
const char *stream_name(const struct stream *strm);
int  stream_debug(struct re_printf *pf, const struct stream *s);


#ifndef UAMODAPI_USE		/* Internal API */

#include "data.h"
#include "metric.h"

enum media_type {
	MEDIA_AUDIO = 0,
	MEDIA_VIDEO,
};

struct rtp_header;

enum {STREAM_PRESZ = 4+12}; /* same as RTP_HEADER_SIZE */

typedef void (stream_rtp_h)(const struct rtp_header *hdr,
			    struct rtpext *extv, size_t extc,
			    struct mbuf *mb, unsigned lostc, void *arg);
typedef int (stream_pt_h)(uint8_t pt, struct mbuf *mb, void *arg);


/** Defines a generic media stream */
struct stream {
#ifndef RELEASE
	uint32_t magic;          /**< Magic number for debugging            */
#endif
	struct le le;            /**< Linked list element                   */
	struct config_avt cfg;   /**< Stream configuration                  */
	struct sdp_media *sdp;   /**< SDP Media line                        */
	enum sdp_dir ldir;       /**< SDP direction of the stream           */
	struct rtp_sock *rtp;    /**< RTP Socket                            */
	struct rtcp_stats rtcp_stats;/**< RTCP statistics                   */
	struct jbuf *jbuf;       /**< Jitter Buffer for incoming RTP        */
	const struct mnat *mnat; /**< Media NAT traversal module            */
	struct mnat_media *mns;  /**< Media NAT traversal state             */
	const struct menc *menc; /**< Media encryption module               */
	struct menc_sess *mencs; /**< Media encryption session state        */
	struct menc_media *mes;  /**< Media Encryption media state          */
	struct metric metric_tx; /**< Metrics for transmit                  */
	struct metric metric_rx; /**< Metrics for receiving                 */
	struct sa raddr_rtp;     /**< Remote RTP address                    */
	struct sa raddr_rtcp;    /**< Remote RTCP address                   */
	enum media_type type;    /**< Media type, e.g. audio/video          */
	char *cname;             /**< RTCP Canonical end-point identifier   */
	uint32_t ssrc_rx;        /**< Incoming syncronizing source          */
	uint32_t pseq;           /**< Sequence number for incoming RTP      */
	bool pseq_set;           /**< True if sequence number is set        */
	int pt_enc;              /**< Payload type for encoding             */
	int pt_dec;              /**< Payload type for decoding             */
	bool rtcp_mux;           /**< RTP/RTCP multiplex supported by peer  */
	bool jbuf_started;       /**< True if jitter-buffer was started     */
	stream_pt_h *pth;        /**< Stream payload type handler           */
	struct tmr tmr_rtp;      /**< Timer for detecting RTP timeout       */
	uint64_t ts_last;        /**< Timestamp of last received RTP pkt    */
	bool terminated;         /**< Stream is terminated flag             */
	uint32_t rtp_timeout_ms; /**< RTP Timeout value in [ms]             */
	bool rtp_estab;          /**< True if RTP stream is established     */
	bool hold;               /**< Stream is on-hold (local)             */
	bool mnat_connected;     /**< Media NAT is connected                */
	bool menc_secure;        /**< Media stream is secure                */
	stream_rtp_h *rtph;      /**< Stream RTP handler                    */
	stream_rtcp_h *rtcph;    /**< Stream RTCP handler                   */
	void *arg;               /**< Handler argument                      */
	stream_mnatconn_h *mnatconnh;/**< Medianat connected handler        */
	stream_rtpestab_h *rtpestabh;/**< RTP established handler           */
	stream_rtcp_h *sessrtcph;    /**< Stream RTCP handler               */
	stream_error_h *errorh;  /**< Stream error handler                  */
	void *sess_arg;          /**< Session handlers argument             */
};

int  stream_alloc(struct stream **sp, struct list *streaml,
		  const struct stream_param *prm,
		  const struct config_avt *cfg,
		  struct sdp_session *sdp_sess,
		  enum media_type type, int label,
		  const struct mnat *mnat, struct mnat_sess *mnat_sess,
		  const struct menc *menc, struct menc_sess *menc_sess,
		  bool offerer,
		  stream_rtp_h *rtph, stream_rtcp_h *rtcph, stream_pt_h *pth,
		  void *arg);
int  stream_send(struct stream *s, bool ext, bool marker, int pt, uint32_t ts,
		 struct mbuf *mb);
void stream_update_encoder(struct stream *s, int pt_enc);
int  stream_jbuf_stat(struct re_printf *pf, const struct stream *s);
void stream_hold(struct stream *s, bool hold);
void stream_set_ldir(struct stream *s, enum sdp_dir dir);
void stream_set_srate(struct stream *s, uint32_t srate_tx, uint32_t srate_rx);
void stream_send_fir(struct stream *s, bool pli);
void stream_reset(struct stream *s);
void stream_set_bw(struct stream *s, uint32_t bps);
int  stream_print(struct re_printf *pf, const struct stream *s);
void stream_enable_rtp_timeout(struct stream *strm, uint32_t timeout_ms);
bool stream_is_ready(const struct stream *strm);
int  stream_decode(struct stream *s);
void stream_silence_on(struct stream *s, bool on);

#endif /* ifndef UAMODAPI_USE */

#endif /* UASTREAM_H_INCLUDED */
