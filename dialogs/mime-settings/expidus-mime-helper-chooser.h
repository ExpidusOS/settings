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

#ifndef __EXPIDUS_MIME_HELPER_CHOOSER_H__
#define __EXPIDUS_MIME_HELPER_CHOOSER_H__

#include "expidus-mime-helper.h"

G_BEGIN_DECLS

#define EXPIDUS_MIME_TYPE_HELPER_CHOOSER            (expidus_mime_helper_chooser_get_type ())
#define EXPIDUS_MIME_HELPER_CHOOSER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_MIME_TYPE_HELPER_CHOOSER, ExpidusMimeHelperChooser))
#define EXPIDUS_MIME_HELPER_CHOOSER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_MIME_TYPE_HELPER_CHOOSER, ExpidusMimeHelperChooserClass))
#define EXPIDUS_MIME_IS_HELPER_CHOOSER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_MIME_TYPE_HELPER_CHOOSER))
#define EXPIDUS_MIME_IS_HELPER_CHOOSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_MIME_TYPE_HELPER_CHOOSER))
#define EXPIDUS_MIME_HELPER_CHOOSER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_MIME_TYPE_HELPER_CHOOSER, ExpidusMimeHelperChooserClass))

typedef struct _ExpidusMimeHelperChooserClass ExpidusMimeHelperChooserClass;
typedef struct _ExpidusMimeHelperChooser      ExpidusMimeHelperChooser;

GType                   expidus_mime_helper_chooser_get_type      (void) G_GNUC_CONST;

GtkWidget              *expidus_mime_helper_chooser_new           (ExpidusMimeHelperCategory       category) G_GNUC_MALLOC;

ExpidusMimeHelperCategory  expidus_mime_helper_chooser_get_category  (const ExpidusMimeHelperChooser *chooser);
void                    expidus_mime_helper_chooser_set_category  (ExpidusMimeHelperChooser       *chooser,
                                                                ExpidusMimeHelperCategory       category);

gboolean                expidus_mime_helper_chooser_get_is_valid  (const ExpidusMimeHelperChooser *chooser);

G_END_DECLS

#endif /* !__EXPIDUS_MIME_HELPER_CHOOSER_H__ */
