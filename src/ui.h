/**
 * @file ui.h
 * @brief User Interface
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAUI_H_INCLUDED
#define UAUI_H_INCLUDED

#include "rsua-re/re.h"

typedef int  (ui_output_h)(const char *str);

/** Defines a User-Interface module */
struct ui {
	struct le le;          /**< Linked-list element                   */
	const char *name;      /**< Name of the UI-module                 */
	ui_output_h *outputh;  /**< Handler for output strings (optional) */
};

struct ui_sub;

int ui_init(struct ui_sub **uisp);
void ui_register(struct ui_sub *uis, struct ui *ui);
void ui_unregister(struct ui *ui);

void ui_reset(struct ui_sub *uis);
void ui_input_key(struct ui_sub *uis, char key, struct re_printf *pf);
void ui_input_str(const char *str);
int  ui_input_pl(struct re_printf *pf, const struct pl *pl);
int  ui_input_long_command(struct re_printf *pf, const struct pl *pl);
void ui_output(struct ui_sub *uis, const char *fmt, ...);
bool ui_isediting(const struct ui_sub *uis);
int  ui_password_prompt(char **passwordp);

#endif /* UAUI_H_INCLUDED */
