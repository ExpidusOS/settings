/*
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

#ifndef __WORKSPACES_H__
#define __WORKSPACES_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define EXPIDUS_TYPE_WORKSPACES_HELPER     (expidus_workspaces_helper_get_type())
#define EXPIDUS_WORKSPACES_HELPER(obj)     (G_TYPE_CHECK_INSTANCE_CAST((obj), EXPIDUS_TYPE_WORKSPACES_HELPER, ExpidusWorkspacesHelper))
#define EXPIDUS_IS_WORKSPACES_HELPER(obj)  (G_TYPE_CHECK_INSTANCE_TYPE((obj), EXPIDUS_TYPE_WORKSPACES_HELPER))

typedef struct _ExpidusWorkspacesHelper       ExpidusWorkspacesHelper;
typedef struct _ExpidusWorkspacesHelperClass  ExpidusWorkspacesHelperClass;

GType expidus_workspaces_helper_get_type(void) G_GNUC_CONST;
#ifdef GDK_WINDOWING_X11
void expidus_workspaces_helper_disable_wm_check (gboolean disable_wm_check);
#endif

G_END_DECLS

#endif /* __WORKSPACES_H__ */
