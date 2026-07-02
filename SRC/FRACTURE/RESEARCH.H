/*****************************************************************************
 *   RESEARCH.H                                        Fracture - Research     *
 *                                                                           *
 *   A linear knowledge track. A player spends gathered resources to advance   *
 *   their research tier, and each tier permanently improves how they play:     *
 *   a growing bonus to what searching the land yields, and at the higher       *
 *   tiers a discount on the Action-Point cost of travel.                       *
 *                                                                           *
 *   The tier is a single byte kept in the player record (no separate file),    *
 *   so an existing player simply starts at tier 0. The effects are applied by   *
 *   the gathering and travel code, which asks this module for the bonus.        *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_RESEARCH_H
#define FRACTURE_RESEARCH_H

#include "gcomm.h"
#include "PLAYER.H"                  /* struct fracplyr, RES_* via RESOURCE.H */

#define RSCH_MAXLVL 4           /* tiers 0..4                               */

                                /* rsch_advance results                     */
#define RADV_OK    0
#define RADV_OFF   1            /*   research disabled by the sysop         */
#define RADV_MAX   2            /*   already at the top tier                */
#define RADV_POOR  3            /*   cannot afford the next tier            */

/* Open the research system, latching the sysop master toggle.                 */
VOID  rsch_open(GBOOL enabled);
GBOOL rsch_enabled(VOID);
INT   rsch_max(VOID);                /* highest tier (RSCH_MAXLVL)          */

const CHAR *rsch_name(INT level);    /* name of the tier                    */
INT   rsch_gather_pct(INT level);    /* % bonus to gather yield at a tier   */
INT   rsch_travel_disc(INT level);   /* Action Points shaved off travel     */

/* Apply the tier's gather bonus to a raw yield amount.                        */
LONG  rsch_gather(INT level, LONG amt);

/* Fill cost[] with the resources needed to advance from `level` to the next   */
/* tier. Returns FALSE (zeroing cost[]) at the top tier.                        */
GBOOL rsch_cost(INT level, LONG cost[RES_COUNT]);

/* Advance *p one tier: checks and deducts the cost, bumps p->rschlvl. Returns  */
/* a RADV_* code. The caller persists *p (pl_write) on RADV_OK.                 */
INT   rsch_advance(struct fracplyr *p);

#endif /* FRACTURE_RESEARCH_H */
