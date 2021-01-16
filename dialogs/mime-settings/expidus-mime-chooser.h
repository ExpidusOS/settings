/*
 * Copyright (C) 2012 Nick Schermer <nick@expidus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __EXPIDUS_MIME_CHOOSER_H__
#define __EXPIDUS_MIME_CHOOSER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _ExpidusMimeChooserClass ExpidusMimeChooserClass;
typedef struct _ExpidusMimeChooser      ExpidusMimeChooser;

#define EXPIDUS_TYPE_MIME_CHOOSER            (expidus_mime_chooser_get_type ())
#define EXPIDUS_MIME_CHOOSER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_MIME_CHOOSER, ExpidusMimeChooser))
#define EXPIDUS_MIME_CHOOSER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_MIME_CHOOSER, ExpidusMimeChooserClass))
#define EXPIDUS_IS_MIME_CHOOSER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_MIME_CHOOSER))
#define EXPIDUS_IS_MIME_CHOOSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_MIME_CHOOSER))
#define EXPIDUS_MIME_CHOOSER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_MIME_CHOOSER, ExpidusMimeChooserClass))

GType      expidus_mime_chooser_get_type      (void) G_GNUC_CONST;

void       expidus_mime_chooser_set_mime_type (ExpidusMimeChooser *chooser,
                                            const gchar     *mime_type);

GAppInfo  *expidus_mime_chooser_get_app_info  (ExpidusMimeChooser *chooser);

G_END_DECLS

#endif /* !__EXPIDUS_MIME_CHOOSER_H__ */
