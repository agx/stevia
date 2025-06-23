/*
 * Copyright (C) 2022 Purism SPC
 *               2025 The Phosh Developers
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#define G_LOG_DOMAIN "pos-completer-hunspell"

#include "pos-config.h"

#include "pos-completer-base.h"
#include "pos-completer-priv.h"
#include "pos-completer-hunspell.h"

#include <gio/gio.h>

#include <hunspell.h>

#define MAX_COMPLETIONS 3
#define MAX_ADDITIONAL_RESULTS 3

enum {
  PROP_0,
  PROP_NAME,
  PROP_PREEDIT,
  PROP_BEFORE_TEXT,
  PROP_AFTER_TEXT,
  PROP_COMPLETIONS,
  PROP_LAST_PROP
};
static GParamSpec *props[PROP_LAST_PROP];

/**
 * PosCompleterHunspell:
 *
 * A completer using hunspell.
 *
 * Uses [hunspell](http://hunspell.github.io/) to suggest completions
 * based on typo corrections.
 */
struct _PosCompleterHunspell {
  PosCompleterBase      parent;

  char                 *name;
  GString              *preedit;
  GStrv                 completions;
  guint                 max_completions;

  Hunhandle            *handle;
};


static void pos_completer_hunspell_interface_init (PosCompleterInterface *iface);
static void pos_completer_hunspell_initable_interface_init (GInitableIface *iface);

G_DEFINE_TYPE_WITH_CODE (PosCompleterHunspell, pos_completer_hunspell, POS_TYPE_COMPLETER_BASE,
                         G_IMPLEMENT_INTERFACE (POS_TYPE_COMPLETER,
                                                pos_completer_hunspell_interface_init)
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE,
                                                pos_completer_hunspell_initable_interface_init))

static void
pos_completer_hunspell_take_completions (PosCompleter *iface, GStrv completions)
{
  PosCompleterHunspell *self = POS_COMPLETER_HUNSPELL (iface);
  g_auto (GStrv) additional_results = NULL;
  g_autoptr (GStrvBuilder) builder = g_strv_builder_new ();

  additional_results = pos_completer_base_get_additional_results (POS_COMPLETER_BASE (self),
                                                                  self->preedit->str,
                                                                  MAX_ADDITIONAL_RESULTS);

  if (completions)
    g_strv_builder_addv (builder, (const char **)completions);
  if (additional_results)
    g_strv_builder_addv (builder, (const char **)additional_results);
  g_strfreev (self->completions);
  self->completions = completions;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_COMPLETIONS]);
}


static const char *
pos_completer_hunspell_get_preedit (PosCompleter *iface)
{
  PosCompleterHunspell *self = POS_COMPLETER_HUNSPELL (iface);

  return self->preedit->str;
}


static void
pos_completer_hunspell_set_preedit (PosCompleter *iface, const char *preedit)
{
  PosCompleterHunspell *self = POS_COMPLETER_HUNSPELL (iface);

  if (g_strcmp0 (self->preedit->str, preedit) == 0)
    return;

  g_string_truncate (self->preedit, 0);
  if (preedit) {
    g_string_append (self->preedit, preedit);
  } else {
    pos_completer_hunspell_take_completions (POS_COMPLETER (self), NULL);
  }

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_PREEDIT]);
}


static void
pos_completer_hunspell_set_property (GObject      *object,
                                     guint         property_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  PosCompleterHunspell *self = POS_COMPLETER_HUNSPELL (object);

  switch (property_id) {
  case PROP_PREEDIT:
    pos_completer_hunspell_set_preedit (POS_COMPLETER (self), g_value_get_string (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
pos_completer_hunspell_get_property (GObject    *object,
                                     guint       property_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  PosCompleterHunspell *self = POS_COMPLETER_HUNSPELL (object);

  switch (property_id) {
  case PROP_NAME:
    g_value_set_string (value, self->name);
    break;
  case PROP_PREEDIT:
    g_value_set_string (value, self->preedit->str);
    break;
  case PROP_BEFORE_TEXT:
    g_value_set_string (value, "");
    break;
  case PROP_AFTER_TEXT:
    g_value_set_string (value, "");
    break;
  case PROP_COMPLETIONS:
    g_value_set_boxed (value, self->completions);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
pos_completer_hunspell_finalize (GObject *object)
{
  PosCompleterHunspell *self = POS_COMPLETER_HUNSPELL(object);

  g_clear_pointer (&self->handle, Hunspell_destroy);
  g_clear_pointer (&self->completions, g_strfreev);
  g_string_free (self->preedit, TRUE);

  G_OBJECT_CLASS (pos_completer_hunspell_parent_class)->finalize (object);
}


static void
pos_completer_hunspell_class_init (PosCompleterHunspellClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = pos_completer_hunspell_get_property;
  object_class->set_property = pos_completer_hunspell_set_property;
  object_class->finalize = pos_completer_hunspell_finalize;

  g_object_class_override_property (object_class, PROP_NAME, "name");
  props[PROP_NAME] = g_object_class_find_property (object_class, "name");

  g_object_class_override_property (object_class, PROP_PREEDIT, "preedit");
  props[PROP_PREEDIT] = g_object_class_find_property (object_class, "preedit");

  g_object_class_override_property (object_class, PROP_BEFORE_TEXT, "before-text");
  props[PROP_BEFORE_TEXT] = g_object_class_find_property (object_class, "before-text");

  g_object_class_override_property (object_class, PROP_AFTER_TEXT, "after-text");
  props[PROP_AFTER_TEXT] = g_object_class_find_property (object_class, "after-text");

  g_object_class_override_property (object_class, PROP_COMPLETIONS, "completions");
  props[PROP_COMPLETIONS] = g_object_class_find_property (object_class, "completions");
}


static gboolean
pos_completer_hunspell_set_language (PosCompleter *completer,
                                     const char   *lang,
                                     const char   *region,
                                     GError      **error)
{
  PosCompleterHunspell *self = POS_COMPLETER_HUNSPELL (completer);
  g_autofree char *dict_path = NULL;
  g_autofree char *aff_path = NULL;
  Hunhandle *handle = NULL;
  const char *encoding;

  if (pos_completer_hunspell_find_dict (lang, region, &aff_path, &dict_path) == FALSE) {
    g_set_error (error,
                 POS_COMPLETER_ERROR,
                 POS_COMPLETER_ERROR_ENGINE_INIT,
                 "Failed to find dictionary for %s-%s", lang, region);
    return FALSE;
  }

  g_debug ("Using affix '%s' and dict '%s'", aff_path, dict_path);
  handle = Hunspell_create (aff_path, dict_path);
  if (handle == NULL) {
    g_set_error_literal (error,
                         POS_COMPLETER_ERROR,
                         POS_COMPLETER_ERROR_ENGINE_INIT,
                         "Failed to init hunspell");
    return FALSE;
  }

  encoding = Hunspell_get_dic_encoding (handle);
  if (g_strcmp0 (encoding, "UTF-8")) {
    g_set_error (error,
                 POS_COMPLETER_ERROR,
                 POS_COMPLETER_ERROR_ENGINE_INIT,
                 "Invalid dictionary encoding '%s'", encoding);
    return FALSE;
  }

  g_clear_pointer (&self->handle, Hunspell_destroy);
  self->handle = g_steal_pointer (&handle);

  return TRUE;
}


static gboolean
pos_completer_hunspell_initable_init (GInitable    *initable,
                                      GCancellable *cancelable,
                                      GError      **error)
{
  PosCompleterHunspell *self = POS_COMPLETER_HUNSPELL (initable);
  g_autoptr (GError) local_err = NULL;

  if (pos_completer_hunspell_set_language (POS_COMPLETER (self),
                                           POS_COMPLETER_DEFAULT_LANG,
                                           POS_COMPLETER_DEFAULT_REGION,
                                           &local_err) == FALSE) {
    g_propagate_error (error, g_steal_pointer (&local_err));
    return FALSE;
  }

  return TRUE;
}


static void
pos_completer_hunspell_initable_interface_init (GInitableIface *iface)
{
  iface->init = pos_completer_hunspell_initable_init;
}


static const char *
pos_completer_hunspell_get_name (PosCompleter *iface)
{
  PosCompleterHunspell *self = POS_COMPLETER_HUNSPELL (iface);

  return self->name;
}


static gboolean
pos_completer_hunspell_feed_symbol (PosCompleter *iface, const char *symbol)
{
  PosCompleterHunspell *self = POS_COMPLETER_HUNSPELL (iface);
  g_autofree char *preedit = g_strdup (self->preedit->str);
  g_autoptr (GPtrArray) completions = g_ptr_array_new ();
  char **suggestions;
  int ret;

  if (pos_completer_add_preedit (POS_COMPLETER (self), self->preedit, symbol)) {
    g_signal_emit_by_name (self, "commit-string", self->preedit->str);
    pos_completer_hunspell_set_preedit (POS_COMPLETER (self), NULL);

    /* Make sure enter is processed as raw keystroke */
    if (g_strcmp0 (symbol, "KEY_ENTER") == 0)
      return FALSE;

    return TRUE;
  }

  /* preedit didn't change and wasn't committed so we didn't handle it */
  if (g_strcmp0 (self->preedit->str, preedit) == 0)
    return FALSE;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_PREEDIT]);

  g_debug ("Looking up string '%s'", self->preedit->str);

  if (Hunspell_spell (self->handle, self->preedit->str))
    g_ptr_array_add (completions, g_strdup (self->preedit->str));

  ret = Hunspell_suggest (self->handle, &suggestions, self->preedit->str);
  if (ret > 0) {
    for (int i = 0; i < ret && i < self->max_completions; i++)
      g_ptr_array_add (completions, g_strdup (suggestions[i]));
  }
  g_ptr_array_add (completions, NULL);

  pos_completer_hunspell_take_completions (POS_COMPLETER (self),
                                           (char **)g_ptr_array_steal (completions, NULL));
  return TRUE;
}


static void
pos_completer_hunspell_interface_init (PosCompleterInterface *iface)
{
  iface->get_name = pos_completer_hunspell_get_name;
  iface->feed_symbol = pos_completer_hunspell_feed_symbol;
  iface->get_preedit = pos_completer_hunspell_get_preedit;
  iface->set_preedit = pos_completer_hunspell_set_preedit;
  iface->set_language = pos_completer_hunspell_set_language;
}


static void
pos_completer_hunspell_init (PosCompleterHunspell *self)
{
  self->max_completions = MAX_COMPLETIONS;
  self->preedit = g_string_new (NULL);
  self->name = "hunspell";
}

/**
 * pos_completer_hunspell_new:
 * err: An error location
 *
 * Returns:(transfer full): A new completer
 */
PosCompleter *
pos_completer_hunspell_new (GError **err)
{
  return POS_COMPLETER (g_initable_new (POS_TYPE_COMPLETER_HUNSPELL, NULL, err, NULL));
}


/**
 * pos_completer_hunspell_find_dict:
 * lang: The language to find the dictionary for
 * region: The region to find the dictionary for
 * aff_path: (out)(nullable): The location of the hunspell aff file
 * dict_path: (out)(nullable): The location of the hunspell dict file
 *
 * Returns: `TRUE` if the huspell dictionary was found, otherwise `FALSE`
 */
gboolean
pos_completer_hunspell_find_dict (const char  *lang,
                                  const char  *region,
                                  char       **aff_path,
                                  char       **dict_path)
{
  g_auto (GStrv) paths = g_strsplit (POS_HUNSPELL_DICT_PATH, ":", -1);
  g_autofree char *upcase_region = g_ascii_strup (region, -1);
  g_autofree char *locale = NULL;

  g_assert (lang);
  g_assert (region);

  locale = g_strdup_printf ("%s_%s", lang, upcase_region);
  for (int i = 0; paths[i] != NULL; i++) {
    g_autofree char *dict = g_strdup_printf ("%s/%s.dic", paths[i], locale);
    g_autofree char *aff = g_strdup_printf ("%s/%s.aff", paths[i], locale);

    if (g_file_test (dict, G_FILE_TEST_EXISTS) &&
        g_file_test (aff, G_FILE_TEST_EXISTS)) {
      if (aff_path)
        *aff_path = g_steal_pointer (&aff);
      if (dict_path)
        *dict_path = g_steal_pointer (&dict);
      return TRUE;
    }
  }
  return FALSE;
}
