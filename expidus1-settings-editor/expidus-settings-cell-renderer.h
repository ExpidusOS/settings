/*
 *  expidus1-settings-editor
 *
 *  Copyright (c) 2012      Nick Schermer <nick@expidus.org>
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

#ifndef __EXPIDUS_SETTINGS_CELL_RENDERER_H__
#define __EXPIDUS_SETTINGS_CELL_RENDERER_H__

#include <gtk/gtk.h>

#define EXPIDUS_TYPE_SETTINGS_CELL_RENDERER            (expidus_settings_cell_renderer_get_type ())
#define EXPIDUS_SETTINGS_CELL_RENDERER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_SETTINGS_CELL_RENDERER, ExpidusSettingsCellRenderer))
#define EXPIDUS_SETTINGS_CELL_RENDERER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_SETTINGS_CELL_RENDERER, ExpidusSettingsCellRendererClass))
#define EXPIDUS_IS_SETTINGS_CELL_RENDERER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_SETTINGS_CELL_RENDERER))
#define EXPIDUS_IS_SETTINGS_CELL_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_SETTINGS_CELL_RENDERER))
#define EXPIDUS_SETTINGS_CELL_RENDERER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_SETTINGS_CELL_RENDERER, ExpidusSettingsCellRendererClass))

G_BEGIN_DECLS

typedef struct _ExpidusSettingsCellRenderer      ExpidusSettingsCellRenderer;
typedef struct _ExpidusSettingsCellRendererClass ExpidusSettingsCellRendererClass;

GType            expidus_settings_cell_renderer_get_type (void) G_GNUC_CONST;

GtkCellRenderer *expidus_settings_cell_renderer_new      (void);

GType            expidus_settings_array_type             (void);

G_END_DECLS

#endif  /* __EXPIDUS_SETTINGS_CELL_RENDERER_H__ */
