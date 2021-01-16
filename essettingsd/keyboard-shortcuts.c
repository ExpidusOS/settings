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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <X11/Xlib.h>

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include <libexpidus1ui/libexpidus1ui.h>
#include <libexpidus1util/libexpidus1util.h>
#include <esconf/esconf.h>
#include <libexpidus1kbd-private/expidus-shortcuts-provider.h>
#include <libexpidus1kbd-private/expidus-shortcuts-grabber.h>

#include "debug.h"
#include "keyboard-shortcuts.h"



/* Property identifiers */
enum
{
  PROP_0,
};



static void            expidus_keyboard_shortcuts_helper_finalize           (GObject                          *object);
static void            expidus_keyboard_shortcuts_helper_shortcut_added     (ExpidusShortcutsProvider            *provider,
                                                                          const gchar                      *shortcut,
                                                                          ExpidusKeyboardShortcutsHelper      *helper);
static void            expidus_keyboard_shortcuts_helper_shortcut_removed   (ExpidusShortcutsProvider            *provider,
                                                                          const gchar                      *shortcut,
                                                                          ExpidusKeyboardShortcutsHelper      *helper);
static void            expidus_keyboard_shortcuts_helper_shortcut_activated (ExpidusShortcutsGrabber             *grabber,
                                                                          const gchar                      *shortcut,
                                                                          gint                              timestamp,
                                                                          ExpidusKeyboardShortcutsHelper      *helper);
static void            expidus_keyboard_shortcuts_helper_load_shortcuts     (ExpidusKeyboardShortcutsHelper      *helper);



struct _ExpidusKeyboardShortcutsHelperClass
{
  GObjectClass __parent__;
};

struct _ExpidusKeyboardShortcutsHelper
{
  GObject __parent__;

  /* Esconf channel used for managing the keyboard shortcuts */
  EsconfChannel         *channel;

  ExpidusShortcutsGrabber  *grabber;
  ExpidusShortcutsProvider *provider;
};



G_DEFINE_TYPE (ExpidusKeyboardShortcutsHelper, expidus_keyboard_shortcuts_helper, G_TYPE_OBJECT)



static void
expidus_keyboard_shortcuts_helper_class_init (ExpidusKeyboardShortcutsHelperClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = expidus_keyboard_shortcuts_helper_finalize;
}



static void
expidus_keyboard_shortcuts_helper_init (ExpidusKeyboardShortcutsHelper *helper)
{
  /* Create shortcuts grabber */
  helper->grabber = expidus_shortcuts_grabber_new ();

  /* Be notified when a shortcut is pressed */
  g_signal_connect (helper->grabber, "shortcut-activated", G_CALLBACK (expidus_keyboard_shortcuts_helper_shortcut_activated), helper);

  /* Create shortcuts provider */
  helper->provider = expidus_shortcuts_provider_new ("commands");

  /* Be notified of property changes */
  g_signal_connect (helper->provider, "shortcut-added", G_CALLBACK (expidus_keyboard_shortcuts_helper_shortcut_added), helper);
  g_signal_connect (helper->provider, "shortcut-removed", G_CALLBACK (expidus_keyboard_shortcuts_helper_shortcut_removed), helper);

  expidus_keyboard_shortcuts_helper_load_shortcuts (helper);
}



static void
expidus_keyboard_shortcuts_helper_finalize (GObject *object)
{
  ExpidusKeyboardShortcutsHelper *helper = EXPIDUS_KEYBOARD_SHORTCUTS_HELPER (object);

  /* Free shortcuts provider */
  g_object_unref (helper->provider);

  /* Free shortcuts grabber */
  g_object_unref (helper->grabber);

  (*G_OBJECT_CLASS (expidus_keyboard_shortcuts_helper_parent_class)->finalize) (object);
}



static void
expidus_keyboard_shortcuts_helper_shortcut_added (ExpidusShortcutsProvider       *provider,
                                               const gchar                 *shortcut,
                                               ExpidusKeyboardShortcutsHelper *helper)
{
  g_return_if_fail (EXPIDUS_IS_KEYBOARD_SHORTCUTS_HELPER (helper));
  expidus_shortcuts_grabber_add (helper->grabber, shortcut);

  essettings_dbg (XFSD_DEBUG_KEYBOARD_SHORTCUTS, "add \"%s\"", shortcut);
}



static void
expidus_keyboard_shortcuts_helper_shortcut_removed (ExpidusShortcutsProvider       *provider,
                                                 const gchar                 *shortcut,
                                                 ExpidusKeyboardShortcutsHelper *helper)
{
  g_return_if_fail (EXPIDUS_IS_KEYBOARD_SHORTCUTS_HELPER (helper));
  expidus_shortcuts_grabber_remove (helper->grabber, shortcut);

  essettings_dbg (XFSD_DEBUG_KEYBOARD_SHORTCUTS, "remove \"%s\"", shortcut);
}



static void
_expidus_keyboard_shortcuts_helper_load_shortcut (ExpidusShortcut                *shortcut,
                                               ExpidusKeyboardShortcutsHelper *helper)
{
  g_return_if_fail (shortcut != NULL);
  g_return_if_fail (EXPIDUS_IS_KEYBOARD_SHORTCUTS_HELPER (helper));

  expidus_shortcuts_grabber_add (helper->grabber, shortcut->shortcut);

  essettings_dbg_filtered (XFSD_DEBUG_KEYBOARD_SHORTCUTS, "loaded \"%s\" => \"%s\"",
                           shortcut->shortcut, shortcut->command);
}



static void
expidus_keyboard_shortcuts_helper_load_shortcuts (ExpidusKeyboardShortcutsHelper *helper)
{
  GList *shortcuts;

  g_return_if_fail (EXPIDUS_IS_KEYBOARD_SHORTCUTS_HELPER (helper));

  /* Load shortcuts one by one */
  shortcuts = expidus_shortcuts_provider_get_shortcuts (helper->provider);
  g_list_foreach (shortcuts, (GFunc) _expidus_keyboard_shortcuts_helper_load_shortcut, helper);
  essettings_dbg (XFSD_DEBUG_KEYBOARD_SHORTCUTS, "%d shortcuts loaded", g_list_length (shortcuts));
  expidus_shortcuts_free (shortcuts);
}



static void
expidus_keyboard_shortcuts_helper_shortcut_activated (ExpidusShortcutsGrabber        *grabber,
                                                   const gchar                 *shortcut,
                                                   gint                         timestamp,
                                                   ExpidusKeyboardShortcutsHelper *helper)
{
  ExpidusShortcut  *sc;
  GError        *error = NULL;
  gchar        **argv;
  gboolean       succeed;

  g_return_if_fail (EXPIDUS_IS_KEYBOARD_SHORTCUTS_HELPER (helper));
  g_return_if_fail (EXPIDUS_IS_SHORTCUTS_PROVIDER (helper->provider));

  /* Ignore empty shortcuts */
  if (shortcut == NULL || g_utf8_strlen (shortcut, -1) == 0)
    return;

  /* Get shortcut from the provider */
  sc = expidus_shortcuts_provider_get_shortcut (helper->provider, shortcut);

  if (G_UNLIKELY (sc == NULL))
   {
      essettings_dbg (XFSD_DEBUG_KEYBOARD_SHORTCUTS, "\"%s\" not found", shortcut);
      return;
   }

  essettings_dbg (XFSD_DEBUG_KEYBOARD_SHORTCUTS,
                  "activated \"%s\" (command=\"%s\", snotify=%d, stamp=%d)",
                  shortcut, sc->command, sc->snotify, timestamp);

  /* Handle the argv ourselfs, because expidus_spawn_command_line_on_screen() does
   * not accept a custom timestamp for startup notification */
  succeed = g_shell_parse_argv (sc->command, NULL, &argv, &error);
  if (G_LIKELY (succeed))
    {
      succeed = expidus_spawn_on_screen (expidus_gdk_screen_get_active (NULL),
                                      NULL, argv, NULL, G_SPAWN_SEARCH_PATH,
                                      sc->snotify, timestamp, NULL, &error);

      g_strfreev (argv);
    }

  if (!succeed)
    {
      expidus_dialog_show_error (NULL, error, _("Failed to launch shortcut \"%s\""), shortcut);
      g_error_free (error);
    }

  expidus_shortcut_free (sc);
}
