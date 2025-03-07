/*
 * Copyright (C) 2025 The Phosh Developers
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#include "pos-osk-widget.c"


#include "pos-osk-key.h"
#include "pos-main.h"

#include <glib.h>

static void
test_switch_layer (void)
{
  gboolean success;
  g_autoptr (GError) err = NULL;
  PosOskWidget *osk_widget = pos_osk_widget_new (PHOSH_OSK_FEATURE_DEFAULT);
  PosOskWidgetRow *row;
  PosOskKey *shift, *symbols, *any;

  pos_init ();

  success = pos_osk_widget_set_layout (osk_widget, "us", "us", "English (US)", "us", NULL, &err);
  g_assert_no_error (err);
  g_assert_true (success);

  pos_osk_widget_set_layer (osk_widget, POS_OSK_WIDGET_LAYER_NORMAL);

  row = pos_osk_widget_get_row (osk_widget, 2);
  shift = g_ptr_array_index (row->keys, 0);
  g_assert_cmpint (pos_osk_key_get_use (shift), ==, POS_OSK_KEY_USE_TOGGLE);
  g_assert_cmpint (pos_osk_key_get_layer (shift), ==, POS_OSK_WIDGET_LAYER_CAPS);

  row = pos_osk_widget_get_row (osk_widget, 3);
  symbols = g_ptr_array_index (row->keys, 0);
  g_assert_cmpint (pos_osk_key_get_use (symbols), ==, POS_OSK_KEY_USE_TOGGLE);
  g_assert_cmpint (pos_osk_key_get_layer (symbols), ==, POS_OSK_WIDGET_LAYER_SYMBOLS);

  row = pos_osk_widget_get_row (osk_widget, 0);
  any = g_ptr_array_index (row->keys, 0);
  g_assert_cmpint (pos_osk_key_get_use (any), ==, POS_OSK_KEY_USE_KEY);

  /* Regular key doesn't switch layers */
  pos_osk_widget_set_layer (osk_widget, POS_OSK_WIDGET_LAYER_NORMAL);
  switch_layer (osk_widget, any);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_NORMAL);

  /* shift switches to caps layer… */
  pos_osk_widget_set_layer (osk_widget, POS_OSK_WIDGET_LAYER_NORMAL);
  switch_layer (osk_widget, shift);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_CAPS);
  /* …and back  to normal */
  switch_layer (osk_widget, shift);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_NORMAL);

  /* symbols switches to symbols layer… */
  pos_osk_widget_set_layer (osk_widget, POS_OSK_WIDGET_LAYER_NORMAL);
  switch_layer (osk_widget, symbols);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_SYMBOLS);
  /* …and back  to normal */
  switch_layer (osk_widget, symbols);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_NORMAL);

  /* symbols then shift switches to symbols2 layer… */
  pos_osk_widget_set_layer (osk_widget, POS_OSK_WIDGET_LAYER_NORMAL);
  switch_layer (osk_widget, symbols);
  switch_layer (osk_widget, shift);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_SYMBOLS2);
  /* … shift then back  to symbols */
  switch_layer (osk_widget, shift);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_SYMBOLS);
  /* … symbols then back to normal */
  switch_layer (osk_widget, symbols);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_NORMAL);

  /* shift then symbols switches to symbols2 layer… */
  pos_osk_widget_set_layer (osk_widget, POS_OSK_WIDGET_LAYER_NORMAL);
  switch_layer (osk_widget, shift);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_CAPS);
  switch_layer (osk_widget, symbols);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_SYMBOLS2);
  /* … symbols then back to normal */
  switch_layer (osk_widget, symbols);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_NORMAL);
  /* shift on symbols2 goes back to symbols */
  pos_osk_widget_set_layer (osk_widget, POS_OSK_WIDGET_LAYER_SYMBOLS2);
  switch_layer (osk_widget, shift);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_SYMBOLS);

  /* shift switches caps lock back to normal */
  pos_osk_widget_set_layer (osk_widget, POS_OSK_WIDGET_LAYER_NORMAL);
  set_caps_lock (osk_widget, TRUE);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_CAPS);
  switch_layer (osk_widget, shift);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_NORMAL);
  g_assert_false (osk_widget->caps_lock);

  /* symbol switches caps lock to symbol and disables caps lock */
  pos_osk_widget_set_layer (osk_widget, POS_OSK_WIDGET_LAYER_NORMAL);
  set_caps_lock (osk_widget, TRUE);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_CAPS);
  switch_layer (osk_widget, symbols);
  g_assert_cmpint (pos_osk_widget_get_layer (osk_widget), ==, POS_OSK_WIDGET_LAYER_SYMBOLS2);
  g_assert_false (osk_widget->caps_lock);
}


int
main (int argc, char *argv[])
{
  gtk_test_init (&argc, &argv, NULL);

  g_test_add_func ("/pos/osk-widget/switch_layer", test_switch_layer);

  return g_test_run ();
}
