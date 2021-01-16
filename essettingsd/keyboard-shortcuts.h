/* vi:set sw=2 sts=2 ts=2 et ai: */
/*-
 * Copyright (c) 2008 Jannis Pohlmann <jannis@expidus.org>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __EXPIDUS_KEYBOARD_SHORTCUTS_HELPER_H__
#define __EXPIDUS_KEYBOARD_SHORTCUTS_HELPER_H__

#include <glib-object.h>

G_BEGIN_DECLS;

typedef struct _ExpidusKeyboardShortcutsHelperClass   ExpidusKeyboardShortcutsHelperClass;
typedef struct _ExpidusKeyboardShortcutsHelper        ExpidusKeyboardShortcutsHelper;

#define EXPIDUS_TYPE_KEYBOARD_SHORTCUTS_HELPER            (expidus_keyboard_shortcuts_helper_get_type ())
#define EXPIDUS_KEYBOARD_SHORTCUTS_HELPER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXPIDUS_TYPE_KEYBOARD_SHORTCUTS_HELPER, ExpidusKeyboardShortcutsHelper))
#define EXPIDUS_KEYBOARD_SHORTCUTS_HELPER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXPIDUS_TYPE_KEYBOARD_SHORTCUTS_HELPER, ExpidusKeyboardShortcutsHelperClass))
#define EXPIDUS_IS_KEYBOARD_SHORTCUTS_HELPER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXPIDUS_TYPE_KEYBOARD_SHORTCUTS_HELPER))
#define EXPIDUS_IS_KEYBOARD_SHORTCUTS_HELPER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXPIDUS_TYPE_KEYBOARD_SHORTCUTS_HELPER)
#define EXPIDUS_KEYBOARD_SHORTCUTS_HELPER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXPIDUS_TYPE_KEYBOARD_SHORTCUTS_HELPER, ExpidusKeyboardShortcutsHelperClass))

GType expidus_keyboard_shortcuts_helper_get_type (void) G_GNUC_CONST;

G_END_DECLS;

#endif /* !__EXPIDUS_KEYBOARD_SHORTCUTS_HELPER_H__ */
