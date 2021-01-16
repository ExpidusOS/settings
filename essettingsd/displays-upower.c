/*
 *  Copyright (C) 2012 Lionel Le Folgoc <lionel@lefolgoc.net>
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <upower.h>

#include <X11/extensions/Xrandr.h>

#include "debug.h"
#include "displays-upower.h"



static void             expidus_displays_upower_dispose                        (GObject                 *object);

#if UP_CHECK_VERSION(0, 99, 0)
static void             expidus_displays_upower_property_changed               (UpClient                *client,
                                                                             GParamSpec              *pspec,
                                                                             ExpidusDisplaysUPower      *upower);
#else
static void             expidus_displays_upower_property_changed               (UpClient                *client,
                                                                             ExpidusDisplaysUPower      *upower);
#endif


struct _ExpidusDisplaysUPowerClass
{
    GObjectClass __parent__;

    void         (*lid_changed)     (ExpidusDisplaysUPower *upower,
                                     gboolean            lid_is_closed);
};

struct _ExpidusDisplaysUPower
{
    GObject   __parent__;

    UpClient *client;
    gint      handler;

    guint     lid_is_closed : 1;
};

enum
{
    LID_CHANGED,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};



G_DEFINE_TYPE (ExpidusDisplaysUPower, expidus_displays_upower, G_TYPE_OBJECT);



static void
expidus_displays_upower_class_init (ExpidusDisplaysUPowerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->dispose = expidus_displays_upower_dispose;

    signals[LID_CHANGED] =
        g_signal_new ("lid-changed",
                      EXPIDUS_TYPE_DISPLAYS_UPOWER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (ExpidusDisplaysUPowerClass, lid_changed),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__BOOLEAN,
                      G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
}



static void
expidus_displays_upower_init (ExpidusDisplaysUPower *upower)
{
    upower->client = up_client_new ();
    if (!UP_IS_CLIENT (upower->client))
    {
        upower->handler = 0;
        upower->lid_is_closed = 0;
        return;
    }

    upower->lid_is_closed = up_client_get_lid_is_closed (upower->client);
#if UP_CHECK_VERSION(0, 99, 0)
    upower->handler = g_signal_connect (G_OBJECT (upower->client),
                                        "notify",
                                        G_CALLBACK (expidus_displays_upower_property_changed),
                                        upower);
#else
    upower->handler = g_signal_connect (G_OBJECT (upower->client),
                                        "changed",
                                        G_CALLBACK (expidus_displays_upower_property_changed),
                                        upower);
#endif
}



static void
expidus_displays_upower_dispose (GObject *object)
{
    ExpidusDisplaysUPower *upower = EXPIDUS_DISPLAYS_UPOWER (object);

    if (upower->handler > 0)
    {
        g_signal_handler_disconnect (G_OBJECT (upower->client),
                                     upower->handler);
        g_object_unref (upower->client);
        upower->handler = 0;
    }

    (*G_OBJECT_CLASS (expidus_displays_upower_parent_class)->dispose) (object);
}



static void
#if UP_CHECK_VERSION(0, 99, 0)
expidus_displays_upower_property_changed (UpClient           *client,
                                       GParamSpec         *pspec,
                                       ExpidusDisplaysUPower *upower)
#else
expidus_displays_upower_property_changed (UpClient           *client,
                                       ExpidusDisplaysUPower *upower)
#endif
{
    gboolean lid_is_closed;

    /* no lid, no chocolate */
    if (!up_client_get_lid_is_present (client))
        return;

    lid_is_closed = up_client_get_lid_is_closed (client);
    if (upower->lid_is_closed != lid_is_closed)
    {
        essettings_dbg (XFSD_DEBUG_DISPLAYS, "UPower lid event received (%s -> %s).",
                        XFSD_LID_STR (upower->lid_is_closed), XFSD_LID_STR (lid_is_closed));

        upower->lid_is_closed = lid_is_closed;
        g_signal_emit (G_OBJECT (upower), signals[LID_CHANGED], 0, upower->lid_is_closed);
    }
}
