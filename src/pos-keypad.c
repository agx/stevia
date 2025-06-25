/*
 * Copyright (C) 2024 The Phosh Developers
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#define G_LOG_DOMAIN "pos-keypad-"

#include "pos-config.h"
#include "pos-keypad.h"
#include "pos-keypad-button.h"

#include <gtk/gtk.h>

#include <locale.h>

/* Ease 3to4 migration */
#define gtk_grid_remove(x,y) gtk_container_remove (GTK_CONTAINER (x), y)

/**
 * PosKeypad:
 */

enum {
  PROP_0,
  PROP_LETTERS_VISIBLE,
  PROP_SYMBOLS_VISIBLE,
  PROP_DECIMAL_SEPARATOR_VISIBLE,
  PROP_END_ACTION,
  PROP_START_ACTION,
  PROP_LAST_PROP,
};
static GParamSpec *props[PROP_LAST_PROP];

enum {
  KEY,
  DONE,
  LAST_SIGNAL
};
static int signals[LAST_SIGNAL];

struct _PosKeypad {
  GtkBin      parent;

  GtkGrid    *grid;
  GtkLabel   *decimal_separator_label;
  GtkGesture *long_press_zero_gesture;

  guint16     row_spacing;
  guint16     column_spacing;
  gboolean    symbols_visible;
  gboolean    letters_visible;
  gboolean    decimal_separator_visible;
  char       *decimal_separator;
};
G_DEFINE_TYPE (PosKeypad, pos_keypad, GTK_TYPE_BIN)


static void
on_done_clicked (PosKeypad *self)
{
  g_signal_emit (self, signals[DONE], 0);
}


static void
symbol_clicked (PosKeypad *self, const char *key)
{
  g_signal_emit (self, signals[KEY], 0, key);
}


static void
on_button_clicked (PosKeypad *self, PosKeypadButton *btn)
{
  char digit = pos_keypad_button_get_digit (btn);
  g_autofree char *text = g_strdup_printf ("%c", digit);

  symbol_clicked (self, text);
}


static void
on_asterisk_clicked (PosKeypad *self)
{
  symbol_clicked (self, "*");
}


static void
on_hash_clicked (PosKeypad *self)
{
  symbol_clicked (self, "#");
}


static void
on_decimal_separator_clicked (PosKeypad *self)
{
  symbol_clicked (self, self->decimal_separator);
}


static void
on_backspace_clicked (PosKeypad *self)
{
  symbol_clicked (self, "KEY_BACKSPACE");
}


static void
on_enter_clicked (PosKeypad *self)
{
  symbol_clicked (self, "KEY_ENTER");
}


static void
on_zero_long_pressed (PosKeypad *self, gdouble x, gdouble y, GtkGesture *gesture)
{
  if (!self->symbols_visible)
    return;

  symbol_clicked (self, "+");
  gtk_gesture_set_state (GTK_GESTURE (gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}


static void
pos_keypad_set_property (GObject      *object,
                         guint         property_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  PosKeypad *self = POS_KEYPAD (object);

  switch (property_id) {
  case PROP_LETTERS_VISIBLE:
    pos_keypad_set_letters_visible (self, g_value_get_boolean (value));
    break;
  case PROP_SYMBOLS_VISIBLE:
    pos_keypad_set_symbols_visible (self, g_value_get_boolean (value));
    break;
  case PROP_DECIMAL_SEPARATOR_VISIBLE:
    pos_keypad_set_decimal_separator_visible (self, g_value_get_boolean (value));
    break;
  case PROP_END_ACTION:
    pos_keypad_set_end_action (self, g_value_get_object (value));
    break;
  case PROP_START_ACTION:
    pos_keypad_set_start_action (self, g_value_get_object (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
pos_keypad_get_property (GObject    *object,
                         guint       property_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  PosKeypad *self = POS_KEYPAD (object);

  switch (property_id) {
  case PROP_LETTERS_VISIBLE:
    g_value_set_boolean (value, pos_keypad_get_letters_visible (self));
    break;
  case PROP_SYMBOLS_VISIBLE:
    g_value_set_boolean (value, pos_keypad_get_symbols_visible (self));
    break;
  case PROP_DECIMAL_SEPARATOR_VISIBLE:
    g_value_set_boolean (value, pos_keypad_get_decimal_separator_visible (self));
    break;
  case PROP_START_ACTION:
    g_value_set_object (value, pos_keypad_get_start_action (self));
    break;
  case PROP_END_ACTION:
    g_value_set_object (value, pos_keypad_get_end_action (self));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
pos_keypad_finalize (GObject *object)
{
  PosKeypad *self = POS_KEYPAD (object);

  g_clear_object (&self->long_press_zero_gesture);

  G_OBJECT_CLASS (pos_keypad_parent_class)->finalize (object);
}

static void
pos_keypad_class_init (PosKeypadClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = pos_keypad_finalize;

  object_class->set_property = pos_keypad_set_property;
  object_class->get_property = pos_keypad_get_property;

  /**
   * PosKeypad:letters-visible:
   *
   * Whether the keypad should display the standard letters below the digits on
   * its buttons.
   */
  props[PROP_LETTERS_VISIBLE] =
    g_param_spec_boolean ("letters-visible", "", "",
                          TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
  /**
   * PosKeypad:symbols-visible:
   *
   * Whether the keypad should display the hash and asterisk buttons, and should
   * display the plus symbol at the bottom of its 0 button.
   */
  props[PROP_SYMBOLS_VISIBLE] =
    g_param_spec_boolean ("symbols-visible", "", "",
                          TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
  /**
   * PosKeypad:decimal-separator-visible:
   *
   * Whether the keypad should display a button with the decimal separator
   * of the current locale.
   */
  props[PROP_DECIMAL_SEPARATOR_VISIBLE] =
    g_param_spec_boolean ("decimal-separator-visible", "", "",
                          TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
  /**
   * PosKeypad:end-action:
   *
   * The widget for the lower end corner of @self.
   */
  props[PROP_END_ACTION] =
    g_param_spec_object ("end-action", "", "",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
  /**
   * PosKeypad:start-action:
   *
   * The widget for the lower start corner of @self.
   */
  props[PROP_START_ACTION] =
    g_param_spec_object ("start-action", "", "",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, PROP_LAST_PROP, props);

  /**
   * PosKeypad::key
   * @self: The keypad
   * @emoji: The key pressed by the user
   *
   * The user pressed a key.
   */
  signals[KEY] =
    g_signal_new ("key",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1, G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);
  /**
   * PosKeypad::done:
   *
   * The user is done with this layout.
   */
  signals[DONE] =
    g_signal_new ("done",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL,
                  G_TYPE_NONE, 0);

  g_type_ensure (POS_TYPE_KEYPAD_BUTTON);

  gtk_widget_class_set_template_from_resource (widget_class, "/mobi/phosh/stevia/ui/keypad.ui");

  gtk_widget_class_bind_template_child (widget_class, PosKeypad, grid);
  gtk_widget_class_bind_template_child (widget_class, PosKeypad, decimal_separator_label);
  gtk_widget_class_bind_template_child (widget_class, PosKeypad, long_press_zero_gesture);

  gtk_widget_class_bind_template_callback (widget_class, on_asterisk_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_backspace_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_button_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_decimal_separator_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_done_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_enter_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_hash_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_zero_long_pressed);

  gtk_widget_class_set_css_name (widget_class, "pos-keypad");
}


static void
pos_keypad_init (PosKeypad *self)
{
  g_autoptr (GError) error = NULL;
  struct lconv *locale_data = localeconv ();

  gtk_widget_init_template (GTK_WIDGET (self));
  self->decimal_separator = g_strdup (locale_data->decimal_point);

  gtk_widget_set_direction (GTK_WIDGET (self->grid), GTK_TEXT_DIR_LTR);
  gtk_label_set_label (self->decimal_separator_label, self->decimal_separator);
}

/**
 * pos_keypad_new:
 *
 * Create a new #PosKeypad widget.
 *
 * Returns: the newly created #PosKeypad widget
 */
GtkWidget *
pos_keypad_new (void)
{
  return g_object_new (POS_TYPE_KEYPAD, NULL);
}

/**
 * pos_keypad_set_letters_visible:
 * @self: a #PosKeypad
 * @visible: whether the letters below the digits should be visible
 *
 * Sets whether @self should display the standard letters below the digits on
 * its buttons.
 */
void
pos_keypad_set_letters_visible (PosKeypad *self,
                                gboolean   visible)
{
  g_return_if_fail (POS_IS_KEYPAD (self));

  visible = !!visible;

  if (self->letters_visible == visible)
    return;

  self->letters_visible = visible;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_LETTERS_VISIBLE]);
}

/**
 * pos_keypad_get_letters_visible:
 * @self: a #PosKeypad
 *
 * Returns whether @self should display the standard letters below the digits on
 * its buttons.
 *
 * Returns: whether the letters below the digits should be visible
 */
gboolean
pos_keypad_get_letters_visible (PosKeypad *self)
{
  g_return_val_if_fail (POS_IS_KEYPAD (self), FALSE);

  return self->letters_visible;
}

/**
 * pos_keypad_set_symbols_visible:
 * @self: a #PosKeypad
 * @visible: whether the hash, plus, and asterisk symbols should be visible
 *
 * Sets whether @self should display the hash and asterisk buttons, and should
 * display the plus symbol at the bottom of its 0 button.
 */
void
pos_keypad_set_symbols_visible (PosKeypad *self,
                                gboolean   visible)
{
  g_return_if_fail (POS_IS_KEYPAD (self));

  visible = !!visible;

  if (self->symbols_visible == visible)
    return;

  self->symbols_visible = visible;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_SYMBOLS_VISIBLE]);
}

/**
 * pos_keypad_get_symbols_visible:
 * @self: a #PosKeypad
 *
 * Returns whether @self should display the standard letters below the digits on
 * its buttons.
 *
 * Returns Whether @self should display the hash and asterisk buttons, and
 * should display the plus symbol at the bottom of its 0 button.
 *
 * Returns: whether the hash, plus, and asterisk symbols should be visible
 */
gboolean
pos_keypad_get_symbols_visible (PosKeypad *self)
{
  g_return_val_if_fail (POS_IS_KEYPAD (self), FALSE);

  return self->symbols_visible;
}

/**
 * pos_keypad_set_decimal_separator_visible:
 * @self: a #PosKeypad
 * @visible: whether the hash, plus, and asterisk decimal_separator should be visible
 *
 * Sets whether @self should display the hash and asterisk buttons, and should
 * display the plus symbol at the bottom of its 0 button.
 */
void
pos_keypad_set_decimal_separator_visible (PosKeypad *self, gboolean visible)
{
  g_return_if_fail (POS_IS_KEYPAD (self));

  visible = !!visible;

  if (self->decimal_separator_visible == visible)
    return;

  self->decimal_separator_visible = visible;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_DECIMAL_SEPARATOR_VISIBLE]);
}

/**
 * pos_keypad_get_decimal_separator_visible:
 * @self: a #PosKeypad
 *
 * Returns whether @self should display the standard letters below the digits on
 * its buttons.
 *
 * Returns Whether @self should display the hash and asterisk buttons, and
 * should display the plus symbol at the bottom of its 0 button.
 *
 * Returns: whether the hash, plus, and asterisk decimal_separator should be visible
 */
gboolean
pos_keypad_get_decimal_separator_visible (PosKeypad *self)
{
  g_return_val_if_fail (POS_IS_KEYPAD (self), FALSE);

  return self->decimal_separator_visible;
}

/**
 * pos_keypad_set_start_action:
 * @self: a #PosKeypad
 * @start_action: (nullable): the start action widget
 *
 * Sets the widget for the lower left corner of @self.
 */
void
pos_keypad_set_start_action (PosKeypad *self,
                             GtkWidget *start_action)
{
  GtkWidget *old_widget;

  g_return_if_fail (POS_IS_KEYPAD (self));
  g_return_if_fail (start_action == NULL || GTK_IS_WIDGET (start_action));

  old_widget = gtk_grid_get_child_at (self->grid, 0, 3);

  if (old_widget == start_action)
    return;

  if (old_widget != NULL)
    gtk_grid_remove (self->grid, old_widget);

  if (start_action != NULL)
    gtk_grid_attach (self->grid, start_action, 0, 3, 1, 1);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_START_ACTION]);
}

/**
 * pos_keypad_get_start_action:
 * @self: a #PosKeypad
 *
 * Returns the widget for the lower left corner @self.
 *
 * Returns: (transfer none) (nullable): the start action widget
 */
GtkWidget *
pos_keypad_get_start_action (PosKeypad *self)
{
  g_return_val_if_fail (POS_IS_KEYPAD (self), NULL);

  return gtk_grid_get_child_at (self->grid, 0, 3);
}

/**
 * pos_keypad_set_end_action:
 * @self: a #PosKeypad
 * @end_action: (nullable): the end action widget
 *
 * Sets the widget for the lower right corner of @self.
 */
void
pos_keypad_set_end_action (PosKeypad *self,
                           GtkWidget *end_action)
{
  GtkWidget *old_widget;

  g_return_if_fail (POS_IS_KEYPAD (self));
  g_return_if_fail (end_action == NULL || GTK_IS_WIDGET (end_action));

  old_widget = gtk_grid_get_child_at (self->grid, 2, 3);

  if (old_widget == end_action)
    return;

  if (old_widget != NULL)
    gtk_grid_remove (self->grid, old_widget);

  if (end_action != NULL)
    gtk_grid_attach (self->grid, end_action, 2, 3, 1, 1);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_END_ACTION]);
}

/**
 * pos_keypad_get_end_action:
 * @self: a #PosKeypad
 *
 * Returns the widget for the lower right corner of @self.
 *
 * Returns: (transfer none) (nullable): the end action widget
 */
GtkWidget *
pos_keypad_get_end_action (PosKeypad *self)
{
  g_return_val_if_fail (POS_IS_KEYPAD (self), NULL);

  return gtk_grid_get_child_at (self->grid, 2, 3);
}
