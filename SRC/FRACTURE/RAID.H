/*****************************************************************************
 *   RAID.H                                                Fracture - Raiding  *
 *                                                                           *
 *   Player-versus-player raiding. A player with an established camp can raid   *
 *   a rival's camp to carry off a share of their resources. The clash is        *
 *   decided by each side's camp tier and research plus a die roll, with the      *
 *   defender getting a home-ground edge. Raiding costs Action Points, and both   *
 *   raider and target must have built past the first camp tier, so newcomers      *
 *   are safe and raids are a deliberate use of the day.                          *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_RAID_H
#define FRACTURE_RAID_H

#include "gcomm.h"
#include "PLAYER.H"

                                /* raid_do outcomes                         */
#define RD_WIN    0             /*   raid succeeded, loot taken             */
#define RD_REPEL  1             /*   raid driven off by the defender        */
#define RD_OFF    2             /*   raiding disabled by the sysop          */
#define RD_SELF   3             /*   tried to raid own camp                 */
#define RD_NOTGT  4             /*   no such rival                          */
#define RD_PROT   5             /*   target too small to raid               */
#define RD_WEAK   6             /*   raider's own camp too small            */
#define RD_NOAP   7             /*   not enough Action Points               */

struct raidout {
    INT  outcome;               /*   an RD_* code                           */
    CHAR target[UIDSIZ];        /*   target user-id as found                */
    LONG loot;                  /*   total resources taken (RD_WIN)         */
    INT  atkroll;               /*   attacker clash roll                    */
    INT  defroll;               /*   defender clash roll                    */
};

/* Open the raid system, latching the sysop settings: master toggle, Action    */
/* Point cost per raid, the percent of each of the target's resources a         */
/* successful raid carries off, the Fame a win earns, and the camp tier a        */
/* player must have built to raid or be raided.                                  */
VOID  raid_open(GBOOL enabled, INT apcost, INT lootpct, INT fame, INT mincamp);
GBOOL raid_enabled(VOID);
INT   raid_apcost(VOID);
INT   raid_fame(VOID);
INT   raid_mincamp(VOID);

/* Resolve a raid by *atk against the player `targetid`. On a mountable raid it  */
/* deducts the AP cost from *atk, decides the clash, moves loot into *atk        */
/* (capped by camp storage), marks the target as raided, and writes the          */
/* target's record. Fills *out. The caller persists *atk on RD_WIN / RD_REPEL.   */
VOID  raid_do(struct fracplyr *atk, const CHAR *targetid, struct raidout *out);

#endif /* FRACTURE_RAID_H */
