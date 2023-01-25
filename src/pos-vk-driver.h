/*
 * Copyright (C) 2022 Purism SPC
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <pos-virtual-keyboard.h>

#include <glib-object.h>

G_BEGIN_DECLS

#define POS_TYPE_VK_DRIVER (pos_vk_driver_get_type ())

G_DECLARE_FINAL_TYPE (PosVkDriver, pos_vk_driver, POS, VK_DRIVER, GObject)

PosVkDriver *pos_vk_driver_new (PosVirtualKeyboard *virtual_keyboard);
void pos_vk_driver_key_down (PosVkDriver *virtual_keyboard, const char *key);
void pos_vk_driver_key_up (PosVkDriver *virtual_keyboard, const char *key);
void pos_vk_driver_key_press_gdk (PosVkDriver *self, guint gdk_keycode, GdkModifierType modifiers);

G_END_DECLS
