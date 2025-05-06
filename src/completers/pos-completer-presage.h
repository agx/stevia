/*
 * Copyright (C) 2022 Purism SPC
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "pos-completer.h"
#include "pos-completer-base.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define POS_TYPE_COMPLETER_PRESAGE (pos_completer_presage_get_type ())

G_DECLARE_FINAL_TYPE (PosCompleterPresage, pos_completer_presage, POS, COMPLETER_PRESAGE,
                      PosCompleterBase)

PosCompleter *pos_completer_presage_new (GError **err);

G_END_DECLS
