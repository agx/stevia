/*
 * Copyright (C) 2025 Phosh.mobi e.V.
 *
 * SPDX-License-Identifier: GPL-3-or-later
 */
#pragma once

#include "xdg-output-unstable-v1-client-protocol.h"

#include <glib.h>
#include <glib-object.h>

#include <wayland-client.h>

G_BEGIN_DECLS

typedef enum wl_transform PosOutputTransform;

#define POS_TYPE_OUTPUT (pos_output_get_type ())

G_DECLARE_FINAL_TYPE (PosOutput, pos_output, POS, OUTPUT, GObject)

PosOutput *        pos_output_new (struct wl_output *output);
gboolean           pos_output_is_portrait (PosOutput *self);
guint              pos_output_get_logical_width (PosOutput *self);
guint              pos_output_get_logical_height (PosOutput *self);
double             pos_output_get_logical_pixel_size (PosOutput *self);
guint              pos_output_get_transformed_phys_width (PosOutput *self);
guint              pos_output_get_transformed_phys_height (PosOutput *self);

G_END_DECLS;
