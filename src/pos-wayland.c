/*
 * Copyright (C) 2025 Phosh.mobi e.V.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#define G_LOG_DOMAIN "pos-wayland"

#include "pos-config.h"
#include "pos-wayland.h"

#include <gdk/gdkwayland.h>

/**
 * PosWayland:
 *
 * A wayland registry listener
 *
 * The #PosWayland singleton is responsible for listening to wayland
 * registry events registering the objects that show up there to make
 * them available to Pos's other classes.
 */

enum {
  READY,
  N_SIGNALS
};
static guint signals[N_SIGNALS];

struct _PosWayland {
  GObject                                  parent;

  struct wl_display                       *display;
  struct wl_registry                      *registry;
  struct zwp_input_method_manager_v2      *input_method_manager;
  struct zwp_virtual_keyboard_manager_v1  *zwp_virtual_keyboard_manager_v1;
  struct wl_seat                          *wl_seat;
  struct zwlr_foreign_toplevel_manager_v1 *zwlr_foreign_toplevel_manager_v1;
  struct zwlr_layer_shell_v1              *layer_shell;
  struct zxdg_output_manager_v1           *zxdg_output_manager_v1;
  struct zphoc_device_state_v1            *zphoc_device_state_v1;
  struct zwlr_data_control_manager_v1     *wlr_data_control_manager;

  gboolean                                 ready;
};

G_DEFINE_TYPE (PosWayland, pos_wayland, G_TYPE_OBJECT)


static void
registry_handle_global (void               *data,
                        struct wl_registry *registry,
                        uint32_t            name,
                        const char         *interface,
                        uint32_t            version)
{
  PosWayland *self = POS_WAYLAND (data);

  g_assert (POS_IS_WAYLAND (self));

  if (strcmp (interface, zwp_input_method_manager_v2_interface.name) == 0) {
    self->input_method_manager = wl_registry_bind (registry, name,
                                                   &zwp_input_method_manager_v2_interface, 1);
  } else if (strcmp (interface, wl_seat_interface.name) == 0) {
    self->wl_seat = wl_registry_bind (registry, name, &wl_seat_interface, version);
  } else if (!strcmp (interface, zwlr_layer_shell_v1_interface.name)) {
    self->layer_shell = wl_registry_bind (registry, name, &zwlr_layer_shell_v1_interface, 1);
  } else if (!strcmp (interface, zwlr_foreign_toplevel_manager_v1_interface.name)) {
    self->zwlr_foreign_toplevel_manager_v1 = wl_registry_bind (registry, name,
                                                               &zwlr_foreign_toplevel_manager_v1_interface,
                                                               1);
  } else if (!strcmp (interface, zwp_virtual_keyboard_manager_v1_interface.name)) {
    self->zwp_virtual_keyboard_manager_v1 = wl_registry_bind (registry, name,
                                                              &zwp_virtual_keyboard_manager_v1_interface,
                                                              1);
  } else if (!strcmp (interface, zphoc_device_state_v1_interface.name)) {
    self->zphoc_device_state_v1 = wl_registry_bind (registry, name,
                                                    &zphoc_device_state_v1_interface,
                                                    MIN (2, version));
  } else if (!strcmp (interface, zwlr_data_control_manager_v1_interface.name)) {
    self->wlr_data_control_manager = wl_registry_bind (registry, name,
                                                       &zwlr_data_control_manager_v1_interface, 1);
  } else if (!strcmp (interface, zxdg_output_manager_v1_interface.name)) {
    self->zxdg_output_manager_v1 = wl_registry_bind (registry,
                                                     name,
                                                     &zxdg_output_manager_v1_interface,
                                                     3);
  }

  if (pos_wayland_has_wl_protcols (self) && !self->ready) {
    g_debug ("Bound all reqeuired protocols");
    self->ready = TRUE;
    g_signal_emit (self, signals[READY], 0);
  }
}


static void
registry_handle_global_remove (void               *data,
                               struct wl_registry *registry,
                               uint32_t            name)
{
  g_warning ("Global %d removed but not handled", name);
}


static const struct wl_registry_listener registry_listener = {
  registry_handle_global,
  registry_handle_global_remove
};


static void
register_globals (gpointer data)
{
  PosWayland *self = POS_WAYLAND (data);

  self->registry = wl_display_get_registry (self->display);
  wl_registry_add_listener (self->registry, &registry_listener, self);

  /* Wait until we have been notified about the wayland globals we require */
  wl_display_roundtrip (self->display);
}


static void
pos_wayland_constructed (GObject *object)
{
  PosWayland *self = POS_WAYLAND (object);
  GdkDisplay *gdk_display;

  G_OBJECT_CLASS (pos_wayland_parent_class)->constructed (object);

  gdk_set_allowed_backends ("wayland");
  gdk_display = gdk_display_get_default ();
  self->display = gdk_wayland_display_get_wl_display (gdk_display);

  if (self->display == NULL)
    g_error ("Failed to get display: %m\n");

  g_idle_add_once (register_globals, self);
}


static void
pos_wayland_dispose (GObject *object)
{
  PosWayland *self = POS_WAYLAND (object);


  g_clear_pointer (&self->registry, wl_registry_destroy);

  g_clear_pointer (&self->input_method_manager, &zwp_input_method_manager_v2_destroy);
  g_clear_pointer (&self->zwp_virtual_keyboard_manager_v1,
                   zwp_virtual_keyboard_manager_v1_destroy);
  g_clear_pointer (&self->wl_seat, wl_seat_destroy);
  g_clear_pointer (&self->zwlr_foreign_toplevel_manager_v1,
                   zwlr_foreign_toplevel_manager_v1_destroy);
  g_clear_pointer (&self->layer_shell, &zwlr_layer_shell_v1_destroy);
  g_clear_pointer (&self->zxdg_output_manager_v1, zxdg_output_manager_v1_destroy);
  g_clear_pointer (&self->zphoc_device_state_v1, zphoc_device_state_v1_destroy);
  g_clear_pointer (&self->wlr_data_control_manager, zwlr_data_control_manager_v1_destroy);

  G_OBJECT_CLASS (pos_wayland_parent_class)->dispose (object);
}


static void
pos_wayland_class_init (PosWaylandClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = pos_wayland_constructed;
  object_class->dispose = pos_wayland_dispose;

  signals[READY] =
    g_signal_new ("ready",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL,
                  G_TYPE_NONE, 0);
}


static void
pos_wayland_init (PosWayland *self)
{
}

/**
 * pos_wayland_get_default:
 *
 * Get the Wayland singleton for handling Wayland protocol
 * interactions
 *
 * Returns:(transfer none): The Wayland singleton
 */
PosWayland *
pos_wayland_get_default (void)
{
  static PosWayland *instance;

  if (instance == NULL) {
    instance = g_object_new (POS_TYPE_WAYLAND, NULL);
    g_object_add_weak_pointer (G_OBJECT (instance), (gpointer *)&instance);
  }
  return instance;
}


struct zwp_input_method_manager_v2 *
pos_wayland_get_zwp_input_method_manager_v2 (PosWayland *self)
{
  g_assert (POS_IS_WAYLAND (self));

  return self->input_method_manager;
}


struct zwp_virtual_keyboard_manager_v1 *
pos_wayland_get_zwp_virtual_keyboard_manager_v1 (PosWayland *self)
{
  g_assert (POS_IS_WAYLAND (self));

  return self->zwp_virtual_keyboard_manager_v1;
}


struct wl_seat *
pos_wayland_get_wl_seat (PosWayland *self)
{
  g_assert (POS_IS_WAYLAND (self));

  return self->wl_seat;
}


struct zwlr_foreign_toplevel_manager_v1 *
pos_wayland_get_zwlr_foreign_toplevel_manager_v1 (PosWayland *self)
{
  g_assert (POS_IS_WAYLAND (self));

  return self->zwlr_foreign_toplevel_manager_v1;
}


struct zwlr_layer_shell_v1 *
pos_wayland_get_zwlr_layer_shell_v1 (PosWayland *self)
{
  g_assert (POS_IS_WAYLAND (self));

  return self->layer_shell;
}


struct zxdg_output_manager_v1*
pos_wayland_get_zxdg_output_manager_v1 (PosWayland *self)
{
  g_assert (POS_IS_WAYLAND (self));

  return self->zxdg_output_manager_v1;
}


struct zphoc_device_state_v1 *
pos_wayland_get_zphoc_device_state_v1 (PosWayland *self)
{
  g_assert (POS_IS_WAYLAND (self));

  return self->zphoc_device_state_v1;
}


struct zwlr_data_control_manager_v1 *
pos_wayland_get_zwlr_data_control_manager_v1 (PosWayland *self)
{
  g_assert (POS_IS_WAYLAND (self));

  return self->wlr_data_control_manager;
}


gboolean
pos_wayland_has_wl_protcols (PosWayland *self)
{
  g_assert (POS_WAYLAND (self));

  return (self->input_method_manager &&
          self->zwp_virtual_keyboard_manager_v1 &&
          self->wl_seat &&
          self->zwlr_foreign_toplevel_manager_v1 &&
          self->layer_shell &&
          self->zxdg_output_manager_v1 &&
          self->zphoc_device_state_v1 &&
          self->wlr_data_control_manager);
}
