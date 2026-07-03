/*****************************************************************************
 *   PLAYER.H                                            Fracture - Players   *
 *                                                                           *
 *   Persistent per-player state and the routines that store it. One record  *
 *   exists per BBS user who has entered Fracture, held in FRACTURE.DAT (a    *
 *   Btrieve file keyed on the user-id) and accessed through the DFA API.     *
 *                                                                           *
 *   Keep the record lean; new per-player systems (camp, resources,          *
 *   research) extend this structure as they are built.                      *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_PLAYER_H
#define FRACTURE_PLAYER_H

#include "gcomm.h"
#include "RESOURCE.H"

struct fracplyr {                /* one persistent player record             */
    CHAR   userid[UIDSIZ];       /*   key 0: BBS user-id                      */
    USHORT lastday;             /*   packed today() of the last AP refill   */
    INT    actpts;              /*   Action Points remaining today          */
    LONG   fame;                /*   Fame score (primary ranking)           */
    LONG   explores;           /*   lifetime explorations                  */
    LONG   discoveries;        /*   lifetime notable discoveries           */
    UCHAR  posx;               /*   current map column (0..world size-1)   */
    UCHAR  posy;               /*   current map row    (0..world size-1)   */
    UCHAR  started;            /*   has the start position been placed?     */
    UCHAR  camplvl;            /*   camp tier (0..CAMP_MAXLVL)             */
    LONG   res[RES_COUNT];     /*   gathered resources, indexed by RES_*    */
    UCHAR  campx;              /*   camp home tile column                   */
    UCHAR  campy;              /*   camp home tile row                      */
    UCHAR  rschlvl;            /*   research tier (0..RSCH_MAXLVL)          */
    UCHAR  raidhit;            /*   raids suffered since last login          */
};

VOID  pl_open(VOID);            /* open the player file, creating it if new  */
VOID  pl_close(VOID);           /* close the player file                     */

/* Load the player for userid into *p, creating the record on first visit    */
/* and refilling Action Points if a new day has begun. Returns TRUE when a   */
/* brand-new explorer was just created.                                      */
GBOOL pl_ensure(const CHAR *userid, struct fracplyr *p, INT dailyap);

VOID  pl_write(struct fracplyr *p);   /* update existing or insert new       */
VOID  pl_delete(const CHAR *userid);  /* remove a player's record            */

/* Load an existing player by user-id, without creating or refilling Action    */
/* Points. Returns TRUE if found. Used to read and modify another player, such  */
/* as a raid target.                                                            */
GBOOL pl_load(const CHAR *userid, struct fracplyr *p);

/* As pl_load, but matches the user-id case-insensitively (scans the file),   */
/* so a raid target need only be spelled right, not cased right. *p receives   */
/* the record with its stored user-id. Returns TRUE if a match was found.      */
GBOOL pl_load_ci(const CHAR *userid, struct fracplyr *p);

/* Print a list of raidable rivals - players other than `selfid` whose camp     */
/* tier is at least `mincamp` - one per line, and return how many were shown.   */
INT   pl_raidlist(const CHAR *selfid, INT mincamp);

#endif /* FRACTURE_PLAYER_H */
