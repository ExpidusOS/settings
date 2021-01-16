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

#ifndef __EXPIDUS_MIME_HELPER_H__
#define __EXPIDUS_MIME_HELPER_H__

#include <endo/endo.h>

G_BEGIN_DECLS

typedef enum /*< enum,prefix=EXPIDUS_MIME_HELPER >*/
{
  EXPIDUS_MIME_HELPER_WEBBROWSER,        /*< nick=WebBrowser >*/
  EXPIDUS_MIME_HELPER_MAILREADER,        /*< nick=MailReader >*/
  EXPIDUS_MIME_HELPER_FILEMANAGER,       /*< nick=FileManager >*/
  EXPIDUS_MIME_HELPER_TERMINALEMULATOR,  /*< nick=TerminalEmulator >*/
  EXPIDUS_MIME_HELPER_N_CATEGORIES,      /*< skip >*/
} ExpidusMimeHelperCategory;

typedef struct _ExpidusMimeHelperClass ExpidusMimeHelperClass;
typedef struct _ExpidusMimeHelper      ExpidusMimeHelper;

#define EXPIDUS_MIME_TYPE_HELPER            (expidus_mime_helper_get_type ())
#define EXPIDUS_MIME_HELPER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_MIME_TYPE_HELPER, ExpidusMimeHelper))
#define EXPIDUS_MIME_HELPER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_MIME_TYPE_HELPER, ExpidusMimeHelperClass))
#define EXPIDUS_MIME_IS_HELPER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_MIME_TYPE_HELPER))
#define EXPIDUS_MIME_IS_HELPER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_MIME_TYPE_HELPER))
#define EXPIDUS_MIME_HELPER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_MIME_TYPE_HELPER, ExpidusMimeHelperClass))

GType              expidus_mime_helper_get_type      (void) G_GNUC_CONST;
ExpidusMimeHelperCategory  expidus_mime_helper_get_category  (const ExpidusMimeHelper   *helper);
const gchar       *expidus_mime_helper_get_id        (const ExpidusMimeHelper   *helper);
const gchar       *expidus_mime_helper_get_name      (const ExpidusMimeHelper   *helper);
const gchar       *expidus_mime_helper_get_icon      (const ExpidusMimeHelper   *helper);
const gchar       *expidus_mime_helper_get_command   (const ExpidusMimeHelper   *helper);
gboolean           expidus_mime_helper_execute       (ExpidusMimeHelper         *helper,
                                             GdkScreen         *screen,
                                             const gchar       *parameter,
                                             GError           **error);


#define EXPIDUS_MIME_TYPE_HELPER_DATABASE             (expidus_mime_helper_database_get_type ())
#define EXPIDUS_MIME_HELPER_DATABASE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_MIME_TYPE_HELPER_DATABASE, ExpidusMimeHelperDatabase))
#define EXPIDUS_MIME_HELPER_DATABASE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_MIME_TYPE_HELPER_DATABASE, ExpidusMimeHelperDatabaseClass))
#define EXPIDUS_MIME_IS_HELPER_DATABASE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_MIME_TYPE_HELPER_DATABASE))
#define EXPIDUS_MIME_IS_HELPER_DATABASE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_MIME_TYPE_HELPER_DATABASE))
#define EXPIDUS_MIME_HELPER_DATABASE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_MIME_TYPE_HELPER_DATABASE, ExpidusMimeHelperDatabaseClass))

typedef struct _ExpidusMimeHelperDatabaseClass ExpidusMimeHelperDatabaseClass;
typedef struct _ExpidusMimeHelperDatabase      ExpidusMimeHelperDatabase;

GType               expidus_mime_helper_database_get_type        (void) G_GNUC_CONST;
ExpidusMimeHelperDatabase  *expidus_mime_helper_database_get             (void);
ExpidusMimeHelper          *expidus_mime_helper_database_get_default     (ExpidusMimeHelperDatabase *database,
                                                         ExpidusMimeHelperCategory  category);
gboolean            expidus_mime_helper_database_set_default     (ExpidusMimeHelperDatabase *database,
                                                         ExpidusMimeHelperCategory  category,
                                                         ExpidusMimeHelper         *helper,
                                                         GError           **error);
gboolean            expidus_mime_helper_database_clear_default   (ExpidusMimeHelperDatabase *database,
                                                         ExpidusMimeHelperCategory  category,
                                                         GError           **error);
GList              *expidus_mime_helper_database_get_all         (ExpidusMimeHelperDatabase *database,
                                                         ExpidusMimeHelperCategory  category);
ExpidusMimeHelper          *expidus_mime_helper_database_get_custom      (ExpidusMimeHelperDatabase *database,
                                                         ExpidusMimeHelperCategory  category);
void                expidus_mime_helper_database_set_custom      (ExpidusMimeHelperDatabase *database,
                                                         ExpidusMimeHelperCategory  category,
                                                         const gchar       *command);
gboolean            expidus_mime_helper_database_get_dismissed   (ExpidusMimeHelperDatabase *database,
                                                         ExpidusMimeHelperCategory  category);
gboolean            expidus_mime_helper_database_set_dismissed   (ExpidusMimeHelperDatabase *database,
                                                         ExpidusMimeHelperCategory  category,
                                                         gboolean           dismissed);

G_END_DECLS

#endif /* !__EXPIDUS_MIME_HELPER_H__ */
