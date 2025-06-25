/*
 * Copyright (C) 2023 The Phosh Developers
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#define G_LOG_DOMAIN "pos-char-popup"

#include "pos-config.h"

#include "pos-char-popup.h"

enum {
  SELECTED,
  N_SIGNALS
};
static guint signals[N_SIGNALS];

enum {
  PROP_0,
  PROP_SYMBOLS,
  PROP_LAST_PROP
};
static GParamSpec *props[PROP_LAST_PROP];

/**
 * PosCharPopup:
 *
 * A character popup
 */
struct _PosCharPopup {
  GtkPopover parent;

  GtkWidget *symbols_grid;
};
G_DEFINE_TYPE (PosCharPopup, pos_char_popup, GTK_TYPE_POPOVER)


static void
pos_char_popup_set_property (GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  PosCharPopup *self = POS_CHAR_POPUP (object);

  switch (property_id) {
  case PROP_SYMBOLS:
    pos_char_popup_set_symbols (self, g_value_get_boxed (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
pos_char_popup_get_property (GObject    *object,
                             guint       property_id,
                             GValue     *value,
                             GParamSpec *pspec)
{

  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
pos_char_popup_class_init (PosCharPopupClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->get_property = pos_char_popup_get_property;
  object_class->set_property = pos_char_popup_set_property;

  props[PROP_SYMBOLS] =
    g_param_spec_boxed ("symbols",
                        "",
                        "",
                        G_TYPE_STRV,
                        G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, PROP_LAST_PROP, props);

  signals[SELECTED] = g_signal_new ("selected",
                                    G_TYPE_FROM_CLASS (klass),
                                    G_SIGNAL_RUN_LAST,
                                    0, NULL, NULL, NULL,
                                    G_TYPE_NONE,
                                    1,
                                    G_TYPE_STRING);

  gtk_widget_class_set_css_name (widget_class, "pos-char-popup");

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/mobi/phosh/stevia/ui/char-popup.ui");
  gtk_widget_class_bind_template_child (widget_class, PosCharPopup, symbols_grid);
}


static void
pos_char_popup_init (PosCharPopup *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}


PosCharPopup *
pos_char_popup_new (GtkWidget *relative_to, GStrv symbols)
{
  return POS_CHAR_POPUP (g_object_new (POS_TYPE_CHAR_POPUP,
                                       "relative-to", relative_to,
                                       "symbols", symbols,
                                       NULL));
}


static void
on_button_clicked (PosCharPopup *self, GtkButton *btn)
{
  const char *symbol;

  symbol = gtk_button_get_label (btn);

  g_signal_emit (self, signals[SELECTED], 0, symbol);
}


static guint
elements_per_row (guint n_syms)
{
  switch (n_syms) {
  case 0 ... 4:
    /* one row */
    return n_syms;
  case 5 ... 10:
    /* two rows */
    return (n_syms + 1) / 2;
  default:
    return n_syms / 5;
  }
}

void
pos_char_popup_set_symbols (PosCharPopup *self, GStrv symbols)
{
  guint n_per_row, n_syms;
  int left = 0, top = 0;

  g_return_if_fail (POS_IS_CHAR_POPUP (self));

  gtk_container_foreach (GTK_CONTAINER (self->symbols_grid),
                         (GtkCallback) gtk_widget_destroy, NULL);

  if (symbols == NULL)
    return;

  n_syms = g_strv_length (symbols);
  n_per_row = elements_per_row (n_syms);

  for (int i = 0; i < n_syms; i++) {
    GtkWidget *btn = gtk_button_new_with_label (symbols[i]);

    g_signal_connect_swapped (btn, "clicked", G_CALLBACK (on_button_clicked), self);
    gtk_widget_set_visible (btn, TRUE);

    if (left == n_per_row) {
      left = 0;
      top++;
    }

    gtk_grid_attach (GTK_GRID (self->symbols_grid), btn, left, top, 1, 1);
    left++;
  }
}
