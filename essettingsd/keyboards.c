/*
 *  Copyright (c) 2008 Stephan Arts <stephan@expidus.org>
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
 *  XKB Extension code taken from the original mcs-keyboard-plugin written
 *  by Olivier Fourdan.
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
#include <X11/keysym.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <esconf/esconf.h>
#include <libexpidus1util/libexpidus1util.h>

#include "debug.h"
#include "keyboards.h"



static void expidus_keyboards_helper_finalize                  (GObject                  *object);
static void expidus_keyboards_helper_set_auto_repeat_mode      (ExpidusKeyboardsHelper      *helper);
static void expidus_keyboards_helper_set_repeat_rate           (ExpidusKeyboardsHelper      *helper);
static void expidus_keyboards_helper_channel_property_changed  (EsconfChannel            *channel,
                                                             const gchar              *property_name,
                                                             const GValue             *value,
                                                             ExpidusKeyboardsHelper      *helper);
static void expidus_keyboards_helper_restore_numlock_state     (EsconfChannel            *channel);
static void expidus_keyboards_helper_save_numlock_state        (EsconfChannel            *channel);
static gboolean expidus_keyboards_helper_device_is_keyboard    (XID xid);
static void expidus_keyboards_helper_set_all_settings          (ExpidusKeyboardsHelper      *helper);
#ifdef DEVICE_HOTPLUGGING
static GdkFilterReturn  expidus_keyboards_helper_event_filter  (GdkXEvent                *xevent,
                                                             GdkEvent                 *gdk_event,
                                                             gpointer                 user_data);
#endif




struct _ExpidusKeyboardsHelperClass
{
    GObjectClass __parent__;
};

struct _ExpidusKeyboardsHelper
{
    GObject  __parent__;

    /* esconf channel */
    EsconfChannel *channel;

#ifdef DEVICE_HOTPLUGGING
    /* device presence event type */
    gint device_presence_event_type;
#endif

};



G_DEFINE_TYPE (ExpidusKeyboardsHelper, expidus_keyboards_helper, G_TYPE_OBJECT)



static void
expidus_keyboards_helper_class_init (ExpidusKeyboardsHelperClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = expidus_keyboards_helper_finalize;
}



static void
expidus_keyboards_helper_init (ExpidusKeyboardsHelper *helper)
{
    gint dummy;
    gint marjor_ver, minor_ver;
    Display *xdisplay;
#ifdef DEVICE_HOTPLUGGING
    XEventClass event_class;
#endif

    /* init */
    helper->channel = NULL;

    /* get the default display */
    xdisplay = gdk_x11_display_get_xdisplay (gdk_display_get_default ());

    if (XkbQueryExtension (xdisplay, &dummy, &dummy, &dummy, &marjor_ver, &minor_ver))
    {
        essettings_dbg (XFSD_DEBUG_KEYBOARDS, "initialized xkb %d.%d", marjor_ver, minor_ver);

        /* open the channel */
        helper->channel = esconf_channel_get ("keyboards");

        /* monitor channel changes */
        g_signal_connect (G_OBJECT (helper->channel), "property-changed",
            G_CALLBACK (expidus_keyboards_helper_channel_property_changed), helper);

#ifdef DEVICE_HOTPLUGGING
        if (G_LIKELY (xdisplay != NULL))
        {
            /* monitor device changes */
            gdk_x11_display_error_trap_push (gdk_display_get_default ());
            DevicePresence (xdisplay, helper->device_presence_event_type, event_class);
            XSelectExtensionEvent (xdisplay, RootWindow (xdisplay, DefaultScreen (xdisplay)), &event_class, 1);

            /* add an event filter */
            if (gdk_x11_display_error_trap_pop (gdk_display_get_default ()) == 0)
                gdk_window_add_filter (NULL, expidus_keyboards_helper_event_filter, helper);
            else
                g_warning ("Failed to create device filter");
        }
#endif

        /* load keyboard settings */
        expidus_keyboards_helper_set_all_settings (helper);
    }
    else
    {
        /* warning */
        g_critical ("Failed to initialize the Xkb extension.");
    }
}



static void
expidus_keyboards_helper_finalize (GObject *object)
{
    ExpidusKeyboardsHelper *helper = EXPIDUS_KEYBOARDS_HELPER (object);

    /* Save the numlock state */
    expidus_keyboards_helper_save_numlock_state (helper->channel);

    (*G_OBJECT_CLASS (expidus_keyboards_helper_parent_class)->finalize) (object);
}



static void
expidus_keyboards_helper_set_auto_repeat_mode (ExpidusKeyboardsHelper *helper)
{
    XKeyboardControl values;
    gboolean         repeat;

    /* load setting */
    repeat = esconf_channel_get_bool (helper->channel, "/Default/KeyRepeat", TRUE);

    /* set key repeat */
    values.auto_repeat_mode = repeat ? 1 : 0;

    gdk_x11_display_error_trap_push (gdk_display_get_default ());
    XChangeKeyboardControl (GDK_DISPLAY_XDISPLAY(gdk_display_get_default()), KBAutoRepeatMode, &values);
    if (gdk_x11_display_error_trap_pop (gdk_display_get_default ()) != 0)
        g_critical ("Failed to change keyboard repeat mode");

    essettings_dbg (XFSD_DEBUG_KEYBOARDS, "set auto repeat %s", repeat ? "on" : "off");
}



static void
expidus_keyboards_helper_set_repeat_rate (ExpidusKeyboardsHelper *helper)
{
    XkbDescPtr xkb;
    gint       delay, rate;

    /* load settings */
    delay = esconf_channel_get_int (helper->channel, "/Default/KeyRepeat/Delay", 500);
    rate = esconf_channel_get_int (helper->channel, "/Default/KeyRepeat/Rate", 20);

    gdk_x11_display_error_trap_push (gdk_display_get_default ());

    /* allocate xkb structure */
    xkb = XkbAllocKeyboard ();
    if (G_LIKELY (xkb))
    {
        /* load controls */
        XkbGetControls (GDK_DISPLAY_XDISPLAY(gdk_display_get_default()), XkbRepeatKeysMask, xkb);

        /* set new values */
        xkb->ctrls->repeat_delay = delay;
        xkb->ctrls->repeat_interval = rate != 0 ? 1000 / rate : 0;

        /* set updated controls */
        XkbSetControls (GDK_DISPLAY_XDISPLAY(gdk_display_get_default()), XkbRepeatKeysMask, xkb);

        essettings_dbg (XFSD_DEBUG_KEYBOARDS, "set key repeat (delay=%d, rate=%d)",
                        xkb->ctrls->repeat_delay, xkb->ctrls->repeat_interval);

        /* cleanup */
        XkbFreeControls (xkb, XkbRepeatKeysMask, True);
        XFree (xkb);
    }

    if (gdk_x11_display_error_trap_pop (gdk_display_get_default ()) != 0)
        g_critical ("Failed to change the keyboard repeat");
}



static void
expidus_keyboards_helper_channel_property_changed (EsconfChannel      *channel,
                                               const gchar         *property_name,
                                               const GValue        *value,
                                               ExpidusKeyboardsHelper *helper)
{
    g_return_if_fail (helper->channel == channel);

    if (strcmp (property_name, "/Default/KeyRepeat") == 0)
    {
        /* update auto repeat mode */
        expidus_keyboards_helper_set_auto_repeat_mode (helper);
    }
    else if (strcmp (property_name, "/Default/KeyRepeat/Delay") == 0
             || strcmp (property_name, "/Default/KeyRepeat/Rate") == 0)
    {
        /* update repeat rate */
        expidus_keyboards_helper_set_repeat_rate (helper);
    }
}



static void
expidus_keyboards_helper_restore_numlock_state (EsconfChannel *channel)
{
    unsigned int  numlock_mask;
    Display      *dpy;
    gboolean      state;

    if (esconf_channel_has_property (channel, "/Default/Numlock")
        && esconf_channel_get_bool (channel, "/Default/RestoreNumlock", TRUE))
    {
        state = esconf_channel_get_bool (channel, "/Default/Numlock", FALSE);

        gdk_x11_display_error_trap_push (gdk_display_get_default ());

        dpy = GDK_DISPLAY_XDISPLAY (gdk_display_get_default ());
        numlock_mask = XkbKeysymToModifiers (dpy, XK_Num_Lock);
        XkbLockModifiers (dpy, XkbUseCoreKbd, numlock_mask, state ? numlock_mask : 0);

        if (gdk_x11_display_error_trap_pop (gdk_display_get_default ()) != 0)
            g_critical ("Failed to change numlock modifier");

        essettings_dbg (XFSD_DEBUG_KEYBOARDS, "set numlock %s", state ? "on" : "off");
    }
    else
    {
        essettings_dbg (XFSD_DEBUG_KEYBOARDS, "don't set numlock");
    }
}



static void
expidus_keyboards_helper_save_numlock_state (EsconfChannel *channel)
{
    Display *dpy;
    Bool     numlock_state;
    Atom     numlock;

    gdk_x11_display_error_trap_push (gdk_display_get_default ());

    dpy = GDK_DISPLAY_XDISPLAY (gdk_display_get_default ());
    numlock = XInternAtom(dpy, "Num Lock", False);
    XkbGetNamedIndicator (dpy, numlock, NULL, &numlock_state, NULL, NULL);

    if (gdk_x11_display_error_trap_pop (gdk_display_get_default ()) != 0)
        g_critical ("Failed to get numlock state");

    essettings_dbg (XFSD_DEBUG_KEYBOARDS, "save numlock %s", numlock_state ? "on" : "off");

    esconf_channel_set_bool (channel, "/Default/Numlock", numlock_state);
}



static void
expidus_keyboards_helper_set_all_settings (ExpidusKeyboardsHelper *helper)
{
        expidus_keyboards_helper_set_auto_repeat_mode (helper);
        expidus_keyboards_helper_set_repeat_rate (helper);
        expidus_keyboards_helper_restore_numlock_state (helper->channel);
}



#ifdef DEVICE_HOTPLUGGING
static gboolean
expidus_keyboards_helper_device_is_keyboard (XID xid)
{
    XDeviceInfo *device;
    gboolean device_found;
    XDeviceInfo *device_list;
    Atom         keyboard_type;
    gint         n, ndevices;
    Display     *xdisplay = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());

    keyboard_type = XInternAtom (xdisplay, XI_KEYBOARD, True);
    device_list = XListInputDevices(xdisplay, &ndevices);
    device_found = FALSE;
    for (n = 0; n < ndevices; n++)
    {
        device = &device_list[n];
        /* look for a keyboard that matches this XID */
        if (device->type == keyboard_type &&
            device->id == xid)
        {
            device_found = TRUE;
            break;
        }
    }
    XFreeDeviceList(device_list);

    return device_found;
}
#endif



#ifdef DEVICE_HOTPLUGGING
static GdkFilterReturn
expidus_keyboards_helper_event_filter (GdkXEvent *xevent,
                                    GdkEvent  *gdk_event,
                                    gpointer   user_data)
{
    XEvent                     *event = xevent;
    XDevicePresenceNotifyEvent *dpn_event = xevent;
    ExpidusKeyboardsHelper        *helper = EXPIDUS_KEYBOARDS_HELPER (user_data);

    if (G_UNLIKELY (event->type != helper->device_presence_event_type))
        return GDK_FILTER_CONTINUE;

    if (G_LIKELY (dpn_event->devchange != DeviceAdded))
        return GDK_FILTER_CONTINUE;

    if (!expidus_keyboards_helper_device_is_keyboard(dpn_event->deviceid))
        return GDK_FILTER_CONTINUE;

    /* New keyboard added. Need to reapply settings. */
    expidus_keyboards_helper_set_all_settings (helper);

    return GDK_FILTER_CONTINUE;
}
#endif
