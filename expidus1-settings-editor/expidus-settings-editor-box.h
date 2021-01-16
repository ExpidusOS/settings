/*
 *  expidus1-settings-editor
 *
 *  Copyright (c) 2008      Brian Tarricone <bjt23@cornell.edu>
 *  Copyright (c) 2008      Stephan Arts <stephan@expidus.org>
 *  Copyright (c) 2009-2010 Jérôme Guelfucci <jeromeg@expidus.org>
 *  Copyright (c) 2012      Nick Schermer <nick@expidus.org>
 *  Copyright (c) 2015		Ali Abdallah <ali@aliov.org>
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

#ifndef __EXPIDUS_SETTINGS_EDITOR_BOX_H__
#define __EXPIDUS_SETTINGS_EDITOR_BOX_H__

#include <gtk/gtk.h>

#define EXPIDUS_TYPE_SETTINGS_EDITOR_BOX            (expidus_settings_editor_box_get_type ())
#define EXPIDUS_SETTINGS_EDITOR_BOX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_SETTINGS_EDITOR_BOX, ExpidusSettingsEditorBox))
#define EXPIDUS_SETTINGS_EDITOR_BOX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_SETTINGS_EDITOR_BOX, ExpidusSettingsEditorBoxClass))
#define EXPIDUS_IS_SETTINGS_EDITOR_BOX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_SETTINGS_EDITOR_BOX))
#define EXPIDUS_IS_SETTINGS_EDITOR_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_SETTINGS_EDITOR_BOX))
#define EXPIDUS_SETTINGS_EDITOR_BOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_SETTINGS_EDITOR_BOX, ExpidusSettingsEditorBoxClass))

G_BEGIN_DECLS

typedef struct _ExpidusSettingsEditorBox      ExpidusSettingsEditorBox;
typedef struct _ExpidusSettingsEditorBoxClass ExpidusSettingsEditorBoxClass;

GType      expidus_settings_editor_box_get_type    (void) G_GNUC_CONST;

GtkWidget *expidus_settings_editor_box_new         (gint paned_pos);

G_END_DECLS

#endif  /* __EXPIDUS_SETTINGS_EDITOR_BOX_H__ */
