/*****************************************************************************
 *   CAMP.C                                                  Fracture - Camp  *
 *                                                                           *
 *   Camp tier definitions: the name, storage capacity, and upgrade cost of  *
 *   each level from Tent to Citadel. Kept as plain tables so tiers are easy  *
 *   to rebalance and extend.                                                 *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "CAMP.H"

static const CHAR *campnam[CAMP_MAXLVL + 1] = {
    "Tent", "Cabin", "Outpost", "Settlement", "Fort", "Citadel"
};

/* per-resource storage capacity at each tier */
static const LONG campstor[CAMP_MAXLVL + 1] = {
    50, 150, 400, 1000, 2500, 9999
};

/* worker capacity unlocked at each tier (a Tent houses no crew) */
static const INT campwrk[CAMP_MAXLVL + 1] = {
    0, 2, 4, 7, 11, 16
};

/* cost to upgrade FROM level L to L+1, as [Wood, Stone, Metal, Food, Water] */
static const LONG upcost[CAMP_MAXLVL][RES_COUNT] = {
    {   30,   10,    0, 0, 0 },   /* Tent       -> Cabin      */
    {   80,   50,   10, 0, 0 },   /* Cabin      -> Outpost    */
    {  200,  150,   50, 0, 0 },   /* Outpost    -> Settlement */
    {  500,  400,  150, 0, 0 },   /* Settlement -> Fort       */
    { 1200, 1000,  400, 0, 0 }    /* Fort       -> Citadel    */
};

/* ---------------------------------------------------------------------- *
 * camp_name                                                              *
 * ---------------------------------------------------------------------- */
const CHAR * EXPORT
camp_name(INT level)
{
    if (level < 0 || level > CAMP_MAXLVL)
        return "?";
    return campnam[level];
}

/* ---------------------------------------------------------------------- *
 * camp_storage                                                           *
 * ---------------------------------------------------------------------- */
LONG EXPORT
camp_storage(INT level)
{
    if (level < 0)
        level = 0;
    if (level > CAMP_MAXLVL)
        level = CAMP_MAXLVL;
    return campstor[level];
}

/* ---------------------------------------------------------------------- *
 * camp_workers                                                           *
 * ---------------------------------------------------------------------- */
INT EXPORT
camp_workers(INT level)
{
    if (level < 0)
        level = 0;
    if (level > CAMP_MAXLVL)
        level = CAMP_MAXLVL;
    return campwrk[level];
}

/* ---------------------------------------------------------------------- *
 * camp_cost                                                              *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
camp_cost(INT level, LONG cost[RES_COUNT])
{
    INT i;

    if (level < 0 || level >= CAMP_MAXLVL) {
        for (i = 0; i < RES_COUNT; i++)
            cost[i] = 0;
        return 0;
    }
    for (i = 0; i < RES_COUNT; i++)
        cost[i] = upcost[level][i];
    return 1;
}
