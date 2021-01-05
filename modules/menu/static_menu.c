/**
 * @file call.c Static menu related functions
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */
#include "rsua-mod/modapi.h"

#include "menu.h"


/**
 * Decode a SDP direction
 *
 * @param pl  SDP direction as string
 *
 * @return sdp_dir SDP direction
 */
static enum sdp_dir decode_sdp_enum(const struct pl *pl)
{
	if (!pl)
		return SDP_INACTIVE;

	if (!pl_strcmp(pl, "inactive")) {
		return SDP_INACTIVE;
	}
	else if (!pl_strcmp(pl, "sendonly")) {
		return  SDP_SENDONLY;
	}
	else if (!pl_strcmp(pl, "recvonly")) {
		return SDP_RECVONLY;
	}
	else if (!pl_strcmp(pl, "sendrecv")) {
		return SDP_SENDRECV;
	}

	return SDP_INACTIVE;
}


static const char about_fmt[] =
	".------------------------------------------------------------.\n"
	"|                      "
	"\x1b[34;1m" "bare"
	"\x1b[31;1m" "sip"
	"\x1b[;m"
	" %-10s                    |\n"
	"|                                                            |\n"
	"| Baresip is a portable and modular SIP User-Agent           |\n"
	"| with audio and video support                               |\n"
	"|                                                            |\n"
	"| License:   BSD                                             |\n"
	"| Homepage:  https://github.com/baresip/baresip              |\n"
	"|                                                            |\n"
	"'------------------------------------------------------------'\n"
	;


static int about_box(struct re_printf *pf, void *unused)
{
	(void)unused;

	return re_hprintf(pf, about_fmt, RSUA_VERSION);
}


static int cmd_answer(struct re_printf *pf, void *unused)
{
	struct ua *ua = uag_current();
	struct menu *menu = menu_get();
	int err;
	(void)unused;

	err = re_hprintf(pf, "%s: Answering incoming call\n", ua_aor(ua));

	/* Stop any ongoing ring-tones */
	menu->play = mem_deref(menu->play);

	ua_hold_answer(ua, NULL, VIDMODE_ON);

	return err;
}


/**
 * Accepts the pending call with special audio and video direction
 *
 * @param pf     Print handler for debug output
 * @param arg    Command arguments
 *
 * @return 0 if success, otherwise errorcode
 */
static int cmd_answerdir(struct re_printf *pf, void *arg)
{
	const struct cmd_arg *carg = arg;
	enum sdp_dir adir, vdir;
	struct pl argdir[2] = {PL_INIT, PL_INIT};
	struct ua *ua = uag_current();
	struct menu *menu = menu_get();
	int err = 0;

	const char *usage = "Usage: /acceptdir"
			" audio=<inactive, sendonly, recvonly, sendrecv>"
			" video=<inactive, sendonly, recvonly, sendrecv>\n"
			"/acceptdir <sendonly, recvonly, sendrecv>\n"
			"Audio & video must not be"
			" inactive at the same time\n";
	(void) pf;

	err = re_regex(carg->prm, str_len(carg->prm),
		"audio=[^ ]* video=[^ ]*", &argdir[0], &argdir[1]);
	if (err)
		err = re_regex(carg->prm, str_len(carg->prm),
			"[^ ]*", &argdir[0]);

	if (err) {
		warning("%s", usage);
		return EINVAL;
	}

	if (!pl_isset(&argdir[1]))
		argdir[1] = argdir[0];

	adir = decode_sdp_enum(&argdir[0]);
	vdir = decode_sdp_enum(&argdir[1]);

	if (adir == SDP_INACTIVE && vdir == SDP_INACTIVE) {
		warning("%s", usage);
		return EINVAL;
	}

	err = call_set_media_direction(ua_call(ua), adir, vdir);

	menu->play = mem_deref(menu->play);
	ua_hold_answer(ua, NULL, VIDMODE_ON);

	return 0;
}


static int cmd_set_answermode(struct re_printf *pf, void *arg)
{
	enum answermode mode;
	const struct cmd_arg *carg = arg;
	int err;

	if (0 == str_cmp(carg->prm, "manual")) {
		mode = ANSWERMODE_MANUAL;
	}
	else if (0 == str_cmp(carg->prm, "early")) {
		mode = ANSWERMODE_EARLY;
	}
	else if (0 == str_cmp(carg->prm, "auto")) {
		mode = ANSWERMODE_AUTO;
	}
	else {
		(void)re_hprintf(pf, "Invalid answer mode: %s\n", carg->prm);
		return EINVAL;
	}

	err = account_set_answermode(ua_account(uag_current()), mode);
	if (err)
		return err;

	(void)re_hprintf(pf, "Answer mode changed to: %s\n", carg->prm);

	return 0;
}


static int switch_audio_player(struct re_printf *pf, void *arg)
{
	const struct cmd_arg *carg = arg;
	struct pl pl_driver, pl_device;
	struct config_audio *aucfg;
	struct config *cfg;
	struct audio *a;
	const struct auplay *ap;
	struct le *le;
	char driver[16], device[128] = "";
	int err = 0;

	if (re_regex(carg->prm, str_len(carg->prm), "[^,]+,[~]*",
		     &pl_driver, &pl_device)) {

		return re_hprintf(pf, "\rFormat should be:"
				  " driver,device\n");
	}

	pl_strcpy(&pl_driver, driver, sizeof(driver));
	pl_strcpy(&pl_device, device, sizeof(device));

	ap = auplay_find(data_auplayl(), driver);
	if (!ap) {
		re_hprintf(pf, "no such audio-player: %s\n", driver);
		return 0;
	}
	else if (!list_isempty(&ap->dev_list)) {

		if (!mediadev_find(&ap->dev_list, device)) {
			re_hprintf(pf,
				   "no such device for %s audio-player: %s\n",
				   driver, device);

			mediadev_print(pf, &ap->dev_list);

			return 0;
		}
	}

	re_hprintf(pf, "switch audio player: %s,%s\n",
		   driver, device);

	cfg = data_config();
	if (!cfg) {
		return re_hprintf(pf, "no config object\n");
	}

	aucfg = &cfg->audio;

	str_ncpy(aucfg->play_mod, driver, sizeof(aucfg->play_mod));
	str_ncpy(aucfg->play_dev, device, sizeof(aucfg->play_dev));

	str_ncpy(aucfg->alert_mod, driver, sizeof(aucfg->alert_mod));
	str_ncpy(aucfg->alert_dev, device, sizeof(aucfg->alert_dev));

	for (le = list_tail(ua_calls(uag_current())); le; le = le->prev) {

		struct call *call = le->data;

		a = call_audio(call);

		err = audio_set_player(a, driver, device);
		if (err) {
			re_hprintf(pf, "failed to set audio-player"
				   " (%m)\n", err);
			break;
		}
	}

	return 0;
}


static int switch_audio_source(struct re_printf *pf, void *arg)
{
	const struct cmd_arg *carg = arg;
	struct pl pl_driver, pl_device;
	struct config_audio *aucfg;
	struct config *cfg;
	struct audio *a;
	const struct ausrc *as;
	struct le *le;
	char driver[16], device[128] = "";
	int err = 0;

	if (re_regex(carg->prm, str_len(carg->prm), "[^,]+,[~]*",
		     &pl_driver, &pl_device)) {

		return re_hprintf(pf, "\rFormat should be:"
				  " driver,device\n");
	}

	pl_strcpy(&pl_driver, driver, sizeof(driver));
	pl_strcpy(&pl_device, device, sizeof(device));

	as = ausrc_find(data_ausrcl(), driver);
	if (!as) {
		re_hprintf(pf, "no such audio-source: %s\n", driver);
		return 0;
	}
	else if (!list_isempty(&as->dev_list)) {

		if (!mediadev_find(&as->dev_list, device)) {
			re_hprintf(pf,
				   "no such device for %s audio-source: %s\n",
				   driver, device);

			mediadev_print(pf, &as->dev_list);

			return 0;
		}
	}

	re_hprintf(pf, "switch audio device: %s,%s\n",
		   driver, device);

	cfg = data_config();
	if (!cfg) {
		return re_hprintf(pf, "no config object\n");
	}

	aucfg = &cfg->audio;

	str_ncpy(aucfg->src_mod, driver, sizeof(aucfg->src_mod));
	str_ncpy(aucfg->src_dev, device, sizeof(aucfg->src_dev));

	for (le = list_tail(ua_calls(uag_current())); le; le = le->prev) {

		struct call *call = le->data;

		a = call_audio(call);

		err = audio_set_source(a, driver, device);
		if (err) {
			re_hprintf(pf, "failed to set audio-source"
				   " (%m)\n", err);
			break;
		}
	}

	return 0;
}


/**
 * Print the current SIP Call status for the current User-Agent
 *
 * @param pf     Print handler for debug output
 * @param unused Unused parameter
 *
 * @return 0 if success, otherwise errorcode
 */
static int ua_print_call_status(struct re_printf *pf, void *unused)
{
	struct call *call;
	int err;

	(void)unused;

	call = ua_call(uag_current());
	if (call) {
		err  = re_hprintf(pf, "\n%H\n", call_debug, call);
	}
	else {
		err  = re_hprintf(pf, "\n(no active calls)\n");
	}

	return err;
}


static void clean_number(char *str)
{
	int i = 0, k = 0;

	/* only clean numeric numbers
	 * In other cases trust the user input
	 */
	int err = re_regex(str, sizeof(str), "[A-Za-z]");
	if (err == 0)
		return;

	/* remove (0) which is in some mal-formated numbers
	 * but only if trailed by another character
	 */
	if (str[0] == '+' || (str[0] == '0' && str[1] == '0'))
		while (str[i]) {
			if (str[i] == '('
			 && str[i+1] == '0'
			 && str[i+2] == ')'
			 && (str[i+3] == ' '
				 || (str[i+3] >= '0' && str[i+3] <= '9')
			    )
			) {
				str[i+1] = ' ';
				break;
			}
			++i;
		}
	i = 0;
	while (str[i]) {
		if (str[i] == ' '
		 || str[i] == '.'
		 || str[i] == '-'
		 || str[i] == '/'
		 || str[i] == '('
		 || str[i] == ')')
			++i;
		else
			str[k++] = str[i++];
	}
	str[k] = '\0';
}


static int dial_handler(struct re_printf *pf, void *arg)
{
	const struct cmd_arg *carg = arg;
	struct menu *menu = menu_get();
	int err = 0;

	(void)pf;

	if (str_isset(carg->prm)) {

		mbuf_rewind(menu->dialbuf);
		(void)mbuf_write_str(menu->dialbuf, carg->prm);
		if (menu->clean_number)
			clean_number(carg->prm);

		err = ua_connect(uag_current(), NULL, NULL,
				 carg->prm, VIDMODE_ON);
	}
	else if (menu->dialbuf->end > 0) {

		char *uri;

		menu->dialbuf->pos = 0;
		err = mbuf_strdup(menu->dialbuf, &uri, menu->dialbuf->end);
		if (err)
			return err;
		if (menu->clean_number)
			clean_number(uri);

		err = ua_connect(uag_current(), NULL, NULL, uri, VIDMODE_ON);

		mem_deref(uri);
	}

	if (err) {
		warning("menu: ua_connect failed: %m\n", err);
	}

	return err;
}


static int cmd_dialdir(struct re_printf *pf, void *arg)
{
	const struct cmd_arg *carg = arg;
	enum sdp_dir adir, vdir;
	struct pl argdir[2] = {PL_INIT, PL_INIT};
	struct pl pluri;
	struct call *call;
	char *uri;
	struct ua *ua = uag_current();
	int err = 0;

	const char *usage = "Usage: /dialdir <address/telnr.>"
			" audio=<inactive, sendonly, recvonly, sendrecv>"
			" video=<inactive, sendonly, recvonly, sendrecv>\n"
			"/dialdir <address/telnr.>"
			" <sendonly, recvonly, sendrecv>\n"
			"Audio & video must not be"
			" inactive at the same time\n";
	(void) pf;

	err = re_regex(carg->prm, str_len(carg->prm),
		"[^ ]* audio=[^ ]* video=[^ ]*",
		&pluri, &argdir[0], &argdir[1]);
	if (err)
		err = re_regex(carg->prm, str_len(carg->prm),
			"[^ ]* [^ ]*",&pluri, &argdir[0]);

	if (err) {
		warning("%s", usage);
		return EINVAL;
	}

	if (!pl_isset(&argdir[1]))
		argdir[1] = argdir[0];

	adir = decode_sdp_enum(&argdir[0]);
	vdir = decode_sdp_enum(&argdir[1]);

	if (err) {
		warning("%s", usage);
		return err;
	}

	if (adir == SDP_INACTIVE && vdir == SDP_INACTIVE) {
		warning("%s", usage);
		return EINVAL;
	}

	err = pl_strdup(&uri, &pluri);
	if (err)
		goto out;

	err = ua_connect_dir(ua, &call, NULL, uri, VIDMODE_ON, adir, vdir);
	if (err)
		goto out;

 out:
	mem_deref(uri);

	return err;
}


static int cmd_hangup(struct re_printf *pf, void *unused)
{
	(void)pf;
	(void)unused;

	ua_hangup(uag_current(), NULL, 0, NULL);

	return 0;
}


static int print_commands(struct re_printf *pf, void *unused)
{
	(void)unused;
	return cmd_print(pf, data_commands());
}


static int cmd_print_calls(struct re_printf *pf, void *unused)
{
	(void)unused;
	return ua_print_calls(pf, uag_current());
}


static void options_resp_handler(int err, const struct sip_msg *msg, void *arg)
{
	(void)arg;

	if (err) {
		warning("options reply error: %m\n", err);
		return;
	}

	if (msg->scode < 200)
		return;

	if (msg->scode < 300) {

		mbuf_set_pos(msg->mb, 0);
		info("----- OPTIONS of %r -----\n%b",
		     &msg->to.auri, mbuf_buf(msg->mb),
		     mbuf_get_left(msg->mb));
		return;
	}

	info("%r: OPTIONS failed: %u %r\n", &msg->to.auri,
	     msg->scode, &msg->reason);
}


static int options_command(struct re_printf *pf, void *arg)
{
	const struct cmd_arg *carg = arg;
	int err = 0;

	(void)pf;

	err = ua_options_send(uag_current(), carg->prm,
			      options_resp_handler, NULL);
	if (err) {
		warning("menu: ua_options failed: %m\n", err);
	}

	return err;
}


/**
 * Print the SIP Registration for all User-Agents
 *
 * @param pf     Print handler for debug output
 * @param unused Unused parameter
 *
 * @return 0 if success, otherwise errorcode
 */
static int ua_print_reg_status(struct re_printf *pf, void *unused)
{
	struct le *le;
	int err;

	(void)unused;

	err = re_hprintf(pf, "\n--- User Agents (%u) ---\n",
			 list_count(uag_list()));

	for (le = list_head(uag_list()); le && !err; le = le->next) {
		const struct ua *ua = le->data;

		err  = re_hprintf(pf, "%s ", ua == uag_current() ? ">" : " ");
		err |= ua_print_status(pf, ua);
	}

	err |= re_hprintf(pf, "\n");

	return err;
}


static int cmd_ua_next(struct re_printf *pf, void *unused)
{
	struct menu *menu = menu_get();
	int err;

	(void)pf;
	(void)unused;

	if (!menu->le_cur)
		menu->le_cur = list_head(uag_list());
	if (!menu->le_cur)
		return 0;

	menu->le_cur = menu->le_cur->next ?
		menu->le_cur->next : list_head(uag_list());

	err = re_hprintf(pf, "ua: %s\n", ua_aor(list_ledata(menu->le_cur)));

	uag_current_set(list_ledata(menu->le_cur));

	menu_update_callstatus(uag_call_count());

	return err;
}


static int cmd_ua_delete(struct re_printf *pf, void *arg)
{
	const struct cmd_arg *carg = arg;
	struct ua *ua = NULL;

	if (str_isset(carg->prm)) {
		ua = uag_find_aor(carg->prm);
	}

	if (!ua) {
		return ENOENT;
	}

	if (ua == uag_current()) {
		(void)cmd_ua_next(pf, NULL);
	}

	(void)re_hprintf(pf, "deleting ua: %s\n", carg->prm);
	mem_deref(ua);
	(void)ua_print_reg_status(pf, NULL);

	return 0;
}


static int cmd_ua_find(struct re_printf *pf, void *arg)
{
	const struct cmd_arg *carg = arg;
	struct ua *ua = NULL;

	if (str_isset(carg->prm)) {
		ua = uag_find_aor(carg->prm);
	}

	if (!ua) {
		warning("menu: ua_find failed: %s\n", carg->prm);
		return ENOENT;
	}

	re_hprintf(pf, "ua: %s\n", ua_aor(ua));

	uag_current_set(ua);

	menu_update_callstatus(uag_call_count());

	return 0;
}


static int create_ua(struct re_printf *pf, void *arg)
{
	const struct cmd_arg *carg = arg;
	struct ua *ua = NULL;
	struct account *acc;
	int err = 0;

	if (str_isset(carg->prm)) {

		(void)re_hprintf(pf, "Creating UA for %s ...\n", carg->prm);
		err = ua_alloc(&ua, carg->prm);
		if (err)
			goto out;
	}

	acc = ua_account(ua);
	if (account_regint(acc)) {
		if (!account_prio(acc))
			(void)ua_register(ua);
		else
			(void)ua_fallback(ua);
	}

	err = ua_print_reg_status(pf, NULL);

 out:
	if (err) {
		(void)re_hprintf(pf, "menu: create_ua failed: %m\n", err);
	}

	return err;
}


static int switch_video_source(struct re_printf *pf, void *arg)
{
	const struct cmd_arg *carg = arg;
	struct pl pl_driver, pl_device;
	struct config_video *vidcfg;
	struct config *cfg;
	struct video *v;
	const struct vidsrc *vs;
	struct le *le;
	char driver[16], device[128] = "";
	int err = 0;

	if (re_regex(carg->prm, str_len(carg->prm), "[^,]+,[~]*",
		     &pl_driver, &pl_device)) {

		return re_hprintf(pf, "\rFormat should be:"
				  " driver,device\n");
	}

	pl_strcpy(&pl_driver, driver, sizeof(driver));
	pl_strcpy(&pl_device, device, sizeof(device));

	vs = vidsrc_find(data_vidsrcl(), driver);
	if (!vs) {
		re_hprintf(pf, "no such video-source: %s\n", driver);
		return 0;
	}
	else if (!list_isempty(&vs->dev_list)) {

		if (!mediadev_find(&vs->dev_list, device)) {
			re_hprintf(pf,
				   "no such device for %s video-source: %s\n",
				   driver, device);

			mediadev_print(pf, &vs->dev_list);

			return 0;
		}
	}

	re_hprintf(pf, "switch video device: %s,%s\n",
		   driver, device);

	cfg = data_config();
	if (!cfg) {
		return re_hprintf(pf, "no config object\n");
	}

	vidcfg = &cfg->video;

	str_ncpy(vidcfg->src_mod, driver, sizeof(vidcfg->src_mod));
	str_ncpy(vidcfg->src_dev, device, sizeof(vidcfg->src_dev));

	for (le = list_tail(ua_calls(uag_current())); le; le = le->prev) {

		struct call *call = le->data;

		v = call_video(call);

		err = video_set_source(v, driver, device);
		if (err) {
			re_hprintf(pf, "failed to set video-source"
				   " (%m)\n", err);
			break;
		}
	}

	return 0;
}


#ifdef USE_TLS
static int cmd_tls_issuer(struct re_printf *pf, void *unused)
{
	int err = 0;
	struct mbuf *mb;
	(void)unused;

	mb = mbuf_alloc(20);
	if (!mb)
		return ENOMEM;

	err = tls_get_issuer(uag_tls(), mb);
	if (err){
		warning("menu: Unable to get certificate issuer (%m)\n", err);
		goto out;
	}

	(void)re_hprintf(pf, "TLS Cert Issuer: %b\n", mb->buf, mb->pos);

 out:
	mem_deref(mb);
	return err;
}


static int cmd_tls_subject(struct re_printf *pf, void *unused)
{
	int err = 0;
	struct mbuf *mb;
	(void)unused;

	mb = mbuf_alloc(20);
	if (!mb)
		return ENOMEM;

	err = tls_get_subject(uag_tls(), mb);
	if (err) {
		warning("menu: Unable to get certificate subject (%m)\n", err);
		goto out;
	}

	(void)re_hprintf(pf, "TLS Cert Subject: %b\n", mb->buf, mb->pos);

 out:
	mem_deref(mb);
	return err;
}
#endif

/*Static call menu*/
static const struct cmd cmdv[] = {

{"about",     0,          0, "About box",               about_box            },
{"accept",    'a',        0, "Accept incoming call",    cmd_answer           },
{"acceptdir", 0,    CMD_PRM, "Accept incoming call with audio and video"
                             "direction.",              cmd_answerdir        },
{"answermode",0,    CMD_PRM, "Set answer mode",         cmd_set_answermode   },
{"auplay",    0,    CMD_PRM, "Switch audio player",     switch_audio_player  },
{"ausrc",     0,    CMD_PRM, "Switch audio source",     switch_audio_source  },
{"callstat",  'c',        0, "Call status",             ua_print_call_status },
{"dial",      'd',  CMD_PRM, "Dial",                    dial_handler         },
{"dialdir",   0,    CMD_PRM, "Dial with audio and video"
                             "direction.",              cmd_dialdir          },
{"hangup",    'b',        0, "Hangup call",             cmd_hangup           },
{"help",      'h',        0, "Help menu",               print_commands       },
{"listcalls", 'l',        0, "List active calls",       cmd_print_calls      },
{"options",   'o',  CMD_PRM, "Options",                 options_command      },
{"reginfo",   'r',        0, "Registration info",       ua_print_reg_status  },
{"uadel",     0,    CMD_PRM, "Delete User-Agent",       cmd_ua_delete        },
{"uafind",    0,    CMD_PRM, "Find User-Agent <aor>",   cmd_ua_find          },
{"uanew",     0,    CMD_PRM, "Create User-Agent",       create_ua            },
{"uanext",    'T',        0, "Toggle UAs",              cmd_ua_next          },
{"vidsrc",    0,    CMD_PRM, "Switch video source",     switch_video_source  },
{NULL,        KEYCODE_ESC,0, "Hangup call",             cmd_hangup           },

#ifdef USE_TLS
{"tlsissuer", 0,          0, "TLS certificate issuer",  cmd_tls_issuer       },
{"tlssubject",0,          0, "TLS certificate subject", cmd_tls_subject      },
#endif

};

static const struct cmd dialcmdv[] = {
/* Numeric keypad inputs: */
{NULL, '#', CMD_PRM, NULL,   dial_handler },
{NULL, '*', CMD_PRM, NULL,   dial_handler },
{NULL, '0', CMD_PRM, NULL,   dial_handler },
{NULL, '1', CMD_PRM, NULL,   dial_handler },
{NULL, '2', CMD_PRM, NULL,   dial_handler },
{NULL, '3', CMD_PRM, NULL,   dial_handler },
{NULL, '4', CMD_PRM, NULL,   dial_handler },
{NULL, '5', CMD_PRM, NULL,   dial_handler },
{NULL, '6', CMD_PRM, NULL,   dial_handler },
{NULL, '7', CMD_PRM, NULL,   dial_handler },
{NULL, '8', CMD_PRM, NULL,   dial_handler },
{NULL, '9', CMD_PRM, NULL,   dial_handler },
};


/**
 * Register static menu of baresip
 *
 * @return int 0 if success, errorcode otherwise
 */
int static_menu_register(void)
{
	return cmd_register(data_commands(), cmdv, ARRAY_SIZE(cmdv));
}


/**
 * Unregister static menu of baresip
 */
void static_menu_unregister(void)
{
	cmd_unregister(data_commands(), cmdv);
}


/**
 * Register dial menu
 *
 * @return int 0 if success, errorcode otherwise
 */
int dial_menu_register(void)
{
	struct commands *data_cmd = data_commands();

	if (!cmds_find(data_cmd, dialcmdv))
		return cmd_register(data_cmd,
			dialcmdv, ARRAY_SIZE(dialcmdv));

	return 0;
}


/**
 * Unregister dial menu
 */
void dial_menu_unregister(void)
{
	cmd_unregister(data_commands(), dialcmdv);
}
