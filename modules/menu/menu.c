/**
 * @file menu.c  Interactive menu
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */
#include <stdlib.h>
#include <time.h>
#include "rsua-mod/modapi.h"
#include "menu.h"


/**
 * @defgroup menu menu
 *
 * Interactive menu
 *
 * This module must be loaded if you want to use the interactive menu
 * to control the Baresip application.
 */
static struct menu menu;


static int menu_set_incall(bool incall)
{
	int err = 0;

	/* Dynamic menus */
	if (incall) {
		dial_menu_unregister();
		err = dynamic_menu_register();
	}
	else {
		dynamic_menu_unregister();
		err = dial_menu_register();
	}
	if (err) {
		warning("menu: set_incall: cmd_register failed (%m)\n", err);
	}

	return err;
}


static void alert_start(void *arg)
{
	(void)arg;

	if (!menu.bell)
		return;

	ui_output(data_uis(), "\033[10;1000]\033[11;1000]\a");

	tmr_start(&menu.tmr_alert, 1000, alert_start, NULL);
}


static void alert_stop(void)
{
	if (!menu.bell)
		return;

	if (tmr_isrunning(&menu.tmr_alert))
		ui_output(data_uis(), "\r");

	tmr_cancel(&menu.tmr_alert);
}


static void tmrstat_handler(void *arg)
{
	struct call *call;
	(void)arg;

	/* the UI will only show the current active call */
	call = ua_call(uag_current());
	if (!call)
		return;

	tmr_start(&menu.tmr_stat, 100, tmrstat_handler, 0);

	if (ui_isediting(data_uis()))
		return;

	if (STATMODE_OFF != menu.statmode) {
		(void)re_fprintf(stderr, "%H\r", call_status, call);
	}
}


void menu_update_callstatus(bool incall)
{
	/* if there are any active calls, enable the call status view */
	if (incall)
		tmr_start(&menu.tmr_stat, 100, tmrstat_handler, 0);
	else
		tmr_cancel(&menu.tmr_stat);
}


static void redial_reset(void)
{
	tmr_cancel(&menu.tmr_redial);
	menu.current_attempts = 0;
}


static char *errorcode_fb_aufile(uint16_t scode)
{
	switch (scode) {

	case 404: return "notfound.wav";
	case 486: return "busy.wav";
	case 487: return NULL; /* ignore */
	default:  return "error.wav";
	}
}


static char *errorcode_key_aufile(uint16_t scode)
{
	switch (scode) {

	case 404: return "notfound_aufile";
	case 486: return "busy_aufile";
	case 487: return NULL; /* ignore */
	default:  return "error_aufile";
	}
}


static void menu_play(const char *ckey, const char *fname, int repeat)
{
	struct config *cfg = data_config();
	struct player *player = data_player();

	struct pl pl = PL_INIT;
	char *file = NULL;

	if (conf_get(conf_cur(), ckey, &pl))
		pl_set_str(&pl, fname);

	if (!pl_isset(&pl))
		return;

	pl_strdup(&file, &pl);
	menu.play = mem_deref(menu.play);
	(void)play_file(&menu.play, player, file, repeat,
			cfg->audio.play_mod,
			cfg->audio.play_dev);
	mem_deref(file);
}


static void play_incoming(const struct ua *ua, bool waiting)
{
	/* stop any ringtones */
	menu.play = mem_deref(menu.play);

	/* Only play the ringtones if answermode is "Manual".
	 * If the answermode is "auto" then be silent.
	 */
	if (ANSWERMODE_MANUAL == account_answermode(ua_account(ua))) {

		if (waiting) {
			menu_play("callwaiting_aufile", "callwaiting.wav", 3);
		}
		else {
			/* Alert user */
			menu_play("ring_aufile", "ring.wav", -1);
		}

		if (menu.bell)
			alert_start(0);
	}
}


static void play_ringback(void)
{
	/* stop any ringtones */
	menu.play = mem_deref(menu.play);

	if (menu.ringback_disabled) {
		info("\nRingback disabled\n");
	}
	else {
		menu_play("ringback_aufile", "ringback.wav", -1);
	}
}


static void play_resume(const struct call *call)
{
	struct le *lec;
	struct le *leu;
	struct ua *uain;
	bool incoming = false;
	bool ringing = false;

	for (leu = uag_list()->head; leu; leu = leu->next) {
		struct ua *ua = leu->data;

		for (lec = ua_calls(ua)->head; lec; lec = lec->next) {
			if (lec->data == call)
				continue;

			switch (call_state(lec->data)) {
			case CALL_STATE_EARLY:
			case CALL_STATE_ESTABLISHED:
				return;
			case CALL_STATE_INCOMING:
				incoming = true;
				uain = ua;
				break;
			case CALL_STATE_RINGING:
				ringing = true;
				break;
			default:
				break;
			}
		}
	}

	if (incoming) {
		play_incoming(uain, uag_call_count() > 2);
	}
	else if (ringing) {
		play_ringback();
	}
}


static bool has_established_call(void)
{
	struct le *lec;
	struct le *leu;

	for (leu = uag_list()->head; leu; leu = leu->next) {
		struct ua *ua = leu->data;

		for (lec = ua_calls(ua)->head; lec; lec = lec->next) {

			switch (call_state(lec->data)) {
			case CALL_STATE_EARLY:
			case CALL_STATE_ESTABLISHED:
				return true;
			default:
				break;
			}
		}
	}

	return false;
}


static void check_registrations(void)
{
	static bool ual_ready = false;
	struct le *le;
	uint32_t n;

	if (ual_ready)
		return;

	for (le = list_head(uag_list()); le; le = le->next) {
		struct ua *ua = le->data;

		if (!ua_isregistered(ua) && !account_prio(ua_account(ua)))
			return;
	}

	n = list_count(uag_list());

	/* We are ready */
	ui_output(data_uis(),
		  "\x1b[32mAll %u useragent%s registered successfully!"
		  " (%u ms)\x1b[;m\n",
		  n, n==1 ? "" : "s",
		  (uint32_t)(tmr_jiffies() - menu.start_ticks));

	ual_ready = true;
}


static void redial_handler(void *arg)
{
	char *uri = NULL;
	int err;
	(void)arg;

	info("now: redialing now. current_attempts=%u, max_attempts=%u\n",
	     menu.current_attempts,
	     menu.redial_attempts);

	if (menu.current_attempts > menu.redial_attempts) {

		info("menu: redial: too many attemptes -- giving up\n");
		return;
	}

	if (menu.dialbuf->end == 0) {
		warning("menu: redial: dialbuf is empty\n");
		return;
	}

	menu.dialbuf->pos = 0;
	err = mbuf_strdup(menu.dialbuf, &uri, menu.dialbuf->end);
	if (err)
		return;

	err = ua_connect(uag_find_aor(menu.redial_aor), NULL, NULL,
			 uri, VIDMODE_ON);
	if (err) {
		warning("menu: redial: ua_connect failed (%m)\n", err);
	}

	mem_deref(uri);
}


static void menu_play_closed(struct call *call)
{
	uint16_t scode;
	const char *key;
	const char *fb;

	/* stop any ringtones */
	menu.play = mem_deref(menu.play);

	if (call_scode(call)) {
		scode = call_scode(call);
		key = errorcode_key_aufile(scode);
		fb = errorcode_fb_aufile(scode);

		menu_play(key, fb, 1);
	}
}


static void ua_event_handler(struct ua *ua, enum ua_event ev,
			     struct call *call, const char *prm, void *arg)
{
	struct call *call2 = NULL;
	bool incall;
	enum sdp_dir ardir, vrdir;
	int err;
	(void)prm;
	(void)arg;

#if 0
	debug("menu: [ ua=%s call=%s ] event: %s (%s)\n",
	      ua_aor(ua), call_id(call), uag_event_str(ev), prm);
#endif


	switch (ev) {

	case UA_EVENT_CALL_INCOMING:

		/* set the current User-Agent to the one with the call */
		uag_current_set(ua);

		ardir =sdp_media_rdir(
			stream_sdpmedia(audio_strm(call_audio(call))));
		vrdir = sdp_media_rdir(
			stream_sdpmedia(video_strm(call_video(call))));
		if (!call_has_video(call))
			vrdir = SDP_INACTIVE;

		info("%s: Incoming call from: %s %s - audio-video: %s-%s -"
		     " (press 'a' to accept)\n",
		     ua_aor(ua), call_peername(call), call_peeruri(call),
		     sdp_dir_name(ardir), sdp_dir_name(vrdir));

		play_incoming(ua, uag_call_count() > 1);
		break;

	case UA_EVENT_CALL_RINGING:
		if (call == ua_call(uag_current()) && !has_established_call())
			play_ringback();
		break;

	case UA_EVENT_CALL_PROGRESS:
		if (call == ua_call(uag_current()))
			menu.play = mem_deref(menu.play);
		break;

	case UA_EVENT_CALL_ESTABLISHED:
		/* stop any ringtones */
		menu.play = mem_deref(menu.play);

		alert_stop();

		/* We must stop the re-dialing if the call was
		   established */
		redial_reset();
		break;

	case UA_EVENT_CALL_CLOSED:
		menu_play_closed(call);

		alert_stop();
		play_resume(call);

		/* Activate the re-dialing if:
		 *
		 * - redial_attempts must be enabled in config
		 * - the closed call must be of outgoing direction
		 * - the closed call must fail with special code 701
		 */
		if (menu.redial_attempts) {

			if (menu.current_attempts
			    ||
			    (call_is_outgoing(call) &&
			     call_scode(call) == 701)) {

				info("menu: call closed"
				     " -- redialing in %u seconds\n",
				     menu.redial_delay);

				++menu.current_attempts;

				str_ncpy(menu.redial_aor, ua_aor(ua),
					 sizeof(menu.redial_aor));

				tmr_start(&menu.tmr_redial,
					  menu.redial_delay*1000,
					  redial_handler, NULL);
			}
			else {
				info("menu: call closed -- not redialing\n");
			}
		}
		break;

	case UA_EVENT_CALL_TRANSFER:
		/*
		 * Create a new call to transfer target.
		 *
		 * NOTE: we will automatically connect a new call to the
		 *       transfer target
		 */

		info("menu: transferring call %s to '%s'\n",
		     call_id(call), prm);

		err = ua_call_alloc(&call2, ua, VIDMODE_ON, NULL, call,
				    call_localuri(call), true);
		if (!err) {
			struct pl pl;

			pl_set_str(&pl, prm);

			err = call_connect(call2, &pl);
			if (err) {
				warning("ua: transfer: connect error: %m\n",
					err);
			}
		}

		if (err) {
			(void)call_notify_sipfrag(call, 500, "Call Error");
			mem_deref(call2);
		}
		break;

	case UA_EVENT_CALL_TRANSFER_FAILED:
		info("menu: transfer failure: %s\n", prm);
		break;

	case UA_EVENT_REGISTER_OK:
		check_registrations();
		break;

	case UA_EVENT_UNREGISTERING:
		return;

	case UA_EVENT_MWI_NOTIFY:
		info("----- MWI for %s -----\n", ua_aor(ua));
		info("%s\n", prm);
		break;

	case UA_EVENT_AUDIO_ERROR:
		info("menu: audio error (%s)\n", prm);
		break;

	default:
		break;
	}

	incall = ev == UA_EVENT_CALL_CLOSED ?
			uag_call_count() > 1 : uag_call_count();
	menu_set_incall(incall);
	menu_update_callstatus(incall);
}


static void message_handler(struct ua *ua, const struct pl *peer,
			    const struct pl *ctype,
			    struct mbuf *body, void *arg)
{
	struct config *cfg;
	(void)ua;
	(void)ctype;
	(void)arg;

	cfg = data_config();

	ui_output(data_uis(), "\r%r: \"%b\"\n",
		  peer, mbuf_buf(body), mbuf_get_left(body));

	(void)play_file(NULL, data_player(), "message.wav", 0,
	                cfg->audio.alert_mod, cfg->audio.alert_dev);
}


/**
 * Get the menu object
 *
 * @return ptr to menu object
 */
struct menu *menu_get(void)
{
	return &menu;
}


static int module_init(void)
{
	struct pl val;
	int err;

	menu.bell = true;
	menu.redial_attempts = 0;
	menu.redial_delay = 5;
	menu.ringback_disabled = false;
	menu.statmode = STATMODE_CALL;
	menu.clean_number = false;
	menu.play = NULL;

	/*
	 * Read the config values
	 */
	conf_get_bool(conf_cur(), "menu_bell", &menu.bell);
	conf_get_bool(conf_cur(), "ringback_disabled",
		      &menu.ringback_disabled);
	conf_get_bool(conf_cur(), "menu_clean_number", &menu.clean_number);

	if (0 == conf_get(conf_cur(), "redial_attempts", &val) &&
	    0 == pl_strcasecmp(&val, "inf")) {
		menu.redial_attempts = (uint32_t)-1;
	}
	else {
		conf_get_u32(conf_cur(), "redial_attempts",
			     &menu.redial_attempts);
	}
	conf_get_u32(conf_cur(), "redial_delay", &menu.redial_delay);

	if (menu.redial_attempts) {
		info("menu: redial enabled with %u attempts and"
		     " %u seconds delay\n",
		     menu.redial_attempts,
		     menu.redial_delay);
	}

	menu.dialbuf = mbuf_alloc(64);
	if (!menu.dialbuf)
		return ENOMEM;

	menu.start_ticks = tmr_jiffies();
	tmr_init(&menu.tmr_alert);

	if (0 == conf_get(conf_cur(), "statmode_default", &val) &&
	    0 == pl_strcasecmp(&val, "off")) {
		menu.statmode = STATMODE_OFF;
	}
	else {
		menu.statmode = STATMODE_CALL;
	}

	err = static_menu_register();
	err |= dial_menu_register();
	if (err)
		return err;

	err = uag_event_register(ua_event_handler, NULL);
	if (err)
		return err;

	err = message_listen(data_message(),
			     message_handler, NULL);
	if (err)
		return err;

	return err;
}


static int module_close(void)
{
	debug("menu: close (redial current_attempts=%d)\n",
	      menu.current_attempts);

	message_unlisten(data_message(), message_handler);

	uag_event_unregister(ua_event_handler);
	static_menu_unregister();
	dial_menu_unregister();
	dynamic_menu_unregister();

	tmr_cancel(&menu.tmr_alert);
	tmr_cancel(&menu.tmr_stat);
	menu.dialbuf = mem_deref(menu.dialbuf);

	menu.le_cur = NULL;

	menu.play = mem_deref(menu.play);

	tmr_cancel(&menu.tmr_redial);

	return 0;
}


const struct mod_export DECL_EXPORTS(menu) = {
	"menu",
	"application",
	module_init,
	module_close
};
