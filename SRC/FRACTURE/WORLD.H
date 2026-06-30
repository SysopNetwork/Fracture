/*****************************************************************************
 *   WORLD.H                                              Fracture - World    *
 *                                                                           *
 *   The game map: a square grid of tiles, each with a terrain type and a    *
 *   discovery state. Tiles are held in FRACWLD.DAT (a Btrieve file keyed on  *
 *   a packed coordinate) and accessed through the DFA API. The map is        *
 *   generated once, on first run.                                            *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_WORLD_H
#define FRACTURE_WORLD_H

#include "gcomm.h"

#define WLDMAX 64               /* max world dimension; coordinate packing   */
                                /* stride, so changing the configured size   */
                                /* never re-maps existing tile keys.         */

                                /* terrain types                            */
#define TER_PLAINS    0
#define TER_FOREST    1
#define TER_MOUNT     2
#define TER_DESERT    3
#define TER_SWAMP     4
#define TER_WATER     5
#define TER_RUINS     6
#define TER_COUNT     7

#define TF_DISCOVERED 0x01      /* tile flag: charted by a player           */

struct fractile {               /* one map tile                             */
    USHORT coord;               /*   key 0: packed y*WLDMAX + x             */
    UCHAR  terrain;             /*   terrain type (TER_*)                   */
    UCHAR  flags;               /*   TF_* bit flags                         */
};

VOID  wld_open(INT size);       /* open the world, creating+generating it   */
                                /* at the given size on first run           */
VOID  wld_close(VOID);
INT   wld_size(VOID);           /* configured world dimension               */

VOID  wld_get(INT x, INT y, struct fractile *t);  /* fetch a tile           */
VOID  wld_put(struct fractile *t);                /* save a tile            */

const CHAR *wld_name(INT terrain);   /* terrain display name                */
CHAR        wld_glyph(INT terrain);  /* terrain single-character map glyph  */

#endif /* FRACTURE_WORLD_H */
