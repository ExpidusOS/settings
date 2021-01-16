/*
 *  Copyright (c) 2008 Stephan Arts <stephan@expidus.org>
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
 *
 *  XKB Extension code taken from the original mcs-keyboard-plugin written
 *  by Olivier Fourdan.
 */

#ifndef __ACCESSIBILITY_H__
#define __ACCESSIBILITY_H__

typedef struct _ExpidusAccessibilityHelperClass ExpidusAccessibilityHelperClass;
typedef struct _ExpidusAccessibilityHelper      ExpidusAccessibilityHelper;

#define EXPIDUS_TYPE_ACCESSIBILITY_HELPER            (expidus_accessibility_helper_get_type ())
#define EXPIDUS_ACCESSIBILITY_HELPER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_ACCESSIBILITY_HELPER, ExpidusAccessibilityHelper))
#define EXPIDUS_ACCESSIBILITY_HELPER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_ACCESSIBILITY_HELPER, ExpidusAccessibilityHelperClass))
#define EXPIDUS_IS_ACCESSIBILITY_HELPER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_ACCESSIBILITY_HELPER))
#define EXPIDUS_IS_ACCESSIBILITY_HELPER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_ACCESSIBILITY_HELPER))
#define EXPIDUS_ACCESSIBILITY_HELPER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_ACCESSIBILITY_HELPER, ExpidusAccessibilityHelperClass))

GType expidus_accessibility_helper_get_type (void) G_GNUC_CONST;

#endif /* !__ACCESSIBILITY_H__ */
