/*
 *  Copyright (c) 2008 Olivier Fourdan <olivier@expidus.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <esconf/esconf.h>
#include <libexpidus1util/libexpidus1util.h>

#ifdef HAVE_LIBXKLAVIER
#include <libxklavier/xklavier.h>
#endif /* HAVE_LIBXKLAVIER */

#include "debug.h"
#include "keyboard-layout.h"

static void expidus_keyboard_layout_helper_finalize                  (GObject                       *object);
static void expidus_keyboard_layout_helper_process_xmodmap           (void);

#ifdef HAVE_LIBXKLAVIER
static void expidus_keyboard_layout_helper_set_model                 (ExpidusKeyboardLayoutHelper      *helper);
static void expidus_keyboard_layout_helper_set_layout                (ExpidusKeyboardLayoutHelper      *helper);
static void expidus_keyboard_layout_helper_set_variant               (ExpidusKeyboardLayoutHelper      *helper);
static void expidus_keyboard_layout_helper_set_grpkey                (ExpidusKeyboardLayoutHelper      *helper);
static void expidus_keyboard_layout_helper_set_composekey            (ExpidusKeyboardLayoutHelper      *helper);
static void expidus_keyboard_layout_helper_channel_property_changed  (EsconfChannel                 *channel,
                                                                   const gchar                   *property_name,
                                                                   const GValue                  *value,
                                                                   ExpidusKeyboardLayoutHelper      *helper);
static gchar* expidus_keyboard_layout_get_option                     (gchar                        **options,
                                                                   const gchar                   *option_name,
                                                                   gchar                        **other_options);
static GdkFilterReturn handle_xevent                              (GdkXEvent                     *xev,
                                                                   GdkEvent                      *event,
                                                                   ExpidusKeyboardLayoutHelper      *helper);
static void expidus_keyboard_layout_reset_xkl_config                 (XklEngine                     *xklengine,
                                                                   ExpidusKeyboardLayoutHelper      *helper);
#endif /* HAVE_LIBXKLAVIER */

struct _ExpidusKeyboardLayoutHelperClass
{
    GObjectClass __parent__;
};

struct _ExpidusKeyboardLayoutHelper
{
    GObject  __parent__;

    /* esconf channel */
    EsconfChannel     *channel;

    gboolean           xkb_disable_settings;

#ifdef HAVE_LIBXKLAVIER
    /* libxklavier */
    XklEngine         *engine;
    XklConfigRegistry *registry;
    XklConfigRec      *config;
    gchar             *system_keyboard_model;
#endif /* HAVE_LIBXKLAVIER */
};

G_DEFINE_TYPE (ExpidusKeyboardLayoutHelper, expidus_keyboard_layout_helper, G_TYPE_OBJECT);

static void
expidus_keyboard_layout_helper_class_init (ExpidusKeyboardLayoutHelperClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = expidus_keyboard_layout_helper_finalize;
}

static void
expidus_keyboard_layout_helper_init (ExpidusKeyboardLayoutHelper *helper)
{
    /* init */
    helper->channel = NULL;

    /* open the channel */
    helper->channel = esconf_channel_get ("keyboard-layout");

    helper->xkb_disable_settings = esconf_channel_get_bool (helper->channel, "/Default/XkbDisable", TRUE);

#ifdef HAVE_LIBXKLAVIER
    helper->engine = xkl_engine_get_instance (GDK_DISPLAY_XDISPLAY(gdk_display_get_default()));
    if (helper->engine == NULL) {
        g_warning ("Failed to get Xkl engine for display");
    } else {
        /* monitor channel changes */
        g_signal_connect(G_OBJECT(helper->channel), "property-changed", G_CALLBACK(expidus_keyboard_layout_helper_channel_property_changed), helper);

        helper->config = xkl_config_rec_new();
        xkl_config_rec_get_from_server(helper->config, helper->engine);
        helper->system_keyboard_model = g_strdup(helper->config->model);

        gdk_window_add_filter(NULL, (GdkFilterFunc)handle_xevent, helper);
        g_signal_connect(helper->engine, "X-new-device",
                         G_CALLBACK(expidus_keyboard_layout_reset_xkl_config), helper);
        xkl_engine_start_listen(helper->engine, XKLL_TRACK_KEYBOARD_STATE);

        /* load settings */
        expidus_keyboard_layout_helper_set_model(helper);
        expidus_keyboard_layout_helper_set_layout(helper);
        expidus_keyboard_layout_helper_set_variant(helper);
        expidus_keyboard_layout_helper_set_grpkey(helper);
        expidus_keyboard_layout_helper_set_composekey(helper);
    }

#endif /* HAVE_LIBXKLAVIER */

    expidus_keyboard_layout_helper_process_xmodmap ();
}

static void
expidus_keyboard_layout_helper_finalize (GObject *object)
{
#ifdef HAVE_LIBXKLAVIER
    ExpidusKeyboardLayoutHelper *helper = EXPIDUS_KEYBOARD_LAYOUT_HELPER (object);

    if (helper->engine != NULL)
    {
        xkl_engine_stop_listen (helper->engine, XKLL_TRACK_KEYBOARD_STATE);
        gdk_window_remove_filter (NULL, (GdkFilterFunc) handle_xevent, helper);
        g_object_unref (helper->config);
        g_object_unref (helper->engine);
        g_free (helper->system_keyboard_model);
    }
#endif /* HAVE_LIBXKLAVIER */

    G_OBJECT_CLASS (expidus_keyboard_layout_helper_parent_class)->finalize (object);
}


static void
expidus_keyboard_layout_helper_process_xmodmap (void)
{
    const gchar *xmodmap_path;

    xmodmap_path = g_build_filename (expidus_get_homedir (), ".Xmodmap", NULL);

    if (g_file_test (xmodmap_path, G_FILE_TEST_EXISTS))
    {
        /* There is a .Xmodmap file, try to use it */
        const gchar *xmodmap_command;
        GError      *error = NULL;

        xmodmap_command = g_strconcat ("xmodmap ", xmodmap_path, NULL);

        essettings_dbg (XFSD_DEBUG_KEYBOARD_LAYOUT, "spawning \"%s\"", xmodmap_command);

        /* Launch the xmodmap command and only print errors when in debugging mode */
        if (!g_spawn_command_line_async (xmodmap_command, &error))
        {
            DBG ("Xmodmap call failed: %s", error->message);
            g_error_free (error);
        }
    }

    g_free ((gchar*) xmodmap_path);
}

#ifdef HAVE_LIBXKLAVIER

static void
expidus_keyboard_layout_helper_set_model (ExpidusKeyboardLayoutHelper *helper)
{
    gchar *xkbmodel;

    g_return_if_fail (helper->engine != NULL);

    if (!helper->xkb_disable_settings)
    {
        xkbmodel = esconf_channel_get_string (helper->channel, "/Default/XkbModel", NULL);
        if (!xkbmodel || !*xkbmodel)
        {
            /* If xkb model is not set by user, we want to try to use the system default */
            g_free (xkbmodel);
            xkbmodel = g_strdup (helper->system_keyboard_model);
        }

        if (g_strcmp0 (helper->config->model, xkbmodel) != 0)
        {
            g_free (helper->config->model);
            helper->config->model = xkbmodel;
            xkl_config_rec_activate (helper->config, helper->engine);

            essettings_dbg (XFSD_DEBUG_KEYBOARD_LAYOUT, "set model to \"%s\"", xkbmodel);
        }
        else
        {
            g_free (xkbmodel);
        }
    }
}

static void
expidus_keyboard_layout_helper_set (ExpidusKeyboardLayoutHelper *helper,
                                 const gchar *esconf_option_name,
                                 gchar ***xkl_config_option,
                                 const gchar *debug_name)
{
    gchar *esconf_values, *xkl_values;
    gchar **values;

    g_return_if_fail (helper->engine != NULL);

    if (!helper->xkb_disable_settings)
    {
        esconf_values  = g_strjoinv (",", *xkl_config_option);
        xkl_values  = esconf_channel_get_string (helper->channel,
                                                 esconf_option_name, esconf_values);

        if (g_strcmp0 (esconf_values, xkl_values) != 0)
        {
            values = g_strsplit_set (xkl_values, ",", 0);
            g_strfreev (*xkl_config_option);
            *xkl_config_option = values;
            xkl_config_rec_activate (helper->config, helper->engine);

            essettings_dbg (XFSD_DEBUG_KEYBOARD_LAYOUT, "set %s to \"%s\"", debug_name, xkl_values);
        }

        g_free (esconf_values);
        g_free (xkl_values);
    }
}

static void
expidus_keyboard_layout_helper_set_layout (ExpidusKeyboardLayoutHelper *helper)
{
    expidus_keyboard_layout_helper_set (helper, "/Default/XkbLayout",
                                     &helper->config->layouts,
                                     "layouts");
}

static void
expidus_keyboard_layout_helper_set_variant (ExpidusKeyboardLayoutHelper *helper)
{
    expidus_keyboard_layout_helper_set (helper, "/Default/XkbVariant",
                                     &helper->config->variants,
                                     "variants");
}

/**
 * @options - Xkl config options (array of strings terminated in NULL)
 * @option_name the name of the xkb option to look for (e.g., "grp:")
 * @_other_options if not NULL, will be set to the input option string
 *                 excluding @option_name. Needs to be freed with g_free().
 * @return the string in @options array corresponding to @option_name,
 *         or NULL if not found
 */
static gchar*
expidus_keyboard_layout_get_option (gchar **options,
                                 const gchar *option_name,
                                 gchar **_other_options)
{
    gchar **iter;
    gchar  *option_value  = NULL;
    gchar  *other_options = NULL;

    for (iter = options; iter && *iter; iter++)
    {
        if (g_str_has_prefix (*iter, option_name))
        {
            option_value = *iter;
        }
        else if (_other_options)
        {
            gchar *tmp = other_options;
            if (other_options)
            {
                other_options = g_strconcat (other_options, ",", *iter, NULL);
            }
            else
            {
                other_options = g_strdup (*iter);
            }
            g_free (tmp);
        }
    }

    if (_other_options)
        *_other_options = other_options;

    return option_value;
}

static void
expidus_keyboard_layout_helper_set_option (ExpidusKeyboardLayoutHelper *helper,
                                        const gchar *xkb_option_name,
                                        const gchar *esconf_option_name)
{
    g_return_if_fail (helper->engine != NULL);

    if (!helper->xkb_disable_settings)
    {
        gchar *option_value;
        gchar *xkl_option_value;
        gchar *other_options;

        xkl_option_value = expidus_keyboard_layout_get_option (helper->config->options,
                                                            xkb_option_name, &other_options);

        option_value = esconf_channel_get_string (helper->channel, esconf_option_name,
                                                  xkl_option_value);
        if (g_strcmp0 (option_value, xkl_option_value) != 0)
        {
            gchar *options_string;
            if (other_options == NULL)
            {
                options_string = g_strdup (option_value);
            }
            else
            {
                if (strlen (option_value) != 0)
                {
                    options_string = g_strconcat (option_value, ",", other_options, NULL);
                }
                else
                {
                    options_string = strdup (other_options);
                }
            }

            g_strfreev (helper->config->options);
            helper->config->options = g_strsplit (options_string, ",", 0);
            xkl_config_rec_activate (helper->config, helper->engine);

            essettings_dbg (XFSD_DEBUG_KEYBOARD_LAYOUT, "set %s to \"%s\"",
                            xkb_option_name, option_value);
            g_free (options_string);
        }

        g_free (other_options);
        g_free (option_value);
    }
}

static void
expidus_keyboard_layout_helper_set_grpkey (ExpidusKeyboardLayoutHelper *helper)
{
    expidus_keyboard_layout_helper_set_option (helper, "grp:", "/Default/XkbOptions/Group");
}

static void
expidus_keyboard_layout_helper_set_composekey (ExpidusKeyboardLayoutHelper *helper)
{
    expidus_keyboard_layout_helper_set_option (helper, "compose:", "/Default/XkbOptions/Compose");
}

static void
expidus_keyboard_layout_helper_channel_property_changed (EsconfChannel      *channel,
                                               const gchar               *property_name,
                                               const GValue              *value,
                                               ExpidusKeyboardLayoutHelper  *helper)
{
    g_return_if_fail (helper->channel == channel);
    g_return_if_fail (helper->engine != NULL);

    if (strcmp (property_name, "/Default/XkbDisable") == 0)
    {
        helper->xkb_disable_settings = g_value_get_boolean (value);
        /* Apply all settings */
        expidus_keyboard_layout_helper_set_model (helper);
        expidus_keyboard_layout_helper_set_layout (helper);
        expidus_keyboard_layout_helper_set_variant (helper);
        expidus_keyboard_layout_helper_set_grpkey (helper);
    }
    else if (strcmp (property_name, "/Default/XkbModel") == 0)
    {
        expidus_keyboard_layout_helper_set_model (helper);
    }
    else if (strcmp (property_name, "/Default/XkbLayout") == 0)
    {
        expidus_keyboard_layout_helper_set_layout (helper);
    }
    else if (strcmp (property_name, "/Default/XkbVariant") == 0)
    {
        expidus_keyboard_layout_helper_set_variant (helper);
    }
    else if (strcmp (property_name, "/Default/XkbOptions/Group") == 0)
    {
        expidus_keyboard_layout_helper_set_grpkey (helper);
    }
    else if (strcmp (property_name, "/Default/XkbOptions/Compose") == 0)
    {
        expidus_keyboard_layout_helper_set_composekey (helper);
    }

    expidus_keyboard_layout_helper_process_xmodmap ();
}

static GdkFilterReturn
handle_xevent (GdkXEvent * xev, GdkEvent * event, ExpidusKeyboardLayoutHelper *helper)
{
    XEvent *xevent = (XEvent *) xev;
    xkl_engine_filter_events (helper->engine, xevent);

    return GDK_FILTER_CONTINUE;
}

static void
expidus_keyboard_layout_reset_xkl_config (XklEngine *xklengine,
                                       ExpidusKeyboardLayoutHelper *helper)
{
    g_return_if_fail (helper->engine != NULL);

    if (!helper->xkb_disable_settings)
    {
        gchar *esconf_model;

        essettings_dbg (XFSD_DEBUG_KEYBOARD_LAYOUT,
                        "New keyboard detected; restoring XKB settings.");

        xkl_config_rec_reset (helper->config);
        xkl_config_rec_get_from_server (helper->config, helper->engine);

        esconf_model = esconf_channel_get_string (helper->channel, "/Default/XkbModel", NULL);
        if (esconf_model && *esconf_model &&
            g_strcmp0 (esconf_model, helper->config->model) != 0 &&
            g_strcmp0 (helper->system_keyboard_model, helper->config->model) != 0)
        {
            /* We get X-new-device notifications multiple times for a single keyboard device (why?);
               if keyboard model is set in user preferences,
               we'll reset the default to the user preference when first notified
               and we don't want to use that as a system default the next time
               the user tries to reset keyboard model to the default in expidus1-keyboard-settings.

               The above conditional says: if user set the keyboard model and that's the one
               we see here, don't assume it's the system default since it was us who set it
               on the previous notification.
             */
            g_free (helper->system_keyboard_model);
            helper->system_keyboard_model = g_strdup (helper->config->model);
            essettings_dbg (XFSD_DEBUG_KEYBOARD_LAYOUT,
                            "system default keyboard model reset: %s",
                            helper->system_keyboard_model);
        }
        g_free (esconf_model);

        expidus_keyboard_layout_helper_set_model (helper);
        expidus_keyboard_layout_helper_set_layout (helper);
        expidus_keyboard_layout_helper_set_variant (helper);
        expidus_keyboard_layout_helper_set_grpkey (helper);
        expidus_keyboard_layout_helper_set_composekey (helper);

        expidus_keyboard_layout_helper_process_xmodmap ();
    }
}
#endif /* HAVE_LIBXKLAVIER */
