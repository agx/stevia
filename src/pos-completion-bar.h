/*
 * Copyright (C) 2022 Purism SPC
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define POS_TYPE_COMPLETION_BAR (pos_completion_bar_get_type ())

G_DECLARE_FINAL_TYPE (PosCompletionBar, pos_completion_bar, POS, COMPLETION_BAR, GtkBox)

PosCompletionBar *pos_completion_bar_new (void);
void              pos_completion_bar_set_completions (PosCompletionBar *self,
                                                      GStrv             completions);

G_END_DECLS
