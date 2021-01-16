/*
 *  Copyright (C) 2012 Lionel Le Folgoc <lionel@lefolgoc.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __DISPLAYS_UPOWER_H__
#define __DISPLAYS_UPOWER_H__

#include <glib-object.h>

typedef struct _ExpidusDisplaysUPowerClass ExpidusDisplaysUPowerClass;
typedef struct _ExpidusDisplaysUPower      ExpidusDisplaysUPower;

#define EXPIDUS_TYPE_DISPLAYS_UPOWER            (expidus_displays_upower_get_type ())
#define EXPIDUS_DISPLAYS_UPOWER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_DISPLAYS_UPOWER, ExpidusDisplaysUPower))
#define EXPIDUS_DISPLAYS_UPOWER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_DISPLAYS_UPOWER, ExpidusDisplaysUPowerClass))
#define EXPIDUS_IS_DISPLAYS_UPOWER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_DISPLAYS_UPOWER))
#define EXPIDUS_IS_DISPLAYS_UPOWER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_DISPLAYS_UPOWER))
#define EXPIDUS_DISPLAYS_UPOWER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_DISPLAYS_UPOWER, ExpidusDisplaysUPowerClass))

#define XFSD_LID_STR(b) (b ? "closed" : "open")

GType expidus_displays_upower_get_type (void) G_GNUC_CONST;

#endif /* !__DISPLAYS_UPOWER_H__ */
