/*
 *  expidus1-settings-manager
 *
 *  Copyright (c) 2008 Brian Tarricone <bjt23@cornell.edu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License ONLY.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <gtk/gtk.h>

#include <gdk/gdkx.h>

#include <libexpidus1util/libexpidus1util.h>
#include <markon/markon.h>
#include <esconf/esconf.h>

#include "expidus-settings-manager-dialog.h"

static gboolean opt_version = FALSE;
static gchar   *opt_dialog = NULL;

static GOptionEntry option_entries[] =
{
    { "version", 'V', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_NONE, &opt_version, N_("Version information"), NULL },
    { "dialog", 'd', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_STRING, &opt_dialog, N_("Settings dialog to show"), NULL },
    { NULL }
};



int
main(int argc,
     char **argv)
{
    GtkWidget *dialog;
    GError    *error = NULL;

    expidus_textdomain(GETTEXT_PACKAGE, LOCALEDIR, "UTF-8");

    if (!gtk_init_with_args (&argc, &argv, NULL, option_entries, GETTEXT_PACKAGE, &error))
    {
        if (G_LIKELY (error != NULL))
        {
            g_print ("%s: %s.\n", G_LOG_DOMAIN, error->message);
            g_print (_("Type '%s --help' for usage."), G_LOG_DOMAIN);
            g_print ("\n");

            g_error_free (error);
        }
        else
        {
            g_error ("Unable to open display.");
        }
        return EXIT_FAILURE;
    }

    if (G_UNLIKELY (opt_version))
    {
        g_print ("%s %s (Expidus %s)\n\n", G_LOG_DOMAIN, PACKAGE_VERSION, expidus_version_string());
        g_print ("%s\n", "Copyright (c) 2008-2019");
        g_print ("\t%s\n\n", _("The Expidus development team. All rights reserved."));
        g_print (_("Please report bugs to <%s>."), PACKAGE_BUGREPORT);
        g_print ("\n");
        return EXIT_SUCCESS;
    }

    /* initialize esconf */
    if (G_UNLIKELY (!esconf_init (&error)))
    {
        /* print error and leave */
        g_critical ("Failed to connect to Esconf daemon: %s", error->message);
        g_error_free (error);

        return EXIT_FAILURE;
    }

    /* Workaround for xinput2's subpixel handling triggering unwanted enter/leave-notify events:
     * https://bugs.freedesktop.org/show_bug.cgi?id=92681 */
    g_setenv("GDK_CORE_DEVICE_EVENTS", "1", TRUE);

    markon_set_environment ("EXPIDUS");

    dialog = expidus_settings_manager_dialog_new ();
    gtk_window_set_type_hint (GTK_WINDOW (dialog), GDK_WINDOW_TYPE_HINT_NORMAL);
    gtk_window_present (GTK_WINDOW (dialog));

    if (opt_dialog != NULL
        && !expidus_settings_manager_dialog_show_dialog (EXPIDUS_SETTINGS_MANAGER_DIALOG(dialog), opt_dialog))
    {
        g_message ("Dialog \"%s\" not found.", opt_dialog);
    }

    /* To prevent the settings dialog to be saved in the session */
    gdk_x11_set_sm_client_id ("FAKE ID");

    gtk_main();

    esconf_shutdown ();

    return EXIT_SUCCESS;
}
