/*****************************************************************************
 *   CAMP.H                                                  Fracture - Camp  *
 *                                                                           *
 *   Every player owns one camp: a home base on a tile that can be upgraded  *
 *   through a series of tiers. The camp tier sets resource storage capacity  *
 *   now, and will gate production, workers, and defense as those systems     *
 *   arrive. This module holds the tier data (names, storage, upgrade costs); *
 *   the camp's current level and location live in the player record.         *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_CAMP_H
#define FRACTURE_CAMP_H

#include "gcomm.h"
#include "RESOURCE.H"

#define CAMP_MAXLVL 5           /* tiers 0..5: Tent..Citadel                */

const CHAR *camp_name(INT level);      /* tier display name                  */
LONG        camp_storage(INT level);   /* per-resource storage capacity      */

/* Fill cost[] with the resources needed to upgrade from `level` to the next */
/* tier. Returns FALSE (and zeroes cost[]) when already at the maximum tier. */
GBOOL       camp_cost(INT level, LONG cost[RES_COUNT]);

#endif /* FRACTURE_CAMP_H */
