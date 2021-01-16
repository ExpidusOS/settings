/*
 *  Copyright (c) 2014 Olivier Fourdan <fourdan@expidus.org>
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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <esconf/esconf.h>
#include <libexpidus1util/libexpidus1util.h>
#include "gtk-decorations.h"

#define DEFAULT_LAYOUT "O|HMC"

static void expidus_decorations_helper_finalize                  (GObject                  *object);
static void expidus_decorations_helper_channel_property_changed  (EsconfChannel            *channel,
                                                               const gchar              *property_name,
                                                               const GValue             *value,
                                                               ExpidusDecorationsHelper    *helper);

struct _ExpidusDecorationsHelperClass
{
    GObjectClass __parent__;
};

struct _ExpidusDecorationsHelper
{
    GObject  __parent__;

    /* esconf channel */
    EsconfChannel *wm_channel;
    EsconfChannel *xsettings_channel;
};

G_DEFINE_TYPE (ExpidusDecorationsHelper, expidus_decorations_helper, G_TYPE_OBJECT)

static void
expidus_decorations_helper_class_init (ExpidusDecorationsHelperClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = expidus_decorations_helper_finalize;
}

static const gchar *
expidus_decorations_button_layout_xlate (char c)
{
    switch (c)
    {
        case 'O':
            return "menu";
            break;
        case 'H':
            return "minimize";
            break;
        case 'M':
            return "maximize";
            break;
        case 'C':
            return "close";
            break;
        case '|':
            return ":";
            break;
        default:
            return NULL;
    }
    return NULL;
}

static void
expidus_decorations_set_decoration_layout (ExpidusDecorationsHelper *helper,
                                        const gchar *value)
{
    GString *join;
    const gchar *gtk_name;
    gchar *gtk_decoration_layout;
    gboolean add_comma;
    int len, i;

    add_comma = FALSE;
    len = strlen (value);
    join = g_string_new (NULL);
    for (i = 0; i < len; i++)
    {
        gtk_name = expidus_decorations_button_layout_xlate(value[i]);
        if (gtk_name)
        {
            if (add_comma && value[i] != '|')
                join = g_string_append (join, ",");
            join = g_string_append (join, gtk_name);
            add_comma = (value[i] != '|');
        }
    }

    gtk_decoration_layout = g_string_free (join, FALSE);
    esconf_channel_set_string (helper->xsettings_channel,
                             "/Gtk/DecorationLayout", gtk_decoration_layout);
    g_free (gtk_decoration_layout);
}

static void
expidus_decorations_helper_channel_property_changed (EsconfChannel         *channel,
                                                  const gchar           *property_name,
                                                  const GValue          *value,
                                                  ExpidusDecorationsHelper *helper)
{
    if (strcmp (property_name, "/general/button_layout") == 0)
    {
        expidus_decorations_set_decoration_layout (helper, g_value_get_string (value));
    }
}

static void
expidus_decorations_helper_init (ExpidusDecorationsHelper *helper)
{
    gchar *layout;

    helper->wm_channel = esconf_channel_get ("eswm1");
    helper->xsettings_channel = esconf_channel_get ("xsettings");

    layout = esconf_channel_get_string  (helper->wm_channel,
                                         "/general/button_layout", DEFAULT_LAYOUT);
    expidus_decorations_set_decoration_layout (helper, layout);
    g_free (layout);

    /* monitor WM channel changes */
    g_signal_connect (G_OBJECT (helper->wm_channel), "property-changed",
        G_CALLBACK (expidus_decorations_helper_channel_property_changed), helper);
}

static void
expidus_decorations_helper_finalize (GObject *object)
{
    (*G_OBJECT_CLASS (expidus_decorations_helper_parent_class)->finalize) (object);
}

