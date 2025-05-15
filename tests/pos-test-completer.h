/*
 * Copyright © 2025 The Phosh Developers
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <glib.h>

G_BEGIN_DECLS

#define pos_completer_assert_initial_state(c, n, d) G_STMT_START { \
  g_assert_cmpstr (pos_completer_get_name (c), ==, n); \
  g_assert_cmpstr (pos_completer_get_display_name (c), ==, d); \
  g_assert_cmpstrv (pos_completer_get_completions (c), NULL); \
  g_assert_cmpstr (pos_completer_get_preedit (c), ==, ""); \
  g_assert_cmpstr (pos_completer_get_before_text (c), ==, ""); \
  g_assert_cmpstr (pos_completer_get_after_text (c), ==, ""); \
} G_STMT_END

G_END_DECLS
