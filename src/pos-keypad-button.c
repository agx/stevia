/*
 * Copyright (C) 2024 The Phosh Developers
 *
 * SPDX-License-Identifier: GPL-3-or-later
 *
 * loosely based on CuiKeypad which is
 * Copyright (C) 2021 Purism SPC
 */

#include "pos-config.h"

#include <glib.h>
#include <glib/gi18n-lib.h>

#include "pos-keypad-button.h"

enum {
  PROP_0,
  PROP_DIGIT,
  PROP_SYMBOLS,
  PROP_SHOW_SYMBOLS,
  PROP_LAST_PROP,
};
static GParamSpec *props[PROP_LAST_PROP];

struct _PosKeypadButton {
  GtkButton parent_instance;

  GtkLabel *label, *secondary_label;
  gchar    *symbols;
};

G_DEFINE_TYPE (PosKeypadButton, pos_keypad_button, GTK_TYPE_BUTTON)

static void
format_label (PosKeypadButton *self)
{
  g_autofree gchar *text = NULL;
  gchar *secondary_text = NULL;

  if (self->symbols != NULL && *(self->symbols) != '\0') {
    secondary_text = g_utf8_find_next_char (self->symbols, NULL);
    text = g_strndup (self->symbols, 1);
  }

  gtk_label_set_label (self->label, text);
  gtk_label_set_label (self->secondary_label, secondary_text);
}

static void
pos_keypad_button_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  PosKeypadButton *self = POS_KEYPAD_BUTTON (object);

  switch (property_id) {
  case PROP_SYMBOLS:
    if (g_strcmp0 (self->symbols, g_value_get_string (value)) != 0) {
      g_free (self->symbols);
      self->symbols = g_value_dup_string (value);
      format_label (self);
      g_object_notify_by_pspec (G_OBJECT (self), props[PROP_SYMBOLS]);
    }
    break;

  case PROP_SHOW_SYMBOLS:
    pos_keypad_button_show_symbols (self, g_value_get_boolean (value));
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
pos_keypad_button_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  PosKeypadButton *self = POS_KEYPAD_BUTTON (object);

  switch (property_id) {
  case PROP_DIGIT:
    g_value_set_schar (value, pos_keypad_button_get_digit (self));
    break;

  case PROP_SYMBOLS:
    g_value_set_string (value, pos_keypad_button_get_symbols (self));
    break;

  case PROP_SHOW_SYMBOLS:
    g_value_set_boolean (value, gtk_widget_is_visible (GTK_WIDGET (self->secondary_label)));
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
pos_keypad_button_finalize (GObject *object)
{
  PosKeypadButton *self = POS_KEYPAD_BUTTON (object);

  g_clear_pointer (&self->symbols, g_free);
  G_OBJECT_CLASS (pos_keypad_button_parent_class)->finalize (object);
}


static void
pos_keypad_button_class_init (PosKeypadButtonClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->set_property = pos_keypad_button_set_property;
  object_class->get_property = pos_keypad_button_get_property;

  object_class->finalize = pos_keypad_button_finalize;

  props[PROP_DIGIT] =
    g_param_spec_int ("digit", "", "",
                      -1, G_MAXINT, 0,
                      G_PARAM_READABLE);

  props[PROP_SYMBOLS] =
    g_param_spec_string ("symbols", "", "",
                         "",
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  props[PROP_SHOW_SYMBOLS] =
    g_param_spec_boolean ("show-symbols", "", "",
                          TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, PROP_LAST_PROP, props);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/mobi/phosh/stevia/ui/keypad-button.ui");
  gtk_widget_class_bind_template_child (widget_class, PosKeypadButton, label);
  gtk_widget_class_bind_template_child (widget_class, PosKeypadButton, secondary_label);
}

static void
pos_keypad_button_init (PosKeypadButton *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  self->symbols = NULL;
}

/**
 * pos_keypad_button_new:
 * @symbols: (nullable): the symbols displayed on the #PosKeypadButton
 *
 * Create a new #PosKeypadButton which displays @symbols,
 * where the first char is used as the main and the other symbols are shown below
 *
 * Returns: the newly created #PosKeypadButton widget
 */
GtkWidget *
pos_keypad_button_new (const gchar *symbols)
{
  return g_object_new (POS_TYPE_KEYPAD_BUTTON, "symbols", symbols, NULL);
}

/**
 * pos_keypad_button_get_digit:
 * @self: a #PosKeypadButton
 *
 * Get the #PosKeypadButton's digit.
 *
 * Returns: the button's digit
 */
char
pos_keypad_button_get_digit (PosKeypadButton *self)
{
  g_return_val_if_fail (POS_IS_KEYPAD_BUTTON (self), '\0');

  if (self->symbols == NULL)
    return ('\0');

  return *(self->symbols);
}

/**
 * pos_keypad_button_get_symbols:
 * @self: a #PosKeypadButton
 *
 * Get the #PosKeypadButton's symbols.
 *
 * Returns: the button's symbols including the digit.
 */
const char*
pos_keypad_button_get_symbols (PosKeypadButton *self)
{
  g_return_val_if_fail (POS_IS_KEYPAD_BUTTON (self), NULL);

  return self->symbols;
}

/**
 * pos_keypad_button_show_symbols:
 * @self: a #PosKeypadButton
 * @visible: whether the second line should be shown or not
 *
 * Sets the visibility of the second line of symbols for #PosKeypadButton
 *
 */
void
pos_keypad_button_show_symbols (PosKeypadButton *self, gboolean visible)
{
  gboolean old_visible;

  g_return_if_fail (POS_IS_KEYPAD_BUTTON (self));

  old_visible = gtk_widget_get_visible (GTK_WIDGET (self->secondary_label));

  if (old_visible != visible) {
    gtk_widget_set_visible (GTK_WIDGET (self->secondary_label), visible);
    g_object_notify_by_pspec (G_OBJECT (self), props[PROP_SHOW_SYMBOLS]);
  }
}
