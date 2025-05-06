/*
 * Copyright (C) 2022 Purism SPC
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#define G_LOG_DOMAIN "pos-completion-bar"

#include "pos-config.h"

#include "pos-completion-bar.h"

enum {
  SELECTED,
  N_SIGNALS
};
static guint signals[N_SIGNALS];

/**
 * PosCompletionBar:
 *
 * A button bar that displays completions and emits "selected" if one
 * is picked.
 */
struct _PosCompletionBar {
  GtkBox             parent;

  GtkBox            *buttons;
  GtkScrolledWindow *scrolled_window;
};
G_DEFINE_TYPE (PosCompletionBar, pos_completion_bar, GTK_TYPE_BOX)


static void
pos_completion_bar_class_init (PosCompletionBarClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  signals[SELECTED] = g_signal_new ("selected",
                                    G_TYPE_FROM_CLASS (klass),
                                    G_SIGNAL_RUN_LAST,
                                    0, NULL, NULL, NULL,
                                    G_TYPE_NONE,
                                    1,
                                    G_TYPE_STRING);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/mobi/phosh/osk-stub/ui/completion-bar.ui");
  gtk_widget_class_bind_template_child (widget_class, PosCompletionBar, buttons);
  gtk_widget_class_bind_template_child (widget_class, PosCompletionBar, scrolled_window);

  gtk_widget_class_set_css_name (widget_class, "pos-completion-bar");
}


static void
pos_completion_bar_init (PosCompletionBar *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}


PosCompletionBar *
pos_completion_bar_new (void)
{
  return POS_COMPLETION_BAR (g_object_new (POS_TYPE_COMPLETION_BAR, NULL));
}


static void
on_button_clicked (PosCompletionBar *self, GtkButton *btn)
{
  const char *completion;

  g_assert (POS_IS_COMPLETION_BAR (self));
  g_assert (GTK_IS_BUTTON (btn));

  completion = g_object_get_data (G_OBJECT (btn), "pos-text");
  g_assert (completion != NULL);

  g_signal_emit (self, signals[SELECTED], 0, completion);
}


void
pos_completion_bar_set_completions (PosCompletionBar *self, GStrv completions)
{
  guint swidth, bwidth;
  gboolean overflow;

  g_return_if_fail (POS_IS_COMPLETION_BAR (self));

  gtk_container_foreach (GTK_CONTAINER (self->buttons), (GtkCallback) gtk_widget_destroy, NULL);

  if (completions == NULL)
    return;

  gtk_widget_set_hexpand (GTK_WIDGET (self->buttons), TRUE);
  gtk_box_set_homogeneous (self->buttons, TRUE);
  gtk_widget_set_halign (GTK_WIDGET (self->buttons), GTK_ALIGN_FILL);

  for (int i = 0; i < g_strv_length (completions); i++) {
    GtkWidget *lbl, *btn;

    lbl = g_object_new (GTK_TYPE_LABEL,
                        "label", completions[i],
                        "visible", TRUE,
                        NULL);
    btn = g_object_new (GTK_TYPE_BUTTON,
                        "child", lbl,
                        "visible", TRUE,
                        NULL);
    g_object_set_data_full (G_OBJECT (btn), "pos-text", g_strdup (completions[i]), g_free);

    g_signal_connect_swapped (btn, "clicked", G_CALLBACK (on_button_clicked), self);
    gtk_container_add (GTK_CONTAINER (self->buttons), btn);
  }

  swidth = gtk_widget_get_allocated_width (GTK_WIDGET (self->scrolled_window));
  bwidth = gtk_widget_get_allocated_width (GTK_WIDGET (self->buttons));

  overflow = (bwidth > swidth);
  /* If elements don't fit turn off homogeneous to fit more of them */
  if (overflow) {
    gtk_box_set_homogeneous (self->buttons, FALSE);
    gtk_widget_set_halign (GTK_WIDGET (self->buttons), GTK_ALIGN_CENTER);
    //gtk_widget_queue_draw (GTK_WIDGET (self->buttons));
  }
}
