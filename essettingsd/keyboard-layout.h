/*
 *  Copyright (c) 2008 Olivier Fourdan <olivier@expidus.org>
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
 */

#ifndef __KEYBOARD_LAYOUT_H__
#define __KEYBOARD_LAYOUT_H__

typedef struct _ExpidusKeyboardLayoutHelperClass ExpidusKeyboardLayoutHelperClass;
typedef struct _ExpidusKeyboardLayoutHelper      ExpidusKeyboardLayoutHelper;

#define EXPIDUS_TYPE_KEYBOARD_LAYOUT_HELPER            (expidus_keyboard_layout_helper_get_type ())
#define EXPIDUS_KEYBOARD_LAYOUT_HELPER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_KEYBOARD_LAYOUT_HELPER, ExpidusKeyboardLayoutHelper))
#define EXPIDUS_KEYBOARD_LAYOUT_HELPER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  EXPIDUS_TYPE_KEYBOARD_LAYOUT_HELPER, ExpidusKeyboardLayoutHelperClass))
#define EXPIDUS_IS_KEYBOARD_LAYOUT_HELPER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_KEYBOARD_LAYOUT_HELPER))
#define EXPIDUS_IS_KEYBOARD_LAYOUT_HELPER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  EXPIDUS_TYPE_KEYBOARD_LAYOUT_HELPER))
#define EXPIDUS_KEYBOARD_LAYOUT_HELPER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  EXPIDUS_TYPE_KEYBOARD_LAYOUT_HELPER, ExpidusKeyboardLayoutHelperClass))

GType expidus_keyboard_layout_helper_get_type (void) G_GNUC_CONST;

#endif /* !__KEYBOARD_LAYOUT_H__ */
