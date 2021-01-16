/*
 *  Copyright (c) 2008 Nick Schermer <nick@expidus.org>
 *  Copyright (C) 2010 Lionel Le Folgoc <lionel@lefolgoc.net>
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

#include <esconf/esconf.h>
#include <gdk/gdk.h>
#include <X11/extensions/Xrandr.h>

#ifndef __EXPIDUS_RANDR_H__
#define __EXPIDUS_RANDR_H__

#define EXPIDUS_RANDR_ROTATIONS_MASK             (RR_Rotate_0|RR_Rotate_90|RR_Rotate_180|RR_Rotate_270)
#define EXPIDUS_RANDR_REFLECTIONS_MASK           (RR_Reflect_X|RR_Reflect_Y)

/* check for randr 1.3 or better */
#if RANDR_MAJOR > 1 || (RANDR_MAJOR == 1 && RANDR_MINOR >= 3)
#define HAS_RANDR_ONE_POINT_THREE
#else
#undef HAS_RANDR_ONE_POINT_THREE
#endif

typedef struct _ExpidusRandr          ExpidusRandr;
typedef struct _ExpidusRandrPrivate   ExpidusRandrPrivate;
typedef struct _ExpidusRRMode         ExpidusRRMode;
typedef struct _ExpidusOutputInfo     ExpidusOutputInfo;
typedef enum   _ExpidusOutputStatus   ExpidusOutputStatus;
typedef struct _ExpidusOutputPosition ExpidusOutputPosition;

enum _ExpidusOutputStatus
{
    EXPIDUS_OUTPUT_STATUS_PRIMARY,
    EXPIDUS_OUTPUT_STATUS_SECONDARY
};

struct _ExpidusRRMode
{
    RRMode  id;
    guint   width;
    guint   height;
    gdouble rate;
};

struct _ExpidusOutputPosition
{
    gint x;
    gint y;
};

struct _ExpidusRandr
{
    /* number of connected outputs */
    guint                noutput;

    /* selected settings for all connected outputs */
    RRMode              *mode;
    gfloat              *scalex;
    gfloat              *scaley;
    Rotation            *rotation;
    Rotation            *rotations;
    ExpidusOutputPosition  *position;
    ExpidusOutputStatus    *status;
    gboolean            *mirrored;
    gchar              **friendly_name;

    /* implementation details */
    ExpidusRandrPrivate    *priv;
};

struct _ExpidusOutputInfo
{
    /* Identifiers */
    guint      id;
    gchar     *display_name;

    /* Status */
    gboolean   on;
    gboolean   connected;
    gboolean   mirrored;

    /* Position */
    gint      x;
    gint      y;

    /* Dimensions */
    gint      width;
    gint      height;
    guint      pref_width;
    guint      pref_height;
    Rotation   rotation;
    gdouble    scalex;
    gdouble    scaley;

    /* Frequency */
    gdouble    rate;

    /* User Data (e.g. GrabInfo) */
    gpointer   user_data;
};

ExpidusRandr        *expidus_randr_new             (GdkDisplay      *display,
                                              GError         **error);

void              expidus_randr_free            (ExpidusRandr        *randr);

void              expidus_randr_reload          (ExpidusRandr        *randr);

void              expidus_randr_save_output     (ExpidusRandr        *randr,
                                              const gchar      *scheme,
                                              EsconfChannel    *channel,
                                              guint             output);

void              expidus_randr_apply           (ExpidusRandr        *randr,
                                              const gchar      *scheme,
                                              EsconfChannel    *channel);

void              expidus_randr_load            (ExpidusRandr        *randr,
                                              const gchar      *scheme,
                                              EsconfChannel    *channel);

guint8 *          expidus_randr_read_edid_data  (Display          *xdisplay,
                                              RROutput          output);

const ExpidusRRMode *expidus_randr_find_mode_by_id (ExpidusRandr        *randr,
                                              guint             output,
                                              RRMode            id);

RRMode            expidus_randr_preferred_mode  (ExpidusRandr        *randr,
                                              guint             output);

RRMode            expidus_randr_clonable_mode   (ExpidusRandr        *randr);

gchar *           expidus_randr_get_edid        (ExpidusRandr        *randr,
                                              guint             noutput);

gchar *           expidus_randr_get_output_info_name (ExpidusRandr *randr,
                                                   guint        noutput);

const ExpidusRRMode *expidus_randr_get_modes       (ExpidusRandr        *randr,
                                              guint             output,
                                              gint             *nmode);

gboolean          expidus_randr_get_positions   (ExpidusRandr        *randr,
                                              guint             output,
                                              gint             *x,
                                              gint             *y);

guint             expidus_randr_mode_width      (const ExpidusRRMode *mode,
                                              Rotation          rot);

guint             expidus_randr_mode_height     (const ExpidusRRMode *mode,
                                              Rotation          rot);

#endif /* !__EXPIDUS_RANDR_H__ */
