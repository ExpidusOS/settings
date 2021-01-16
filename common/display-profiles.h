/*
 *  Copyright (c) 2019 Simon Steinbeiß <simon@expidus.org>
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

#include <glib.h>
#include <esconf/esconf.h>

#include "expidus-randr.h"


gboolean display_settings_profile_name_exists   (EsconfChannel  *channel,
                                                 const gchar    *new_profile_name);
GList*   display_settings_get_profiles          (gchar         **display_infos,
                                                 EsconfChannel  *channel);
