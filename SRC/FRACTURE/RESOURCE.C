/*****************************************************************************
 *   RESOURCE.C                                          Fracture - Resources *
 *                                                                           *
 *   Resource names and the terrain-to-yield rules used when a player        *
 *   searches a tile. Kept separate from the world and player modules so the  *
 *   gathering economy can grow without disturbing either.                    *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "bbsutils.h"                /* lngrnd()                             */
#include "RESOURCE.H"
#include "WORLD.H"

static const CHAR *resnam[RES_COUNT] = {
    "Wood", "Stone", "Metal", "Food", "Water"
};

/* ---------------------------------------------------------------------- *
 * res_name                                                               *
 * ---------------------------------------------------------------------- */
const CHAR * EXPORT
res_name(INT r)
{
    if (r < 0 || r >= RES_COUNT)
        return "?";
    return resnam[r];
}

/* ---------------------------------------------------------------------- *
 * res_yield                                                              *
 *                                                                        *
 * Map a terrain to the resource a search turns up. Each terrain has a     *
 * characteristic yield (forests give Wood, mountains Stone/Metal, water   *
 * gives Water, ruins yield salvaged Metal, and so on). Desert is sparse;  *
 * other terrains occasionally come up empty so gathering still varies.    *
 * ---------------------------------------------------------------------- */
VOID EXPORT
res_yield(INT terrain, INT *type, INT *amount)
{
    INT t = RES_FOOD;
    INT a = 0;

    switch (terrain) {
    case TER_FOREST:
        if (lngrnd(1, 100) <= 25) { t = RES_FOOD;  a = (INT)lngrnd(1, 2); }
        else                      { t = RES_WOOD;  a = (INT)lngrnd(1, 3); }
        break;
    case TER_MOUNT:
        if (lngrnd(1, 100) <= 35) { t = RES_METAL; a = (INT)lngrnd(1, 2); }
        else                      { t = RES_STONE; a = (INT)lngrnd(1, 3); }
        break;
    case TER_PLAINS:
        t = RES_FOOD;  a = (INT)lngrnd(1, 3);
        break;
    case TER_SWAMP:
        if (lngrnd(1, 100) <= 50) { t = RES_WATER; a = (INT)lngrnd(1, 2); }
        else                      { t = RES_FOOD;  a = (INT)lngrnd(1, 2); }
        break;
    case TER_WATER:
        t = RES_WATER; a = (INT)lngrnd(1, 3);
        break;
    case TER_DESERT:
        if (lngrnd(1, 100) <= 40) { t = RES_STONE; a = 1; }   /* sparse */
        break;
    case TER_RUINS:
        t = RES_METAL; a = (INT)lngrnd(1, 2);                 /* salvage */
        break;
    default:
        break;
    }

    /* most terrains occasionally come up empty-handed */
    if (a > 0 && terrain != TER_DESERT && lngrnd(1, 100) <= 10)
        a = 0;

    *type   = t;
    *amount = a;
}
