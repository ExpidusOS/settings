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

#ifndef __EXPIDUS_MIME_WINDOW_H__
#define __EXPIDUS_MIME_WINDOW_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _ExpidusMimeWindowClass ExpidusMimeWindowClass;
typedef struct _ExpidusMimeWindow      ExpidusMimeWindow;

#define EXPIDUS_TYPE_MIME_WINDOW            (expidus_mime_window_get_type ())
#define EXPIDUS_MIME_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_MIME_WINDOW, ExpidusMimeWindow))
#define EXPIDUS_MIME_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_MIME_WINDOW, ExpidusMimeWindowClass))
#define EXPIDUS_IS_MIME_WINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_MIME_WINDOW))
#define EXPIDUS_IS_MIME_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_MIME_WINDOW))
#define EXPIDUS_MIME_WINDOW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_MIME_WINDOW, ExpidusMimeWindowClass))

GType           expidus_mime_window_get_type      (void) G_GNUC_CONST;

ExpidusMimeWindow *expidus_mime_window_new           (void) G_GNUC_MALLOC;
GtkWidget      *expidus_mime_window_create_dialog (ExpidusMimeWindow *settings);
GtkWidget      *expidus_mime_window_create_plug   (ExpidusMimeWindow *settings,
                                                gint            socket_id);

G_END_DECLS

#endif /* !__EXPIDUS_MIME_WINDOW_H__ */
