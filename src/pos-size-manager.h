/*
 * Copyright (C) 2025 Phosh.mobi e.V.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define POS_TYPE_SIZE_MANAGER (pos_size_manager_get_type ())

G_DECLARE_FINAL_TYPE (PosSizeManager, pos_size_manager, POS, SIZE_MANAGER, GObject)

PosSizeManager *pos_size_manager_new (void);

G_END_DECLS
