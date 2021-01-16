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

#ifndef __EXPIDUS_SETTINGS_PROP_DIALOG_H__
#define __EXPIDUS_SETTINGS_PROP_DIALOG_H__

#include <gtk/gtk.h>
#include <esconf/esconf.h>

#define EXPIDUS_TYPE_SETTINGS_PROP_DIALOG            (expidus_settings_prop_dialog_get_type ())
#define EXPIDUS_SETTINGS_PROP_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_SETTINGS_PROP_DIALOG, ExpidusSettingsPropDialog))
#define EXPIDUS_SETTINGS_PROP_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_SETTINGS_PROP_DIALOG, ExpidusSettingsPropDialogClass))
#define EXPIDUS_IS_SETTINGS_PROP_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_SETTINGS_PROP_DIALOG))
#define EXPIDUS_IS_SETTINGS_PROP_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_SETTINGS_PROP_DIALOG))
#define EXPIDUS_SETTINGS_PROP_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_SETTINGS_PROP_DIALOG, ExpidusSettingsPropDialogClass))

G_BEGIN_DECLS

typedef struct _ExpidusSettingsPropDialog      ExpidusSettingsPropDialog;
typedef struct _ExpidusSettingsPropDialogClass ExpidusSettingsPropDialogClass;

GType      expidus_settings_prop_dialog_get_type            (void) G_GNUC_CONST;

GtkWidget *expidus_settings_prop_dialog_new                 (GtkWindow              *parent,
                                                          EsconfChannel          *channel,
                                                          const gchar            *property);

void       expidus_settings_prop_dialog_set_parent_property (ExpidusSettingsPropDialog *dialog,
                                                          const gchar            *property);

G_END_DECLS

#endif  /* __EXPIDUS_SETTINGS_PROP_DIALOG_H__ */
