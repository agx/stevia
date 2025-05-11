/*
 * Copyright © 2025 The Phosh Developers
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#include "pos-config.h"

#include "pos-test-completer.h"

#include "pos-completer-pipe.h"
#ifdef POS_HAVE_FZF
# include "pos-completer-fzf.h"
#endif
#ifdef POS_HAVE_VARNAM
# include "pos-completer-varnam.h"
#endif
#ifdef POS_HAVE_HUNSPELL
# include "pos-completer-hunspell.h"
#endif

#include <gio/gio.h>
#include <glib.h>

#define test_simple_completer(t, n, d) G_STMT_START {     \
  PosCompleter *completer; \
  g_autoptr (GError) err = NULL; \
  completer = pos_completer_##t##_new (&err);    \
  g_assert_no_error (err); \
  pos_completer_assert_initial_state (completer, n, d); \
  g_assert_finalize_object (completer); \
} G_STMT_END


static void
test_completer_pipe (void)
{
  test_simple_completer (pipe, "pipe", NULL);
}


static void
test_completer_fzf (void)
{
#ifdef POS_HAVE_FZF
  test_simple_completer (fzf, "fzf", NULL);
# else
  g_test_skip ("fzf completer not available");
#endif
}


static void
test_completer_varnam (void)
{
#ifdef POS_HAVE_VARNAM
  test_simple_completer (varnam, NULL, "Malayalam");
#else
  g_test_skip ("Varnam completer not available");
#endif
}


static void
test_completer_hunspell (void)
{
#ifdef POS_HAVE_HUNSPELL
  if (!pos_completer_hunspell_find_dict (POS_COMPLETER_DEFAULT_LANG,
                                         POS_COMPLETER_DEFAULT_REGION,
                                         NULL,
                                         NULL)) {
    g_test_skip ("Hunspell data not found");
    return;
  }
  test_simple_completer (hunspell, "hunspell", NULL);
#else
  g_test_skip ("Hunspell completer not available");
#endif
}


int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/pos/completer/pipe", test_completer_pipe);
  g_test_add_func ("/pos/completer/fzf", test_completer_fzf);
  g_test_add_func ("/pos/completer/varnam", test_completer_varnam);
  g_test_add_func ("/pos/completer/hunspell", test_completer_hunspell);

  return g_test_run ();
}
