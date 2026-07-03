/*****************************************************************************
 *   TVARS.H                                        Fracture - Text variables  *
 *                                                                           *
 *   Named MBBS text variables for the message file. Every dynamic value a     *
 *   message shows (the player's name, a resource amount, a raid target...)     *
 *   is published as a named variable - e.g. FRA_FAME - that the host           *
 *   substitutes wherever the variable appears in the message text. Unlike      *
 *   positional printf arguments, named variables let the sysop reword and       *
 *   reorder message text freely without breaking the module.                   *
 *                                                                           *
 *   The module fills a variable's value just before emitting a message that    *
 *   uses it (tvar_set / tvar_setn / tvar_player); a few variables that are      *
 *   always current (world size, weather, season) compute their value live.      *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_TVARS_H
#define FRACTURE_TVARS_H

#include "gcomm.h"
#include "PLAYER.H"

/* Filled variable slots. The name the message file uses for each slot is     */
/* the same with an FRA_ prefix (TV_FAME <-> FRA_FAME); see TVARS.C.          */

#define TV_NAME     0           /* player user-id                            */
#define TV_FAME     1           /* player Fame                               */
#define TV_AP       2           /* Action Points remaining                   */
#define TV_EXPL     3           /* lifetime searches                         */
#define TV_DISC     4           /* lifetime first discoveries                */
#define TV_X        5           /* x position (or an event's x)              */
#define TV_Y        6           /* y position (or an event's y)              */
#define TV_CAMP     7           /* camp tier name                            */
#define TV_CAMPX    8           /* camp x                                    */
#define TV_CAMPY    9           /* camp y                                    */
#define TV_CAP      10          /* camp storage cap per resource             */
#define TV_WOOD     11          /* resources held, one slot each             */
#define TV_STONE    12
#define TV_METAL    13
#define TV_FOOD     14
#define TV_WATER    15

#define TV_TERRAIN  16          /* a terrain name                            */
#define TV_RES      17          /* a resource name                           */
#define TV_AMT      18          /* a quantity (gathered, produced, traded)   */
#define TV_GAIN     19          /* Fame gained or spent by an action         */
#define TV_NEXT     20          /* the next camp tier's name                 */
#define TV_CWOOD    21          /* a cost, per resource                      */
#define TV_CSTONE   22
#define TV_CMETAL   23
#define TV_CFOOD    24
#define TV_CREW     25          /* crew slots filled                         */
#define TV_CREWCAP  26          /* crew slots the camp allows                */
#define TV_WORKER   27          /* a worker type name                        */
#define TV_TIER     28          /* a research tier name                      */
#define TV_PCT      29          /* a percentage (research gather bonus)      */
#define TV_COST     30          /* an Action Point cost                      */
#define TV_TARGET   31          /* a raid target's user-id                   */
#define TV_LOOT     32          /* resources carried off by a raid           */
#define TV_DATE     33          /* a chronicle date (ncdate)                 */
#define TV_WHO      34          /* a chronicle actor's user-id               */

#define TV_COUNT    35          /* number of filled slots                    */

/* Register every variable with the host. Call once at module init.          */
VOID tvar_init(VOID);

/* Fill one slot with a string / a number (rendered "%ld").                  */
VOID tvar_set(INT slot, const CHAR *val);
VOID tvar_setn(INT slot, LONG val);

/* Fill every player-derived slot (TV_NAME..TV_WATER) from one record.       */
VOID tvar_player(struct fracplyr *p);

#endif /* FRACTURE_TVARS_H */
