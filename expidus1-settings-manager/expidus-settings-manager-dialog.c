/*
 *  expidus1-settings-manager
 *
 *  Copyright (c) 2008 Brian Tarricone <bjt23@cornell.edu>
 *                     Jannis Pohlmann <jannis@expidus.org>
 *  Copyright (c) 2012 Nick Schermer <nick@expidus.org>
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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gdk/gdkkeysyms.h>

#include <libexpidus1util/libexpidus1util.h>
#include <libexpidus1ui/libexpidus1ui.h>
#include <esconf/esconf.h>
#include <markon/markon.h>
#include <endo/endo.h>

#include "expidus-settings-manager-dialog.h"

#define TEXT_WIDTH (128)
#define ICON_WIDTH (48)



struct _ExpidusSettingsManagerDialogClass
{
    ExpidusTitledDialogClass __parent__;
};

struct _ExpidusSettingsManagerDialog
{
    ExpidusTitledDialog __parent__;

    EsconfChannel  *channel;
    MarkonMenu     *menu;

    GtkListStore   *store;

    GtkWidget      *revealer;
    GtkWidget      *filter_button;
    GtkWidget      *filter_entry;
    gchar          *filter_text;

    GtkWidget      *category_viewport;
    GtkWidget      *category_scroll;
    GtkWidget      *category_box;

    GList          *categories;

    GtkCssProvider *css_provider;

    GtkWidget      *socket_scroll;
    GtkWidget      *socket_viewport;
    MarkonMenuItem *socket_item;

    GtkWidget      *button_back;
    GtkWidget      *button_help;

    gchar          *help_page;
    gchar          *help_component;
    gchar          *help_version;
};

typedef struct
{
    MarkonMenuDirectory       *directory;
    ExpidusSettingsManagerDialog *dialog;
    GtkWidget                 *iconview;
    GtkWidget                 *box;
}
DialogCategory;



enum
{
    COLUMN_NAME,
    COLUMN_ICON_NAME,
    COLUMN_TOOLTIP,
    COLUMN_MENU_ITEM,
    COLUMN_MENU_DIRECTORY,
    COLUMN_FILTER_TEXT,
    N_COLUMNS
};



static void     expidus_settings_manager_dialog_finalize        (GObject                   *object);
static void     expidus_settings_manager_dialog_style_updated   (GtkWidget                 *widget);
static void     expidus_settings_manager_dialog_set_hover_style (ExpidusSettingsManagerDialog *dialog);
static void     expidus_settings_manager_dialog_response        (GtkDialog                 *widget,
                                                              gint                       response_id);
static void     expidus_settings_manager_dialog_set_title       (ExpidusSettingsManagerDialog *dialog,
                                                              const gchar               *title,
                                                              const gchar               *icon_name);
static void     expidus_settings_manager_dialog_go_back         (ExpidusSettingsManagerDialog *dialog);
static void     expidus_settings_manager_show_filter_toggled    (GtkToggleButton           *button,
                                                              gpointer                   user_data);
static void     expidus_settings_manager_dialog_entry_changed   (GtkWidget                 *entry,
                                                              ExpidusSettingsManagerDialog *dialog);
static gboolean expidus_settings_manager_dialog_entry_key_press (GtkWidget                 *entry,
                                                              GdkEventKey               *event,
                                                              ExpidusSettingsManagerDialog *dialog);
static void     expidus_settings_manager_dialog_menu_reload     (ExpidusSettingsManagerDialog *dialog);
static void     expidus_settings_manager_dialog_scroll_to_item  (GtkWidget                 *iconview,
                                                              ExpidusSettingsManagerDialog *dialog);



G_DEFINE_TYPE (ExpidusSettingsManagerDialog, expidus_settings_manager_dialog, EXPIDUS_TYPE_TITLED_DIALOG)



static void
expidus_settings_manager_dialog_class_init (ExpidusSettingsManagerDialogClass *klass)
{
    GObjectClass   *gobject_class;
    GtkDialogClass *gtkdialog_class;
    GtkWidgetClass *gtkwiget_class;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = expidus_settings_manager_dialog_finalize;

    gtkwiget_class = GTK_WIDGET_CLASS (klass);
    gtkwiget_class->style_updated = expidus_settings_manager_dialog_style_updated;

    gtkdialog_class = GTK_DIALOG_CLASS (klass);
    gtkdialog_class->response = expidus_settings_manager_dialog_response;
}



static void
expidus_settings_manager_queue_resize (ExpidusSettingsManagerDialog *dialog)
{
    GList *li;
    DialogCategory *category;

    for (li = dialog->categories; li != NULL; li = li->next)
    {
        category = li->data;
        gtk_widget_queue_resize (GTK_WIDGET (category->iconview));
    }
}

static gboolean
expidus_settings_manager_queue_resize_cb (gpointer user_data)
{
    ExpidusSettingsManagerDialog *dialog = user_data;

    expidus_settings_manager_queue_resize (dialog);
    return FALSE;
}



static void
expidus_settings_manager_dialog_check_resize (GtkWidget *widget, gpointer *user_data)
{
    ExpidusSettingsManagerDialog *dialog = EXPIDUS_SETTINGS_MANAGER_DIALOG (user_data);
    expidus_settings_manager_queue_resize (dialog);
}



static void
expidus_settings_manager_dialog_init (ExpidusSettingsManagerDialog *dialog)
{
    GtkWidget *dialog_vbox;
    GtkWidget *box;
    GtkWidget *entry;
    GtkWidget *scroll;
    GtkWidget *viewport;
    GtkWidget *image;
    GtkWidget *button;
    gchar     *path;

    dialog->channel = esconf_channel_get ("expidus1-settings-manager");

    dialog->store = gtk_list_store_new (N_COLUMNS,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        MARKON_TYPE_MENU_ITEM,
                                        MARKON_TYPE_MENU_DIRECTORY,
                                        G_TYPE_STRING);

    path = expidus_resource_lookup (EXPIDUS_RESOURCE_CONFIG, "menus/expidus-settings-manager.menu");
    dialog->menu = markon_menu_new_for_path (path != NULL ? path : MENUFILE);
    g_free (path);

    gtk_window_set_default_size (GTK_WINDOW (dialog),
      esconf_channel_get_int (dialog->channel, "/last/window-width", 640),
      esconf_channel_get_int (dialog->channel, "/last/window-height", 500));
    expidus_settings_manager_dialog_set_title (dialog, NULL, NULL);

    /* Add a buttonbox (Help, All Settings, Close) at bottom of the main box */
    expidus_titled_dialog_create_action_area (EXPIDUS_TITLED_DIALOG (dialog));

    dialog->button_help = expidus_titled_dialog_add_button (EXPIDUS_TITLED_DIALOG (dialog), _("_Help"), GTK_RESPONSE_HELP);
    image = gtk_image_new_from_icon_name ("help-browser", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image (GTK_BUTTON (dialog->button_help), image);

    dialog->button_back = expidus_titled_dialog_add_button (EXPIDUS_TITLED_DIALOG (dialog), _("All _Settings"), GTK_RESPONSE_NONE);
    image = gtk_image_new_from_icon_name ("go-previous-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image (GTK_BUTTON (dialog->button_back), image);
    gtk_widget_set_sensitive (dialog->button_back, FALSE);
    g_signal_connect_swapped (G_OBJECT (dialog->button_back), "clicked",
        G_CALLBACK (expidus_settings_manager_dialog_go_back), dialog);

    button = expidus_titled_dialog_add_button (EXPIDUS_TITLED_DIALOG (dialog), _("_Close"), GTK_RESPONSE_CLOSE);
    image = gtk_image_new_from_icon_name ("window-close-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image (GTK_BUTTON (button), image);

    /* Add the filter bar */
    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    dialog->revealer = gtk_revealer_new ();
    gtk_revealer_set_transition_type (GTK_REVEALER (dialog->revealer), GTK_REVEALER_TRANSITION_TYPE_SLIDE_DOWN);
    gtk_widget_set_margin_top (box, 4);
    gtk_container_add (GTK_CONTAINER (dialog->revealer), box);
    gtk_widget_show (dialog->revealer);

    dialog->filter_button = button = gtk_toggle_button_new ();
    image = gtk_image_new_from_icon_name ("edit-find-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image (GTK_BUTTON (button), image);
    gtk_header_bar_pack_end (GTK_HEADER_BAR (gtk_dialog_get_header_bar (GTK_DIALOG (dialog))), button);
    gtk_widget_show (button);
    g_signal_connect (G_OBJECT (button), "toggled", G_CALLBACK (expidus_settings_manager_show_filter_toggled), dialog);
    esconf_g_property_bind (dialog->channel, "/last/filter-visible", G_TYPE_BOOLEAN, G_OBJECT (button), "active");

    dialog->filter_entry = entry = gtk_search_entry_new ();
    gtk_box_pack_start (GTK_BOX (box), entry, TRUE, FALSE, 0);
    gtk_widget_set_valign (entry, GTK_ALIGN_CENTER);
    g_signal_connect (G_OBJECT (entry), "changed",
        G_CALLBACK (expidus_settings_manager_dialog_entry_changed), dialog);
    g_signal_connect (G_OBJECT (entry), "key-press-event",
        G_CALLBACK (expidus_settings_manager_dialog_entry_key_press), dialog);
    gtk_widget_show_all (box);

    dialog_vbox = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

    gtk_box_pack_start (GTK_BOX (dialog_vbox), dialog->revealer, FALSE, TRUE, 0);

    dialog->category_scroll = scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll), GTK_SHADOW_ETCHED_IN);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (dialog_vbox), scroll, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (scroll), 6);
    gtk_widget_show (scroll);

    viewport = dialog->category_viewport = gtk_viewport_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (scroll), viewport);
    gtk_viewport_set_shadow_type (GTK_VIEWPORT (viewport), GTK_SHADOW_NONE);
    gtk_widget_show (viewport);

    dialog->category_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add (GTK_CONTAINER (viewport), dialog->category_box);
    gtk_container_set_border_width (GTK_CONTAINER (dialog->category_box), 6);
    gtk_widget_show (dialog->category_box);
    gtk_widget_set_size_request (dialog->category_box,
                                 TEXT_WIDTH   /* text */
                                 + ICON_WIDTH /* icon */
                                 + (5 * 6)    /* borders */, -1);

    /* pluggable dialog scrolled window and viewport */
    dialog->socket_scroll = scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll), GTK_SHADOW_NONE);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (dialog_vbox), scroll, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (scroll), 0);

    dialog->socket_viewport = viewport = gtk_viewport_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (scroll), viewport);
    gtk_viewport_set_shadow_type (GTK_VIEWPORT (viewport), GTK_SHADOW_NONE);
    gtk_widget_show (viewport);

    dialog->css_provider = gtk_css_provider_new ();

    expidus_settings_manager_dialog_menu_reload (dialog);

    g_signal_connect_swapped (G_OBJECT (dialog->menu), "reload-required",
        G_CALLBACK (expidus_settings_manager_dialog_menu_reload), dialog);

    g_signal_connect (G_OBJECT (dialog), "check-resize", G_CALLBACK (expidus_settings_manager_dialog_check_resize), dialog);
}



static void
expidus_settings_manager_dialog_finalize (GObject *object)
{
    ExpidusSettingsManagerDialog *dialog = EXPIDUS_SETTINGS_MANAGER_DIALOG (object);

    g_free (dialog->help_page);
    g_free (dialog->help_component);
    g_free (dialog->help_version);

    g_free (dialog->filter_text);

    if (dialog->socket_item != NULL)
        g_object_unref (G_OBJECT (dialog->socket_item));

    g_object_unref (G_OBJECT (dialog->menu));
    g_object_unref (G_OBJECT (dialog->store));

    G_OBJECT_CLASS (expidus_settings_manager_dialog_parent_class)->finalize (object);
}



static void
expidus_settings_manager_dialog_style_updated (GtkWidget *widget)
{
    ExpidusSettingsManagerDialog *dialog = EXPIDUS_SETTINGS_MANAGER_DIALOG (widget);
    GtkStyleContext *context;

    context = gtk_widget_get_style_context (dialog->category_viewport);
    gtk_style_context_add_class (context, "view");
    gtk_style_context_add_class (context, "endoiconview");
    expidus_settings_manager_dialog_set_hover_style (dialog);
}



static void
expidus_settings_manager_dialog_set_hover_style (ExpidusSettingsManagerDialog *dialog)
{
    GtkStyleContext *context;
    GdkRGBA          color;
    gchar           *css_string;
    gchar           *color_text;
    GdkScreen       *screen;

    context = gtk_widget_get_style_context (GTK_WIDGET (dialog));
    /* Reset the provider to make sure we drop the previous Gtk theme style */
    gtk_style_context_remove_provider (context,
                                       GTK_STYLE_PROVIDER (dialog->css_provider));
    /* Get the foreground color for the underline */
    gtk_style_context_get_color (context, GTK_STATE_FLAG_NORMAL, &color);
    color_text = gdk_rgba_to_string (&color);
    /* Set a fake underline with box-shadow and use gtk to highlight the icon of the cell renderer */
    css_string = g_strdup_printf (".endoiconview.view *:hover { -gtk-icon-effect: highlight; box-shadow: inset 0 -1px 1px %s;"
                                  "                            border-left: 1px solid transparent; border-right: 1px solid transparent; }",
                                  color_text);
    gtk_css_provider_load_from_data (dialog->css_provider, css_string, -1, NULL);
    screen = gdk_screen_get_default ();
    /* As we don't have the individual EndoIconView widgets here, we set this provider for the whole screen.
       This is fairly unproblematic as nobody uses the CSS class exiconview. */
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (dialog->css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_free (css_string);
    g_free (color_text);
}



static void
expidus_settings_manager_dialog_response (GtkDialog *widget,
                                       gint       response_id)
{
    ExpidusSettingsManagerDialog *dialog = EXPIDUS_SETTINGS_MANAGER_DIALOG (widget);
    const gchar               *help_component;

    if (response_id == GTK_RESPONSE_NONE)
        return;

    if (response_id == GTK_RESPONSE_HELP)
    {
        if (dialog->help_component != NULL)
            help_component = dialog->help_component;
        else
            help_component = "expidus1-settings";

        expidus_dialog_show_help_with_version (GTK_WINDOW (widget),
                                            help_component,
                                            dialog->help_page,
                                            NULL,
                                            dialog->help_version);
    }
    else
    {
        GdkWindowState state;
        gint           width, height;

        /* Don't save the state for full-screen windows */
        state = gdk_window_get_state (gtk_widget_get_window(GTK_WIDGET(widget)));

        if ((state & (GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_FULLSCREEN)) == 0)
        {
            /* Save window size */
            gtk_window_get_size (GTK_WINDOW (widget), &width, &height);
            esconf_channel_set_int (dialog->channel, "/last/window-width", width),
            esconf_channel_set_int (dialog->channel, "/last/window-height", height);
        }

        g_object_unref (dialog->css_provider);
        gtk_widget_destroy (GTK_WIDGET (widget));
        gtk_main_quit ();
    }
}



static void
expidus_settings_manager_dialog_set_title (ExpidusSettingsManagerDialog *dialog,
                                        const gchar               *title,
                                        const gchar               *icon_name)
{
    g_return_if_fail (EXPIDUS_IS_SETTINGS_MANAGER_DIALOG (dialog));

    if (icon_name == NULL)
        icon_name = "com.expidus.settings.manager";
    if (title == NULL)
        title = _("Settings");

    gtk_window_set_title (GTK_WINDOW (dialog), title);
    gtk_window_set_icon_name (GTK_WINDOW (dialog), icon_name);
}



static gint
expidus_settings_manager_dialog_iconview_find (gconstpointer a,
                                            gconstpointer b)
{
    const DialogCategory *category = a;

    return category->iconview == b ? 0 : 1;
}



static gboolean
expidus_settings_manager_dialog_iconview_keynav_failed (EndoIconView               *current_view,
                                                     GtkDirectionType           direction,
                                                     ExpidusSettingsManagerDialog *dialog)
{
    GList          *li;
    GtkTreePath    *path;
    EndoIconView    *new_view;
    gboolean        result = FALSE;
    GtkTreeModel   *model;
    GtkTreeIter     iter;
    gint            col_old, col_new;
    gint            dist_prev, dist_new;
    GtkTreePath    *sel_path;
    DialogCategory *category;

    if (direction == GTK_DIR_UP || direction == GTK_DIR_DOWN)
    {
        /* find this category in the list */
        li = g_list_find_custom (dialog->categories, current_view,
            expidus_settings_manager_dialog_iconview_find);

        /* find the next of previous visible item */
        for (; li != NULL; )
        {
            if (direction == GTK_DIR_DOWN)
                li = g_list_next (li);
            else
                li = g_list_previous (li);

            if (li != NULL)
            {
                category = li->data;
                if (gtk_widget_get_visible (category->box))
                    break;
            }
        }

        /* leave there is no view above or below this one */
        if (li == NULL)
            return FALSE;

        category = li->data;
        new_view = ENDO_ICON_VIEW (category->iconview);

        if (endo_icon_view_get_cursor (current_view, &path, NULL))
        {
            col_old = endo_icon_view_get_item_column (current_view, path);
            gtk_tree_path_free (path);

            dist_prev = 1000;
            sel_path = NULL;

            model = endo_icon_view_get_model (new_view);
            if (gtk_tree_model_get_iter_first (model, &iter))
            {
                do
                {
                     path = gtk_tree_model_get_path (model, &iter);
                     col_new = endo_icon_view_get_item_column (new_view, path);
                     dist_new = ABS (col_new - col_old);

                     if ((direction == GTK_DIR_UP && dist_new <= dist_prev)
                         || (direction == GTK_DIR_DOWN  && dist_new < dist_prev))
                     {
                         if (sel_path != NULL)
                             gtk_tree_path_free (sel_path);

                         sel_path = path;
                         dist_prev = dist_new;
                     }
                     else
                     {
                         gtk_tree_path_free (path);
                     }
                }
                while (gtk_tree_model_iter_next (model, &iter));
            }

            if (G_LIKELY (sel_path != NULL))
            {
                /* move cursor, grab-focus will handle the selection */
                endo_icon_view_set_cursor (new_view, sel_path, NULL, FALSE);
                expidus_settings_manager_dialog_scroll_to_item (GTK_WIDGET (new_view), dialog);
                gtk_tree_path_free (sel_path);

                gtk_widget_grab_focus (GTK_WIDGET (new_view));

                result = TRUE;
            }
        }
    }

    return result;
}



static gboolean
expidus_settings_manager_dialog_query_tooltip (GtkWidget                 *iconview,
                                            gint                       x,
                                            gint                       y,
                                            gboolean                   keyboard_mode,
                                            GtkTooltip                *tooltip,
                                            ExpidusSettingsManagerDialog *dialog)
{
    GtkTreePath    *path;
    GValue          value = { 0, };
    GtkTreeModel   *model;
    GtkTreeIter     iter;
    MarkonMenuItem *item;
    const gchar    *comment;

    if (keyboard_mode)
    {
        if (!endo_icon_view_get_cursor (ENDO_ICON_VIEW (iconview), &path, NULL))
            return FALSE;
    }
    else
    {
        path = endo_icon_view_get_path_at_pos (ENDO_ICON_VIEW (iconview), x, y);
        if (G_UNLIKELY (path == NULL))
            return FALSE;
    }

    model = endo_icon_view_get_model (ENDO_ICON_VIEW (iconview));
    if (gtk_tree_model_get_iter (model, &iter, path))
    {
        gtk_tree_model_get_value (model, &iter, COLUMN_MENU_ITEM, &value);
        item = g_value_get_object (&value);
        g_assert (MARKON_IS_MENU_ITEM (item));

        comment = markon_menu_item_get_comment (item);
        if (!endo_str_is_empty (comment))
            gtk_tooltip_set_text (tooltip, comment);

        g_value_unset (&value);
    }

    gtk_tree_path_free (path);

    return TRUE;
}



static gboolean
expidus_settings_manager_dialog_iconview_focus (GtkWidget                 *iconview,
                                             GdkEventFocus             *event,
                                             ExpidusSettingsManagerDialog *dialog)
{
    GtkTreePath *path;

    if (event->in)
    {
        /* a mouse click will have focus, tab events not */
        if (!endo_icon_view_get_cursor (ENDO_ICON_VIEW (iconview), &path, NULL))
        {
           path = gtk_tree_path_new_from_indices (0, -1);
           endo_icon_view_set_cursor (ENDO_ICON_VIEW (iconview), path, NULL, FALSE);
           expidus_settings_manager_dialog_scroll_to_item (iconview, dialog);
        }

        endo_icon_view_select_path (ENDO_ICON_VIEW (iconview), path);
        gtk_tree_path_free (path);
    }
    else
    {
        endo_icon_view_unselect_all (ENDO_ICON_VIEW (iconview));
    }

    return FALSE;
}



static void
expidus_settings_manager_dialog_remove_socket (ExpidusSettingsManagerDialog *dialog)
{
    GtkWidget *socket;

    socket = gtk_bin_get_child (GTK_BIN (dialog->socket_viewport));
    if (G_UNLIKELY (socket != NULL))
        gtk_container_remove (GTK_CONTAINER (dialog->socket_viewport), socket);

    if (dialog->socket_item != NULL)
    {
        g_object_unref (G_OBJECT (dialog->socket_item));
        dialog->socket_item = NULL;
    }
}



static void
expidus_settings_manager_dialog_go_back (ExpidusSettingsManagerDialog *dialog)
{
    /* make sure no cursor is shown */
    gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET(dialog)), NULL);

    /* reset dialog info */
    expidus_settings_manager_dialog_set_title (dialog, NULL, NULL);

    gtk_widget_show (dialog->category_scroll);
    gtk_widget_hide (dialog->socket_scroll);

    g_free (dialog->help_page);
    dialog->help_page = NULL;
    g_free (dialog->help_component);
    dialog->help_component = NULL;
    g_free (dialog->help_version);
    dialog->help_version = NULL;

    gtk_widget_set_sensitive (dialog->button_back, FALSE);
    gtk_widget_set_sensitive (dialog->button_help, TRUE);

    gtk_widget_show (dialog->filter_button);
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dialog->filter_button)))
        gtk_widget_show (dialog->revealer);
    gtk_entry_set_text (GTK_ENTRY (dialog->filter_entry), "");
    gtk_widget_grab_focus (dialog->filter_entry);

    expidus_settings_manager_dialog_remove_socket (dialog);
}



static void
expidus_settings_manager_dialog_entry_changed (GtkWidget                 *entry,
                                            ExpidusSettingsManagerDialog *dialog)
{
    const gchar    *text;
    gchar          *normalized;
    gchar          *filter_text;
    GList          *li;
    GtkTreeModel   *model;
    gint            n_children;
    DialogCategory *category;

    text = gtk_entry_get_text (GTK_ENTRY (entry));
    if (text == NULL || *text == '\0')
    {
        filter_text = NULL;
    }
    else
    {
        /* create independent search string */
        normalized = g_utf8_normalize (text, -1, G_NORMALIZE_DEFAULT);
        filter_text = g_utf8_casefold (normalized, -1);
        g_free (normalized);
    }

    /* check if we need to update */
    if (g_strcmp0 (dialog->filter_text, filter_text) != 0)
    {
        /* set new filter */
        g_free (dialog->filter_text);
        dialog->filter_text = filter_text;

        /* update the category models */
        for (li = dialog->categories; li != NULL; li = li->next)
        {
            category = li->data;

            /* update model filters */
            model = endo_icon_view_get_model (ENDO_ICON_VIEW (category->iconview));
            gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (model));

            /* set visibility of the category */
            n_children = gtk_tree_model_iter_n_children (model, NULL);
            gtk_widget_set_visible (category->box, n_children > 0);
        }

        g_idle_add (expidus_settings_manager_queue_resize_cb, dialog);
    }
    else
    {
        g_free (dialog->filter_text);
        dialog->filter_text = NULL;
        g_free (filter_text);
    }
}



static void
expidus_settings_manager_show_filter_toggled (GtkToggleButton *button,
                                           gpointer         user_data)
{
    ExpidusSettingsManagerDialog *dialog = EXPIDUS_SETTINGS_MANAGER_DIALOG (user_data);
    gboolean state;

    state = gtk_toggle_button_get_active (button);
    gtk_widget_show (dialog->revealer);
    gtk_revealer_set_reveal_child (GTK_REVEALER (dialog->revealer), state);
    if (state && GTK_IS_WIDGET (dialog->filter_entry))
        gtk_widget_grab_focus (dialog->filter_entry);
}



static gboolean
expidus_settings_manager_dialog_entry_key_press (GtkWidget                 *entry,
                                              GdkEventKey               *event,
                                              ExpidusSettingsManagerDialog *dialog)
{
    GList          *li;
    DialogCategory *category;
    GtkTreePath    *path;
    gint            n_visible_items;
    GtkTreeModel   *model;
    const gchar    *text;

    if (event->keyval == GDK_KEY_Escape)
    {
        text = gtk_entry_get_text (GTK_ENTRY (entry));
        if (text != NULL && *text != '\0')
        {
            gtk_entry_set_text (GTK_ENTRY (entry), "");
            return TRUE;
        }
    }
    else if (event->keyval == GDK_KEY_Return)
    {
        /* count visible children */
        n_visible_items = 0;
        for (li = dialog->categories; li != NULL; li = li->next)
        {
            category = li->data;
            if (gtk_widget_get_visible (category->box))
            {
                model = endo_icon_view_get_model (ENDO_ICON_VIEW (category->iconview));
                n_visible_items += gtk_tree_model_iter_n_children (model, NULL);

                /* stop searching if there are more then 1 items */
                if (n_visible_items > 1)
                    break;
            }
        }

        for (li = dialog->categories; li != NULL; li = li->next)
        {
            category = li->data;

            /* find the first visible category */
            if (!gtk_widget_get_visible (category->box))
                continue;

            path = gtk_tree_path_new_first ();
            if (n_visible_items == 1)
            {
                /* activate this one item */
                endo_icon_view_item_activated (ENDO_ICON_VIEW (category->iconview), path);
            }
            else
            {
                /* select first item in view */
                endo_icon_view_set_cursor (ENDO_ICON_VIEW (category->iconview),
                                          path, NULL, FALSE);
                gtk_widget_grab_focus (category->iconview);
            }
            gtk_tree_path_free (path);
            break;
        }

        return TRUE;
    }

    return FALSE;
}



static void
expidus_settings_manager_dialog_plug_added (GtkWidget                 *socket,
                                         ExpidusSettingsManagerDialog *dialog)
{
    /* set dialog information from desktop file */
    expidus_settings_manager_dialog_set_title (dialog,
        markon_menu_item_get_name (dialog->socket_item),
        markon_menu_item_get_icon_name (dialog->socket_item));

    /* show socket and hide the categories view */
    gtk_widget_show (dialog->socket_scroll);
    gtk_widget_hide (dialog->category_scroll);

    /* button sensitivity */
    gtk_widget_set_sensitive (dialog->button_back, TRUE);
    gtk_widget_set_sensitive (dialog->button_help, dialog->help_page != NULL);
    gtk_widget_hide (dialog->filter_button);
    gtk_widget_hide (dialog->revealer);

    /* plug startup complete */
    gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET(dialog)), NULL);
}



static void
expidus_settings_manager_dialog_plug_removed (GtkWidget                 *socket,
                                           ExpidusSettingsManagerDialog *dialog)
{
    /* this shouldn't happen */
    g_critical ("pluggable dialog \"%s\" crashed",
                markon_menu_item_get_command (dialog->socket_item));

    /* restore dialog */
    expidus_settings_manager_dialog_go_back (dialog);
}



static void
expidus_settings_manager_dialog_spawn (ExpidusSettingsManagerDialog *dialog,
                                    MarkonMenuItem            *item)
{
    gchar          *command;
    gboolean        snotify;
    GdkScreen      *screen;
    GdkDisplay     *display;
    GError         *error = NULL;
    GFile          *desktop_file;
    gchar          *filename;
    ExpidusRc         *rc;
    gboolean        pluggable = FALSE;
    gchar          *cmd;
    gchar          *uri;
    GtkWidget      *socket;
    GdkCursor      *cursor;

    g_return_if_fail (MARKON_IS_MENU_ITEM (item));

    screen = gtk_window_get_screen (GTK_WINDOW (dialog));

    /* expand the field codes */
    uri = markon_menu_item_get_uri (item);
    command = expidus_expand_desktop_entry_field_codes (markon_menu_item_get_command (item),
                                                     NULL,
                                                     markon_menu_item_get_icon_name (item),
                                                     markon_menu_item_get_name (item),
                                                     uri, FALSE);
    g_free (uri);

    /* we need to read some more info from the desktop
     *  file that is not supported by markon */
    desktop_file = markon_menu_item_get_file (item);
    filename = g_file_get_path (desktop_file);
    g_object_unref (desktop_file);

    rc = expidus_rc_simple_open (filename, TRUE);
    g_free (filename);
    if (G_LIKELY (rc != NULL))
    {
        pluggable = expidus_rc_read_bool_entry (rc, "X-ExpidusPluggable", FALSE);
        if (pluggable)
        {
            dialog->help_page = g_strdup (expidus_rc_read_entry (rc, "X-ExpidusHelpPage", NULL));
            dialog->help_component = g_strdup (expidus_rc_read_entry (rc, "X-ExpidusHelpComponent", NULL));
            dialog->help_version = g_strdup (expidus_rc_read_entry (rc, "X-ExpidusHelpVersion", NULL));
        }

        expidus_rc_close (rc);
    }

    if (pluggable)
    {
        /* fake startup notification */
        display = gdk_display_get_default ();
        cursor = gdk_cursor_new_for_display (display, GDK_WATCH);
        gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET(dialog)), cursor);
        g_object_unref (cursor);

        expidus_settings_manager_dialog_remove_socket (dialog);

        /* create fresh socket */
        socket = gtk_socket_new ();
        gtk_container_add (GTK_CONTAINER (dialog->socket_viewport), socket);
        g_signal_connect (G_OBJECT (socket), "plug-added",
            G_CALLBACK (expidus_settings_manager_dialog_plug_added), dialog);
        g_signal_connect (G_OBJECT (socket), "plug-removed",
            G_CALLBACK (expidus_settings_manager_dialog_plug_removed), dialog);
        gtk_widget_show (socket);

        /* for info when the plug is attached */
        dialog->socket_item = g_object_ref (item);

        /* spawn dialog with socket argument */
        cmd = g_strdup_printf ("%s --socket-id=%d", command, (gint)gtk_socket_get_id (GTK_SOCKET (socket)));
        if (!expidus_spawn_command_line_on_screen (screen, cmd, FALSE, FALSE, &error))
        {
            gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET(dialog)), NULL);

            expidus_dialog_show_error (GTK_WINDOW (dialog), error,
                                    _("Unable to start \"%s\""), command);
            g_error_free (error);
        }
        g_free (cmd);
    }
    else
    {
        snotify = markon_menu_item_supports_startup_notification (item);
        if (!expidus_spawn_command_line_on_screen (screen, command, FALSE, snotify, &error))
        {
            expidus_dialog_show_error (GTK_WINDOW (dialog), error,
                                    _("Unable to start \"%s\""), command);
            g_error_free (error);
        }
    }

  g_free (command);
}



static void
expidus_settings_manager_dialog_item_activated (EndoIconView               *iconview,
                                             GtkTreePath               *path,
                                             ExpidusSettingsManagerDialog *dialog)
{
    GtkTreeModel   *model;
    GtkTreeIter     iter;
    MarkonMenuItem *item;

    model = endo_icon_view_get_model (iconview);
    if (gtk_tree_model_get_iter (model, &iter, path))
    {
        gtk_tree_model_get (model, &iter, COLUMN_MENU_ITEM, &item, -1);
        g_assert (MARKON_IS_MENU_ITEM (item));

        expidus_settings_manager_dialog_spawn (dialog, item);

        g_object_unref (G_OBJECT (item));
    }
}



static gboolean
expidus_settings_manager_dialog_filter_category (GtkTreeModel *model,
                                              GtkTreeIter  *iter,
                                              gpointer      data)
{
    GValue          cat_val = { 0, };
    GValue          filter_val = { 0, };
    gboolean        visible;
    DialogCategory *category = data;
    const gchar    *filter_text;

    /* filter only the active category */
    gtk_tree_model_get_value (model, iter, COLUMN_MENU_DIRECTORY, &cat_val);
    visible = g_value_get_object (&cat_val) == G_OBJECT (category->directory);
    g_value_unset (&cat_val);

    /* filter search string */
    if (visible && category->dialog->filter_text != NULL)
    {
        gtk_tree_model_get_value (model, iter, COLUMN_FILTER_TEXT, &filter_val);
        filter_text = g_value_get_string (&filter_val);
        visible = strstr (filter_text, category->dialog->filter_text) != NULL;
        g_value_unset (&filter_val);
    }

    return visible;
}



static void
expidus_settings_manager_dialog_scroll_to_item (GtkWidget                 *iconview,
                                             ExpidusSettingsManagerDialog *dialog)
{
    GtkAllocation *alloc = g_new0 (GtkAllocation, 1);
    GtkTreePath   *path;
    gint           row, row_height;
    gdouble        rows;
    GtkAdjustment *adjustment;
    gdouble        lower, upper;

    if (endo_icon_view_get_cursor (ENDO_ICON_VIEW (iconview), &path, NULL))
    {
        /* get item row */
        row = endo_icon_view_get_item_row (ENDO_ICON_VIEW (iconview), path);
        gtk_tree_path_free (path);

        /* estimated row height */
        gtk_widget_get_allocation(GTK_WIDGET(iconview), alloc);
        rows = alloc->height / 56;
        row_height = alloc->height / MAX (1.0, (gint) rows);

        /* selected item boundries */
        lower = alloc->y + row_height * row;
        upper = alloc->y + row_height * (row + 1);

        /* scroll so item is visible */
        adjustment = gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (dialog->category_viewport));
        gtk_adjustment_clamp_page (adjustment, lower, upper);
    }

    g_free (alloc);
}



static gboolean
expidus_settings_manager_dialog_key_press_event (GtkWidget                 *iconview,
                                              GdkEventKey               *event,
                                              ExpidusSettingsManagerDialog *dialog)
{
    gboolean result;

    /* let endo handle the selection first */
    result = GTK_WIDGET_CLASS (G_OBJECT_GET_CLASS (iconview))->key_press_event (iconview, event);

    /* make sure the selected item is visible */
    if (result)
        expidus_settings_manager_dialog_scroll_to_item (iconview, dialog);

    return result;
}



static gboolean
expidus_settings_manager_start_search (GtkWidget                 *iconview,
                                    ExpidusSettingsManagerDialog *dialog)
{
    gtk_widget_grab_focus (dialog->filter_entry);
    return TRUE;
}



static void
expidus_settings_manager_dialog_category_free (gpointer data)
{
    DialogCategory            *category = data;
    ExpidusSettingsManagerDialog *dialog = category->dialog;

    dialog->categories = g_list_remove (dialog->categories, category);

    g_object_unref (G_OBJECT (category->directory));
    g_slice_free (DialogCategory, category);
}



static void
expidus_settings_manager_dialog_add_category (ExpidusSettingsManagerDialog *dialog,
                                           MarkonMenuDirectory       *directory)
{
    GtkTreeModel    *filter;
    GtkWidget       *iconview;
    GtkWidget       *label;
    GtkWidget       *separator;
    GtkWidget       *vbox;
    PangoAttrList   *attrs;
    GtkCellRenderer *render;
    DialogCategory  *category;

    category = g_slice_new0 (DialogCategory);
    category->directory = (MarkonMenuDirectory *) g_object_ref (G_OBJECT (directory));
    category->dialog = dialog;

    /* filter category from main store */
    filter = gtk_tree_model_filter_new (GTK_TREE_MODEL (dialog->store), NULL);
    gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (filter),
        expidus_settings_manager_dialog_filter_category,
        category, expidus_settings_manager_dialog_category_free);

    category->box = vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start (GTK_BOX (dialog->category_box), vbox, FALSE, TRUE, 0);
    gtk_widget_show (vbox);

    /* create a label for the category title */
    label = gtk_label_new (markon_menu_directory_get_name (directory));
    attrs = pango_attr_list_new ();
    pango_attr_list_insert (attrs, pango_attr_weight_new (PANGO_WEIGHT_BOLD));
    gtk_label_set_attributes (GTK_LABEL (label), attrs);
    pango_attr_list_unref (attrs);
    gtk_widget_set_halign (label, GTK_ALIGN_START);
    gtk_widget_set_valign (label, GTK_ALIGN_CENTER);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 0);
    gtk_widget_show (label);

    /* separate title and content using a horizontal line */
    separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);
    gtk_widget_show (separator);

    category->iconview = iconview = endo_icon_view_new_with_model (GTK_TREE_MODEL (filter));
    gtk_container_add (GTK_CONTAINER (vbox), iconview);
    endo_icon_view_set_orientation (ENDO_ICON_VIEW (iconview), GTK_ORIENTATION_HORIZONTAL);
    endo_icon_view_set_margin (ENDO_ICON_VIEW (iconview), 0);
    endo_icon_view_set_single_click (ENDO_ICON_VIEW (iconview), TRUE);
    endo_icon_view_set_enable_search (ENDO_ICON_VIEW (iconview), FALSE);
    endo_icon_view_set_item_width (ENDO_ICON_VIEW (iconview), TEXT_WIDTH + ICON_WIDTH);
    expidus_settings_manager_dialog_set_hover_style (dialog);
    gtk_widget_show (iconview);

    /* list used for unselecting */
    dialog->categories = g_list_append (dialog->categories, category);

    gtk_widget_set_has_tooltip (iconview, TRUE);
    g_signal_connect (G_OBJECT (iconview), "query-tooltip",
        G_CALLBACK (expidus_settings_manager_dialog_query_tooltip), dialog);
    g_signal_connect (G_OBJECT (iconview), "focus-in-event",
        G_CALLBACK (expidus_settings_manager_dialog_iconview_focus), dialog);
    g_signal_connect (G_OBJECT (iconview), "focus-out-event",
        G_CALLBACK (expidus_settings_manager_dialog_iconview_focus), dialog);
    g_signal_connect (G_OBJECT (iconview), "keynav-failed",
        G_CALLBACK (expidus_settings_manager_dialog_iconview_keynav_failed), dialog);
    g_signal_connect (G_OBJECT (iconview), "item-activated",
        G_CALLBACK (expidus_settings_manager_dialog_item_activated), dialog);
    g_signal_connect (G_OBJECT (iconview), "key-press-event",
        G_CALLBACK (expidus_settings_manager_dialog_key_press_event), dialog);
    g_signal_connect (G_OBJECT (iconview), "start-interactive-search",
        G_CALLBACK (expidus_settings_manager_start_search), dialog);

    render = gtk_cell_renderer_pixbuf_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (iconview), render, FALSE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (iconview), render, "icon-name", COLUMN_ICON_NAME);
    g_object_set (G_OBJECT (render),
                  "stock-size", GTK_ICON_SIZE_DIALOG,
                  "follow-state", TRUE,
                  NULL);

    render = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_end (GTK_CELL_LAYOUT (iconview), render, FALSE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (iconview), render, "text", COLUMN_NAME);
    g_object_set (G_OBJECT (render),
                  "wrap-mode", PANGO_WRAP_WORD,
                  "wrap-width", TEXT_WIDTH,
                  NULL);

    g_object_unref (G_OBJECT (filter));
}



static void
expidus_settings_manager_dialog_menu_collect (MarkonMenu  *menu,
                                           GList      **items)
{
    GList *elements, *li;

    g_return_if_fail (MARKON_IS_MENU (menu));

    elements = markon_menu_get_elements (menu);

    for (li = elements; li != NULL; li = li->next)
    {
        if (MARKON_IS_MENU_ITEM (li->data))
        {
            /* only add visible items */
            if (markon_menu_element_get_visible (li->data))
                *items = g_list_prepend (*items, li->data);
        }
        else if (MARKON_IS_MENU (li->data))
        {
            /* we collect only 1 level deep in a category, so
             * add the submenu items too (should never happen tho) */
            expidus_settings_manager_dialog_menu_collect (li->data, items);
        }
    }

    g_list_free (elements);
}



static gint
expidus_settings_manager_dialog_menu_sort (gconstpointer a,
                                        gconstpointer b)
{
    return g_utf8_collate (markon_menu_item_get_name (MARKON_MENU_ITEM (a)),
                           markon_menu_item_get_name (MARKON_MENU_ITEM (b)));
}



static void
expidus_settings_manager_dialog_menu_reload (ExpidusSettingsManagerDialog *dialog)
{
    GError              *error = NULL;
    GList               *elements, *li;
    GList               *lnext;
    MarkonMenuDirectory *directory;
    GList               *items, *lp;
    GList               *keywords, *kli;
    gint                 i = 0;
    gchar               *item_text;
    gchar               *normalized;
    gchar               *filter_text;
    GString             *item_keywords;
    DialogCategory      *category;

    g_return_if_fail (EXPIDUS_IS_SETTINGS_MANAGER_DIALOG (dialog));
    g_return_if_fail (MARKON_IS_MENU (dialog->menu));

    if (dialog->categories != NULL)
    {
        for (li = dialog->categories; li != NULL; li = lnext)
        {
            lnext = li->next;
            category = li->data;

            gtk_widget_destroy (category->box);
        }

        g_assert (dialog->categories == NULL);

        gtk_list_store_clear (GTK_LIST_STORE (dialog->store));
    }

    if (markon_menu_load (dialog->menu, NULL, &error))
    {
        /* get all menu elements (preserve layout) */
        elements = markon_menu_get_elements (dialog->menu);
        for (li = elements; li != NULL; li = li->next)
        {
            /* only accept toplevel menus */
            if (!MARKON_IS_MENU (li->data))
                continue;

            directory = markon_menu_get_directory (li->data);
            if (G_UNLIKELY (directory == NULL))
                continue;

            items = NULL;

            expidus_settings_manager_dialog_menu_collect (li->data, &items);

            /* add the new category if it has visible items */
            if (G_LIKELY (items != NULL))
            {
                /* insert new items in main store */
                items = g_list_sort (items, expidus_settings_manager_dialog_menu_sort);
                for (lp = items; lp != NULL; lp = lp->next)
                {
                    /* create independent search string based on name, comment and keywords */
                    keywords = markon_menu_item_get_keywords (lp->data);
                    item_keywords = g_string_new (NULL);
                    for (kli = keywords; kli != NULL; kli = kli->next)
                    {
                        g_string_append (item_keywords, kli->data);
                    }
                    item_text = g_strdup_printf ("%s\n%s\n%s",
                        markon_menu_item_get_name (lp->data),
                        markon_menu_item_get_comment (lp->data),
                        item_keywords->str);
                    g_string_free (item_keywords, TRUE);
                    g_list_free (kli);
                    normalized = g_utf8_normalize (item_text, -1, G_NORMALIZE_DEFAULT);
                    g_free (item_text);
                    filter_text = g_utf8_casefold (normalized, -1);
                    g_free (normalized);

                    gtk_list_store_insert_with_values (dialog->store, NULL, i++,
                        COLUMN_NAME, markon_menu_item_get_name (lp->data),
                        COLUMN_ICON_NAME, markon_menu_item_get_icon_name (lp->data),
                        COLUMN_TOOLTIP, markon_menu_item_get_comment (lp->data),
                        COLUMN_MENU_ITEM, lp->data,
                        COLUMN_MENU_DIRECTORY, directory,
                        COLUMN_FILTER_TEXT, filter_text, -1);

                    g_free (filter_text);
                }
                g_list_free (items);

                /* add the new category to the box */
                expidus_settings_manager_dialog_add_category (dialog, directory);
            }
        }

        g_list_free (elements);
    }
    else
    {
        g_critical ("Failed to load menu: %s", error->message);
        g_error_free (error);
    }

    g_idle_add (expidus_settings_manager_queue_resize_cb, dialog);
}


GtkWidget *
expidus_settings_manager_dialog_new (void)
{
    return g_object_new (EXPIDUS_TYPE_SETTINGS_MANAGER_DIALOG, NULL);
}



gboolean
expidus_settings_manager_dialog_show_dialog (ExpidusSettingsManagerDialog *dialog,
                                          const gchar               *dialog_name)
{
    GtkTreeModel   *model = GTK_TREE_MODEL (dialog->store);
    GtkTreeIter     iter;
    MarkonMenuItem *item;
    const gchar    *desktop_id;
    gchar          *name;
    gboolean        found = FALSE;

    g_return_val_if_fail (EXPIDUS_IS_SETTINGS_MANAGER_DIALOG (dialog), FALSE);

    name = g_strdup_printf ("%s.desktop", dialog_name);

    if (gtk_tree_model_get_iter_first (model, &iter))
    {
        do
        {
             gtk_tree_model_get (model, &iter, COLUMN_MENU_ITEM, &item, -1);
             g_assert (MARKON_IS_MENU_ITEM (item));

             desktop_id = markon_menu_item_get_desktop_id (item);
             if (g_strcmp0 (desktop_id, name) == 0)
             {
                  expidus_settings_manager_dialog_spawn (dialog, item);
                  found = TRUE;
             }

             g_object_unref (G_OBJECT (item));
        }
        while (!found && gtk_tree_model_iter_next (model, &iter));
    }

    g_free (name);

    return found;
}
