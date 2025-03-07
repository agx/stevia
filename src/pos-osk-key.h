/*
 * Copyright (C) 2022 Purism SPC
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "pos-enums.h"
#include "pos-enum-types.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Some common symbols used elsewhere */
#define POS_OSK_SYMBOL_SPACE " "
#define POS_OSK_SYMBOL_LEFT "KEY_LEFT"
#define POS_OSK_SYMBOL_RIGHT "KEY_RIGHT"
#define POS_OSK_SYMBOL_UP "KEY_UP"
#define POS_OSK_SYMBOL_DOWN "KEY_DOWN"

#define POS_TYPE_OSK_KEY (pos_osk_key_get_type ())

G_DECLARE_FINAL_TYPE (PosOskKey, pos_osk_key, POS, OSK_KEY, GObject)

#define POS_OSK_KEY_DBG(key) (pos_osk_key_get_label (key) ?: pos_osk_key_get_symbol (key))


PosOskKey          *pos_osk_key_new (const char *label);
void                pos_osk_key_set_width (PosOskKey *self, double width);
double              pos_osk_key_get_width (PosOskKey *self);
PosOskKeyUse        pos_osk_key_get_use (PosOskKey *self);
gboolean            pos_osk_key_get_pressed (PosOskKey *self);
void                pos_osk_key_set_pressed (PosOskKey *self, gboolean pressed);
const char         *pos_osk_key_get_label (PosOskKey *self);
const char         *pos_osk_key_get_icon (PosOskKey *self);
const char         *pos_osk_key_get_symbol (PosOskKey *self);
PosOskWidgetLayer   pos_osk_key_get_layer (PosOskKey *self);
GStrv               pos_osk_key_get_symbols (PosOskKey *self);
void                pos_osk_key_set_box (PosOskKey *self, const GdkRectangle *box);
const GdkRectangle *pos_osk_key_get_box (PosOskKey *self);
gboolean            pos_osk_key_get_expand (PosOskKey *self);

G_END_DECLS
