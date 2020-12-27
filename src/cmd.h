/**
 * @file cmd.h
 * @brief Command interface
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UACMD_H_INCLUDED
#define UACMD_H_INCLUDED

#include "rsua-re/re.h"

/* special keys */
#define KEYCODE_NONE   (0x00)    /* No key           */
#define KEYCODE_REL    (0x04)    /* Key was released */
#define KEYCODE_ESC    (0x1b)    /* Escape key       */


/** Command flags */
enum {
	CMD_PRM  = (1<<0),              /**< Command with parameter */
};

/** Command arguments */
struct cmd_arg {
	char key;         /**< Which key was pressed  */
	char *prm;        /**< Optional parameter     */
	void *data;       /**< Application data       */
};

/** Defines a command */
struct cmd {
	const char *name; /**< Long command           */
	char key;         /**< Short command          */
	int flags;        /**< Optional command flags */
	const char *desc; /**< Description string     */
	re_printf_h *h;   /**< Command handler        */
};

struct cmd_ctx;
struct commands;


int  cmd_init(struct commands **commandsp);
int  cmd_register(struct commands *commands,
		  const struct cmd *cmdv, size_t cmdc);
void cmd_unregister(struct commands *commands, const struct cmd *cmdv);
int  cmd_process(struct commands *commands, struct cmd_ctx **ctxp, char key,
		 struct re_printf *pf, void *data);
int  cmd_process_long(struct commands *commands, const char *str, size_t len,
		      struct re_printf *pf_resp, void *data);
int cmd_print(struct re_printf *pf, const struct commands *commands);
const struct cmd *cmd_find_long(const struct commands *commands,
				const char *name);
struct cmds *cmds_find(const struct commands *commands,
		       const struct cmd *cmdv);

#endif /* UACMD_H_INCLUDED */
