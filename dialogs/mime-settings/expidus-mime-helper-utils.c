/*-
 * Copyright (c) 2003-2006 Benedikt Meurer <benny@expidus.org>.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "expidus-mime-helper-enum-types.h"
#include "expidus-mime-helper-utils.h"



/**
 * expidus_mime_helper_category_from_string:
 * @string          : string representation of a #ExpidusMimeHelperCategory.
 * @category_return : return location for the #ExpidusMimeHelperCategory.
 *
 * Transforms the @string representation of a #ExpidusMimeHelperCategory to
 * a #ExpidusMimeHelperCategory and places it in @category_return.
 *
 * Return value: %TRUE if @string was recognized and @category_return
 *               is set, else %FALSE.
 **/
gboolean
expidus_mime_helper_category_from_string (const gchar       *string,
                                 ExpidusMimeHelperCategory *category_return)
{
  GEnumClass *klass;
  gboolean    found = FALSE;
  guint       n;

  g_return_val_if_fail (category_return != NULL, FALSE);

  if (G_LIKELY (string != NULL))
    {
      klass = g_type_class_ref (EXPIDUS_MIME_TYPE_MIME_HELPER_CATEGORY);
      for (n = 0; !found && n < klass->n_values; ++n)
        if (g_ascii_strcasecmp (string, klass->values[n].value_nick) == 0)
          {
            *category_return = klass->values[n].value;
            found = TRUE;
          }
      g_type_class_unref (klass);
    }

  return found;
}



/**
 * expidus_mime_helper_category_to_string:
 * @category : a #ExpidusMimeHelperCategory.
 *
 * Transforms @category to its canonical string represenation.
 * The caller is responsible to free the returned string using
 * g_free() when no longer needed.
 *
 * Return value: the string representation for @category.
 **/
gchar*
expidus_mime_helper_category_to_string (ExpidusMimeHelperCategory category)
{
  GEnumClass *klass;
  gchar      *string;

  g_return_val_if_fail (category < EXPIDUS_MIME_HELPER_N_CATEGORIES, NULL);

  klass = g_type_class_ref (EXPIDUS_MIME_TYPE_MIME_HELPER_CATEGORY);
  string = g_strdup (klass->values[category].value_nick);
  g_type_class_unref (klass);

  return string;
}
