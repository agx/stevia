/*
 * Copyright (C) 2023-2024 The Phosh Developers
 *               2025 Phosh.mobi e.V.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "pos-completer-base.h"
#include "pos-completer.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define POS_TYPE_COMPLETER_HUNSPELL (pos_completer_hunspell_get_type ())

G_DECLARE_FINAL_TYPE (PosCompleterHunspell, pos_completer_hunspell, POS, COMPLETER_HUNSPELL,
                      PosCompleterBase)

PosCompleter *pos_completer_hunspell_new (GError **error);
gboolean      pos_completer_hunspell_find_dict (const char  *lang,
                                                const char  *region,
                                                char       **aff_path,
                                                char       **dict_path);
G_END_DECLS
