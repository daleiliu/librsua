/**
 * @file main.c
 * @brief replicate baresip functions
 *
 * Copyright (C) 2010 - 2015 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#include <rsua.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

static struct rsua_opts opts;

/* functions */

static void usage(void)
{
	fprintf(stderr,
			 "Usage: baresip [options]\n"
			 "options:\n"
			 "\t-4               Force IPv4 only\n"
#if HAVE_INET6
			 "\t-6               Force IPv6 only\n"
#endif
			 "\t-d               Daemon\n"
			 "\t-e <commands>    Execute commands (repeat)\n"
			 "\t-f <path>        Config path\n"
			 "\t-m <module>      Pre-load modules (repeat)\n"
			 "\t-p <path>        Audio files\n"
			 "\t-h -?            Help\n"
			 "\t-s               Enable SIP trace\n"
			 "\t-t <sec>         Quit after <sec> seconds\n"
			 "\t-n <net_if>      Specify network interface\n"
			 "\t-u <parameters>  Extra UA parameters\n"
			 "\t-v               Verbose debug\n"
			 );
}

int main(int argc, char *argv[])
{
	int err;

	/*
	 * turn off buffering on stdout
	 */
	setbuf(stdout, NULL);

	fprintf(stdout, "baresip-replica v%s \n"
		"Copyright (C) 2020 Dalei Liu\n"
		"Copyright (C) 2010 - 2020 Alfred E. Heggestad et al.\n",
		RSUA_VERSION);

	memset(&opts, 0, sizeof(opts));
	opts.coredump = 1;
	opts.af = AF_UNSPEC;
	opts.use_conf = 1;
	opts.handle_signal = 1;

	for (;;) {
		const int c = getopt(argc, argv, "46de:f:p:hu:n:vst:m:");
		if (0 > c)
			break;

		switch (c) {

		case '?':
		case 'h':
			usage();
			return -2;

		case '4':
			opts.af = AF_INET;
			break;

#if HAVE_INET6
		case '6':
			opts.af = AF_INET6;
			break;
#endif

		case 'd':
			opts.run_daemon = 1;
			break;

		case 'e':
			if (opts.execmdc >= (int)ARRAY_SIZE(opts.execmdv)) {
				fprintf(stderr, "max %zu commands\n",
					ARRAY_SIZE(opts.execmdv));
				err = EINVAL;
				goto out;
			}
			opts.execmdv[opts.execmdc++] = optarg;
			break;

		case 'f':
			opts.conf_path = optarg;
			break;

		case 'm':
			if (opts.modc >= (int)ARRAY_SIZE(opts.modv)) {
				fprintf(stderr, "max %zu modules\n",
					ARRAY_SIZE(opts.modv));
				err = EINVAL;
				goto out;
			}
			opts.modv[opts.modc++] = optarg;
			break;

		case 'p':
			opts.audio_path = optarg;
			break;

		case 's':
			opts.sip_trace = 1;
			break;

		case 't':
			opts.tmo = atoi(optarg);
			break;

		case 'n':
			opts.net_interface = optarg;
			break;

		case 'u':
			opts.ua_eprm = optarg;
			break;

		case 'v':
			opts.verbose = 1;
			break;

		default:
			break;
		}
	}

	err = rsua_init_fromopts(&opts);
	if (err) {
		fprintf(stderr, "main: rsua_init failed: %s\n", strerror(err));
		goto out;
	}

	/* Main loop */
	err = rsua_start(&opts);
	if (err) {
		fprintf(stderr, "main: rsua_start failed: %s\n", strerror(err));
	}

 out:
	rsua_stop();
	rsua_delete();

	return err;
}
