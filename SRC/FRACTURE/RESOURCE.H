/*****************************************************************************
 *   RESOURCE.H                                          Fracture - Resources *
 *                                                                           *
 *   The materials players gather from the land. Resource counts are held    *
 *   in the player record (see PLAYER.H); this module defines the resource   *
 *   set and decides what a given terrain yields when searched.              *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_RESOURCE_H
#define FRACTURE_RESOURCE_H

#include "gcomm.h"

                                /* resource types (index into player res[]) */
#define RES_WOOD   0
#define RES_STONE  1
#define RES_METAL  2
#define RES_FOOD   3
#define RES_WATER  4
#define RES_COUNT  5

const CHAR *res_name(INT r);    /* display name for a resource type          */

/* Decide what a search of the given terrain yields. Sets *type to a RES_*   */
/* value and *amount to the quantity gathered (0 means nothing this time).   */
VOID res_yield(INT terrain, INT *type, INT *amount);

#endif /* FRACTURE_RESOURCE_H */
