/*-
 * Copyright (c) 2003-2006 Benedikt Meurer <benny@expidus.org>.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_GIO_UNIX
#include <gio/gdesktopappinfo.h>
#endif

#include "expidus-mime-helper.h"
#include "expidus-mime-helper-utils.h"



static void       expidus_mime_helper_finalize   (GObject        *object);
static ExpidusMimeHelper *expidus_mime_helper_new        (const gchar    *id,
                                         ExpidusRc         *rc);
static void       clear_bad_entries     (ExpidusRc         *rc);



struct _ExpidusMimeHelperClass
{
  GObjectClass __parent__;
};

struct _ExpidusMimeHelper
{
  GObject __parent__;

  guint             startup_notify : 1;

  gchar            *id;
  gchar            *icon;
  gchar            *name;
  gchar           **commands;
  gchar           **commands_with_parameter;
  gchar           **commands_with_flag;
  ExpidusMimeHelperCategory category;
};



G_DEFINE_TYPE (ExpidusMimeHelper, expidus_mime_helper, G_TYPE_OBJECT)



static void
expidus_mime_helper_class_init (ExpidusMimeHelperClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = expidus_mime_helper_finalize;
}



static void
expidus_mime_helper_init (ExpidusMimeHelper *helpers)
{
}



static void
expidus_mime_helper_finalize (GObject *object)
{
  ExpidusMimeHelper *helper = EXPIDUS_MIME_HELPER (object);

  g_strfreev (helper->commands_with_flag);
  g_strfreev (helper->commands_with_parameter);
  g_strfreev (helper->commands);
  g_free (helper->name);
  g_free (helper->icon);
  g_free (helper->id);

  (*G_OBJECT_CLASS (expidus_mime_helper_parent_class)->finalize) (object);
}



static gchar**
substitute_binary (const gchar *commands,
                   const gchar *binary)
{
  gchar **result;
  gchar **s, **t;
  gchar  *tmp;

  /* split the commands */
  result = g_strsplit (commands, ";", -1);

  /* (pre-)process the result */
  for (s = t = result; *s != NULL; ++s)
    {
      if (**s == '\0')
        {
          g_free (*s);
        }
      else if (binary != NULL)
        {
          tmp = endo_str_replace (*s, "%B", binary);
          g_free (*s);
          *t++ = tmp;
        }
      else
        {
          *t++ = *s;
        }
    }
  *t = NULL;

  return result;
}



static ExpidusMimeHelper*
expidus_mime_helper_new (const gchar *id,
                ExpidusRc      *rc)
{
  const gchar *commands_with_flag;
  const gchar *commands_with_parameter;
  const gchar *commands;
  const gchar *str;
  ExpidusMimeHelper   *helper;
  gchar      **binaries;
  gchar       *binary = NULL;
  guint        n;

  g_return_val_if_fail (id != NULL, NULL);
  g_return_val_if_fail (rc != NULL, NULL);

  expidus_rc_set_group (rc, "Desktop Entry");

  /* allocate a new helper */
  helper = g_object_new (EXPIDUS_MIME_TYPE_HELPER, NULL);
  helper->id = g_strdup (id);
  helper->startup_notify = expidus_rc_read_bool_entry (rc, "StartupNotify", FALSE);

  /* verify the type of the desktop file */
  str = expidus_rc_read_entry_untranslated (rc, "Type", NULL);
  if (G_UNLIKELY (!endo_str_is_equal (str, "X-EXPIDUS-Helper")))
    goto failed;

  /* determine the category of the helper */
  str = expidus_rc_read_entry_untranslated (rc, "X-EXPIDUS-Category", NULL);
  if (!expidus_mime_helper_category_from_string (str, &helper->category))
    goto failed;

  /* determine the name of the helper */
  str = expidus_rc_read_entry (rc, "Name", NULL);
  if (G_UNLIKELY (endo_str_is_empty (str)))
    goto failed;
  helper->name = g_strdup (str);

  /* determine the icon of the helper */
  str = expidus_rc_read_entry_untranslated (rc, "Icon", NULL);
  if (G_LIKELY (!endo_str_is_empty (str)))
    helper->icon = g_strdup (str);

  /* determine the commands */
  commands = expidus_rc_read_entry_untranslated (rc, "X-EXPIDUS-Commands", NULL);
  if (G_UNLIKELY (commands == NULL))
    goto failed;

  commands_with_flag = endo_str_replace (commands, ";", " %s;");

  /* determine the commands (with parameter) */
  commands_with_parameter = expidus_rc_read_entry_untranslated (rc, "X-EXPIDUS-CommandsWithParameter", NULL);
  if (G_UNLIKELY (commands_with_parameter == NULL))
    goto failed;

  /* check if we need binaries for substitution */
  if (strstr (commands, "%B") != NULL || strstr (commands_with_parameter, "%B") != NULL)
    {
      /* determine the binaries */
      str = expidus_rc_read_entry_untranslated (rc, "X-EXPIDUS-Binaries", NULL);
      if (G_UNLIKELY (str == NULL))
        goto failed;

      /* determine the first available binary */
      binaries = g_strsplit (str, ";", -1);
      for (binary = NULL, n = 0; binaries[n] != NULL && binary == NULL; ++n)
        if (G_LIKELY (binaries[n][0] != '\0'))
          binary = g_find_program_in_path (binaries[n]);
      g_strfreev (binaries);

      /* check if we found a binary */
      if (G_UNLIKELY (binary == NULL))
        goto failed;
    }

  /* substitute the binary (if any) */
  helper->commands = substitute_binary (commands, binary);
  helper->commands_with_flag = substitute_binary (commands_with_flag, binary);
  helper->commands_with_parameter = substitute_binary (commands_with_parameter, binary);
  g_free (binary);

  /* verify that we have atleast one command */
  if (G_UNLIKELY (*helper->commands == NULL || *helper->commands_with_parameter == NULL))
    goto failed;

  return helper;

failed:
  g_object_unref (G_OBJECT (helper));
  return NULL;
}



/**
 * expidus_mime_helper_get_category:
 * @helper : a #ExpidusMimeHelper.
 *
 * Returns the #ExpidusMimeHelperCategory of @helper.
 *
 * Return value: the #ExpidusMimeHelperCategory of @helper.
 **/
ExpidusMimeHelperCategory
expidus_mime_helper_get_category (const ExpidusMimeHelper *helper)
{
  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER (helper), EXPIDUS_MIME_HELPER_WEBBROWSER);
  return helper->category;
}



/**
 * expidus_mime_helper_get_id:
 * @helper : a #ExpidusMimeHelper.
 *
 * Returns the unique id (the .desktop file basename) of @helper.
 *
 * Return value: the unique id of @helper.
 **/
const gchar*
expidus_mime_helper_get_id (const ExpidusMimeHelper *helper)
{
  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER (helper), NULL);
  return helper->id;
}



/**
 * expidus_mime_helper_get_name:
 * @helper : a #ExpidusMimeHelper.
 *
 * Returns the (translated) name of the @helper.
 *
 * Return value: the name of @helper.
 **/
const gchar*
expidus_mime_helper_get_name (const ExpidusMimeHelper *helper)
{
  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER (helper), NULL);
  return helper->name;
}



/**
 * expidus_mime_helper_get_icon:
 * @helper : a #ExpidusMimeHelper.
 *
 * Return the name of the themed icon for @helper or
 * the absolute path to an icon file, or %NULL if no
 * icon is available for @helper.
 *
 * Return value: the icon for @helper or %NULL.
 **/
const gchar*
expidus_mime_helper_get_icon (const ExpidusMimeHelper *helper)
{
  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER (helper), NULL);
  return helper->icon;
}



/**
 * expidus_mime_helper_get_command:
 * @helper : a #ExpidusMimeHelper.
 *
 * Returns a reasonable command for @helper.
 *
 * Return value: a command for @helper.
 **/
const gchar*
expidus_mime_helper_get_command (const ExpidusMimeHelper *helper)
{
  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER (helper), NULL);
  return *helper->commands_with_parameter;
}

/* Set the DISPLAY variable, to be use by g_spawn_async. */
static void
set_environment (gchar *display)
{
  g_setenv ("DISPLAY", display, TRUE);
}

/**
 * expidus_mime_helper_execute:
 * @helper    : a #ExpidusMimeHelper.
 * @screen    : the #GdkScreen on which to execute @helper or %NULL to use default.
 * @parameter : the parameter to pass to @helper (e.g. URL for WebBrowser) or %NULL
 *              to just run @helper.
 * @error     : return location for errors or %NULL.
 *
 * Executes @helper on @screen with the given @parameter. Returns %TRUE if the
 * execution succeed, else %FALSE and @error will be set.
 *
 * Return value: %TRUE on success, %FALSE if @error is set.
 **/
gboolean
expidus_mime_helper_execute (ExpidusMimeHelper   *helper,
                    GdkScreen   *screen,
                    const gchar *parameter,
                    GError     **error)
{
  gint64        previous;
  gint64        current;
  GdkDisplay   *display;
  gboolean      succeed = FALSE;
  GError       *err = NULL;
  gchar       **commands;
  gchar       **argv;
  gchar        *command;
  gchar        *display_name;
  guint         n;
  gint          status;
  gint          result;
  gint          pid;
  const gchar  *real_parameter = parameter;

  // FIXME: startup-notification

  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER (helper), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
  g_return_val_if_fail (screen == NULL || GDK_IS_SCREEN (screen), FALSE);

  /* fallback to default screen */
  if (G_UNLIKELY (screen == NULL))
    screen = gdk_screen_get_default ();

  /* strip the mailto part if needed */
  if (real_parameter != NULL && g_str_has_prefix (real_parameter, "mailto:"))
    real_parameter = parameter + 7;

  /* determine the command set to use */
  if (endo_str_is_flag (real_parameter)) {
    commands = helper->commands_with_flag;
  } else if (endo_str_is_empty (real_parameter)) {
    commands = helper->commands;
  } else {
    commands = helper->commands_with_parameter;
  }

  /* verify that we have atleast one command */
  if (G_UNLIKELY (*commands == NULL))
    {
      g_set_error (error, G_SPAWN_ERROR, G_SPAWN_ERROR_INVAL, _("No command specified"));
      return FALSE;
    }

  /* try to run the helper using the various given commands */
  for (n = 0; commands[n] != NULL; ++n)
    {
      /* reset the error */
      g_clear_error (&err);

      /* parse the command */
      command = !endo_str_is_empty (real_parameter) ? endo_str_replace (commands[n], "%s", real_parameter) : g_strdup (commands[n]);
      succeed = g_shell_parse_argv (command, NULL, &argv, &err);
      g_free (command);

      /* check if the parsing failed */
      if (G_UNLIKELY (!succeed))
        continue;

      /* set the display variable */
      display = gdk_screen_get_display (screen);
      display_name = g_strdup (gdk_display_get_name (display));

      /* try to run the command */
      succeed = g_spawn_async (NULL,
        argv,
        NULL,
        G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH,
        (GSpawnChildSetupFunc) set_environment,
        display_name,
        &pid,
        &err);

      /* cleanup */
      g_strfreev (argv);
      g_free (display_name);

      /* check if the execution was successful */
      if (G_LIKELY (succeed))
        {
          /* determine the current time */
          previous = g_get_monotonic_time ();

          /* wait up to 5 seconds to see whether the command worked */
          for (;;)
            {
              /* check if the command exited with an error */
              result = waitpid (pid, &status, WNOHANG);
              if (result < 0)
                {
                  /* something weird happened */
                  err = g_error_new_literal (G_FILE_ERROR, g_file_error_from_errno (errno), g_strerror (errno));
                  succeed = FALSE;
                  break;
                }
              else if (result > 0 && status != 0)
                {
                  /* the command failed */
                  err = g_error_new_literal (G_FILE_ERROR, g_file_error_from_errno (EIO), g_strerror (EIO));
                  succeed = FALSE;
                  break;
                }
              else if (result == pid)
                {
                  /* the command succeed */
                  succeed = TRUE;
                  break;
                }

              /* determine the current time */
              current = g_get_monotonic_time ();

              /* check if the command is still running after 5 seconds (which indicates that the command worked) */
              if ((current - previous) / G_USEC_PER_SEC > 5)
                break;

              /* wait some time */
              g_usleep (50 * 1000);
            }

          /* check if we should retry with the next command */
          if (G_LIKELY (succeed))
            break;
        }
    }

  /* propagate the error */
  if (G_UNLIKELY (!succeed))
    g_propagate_error (error, err);

  return succeed;
}




static void       expidus_mime_helper_database_finalize    (GObject                *object);
static ExpidusMimeHelper *expidus_mime_helper_database_lookup      (ExpidusMimeHelperDatabase      *database,
                                                   ExpidusMimeHelperCategory       category,
                                                   const gchar            *id);



struct _ExpidusMimeHelperDatabaseClass
{
  GObjectClass __parent__;
};

struct _ExpidusMimeHelperDatabase
{
  GObject     __parent__;
  GHashTable *helpers;
};



G_DEFINE_TYPE (ExpidusMimeHelperDatabase, expidus_mime_helper_database, G_TYPE_OBJECT)



static void
expidus_mime_helper_database_class_init (ExpidusMimeHelperDatabaseClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = expidus_mime_helper_database_finalize;
}



static void
expidus_mime_helper_database_init (ExpidusMimeHelperDatabase *database)
{
  database->helpers = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
}



static void
expidus_mime_helper_database_finalize (GObject *object)
{
  ExpidusMimeHelperDatabase *database = EXPIDUS_MIME_HELPER_DATABASE (object);

  g_hash_table_destroy (database->helpers);

  (*G_OBJECT_CLASS (expidus_mime_helper_database_parent_class)->finalize) (object);
}



static ExpidusMimeHelper*
expidus_mime_helper_database_lookup (ExpidusMimeHelperDatabase *database,
                            ExpidusMimeHelperCategory  category,
                            const gchar       *id)
{
  ExpidusMimeHelper *helper;
  ExpidusRc    *rc;
  gchar     *file;
  gchar     *spec;

  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER_DATABASE (database), NULL);
  g_return_val_if_fail (id != NULL, NULL);

  /* generate the spec for the helper */
  spec = g_strconcat ("expidus1/helpers/", id, ".desktop", NULL);

  /* try to find a cached version */
  helper = g_hash_table_lookup (database->helpers, spec);

  /* load the helper from the file */
  if (G_LIKELY (helper == NULL))
    {
      expidus_resource_push_path (EXPIDUS_RESOURCE_DATA, DATADIR);
      file = expidus_resource_lookup (EXPIDUS_RESOURCE_DATA, spec);
      expidus_resource_pop_path (EXPIDUS_RESOURCE_DATA);

      if (G_LIKELY (file != NULL))
        {
          rc = expidus_rc_simple_open (file, TRUE);
          if (G_LIKELY (rc != NULL))
            {
              helper = expidus_mime_helper_new (id, rc);
              expidus_rc_close (rc);
            }
          g_free (file);
        }

      /* add the loaded helper to the cache */
      if (G_LIKELY (helper != NULL))
        {
          g_hash_table_insert (database->helpers, spec, helper);
          spec = NULL;
        }
    }

  if (G_LIKELY (helper != NULL))
    {
      if (expidus_mime_helper_get_category (helper) == category)
        g_object_ref (G_OBJECT (helper));
      else
        helper = NULL;
    }

  g_free (spec);

  return helper;
}



/**
 * expidus_mime_helper_database_get:
 *
 * Returns a reference on the default #ExpidusMimeHelperDatabase
 * instance. The caller is responsible to free the
 * returned object using g_object_unref() when no longer
 * needed.
 *
 * Return value: a reference to the default #ExpidusMimeHelperDatabase.
 **/
ExpidusMimeHelperDatabase*
expidus_mime_helper_database_get (void)
{
  static ExpidusMimeHelperDatabase *database = NULL;

  if (G_LIKELY (database == NULL))
    {
      database = g_object_new (EXPIDUS_MIME_TYPE_HELPER_DATABASE, NULL);
      g_object_add_weak_pointer (G_OBJECT (database), (gpointer) &database);
    }
  else
    {
      g_object_ref (G_OBJECT (database));
    }

  return database;
}



/**
 * expidus_mime_helper_database_get_default:
 * @database : a #ExpidusMimeHelperDatabase.
 * @category : a #ExpidusMimeHelperCategory.
 *
 * Returns a reference on the default #ExpidusMimeHelper for
 * the @category in @database or %NULL if no default
 * #ExpidusMimeHelper is registered for @category.
 *
 * The caller is responsible to free the returned
 * object using g_object_unref() when no longer needed.
 *
 * Return value: the default #ExpidusMimeHelper for @category
 *               or %NULL.
 **/
ExpidusMimeHelper*
expidus_mime_helper_database_get_default (ExpidusMimeHelperDatabase *database,
                                 ExpidusMimeHelperCategory  category)
{
  const gchar *id;
  ExpidusMimeHelper   *helper = NULL;
  ExpidusRc      *rc;
  gchar       *key;

  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER_DATABASE (database), NULL);
  g_return_val_if_fail (category < EXPIDUS_MIME_HELPER_N_CATEGORIES, NULL);

  rc = expidus_rc_config_open (EXPIDUS_RESOURCE_CONFIG, "expidus1/helpers.rc", TRUE);
  if (G_LIKELY (rc != NULL))
    {
      key = expidus_mime_helper_category_to_string (category);
      id = expidus_rc_read_entry_untranslated (rc, key, NULL);
      if (G_LIKELY (id != NULL))
        helper = expidus_mime_helper_database_lookup (database, category, id);

      /* handle migrating from Expidus 4.14 or older */
      if (helper == NULL && g_strcmp0 (id, "Thunar") == 0)
        helper = expidus_mime_helper_database_lookup (database, category, "lunar");

      expidus_rc_close (rc);
      g_free (key);
    }

  return helper;
}



static ExpidusRc*
mimeapps_open (gboolean readonly)
{
  ExpidusRc *rc;

  rc = expidus_rc_config_open (EXPIDUS_RESOURCE_CONFIG, "mimeapps.list", readonly);
  if (G_UNLIKELY (rc == NULL))
    {
      /* deprecated location (glib < 2.41) */
      rc = expidus_rc_config_open (EXPIDUS_RESOURCE_DATA, "applications/mimeapps.list", readonly);
    }

  return rc;
}



/**
 * expidus_mime_helper_database_set_default:
 * @database : a #ExpidusMimeHelperDatabase.
 * @category : a #ExpidusMimeHelperCategory.
 * @helper   : a #ExpidusMimeHelper.
 * @error    : return location for errors or %NULL.
 *
 * Sets the default #ExpidusMimeHelper for @category in @database to
 * @helper. Returns %TRUE on success, %FALSE if @error is set.
 *
 * Return value: %TRUE on success, %FALSE if @error is set.
 **/
gboolean
expidus_mime_helper_database_set_default (ExpidusMimeHelperDatabase *database,
                                 ExpidusMimeHelperCategory  category,
                                 ExpidusMimeHelper         *helper,
                                 GError           **error)
{
  ExpidusRc       *rc, *desktop_file;
  gchar        *key;
  const gchar  *filename;
  gchar       **mimetypes;
  guint         i;
  gchar        *path;
  gchar        *entry;

  g_return_val_if_fail (category < EXPIDUS_MIME_HELPER_N_CATEGORIES, FALSE);
  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER_DATABASE (database), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER (helper), FALSE);

  /* open the helpers.rc for writing */
  rc = expidus_rc_config_open (EXPIDUS_RESOURCE_CONFIG, "expidus1/helpers.rc", FALSE);
  if (G_UNLIKELY (rc == NULL))
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_IO, _("Failed to open %s for writing"), "helpers.rc");
      return FALSE;
    }

  /* save the new setting */
  key = expidus_mime_helper_category_to_string (category);
  expidus_rc_write_entry (rc, key, expidus_mime_helper_get_id (helper));
  g_free (key);

  /* clear the dismissed preference */
  key = g_strconcat (expidus_mime_helper_category_to_string (category), "Dismissed", NULL);
  expidus_rc_delete_entry (rc, key, FALSE);
  expidus_rc_close (rc);
  g_free (key);

  /* get the desktop filename */
  switch (category)
    {
      case EXPIDUS_MIME_HELPER_WEBBROWSER:
        filename = "expidus1-web-browser.desktop";
        break;

      case EXPIDUS_MIME_HELPER_MAILREADER:
        filename = "expidus1-mail-reader.desktop";
        break;

      case EXPIDUS_MIME_HELPER_FILEMANAGER:
        filename = "expidus1-file-manager.desktop";
        break;

      default:
        /* no mimetype support for terminals */
        return TRUE;
    }

  /* open the mimeapp.list file to set the default handler of the mime type */
  rc = mimeapps_open (FALSE);
  if (G_UNLIKELY (rc == NULL))
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_IO, _("Failed to open %s for writing"), "mimeapps.list");
      return FALSE;
    }

  /* open the endo desktop file to read the mimetypes the file supports */
  path = g_build_filename ("applications", filename, NULL);
  desktop_file = expidus_rc_config_open (EXPIDUS_RESOURCE_DATA, path, TRUE);
  g_free (path);

  if (G_UNLIKELY (desktop_file != NULL))
    {
      expidus_rc_set_group (desktop_file, "Desktop Entry");
      mimetypes = expidus_rc_read_list_entry (desktop_file, "X-EXPIDUS-MimeType", ";");
      if (mimetypes != NULL)
        {
#ifdef HAVE_GIO_UNIX
          GDesktopAppInfo *info = g_desktop_app_info_new (filename);
#endif

          expidus_rc_set_group (rc, "Default Applications");

          for (i = 0; mimetypes[i] != NULL; i++)
            if (!endo_str_is_empty (mimetypes[i]))
              expidus_rc_write_entry (rc, mimetypes[i], filename);

          expidus_rc_set_group (rc, "Added Associations");

          for (i = 0; mimetypes[i] != NULL; i++)
            if (!endo_str_is_empty (mimetypes[i]))
              {
                entry = g_strconcat (filename, ";", NULL);
                expidus_rc_write_entry (rc, mimetypes[i], entry);
                g_free (entry);

#ifdef HAVE_GIO_UNIX
                if (info != NULL)
                  {
                    g_app_info_set_as_default_for_type (G_APP_INFO (info),
                                                        mimetypes[i],
                                                        NULL);
                  }
#endif
              }
          g_strfreev (mimetypes);
#ifdef HAVE_GIO_UNIX
          if (info != NULL)
            {
              g_object_unref (info);
            }
#endif
        }

      expidus_rc_close (desktop_file);
    }

  clear_bad_entries (rc);

  expidus_rc_close (rc);

  return TRUE;
}



/**
 * expidus_mime_helper_database_clear_default:
 * @database : a #ExpidusMimeHelperDatabase.
 * @category : a #ExpidusMimeHelperCategory.
 * @error    : return location for errors or %NULL.
 *
 * Clears the default #ExpidusMimeHelper for @category in @database.
 * Returns %TRUE on success, %FALSE if @error is set.
 *
 * Return value: %TRUE on success, %FALSE if @error is set.
 *
 * Since: 0.11.3
 **/
gboolean
expidus_mime_helper_database_clear_default (ExpidusMimeHelperDatabase *database,
                                   ExpidusMimeHelperCategory  category,
                                   GError           **error)
{
  ExpidusRc       *rc, *desktop_file;
  gchar        *key;
  const gchar  *filename;
  gchar       **mimetypes;
  guint         i;
  gchar        *path;

  g_return_val_if_fail (category < EXPIDUS_MIME_HELPER_N_CATEGORIES, FALSE);
  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER_DATABASE (database), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* open the helpers.rc for writing */
  rc = expidus_rc_config_open (EXPIDUS_RESOURCE_CONFIG, "expidus1/helpers.rc", FALSE);
  if (G_UNLIKELY (rc == NULL))
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_IO, _("Failed to open %s for writing"), "helpers.rc");
      return FALSE;
    }

  /* save the new setting */
  key = expidus_mime_helper_category_to_string (category);
  expidus_rc_delete_entry (rc, key, FALSE);
  g_free (key);

  /* clear the dismissed preference */
  key = g_strconcat (expidus_mime_helper_category_to_string (category), "Dismissed", NULL);
  expidus_rc_delete_entry (rc, key, FALSE);
  expidus_rc_close (rc);
  g_free (key);

  /* get the desktop filename */
  switch (category)
    {
      case EXPIDUS_MIME_HELPER_WEBBROWSER:
        filename = "expidus1-web-browser.desktop";
        break;

      case EXPIDUS_MIME_HELPER_MAILREADER:
        filename = "expidus1-mail-reader.desktop";
        break;

      case EXPIDUS_MIME_HELPER_FILEMANAGER:
        filename = "expidus1-file-manager.desktop";
        break;

      default:
        /* no mimetype support for terminals */
        return TRUE;
    }

  /* open the mimeapp.list file to set the default handler of the mime type */
  rc = mimeapps_open (FALSE);
  if (G_UNLIKELY (rc == NULL))
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_IO, _("Failed to open %s for writing"), "mimeapps.list");
      return FALSE;
    }

  /* open the endo desktop file to read the mimetypes the file supports */
  path = g_build_filename ("applications", filename, NULL);
  desktop_file = expidus_rc_config_open (EXPIDUS_RESOURCE_DATA, path, TRUE);
  g_free (path);

  if (G_UNLIKELY (desktop_file != NULL))
    {
      expidus_rc_set_group (desktop_file, "Desktop Entry");
      mimetypes = expidus_rc_read_list_entry (desktop_file, "X-EXPIDUS-MimeType", ";");
      if (mimetypes != NULL)
        {
          expidus_rc_set_group (rc, "Default Applications");

          for (i = 0; mimetypes[i] != NULL; i++)
            if (!endo_str_is_empty (mimetypes[i]))
              expidus_rc_delete_entry (rc, mimetypes[i], FALSE);

          expidus_rc_set_group (rc, "Added Associations");

          for (i = 0; mimetypes[i] != NULL; i++)
            if (!endo_str_is_empty (mimetypes[i]))
              expidus_rc_delete_entry (rc, mimetypes[i], FALSE);

          g_strfreev (mimetypes);
        }

      expidus_rc_close (desktop_file);
    }

  clear_bad_entries (rc);

  expidus_rc_close (rc);

  return TRUE;
}



static void
clear_bad_entry (ExpidusRc *rc,
                 gchar  *key,
                 gchar  *filename)
{
  gchar **values;

  if (expidus_rc_has_entry (rc, key))
    {
      values = expidus_rc_read_list_entry (rc, key, ";");
      if (values != NULL)
        {
          GSList *list = NULL, *item = NULL;
          gint i;

          for (i = 0; values[i] != NULL; i++)
            {
              if (!endo_str_is_empty(values[i]) && g_strcmp0(values[i], filename) != 0)
                {
                  list = g_slist_append (list, g_strdup(values[i]));
                }
            }
          g_strfreev(values);

          if (list == NULL)
            {
              expidus_rc_delete_entry (rc, key, FALSE);
            }
          else
            {
              gchar   *value;
              GString *string = g_string_new (NULL);
              for (item = list; item != NULL; item = g_slist_next (item))
                {
                  g_string_append_printf (string, "%s;", (gchar *)item->data);
                }
              value = g_string_free (string, FALSE);
              expidus_rc_write_entry (rc, key, value);
              g_slist_free_full (list, g_free);
              g_free (value);
            }
        }
    }
}



static void
clear_bad_entries (ExpidusRc *rc)
{
  expidus_rc_set_group (rc, "Added Associations");
  clear_bad_entry (rc, "x-scheme-handler/file", "expidus1-file-manager.desktop"); // Expidus #7257
}



static gint
helper_compare (gconstpointer a,
                gconstpointer b)
{
  return g_utf8_collate (expidus_mime_helper_get_name (a), expidus_mime_helper_get_name (b));
}



/**
 * expidus_mime_helper_database_get_all:
 * @database : a #ExpidusMimeHelperDatabase.
 * @category : a #ExpidusMimeHelperCategory.
 *
 * Looks up all available helpers for @category
 * in @database, sorted in alphabetic order.
 *
 * The returned list keeps references on the included
 * helpers, so be sure to run
 * <informalexample><programlisting>
 *  g_list_foreach (list, (GFunc) g_object_unref, NULL);
 *  g_list_free (list);
 * </programlisting></informalexample>
 * when you are done.
 *
 * Return value: The list of all helpers available
 *               in @category.
 **/
GList*
expidus_mime_helper_database_get_all (ExpidusMimeHelperDatabase *database,
                             ExpidusMimeHelperCategory  category)
{
  ExpidusMimeHelper *helper;
  GList     *helpers = NULL;
  gchar    **specs;
  gchar     *id;
  gchar     *s;
  guint      n;

  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER_DATABASE (database), NULL);
  g_return_val_if_fail (category < EXPIDUS_MIME_HELPER_N_CATEGORIES, NULL);

  expidus_resource_push_path (EXPIDUS_RESOURCE_DATA, DATADIR);
  specs = expidus_resource_match (EXPIDUS_RESOURCE_DATA, "expidus1/helpers/*.desktop", TRUE);
  expidus_resource_pop_path (EXPIDUS_RESOURCE_DATA);

  for (n = 0; specs[n] != NULL; ++n)
    {
      s = strrchr (specs[n], '.');
      if (G_LIKELY (s != NULL))
        *s = '\0';

      id = strrchr (specs[n], '/');
      id = (id != NULL) ? id + 1 : specs[n];

      helper = expidus_mime_helper_database_lookup (database, category, id);
      if (G_LIKELY (helper != NULL))
        helpers = g_list_insert_sorted (helpers, helper, helper_compare);

      g_free (specs[n]);
    }
  g_free (specs);

  return helpers;
}



/**
 * expidus_mime_helper_database_get_custom:
 * @database : a #ExpidusMimeHelperDatabase.
 * @category : a #ExpidusMimeHelperCategory.
 *
 * Returns the custom #ExpidusMimeHelper set for @database
 * or %NULL if no custom #ExpidusMimeHelper is set for
 * @category.
 *
 * The caller is responsible to free the returned
 * object using g_object_unref() when no longer
 * needed.
 *
 * Return value: the custom #ExpidusMimeHelper for @category
 *               in @database or %NULL.
 **/
ExpidusMimeHelper*
expidus_mime_helper_database_get_custom (ExpidusMimeHelperDatabase *database,
                                ExpidusMimeHelperCategory  category)
{
  gchar *string;
  gchar  id[256];

  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER_DATABASE (database), NULL);
  g_return_val_if_fail (category < EXPIDUS_MIME_HELPER_N_CATEGORIES, NULL);

  /* determine the id for the custom helper */
  string = expidus_mime_helper_category_to_string (category);
  g_snprintf (id, sizeof (id), "custom-%s", string);
  g_free (string);

  return expidus_mime_helper_database_lookup (database, category, id);
}



/**
 * expidus_mime_helper_database_set_custom:
 * @database : a #ExpidusMimeHelperDatabase.
 * @category : a #ExpidusMimeHelperCategory.
 * @command  : the custom command.
 *
 * Sets the custom helper for @category in @database
 * to @command.
 **/
void
expidus_mime_helper_database_set_custom (ExpidusMimeHelperDatabase *database,
                                ExpidusMimeHelperCategory  category,
                                const gchar       *command)
{
  ExpidusRc *rc;
  gchar **argv;
  gchar  *category_string;
  gchar  *name;
  gchar  *cmdline;
  gchar  *file;
  gchar   spec[256];

  g_return_if_fail (EXPIDUS_MIME_IS_HELPER_DATABASE (database));
  g_return_if_fail (category < EXPIDUS_MIME_HELPER_N_CATEGORIES);
  g_return_if_fail (!endo_str_is_empty (command));

  /* determine the spec for the custom helper */
  category_string = expidus_mime_helper_category_to_string (category);
  g_snprintf (spec, sizeof (spec), "expidus1/helpers/custom-%s.desktop", category_string);

  /* lookup the resource save location */
  file = expidus_resource_save_location (EXPIDUS_RESOURCE_DATA, spec, TRUE);

  /* write the custom helper file */
  rc = expidus_rc_simple_open (file, FALSE);
  if (G_LIKELY (rc != NULL))
    {
      expidus_rc_set_group (rc, "Desktop Entry");
      expidus_rc_write_bool_entry (rc, "NoDisplay", TRUE);
      expidus_rc_write_entry (rc, "Version", "1.0");
      expidus_rc_write_entry (rc, "Encoding", "UTF-8");
      expidus_rc_write_entry (rc, "Type", "X-EXPIDUS-Helper");
      expidus_rc_write_entry (rc, "X-EXPIDUS-Category", category_string);

      /* check if the command includes a parameter */
      cmdline = (strstr (command, "%s") != NULL) ? g_strdup (command) : g_strconcat (command, " \"%s\"", NULL);

      /* use the command line for the CommandsWithParameter */
      expidus_rc_write_entry (rc, "X-EXPIDUS-CommandsWithParameter", cmdline);

      /* try to parse the command line */
      if (g_shell_parse_argv (cmdline, NULL, &argv, NULL))
        {
          /* use the basename for Name and Icon */
          name = g_path_get_basename (*argv);
          expidus_rc_write_entry (rc, "Icon", name);
          expidus_rc_write_entry (rc, "Name", name);
          g_free (name);

          if (strstr (command, "%s") == NULL) {
            /* trust the user, they defined the command without a parameter (bug #4093) */
            expidus_rc_write_entry (rc, "X-EXPIDUS-Commands", command);
          } else {
            /* use only the binary for the Commands */
            expidus_rc_write_entry (rc, "X-EXPIDUS-Commands", *argv);
          }

          /* cleanup */
          g_strfreev (argv);
        }
      else
        {
          expidus_rc_write_entry (rc, "Name", command);
          expidus_rc_write_entry (rc, "X-EXPIDUS-Commands", command);
        }

      /* save the helper file */
      expidus_rc_close (rc);

      /* cleanup */
      g_free (cmdline);
    }

  /* ditch any cached object */
  g_hash_table_remove (database->helpers, spec);

  /* cleanup */
  g_free (category_string);
  g_free (file);
}

/**
 * expidus_mime_helper_database_get_dismissed:
 * @database : a #ExpidusMimeHelperDatabase.
 * @category : a #ExpidusMimeHelperCategory.
 *
 * Returns %TRUE if errors should no longer be displayed
 * on the default #ExpidusMimeHelper for the @category in @database.
 *
 * Return value: %TRUE if dismissed, %FALSE otherwise.
 **/
gboolean expidus_mime_helper_database_get_dismissed (ExpidusMimeHelperDatabase *database,
                                            ExpidusMimeHelperCategory  category)
{
  ExpidusRc      *rc;
  gchar       *key;
  gboolean     dismissed = FALSE;

  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER_DATABASE (database), FALSE);
  g_return_val_if_fail (category < EXPIDUS_MIME_HELPER_N_CATEGORIES, FALSE);

  rc = expidus_rc_config_open (EXPIDUS_RESOURCE_CONFIG, "expidus1/helpers.rc", TRUE);
  if (G_LIKELY (rc != NULL))
    {
      key = g_strconcat (expidus_mime_helper_category_to_string (category), "Dismissed", NULL);
      dismissed = expidus_rc_read_bool_entry (rc, key, FALSE);
      expidus_rc_close (rc);
      g_free (key);
    }

  return dismissed;
}

/**
 * expidus_mime_helper_database_set_dismissed:
 * @database  : a #ExpidusMimeHelperDatabase.
 * @category  : a #ExpidusMimeHelperCategory.
 * @dismissed : TRUE if the errr should no longer be displayed.
 * @error     : return location for errors or %NULL.
 *
 * Dismisses future errors related to the selected helper category.
 * This setting is cleared any time a new default is configured.
 * Returns %TRUE on success, %FALSE if @error is set.
 *
 * Return value: %TRUE on success, %FALSE if @error is set.
 **/
gboolean
expidus_mime_helper_database_set_dismissed (ExpidusMimeHelperDatabase *database,
                                   ExpidusMimeHelperCategory  category,
                                   gboolean           dismissed)
{
  ExpidusRc       *rc;
  gchar        *key;

  g_return_val_if_fail (category < EXPIDUS_MIME_HELPER_N_CATEGORIES, FALSE);
  g_return_val_if_fail (EXPIDUS_MIME_IS_HELPER_DATABASE (database), FALSE);

  /* open the helpers.rc for writing */
  rc = expidus_rc_config_open (EXPIDUS_RESOURCE_CONFIG, "expidus1/helpers.rc", FALSE);
  if (G_UNLIKELY (rc == NULL))
    {
      return FALSE;
    }

  /* save the new setting */
  key = g_strconcat (expidus_mime_helper_category_to_string (category), "Dismissed", NULL);
  expidus_rc_write_bool_entry (rc, key, dismissed);
  expidus_rc_close (rc);
  g_free (key);

  return TRUE;
}