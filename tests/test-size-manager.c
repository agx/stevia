/*
 * Copyright (C) 2025 Phosh.mobi e.V.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#include "pos-config.h"

#include "pos-size-manager.c"

#include "phosh-osk-enums.h"
#include "pos-output-priv.h"

#include <glib.h>

#define ALL_FLAGS (PHOSH_OSK_SCALING_AUTO_PORTRAIT | PHOSH_OSK_SCALING_BOTTOM_DEAD_ZONE)

static void
test_size_manager_landscape (void)
{
  PosOutput *output = g_object_new (POS_TYPE_OUTPUT, NULL);
  guint dz;

  pos_output_set_logical_width (output, 1000);
  pos_output_set_logical_height (output, 100);

  g_assert_cmpint (calc_height (output, &dz, ALL_FLAGS), ==, POS_INPUT_SURFACE_DEFAULT_HEIGHT);
}


static void
test_size_manager_portrait_too_little_logical_height (void)
{
  PosOutput *output = g_object_new (POS_TYPE_OUTPUT, NULL);
  guint dz;

  pos_output_set_logical_width (output, 100);
  pos_output_set_logical_height (output, 220);

  g_assert_cmpint (calc_height (output, &dz, ALL_FLAGS), ==, POS_INPUT_SURFACE_DEFAULT_HEIGHT);
}


static void
test_size_manager_portrait_too_little_physical_height (void)
{
  PosOutput *output = g_object_new (POS_TYPE_OUTPUT, NULL);
  double pixel_size;
  guint dz;

  pos_output_set_logical_width (output, 360);
  pos_output_set_logical_height (output, 720);
  pos_output_set_physical_height (output, 60);
  pixel_size = pos_output_get_logical_pixel_size (output);

  g_assert_cmpfloat (pixel_size, >, 0.08);
  g_assert_cmpint (calc_height (output, &dz, ALL_FLAGS), ==, POS_INPUT_SURFACE_DEFAULT_HEIGHT);
}

/* OnePlus 6T at scale 2.5 */
static void
test_size_manager_portrait_scale_up_op6t_at_250 (void)
{
  PosOutput *output = g_object_new (POS_TYPE_OUTPUT, NULL);
  double pixel_size;
  guint height, dz;

  pos_output_set_logical_width (output, 1080 / 2.5);
  pos_output_set_logical_height (output, 2340 / 2.5);
  pos_output_set_physical_height (output, 145);
  pixel_size = pos_output_get_logical_pixel_size (output);

  g_assert_cmpfloat (pixel_size, >, 0.06);
  height = calc_height (output, &dz, ALL_FLAGS);

  g_assert_cmpint (height, ==, 258);
  g_assert_cmpint (dz, ==, 51);
}

/* Librem 5 at scale 2 */
static void
test_size_manager_portrait_scale_up_l5_at_200 (void)
{
  PosOutput *output = g_object_new (POS_TYPE_OUTPUT, NULL);
  double pixel_size;
  guint height, dz;

  pos_output_set_logical_width (output, 720 / 2.0);
  pos_output_set_logical_height (output, 1440 / 2.0);
  pos_output_set_physical_height (output, 130);
  pixel_size = pos_output_get_logical_pixel_size (output);

  g_assert_cmpfloat (pixel_size, >, 0.09);
  height = calc_height (output, &dz, ALL_FLAGS);
  g_assert_cmpint (height, ==, 221);
  g_assert_cmpint (dz, ==, 44);
}


int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/pos/size-manager/landscape", test_size_manager_landscape);
  g_test_add_func ("/pos/size-manager/portrait/too_little_logical_height",
                   test_size_manager_portrait_too_little_logical_height);
  g_test_add_func ("/pos/size-manager/portrait/too_little_physical_height",
                   test_size_manager_portrait_too_little_physical_height);
  g_test_add_func ("/pos/size-manager/portrait/scale_up/op6t@250",
                   test_size_manager_portrait_scale_up_op6t_at_250);
  g_test_add_func ("/pos/size-manager/portrait/scale_up/l5@200",
                   test_size_manager_portrait_scale_up_l5_at_200);

  return g_test_run ();
}
