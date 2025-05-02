/*
 * Copyright © 2024 Teemu Ikonen
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "pos-completer.h"

#include <glib.h>

static void
test_capitalize_by_template (void)
{

  char *a1 = "a";
  char *c5 = "ccccc";
  GStrv result = NULL;
  char *completions[3];

  g_assert_null (pos_completer_capitalize_by_template (NULL, NULL));
  g_assert_null (pos_completer_capitalize_by_template ("", NULL));
  g_assert_null (pos_completer_capitalize_by_template ("test", NULL));

  completions[0] = a1;
  completions[1] = c5;
  completions[2] = NULL;
  g_assert_cmpstrv (completions, pos_completer_capitalize_by_template (NULL, completions));
  g_assert_cmpstrv (completions, pos_completer_capitalize_by_template ("", completions));
  g_assert_cmpstrv (completions, pos_completer_capitalize_by_template ("test", completions));

  result = pos_completer_capitalize_by_template ("Test", completions);
  g_assert_nonnull (result);
  g_assert_cmpstr ("A", ==, result[0]);
  g_assert_cmpstr ("Ccccc", ==, result[1]);
  g_assert_null (result[2]);
  g_clear_pointer (&result, g_strfreev);

  result = pos_completer_capitalize_by_template ("tesT", completions);
  g_assert_nonnull (result);
  g_assert_cmpstr ("a", ==, result[0]);
  g_assert_cmpstr ("cccCc", ==, result[1]);
  g_assert_null (result[2]);
  g_clear_pointer (&result, g_strfreev);
}


int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/pos/completer/capitalize_by_template", test_capitalize_by_template);

  return g_test_run ();
}
