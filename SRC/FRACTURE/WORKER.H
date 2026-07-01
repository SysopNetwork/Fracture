/*****************************************************************************
 *   WORKER.H                                          Fracture - Workers     *
 *                                                                           *
 *   Camps can house a crew of NPC workers who produce resources passively,   *
 *   one day's worth on each new day a player logs in. Each worker type works  *
 *   one resource; the whole crew eats Food as upkeep. Camp tier caps the      *
 *   crew size (see camp_workers), so production is another reason to build.    *
 *                                                                           *
 *   A player's crew (a count per worker type and the day production last ran) *
 *   lives in its own file, FRACWRK.DAT, keyed on the user-id - kept separate  *
 *   from the player record so the roster can grow without disturbing it.       *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_WORKER_H
#define FRACTURE_WORKER_H

#include "gcomm.h"
#include "PLAYER.H"                  /* struct fracplyr, RES_* via RESOURCE.H */

                                /* worker types (each works one resource)   */
#define WRK_FORESTER 0          /*   -> Wood                                */
#define WRK_MINER    1          /*   -> Stone                               */
#define WRK_PROSPECT 2          /*   -> Metal                               */
#define WRK_FARMER   3          /*   -> Food                                */
#define WRK_CARRIER  4          /*   -> Water                               */
#define WRK_COUNT    5

                                /* recruit cost, per worker                 */
#define WRK_COSTWOOD 8
#define WRK_COSTFOOD 4

                                /* wrk_hire results                         */
#define WHIRE_OK    0
#define WHIRE_FULL  1           /*   crew already at camp capacity          */
#define WHIRE_POOR  2           /*   cannot afford the recruit cost         */
#define WHIRE_BAD   3           /*   invalid worker type                    */
#define WHIRE_OFF   4           /*   workers disabled by the sysop          */

/* Open the crew file (creating FRACWRK.DAT on first run) and latch the sysop  */
/* settings: `enabled` master toggle, `perday` base units each worker produces */
/* per day, `foodup` Food each worker eats per day.                            */
VOID  wrk_open(GBOOL enabled, INT perday, INT foodup);
VOID  wrk_close(VOID);

GBOOL wrk_enabled(VOID);
const CHAR *wrk_name(INT type);      /* worker display name                 */
INT   wrk_resource(INT type);        /* RES_* this worker produces          */
INT   wrk_perday(VOID);              /* configured base units per worker/day */

/* Fill count[WRK_COUNT] with a player's crew and return the crew total. A     */
/* player with no crew record yields all zeros.                                */
INT   wrk_counts(const CHAR *userid, INT count[WRK_COUNT]);

/* Recruit one worker of `type` for *p: checks the camp capacity and cost,     */
/* deducts the cost from p->res, and records the hire. Returns a WHIRE_* code. */
/* The caller persists *p (pl_write) on WHIRE_OK.                              */
INT   wrk_hire(struct fracplyr *p, INT type);

/* Run one day of production for *p if a new day has begun since it last ran:  */
/* add each worker's output to p->res (capped by camp storage, scaled by the   */
/* day's weather), then deduct the crew's Food upkeep. Returns the total       */
/* resources gained (0 if no crew or already run today); sets *hungry TRUE if   */
/* Food could not cover upkeep. The caller persists *p when the return is > 0   */
/* or *hungry is set.                                                          */
INT   wrk_daily(struct fracplyr *p, GBOOL *hungry);

/* Remove a player's crew record (called when the BBS account is deleted).     */
VOID  wrk_delete(const CHAR *userid);

#endif /* FRACTURE_WORKER_H */
