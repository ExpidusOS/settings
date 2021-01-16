/* vi:set sw=2 sts=2 ts=2 et ai: */
/*-
 * Copyright (c) 2008 Jannis Pohlmann <jannis@expidus.org>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __EXPIDUS_KEYBOARD_SETTINGS_H__
#define __EXPIDUS_KEYBOARD_SETTINGS_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _ExpidusKeyboardSettingsPrivate ExpidusKeyboardSettingsPrivate;
typedef struct _ExpidusKeyboardSettingsClass   ExpidusKeyboardSettingsClass;
typedef struct _ExpidusKeyboardSettings        ExpidusKeyboardSettings;

#define EXPIDUS_TYPE_KEYBOARD_SETTINGS            (expidus_keyboard_settings_get_type ())
#define EXPIDUS_KEYBOARD_SETTINGS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_KEYBOARD_SETTINGS, ExpidusKeyboardSettings))
#define EXPIDUS_KEYBOARD_SETTINGS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_KEYBOARD_SETTINGS, ExpidusKeyboardSettingsClass))
#define EXPIDUS_IS_KEYBOARD_SETTINGS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_KEYBOARD_SETTINGS))
#define EXPIDUS_IS_KEYBOARD_SETTINGS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_KEYBOARD_SETTINGS)
#define EXPIDUS_KEYBOARD_SETTINGS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_KEYBOARD_SETTINGS, ExpidusKeyboardSettingsClass))

GType expidus_keyboard_settings_get_type                      (void) G_GNUC_CONST;

ExpidusKeyboardSettings *expidus_keyboard_settings_new           (void) G_GNUC_MALLOC;
GtkWidget            *expidus_keyboard_settings_create_dialog (ExpidusKeyboardSettings *settings);
GtkWidget            *expidus_keyboard_settings_create_plug   (ExpidusKeyboardSettings *settings,
                                                            gint                  socket_id);



struct _ExpidusKeyboardSettingsClass
{
  GtkBuilderClass __parent__;
};

struct _ExpidusKeyboardSettings
{
  GtkBuilder __parent__;

  ExpidusKeyboardSettingsPrivate *priv;
};

G_END_DECLS

#endif /* !__EXPIDUS_KEYBOARD_SETTINGS_H__ */
