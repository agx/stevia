/*
 * Copyright (C) 2025 The Phosh Developers
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#define G_LOG_DOMAIN "pos-completions-box"

#include "pos-config.h"

#include "pos-completions-box.h"

#include <gmobile.h>
#include <gtk/gtk.h>

/**
 * PosCompletionsBox:
 *
 * A box that contains completions
 */

enum {
  SELECTED,
  N_SIGNALS
};
static guint signals[N_SIGNALS];


struct _PosCompletionsBox {
  GtkBox parent;
};
G_DEFINE_TYPE (PosCompletionsBox, pos_completions_box, GTK_TYPE_BOX)


static void
pos_completions_box_class_init (PosCompletionsBoxClass *klass)
{
  signals[SELECTED] = g_signal_new ("selected",
                                    G_TYPE_FROM_CLASS (klass),
                                    G_SIGNAL_RUN_LAST,
                                    0, NULL, NULL, NULL,
                                    G_TYPE_NONE,
                                    1,
                                    G_TYPE_STRING);
}


static void
pos_completions_box_init (PosCompletionsBox *self)
{
  gtk_orientable_set_orientation (GTK_ORIENTABLE (self), GTK_ORIENTATION_HORIZONTAL);
}


PosCompletionsBox *
pos_completions_box_new (void)
{
  return g_object_new (POS_TYPE_COMPLETIONS_BOX, NULL);
}


static void
on_button_clicked (PosCompletionsBox *self, GtkButton *btn)
{
  const char *completion;

  g_assert (POS_IS_COMPLETIONS_BOX (self));
  g_assert (GTK_IS_BUTTON (btn));

  completion = g_object_get_data (G_OBJECT (btn), "pos-text");
  g_assert (completion != NULL);

  g_signal_emit (self, signals[SELECTED], 0, completion);
}


void
pos_completion_box_set_completions (PosCompletionsBox *self, GStrv completions)
{
  guint n_completions;

  g_return_if_fail (POS_IS_COMPLETIONS_BOX (self));

  gtk_container_foreach (GTK_CONTAINER (self), (GtkCallback) gtk_widget_destroy, NULL);

  if (gm_strv_is_null_or_empty (completions))
    return;

  n_completions = g_strv_length (completions);
  for (int i = 0; i < n_completions; i++) {
    GtkWidget *btn;

    btn = gtk_button_new_with_label (completions[i]);
    gtk_widget_set_visible (GTK_WIDGET (btn), TRUE);
    g_object_set_data_full (G_OBJECT (btn), "pos-text", g_strdup (completions[i]), g_free);

    g_signal_connect_swapped (btn, "clicked", G_CALLBACK (on_button_clicked), self);
    gtk_container_add (GTK_CONTAINER (self), btn);
  }
}
