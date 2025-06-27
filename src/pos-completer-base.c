/*
 * Copyright (C) 2025 Phosh e.V.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#define G_LOG_DOMAIN "pos-completer-base"

#include "pos-config.h"

#include "phosh-osk-enums.h"
#include "pos-enum-types.h"
#include "pos-completer-base.h"
#include "pos-emoji-db.h"

#include <gmobile.h>

#include <glib/gi18n.h>

#define MATCH_MIN_LEN 3

/**
 * PosCompleterBase:
 *
 * Base class for completers implementing common functionality
 */

enum {
  PROP_0,
  PROP_SOURCES,
  PROP_LAST_PROP
};
static GParamSpec *props[PROP_LAST_PROP];

typedef struct _PosCompleterBasePrivate {
  PhoshOskCompletionSourceFlags sources;
} PosCompleterBasePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (PosCompleterBase, pos_completer_base, G_TYPE_OBJECT)


static char *
pos_completer_base_match_keyword (PosCompleterBase *self, const char *match)
{
  if (!match)
    return NULL;

  /* Translators: Used to auto complete the current date, should be a single word */
  if (g_str_has_prefix (_("today"), match)) {
    g_autoptr (GDateTime) now = g_date_time_new_now_local ();

    return g_date_time_format (now, "%x");
    /* Translators: Used to auto complete the a date, should be a single word */
  } else if (g_str_has_prefix (_("tomorrow"), match)) {
    g_autoptr (GDateTime) now = g_date_time_new_now_local ();
    g_autoptr (GDateTime) tomorrow = NULL;

    tomorrow = g_date_time_add_days (now, 1);
    return g_date_time_format (tomorrow, "%x");
    /* Translators: Used to auto complete the current time, should be a single word */
  } else if g_str_equal (match, _("now")) {
    g_autoptr (GDateTime) now = g_date_time_new_now_local ();

    return g_date_time_format (now, "%X");
  }

  return NULL;
}


static void
pos_completer_base_set_property (GObject      *object,
                                 guint         property_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  PosCompleterBase *self = POS_COMPLETER_BASE (object);
  PosCompleterBasePrivate *priv = pos_completer_base_get_instance_private (self);

  switch (property_id) {
  case PROP_SOURCES:
    priv->sources = g_value_get_flags (value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
pos_completer_base_get_property (GObject    *object,
                                 guint       property_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  PosCompleterBase *self = POS_COMPLETER_BASE (object);
  PosCompleterBasePrivate *priv = pos_completer_base_get_instance_private (self);

  switch (property_id) {
  case PROP_SOURCES:
    g_value_set_flags (value, priv->sources);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}


static void
pos_completer_base_class_init (PosCompleterBaseClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = pos_completer_base_get_property;
  object_class->set_property = pos_completer_base_set_property;

  /**
   * PosCompleterBase:sources:
   *
   * Additional completion sources the completer should consult
   */
  props[PROP_SOURCES] =
    g_param_spec_flags ("sources", "", "",
                        PHOSH_TYPE_OSK_COMPLETION_SOURCE_FLAGS,
                        PHOSH_OSK_COMPLETION_SOURCE_NONE,
                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, PROP_LAST_PROP, props);
}


static void
pos_completer_base_init (PosCompleterBase *self)
{
  g_autoptr (GSettings) settings = g_settings_new ("sm.puri.phosh.osk.Completers");

  g_settings_bind (settings, "sources", self, "sources",  G_SETTINGS_BIND_DEFAULT);
}


PosCompleterBase *
pos_completer_base_new (void)
{
  return g_object_new (POS_TYPE_COMPLETER_BASE, NULL);
}


GStrv
pos_completer_base_get_additional_results (PosCompleterBase *self,
                                           const char       *match,
                                           guint             max_results)
{
  PosEmojiDb *emoji_db = pos_emoji_db_get_default ();
  PosCompleterBasePrivate *priv = pos_completer_base_get_instance_private (self);
  g_autoptr (GStrvBuilder) builder = g_strv_builder_new ();
  guint remain = max_results;

  g_assert (POS_IS_COMPLETER_BASE (self));

  if (gm_str_is_null_or_empty (match))
    return NULL;

  if (priv->sources & PHOSH_OSK_COMPLETION_SOURCE_KEYWORD &&
      match && strlen (match) >= MATCH_MIN_LEN &&
      remain > 1) {
    char *result = pos_completer_base_match_keyword (self, match);

    if (!gm_str_is_null_or_empty (result)) {
      g_strv_builder_take (builder, result);
      remain--;
    }
  }

  if (priv->sources & PHOSH_OSK_COMPLETION_SOURCE_EMOJI &&
      match && strlen (match) >= MATCH_MIN_LEN &&
      remain > 1) {
    g_auto (GStrv) results = pos_emoji_db_match_by_name (emoji_db, match, remain);

    if (!gm_strv_is_null_or_empty (results)) {
      g_strv_builder_addv (builder, (const char **)results);
      remain -= g_strv_length (results);
    }
  }

  return g_strv_builder_end (builder);
}
