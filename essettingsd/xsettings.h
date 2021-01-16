/*
 * Copyright (c) 2008 Stephan Arts <stephan@expidus.org>
 * Copyright (c) 2011 Nick Schermer <nick@expidus.org>
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

#ifndef __XSETTINGS_H__
#define __XSETTINGS_H__

typedef struct _ExpidusXSettingsHelperClass ExpidusXSettingsHelperClass;
typedef struct _ExpidusXSettingsHelper      ExpidusXSettingsHelper;

#define EXPIDUS_TYPE_XSETTINGS_HELPER            (expidus_xsettings_helper_get_type ())
#define EXPIDUS_XSETTINGS_HELPER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_XSETTINGS_HELPER, ExpidusXSettingsHelper))
#define EXPIDUS_XSETTINGS_HELPER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_XSETTINGS_HELPER, ExpidusXSettingsHelperClass))
#define EXPIDUS_IS_XSETTINGS_HELPER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_XSETTINGS_HELPER))
#define EXPIDUS_IS_XSETTINGS_HELPER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_XSETTINGS_HELPER))
#define EXPIDUS_XSETTINGS_HELPER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_XSETTINGS_HELPER, ExpidusXSettingsHelperClass))

GType    expidus_xsettings_helper_get_type (void) G_GNUC_CONST;

gboolean expidus_xsettings_helper_register (ExpidusXSettingsHelper *helper,
                                         GdkDisplay          *gdkdisplay,
                                         gboolean             force_replace);

Time     expidus_xsettings_get_server_time (Display             *display,
                                         Window               window);

#endif /* !__XSETTINGS_H__ */
