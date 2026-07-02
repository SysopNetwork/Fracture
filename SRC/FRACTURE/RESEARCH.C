/*****************************************************************************
 *   RESEARCH.C                                        Fracture - Research     *
 *                                                                           *
 *   A linear knowledge track: spend resources to raise the player's research  *
 *   tier, which permanently boosts gather yield and, higher up, cheapens        *
 *   travel. The tier lives in the player record (rschlvl), so there is no        *
 *   data file here - this module is just the tier table, the effects, and       *
 *   the cost to advance.                                                        *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "RESEARCH.H"
#include "RESOURCE.H"

static GBOOL rschon;                 /* sysop master toggle                  */

/* Tier names, indexed by level 0..RSCH_MAXLVL. */
static const CHAR *rnames[RSCH_MAXLVL + 1] = {
    "Untrained", "Sharpened Tools", "Field Survey", "Logistics", "Master Craft"
};

/* Per-tier gather-yield bonus (percent) and travel Action-Point discount. */
static const INT gpct[RSCH_MAXLVL + 1] = { 0, 25, 50, 75, 100 };
static const INT tdisc[RSCH_MAXLVL + 1] = { 0, 0, 0, 1, 1 };

/* ---------------------------------------------------------------------- *
 * open / accessors                                                       *
 * ---------------------------------------------------------------------- */
VOID EXPORT
rsch_open(GBOOL enabled)
{
    rschon = enabled;
}

GBOOL EXPORT
rsch_enabled(VOID)
{
    return rschon;
}

INT EXPORT
rsch_max(VOID)
{
    return RSCH_MAXLVL;
}

static INT
clamp_lvl(INT level)
{
    if (level < 0)           return 0;
    if (level > RSCH_MAXLVL)  return RSCH_MAXLVL;
    return level;
}

const CHAR * EXPORT
rsch_name(INT level)
{
    if (level < 0 || level > RSCH_MAXLVL)
        return "?";
    return rnames[level];
}

INT EXPORT
rsch_gather_pct(INT level)
{
    return gpct[clamp_lvl(level)];
}

INT EXPORT
rsch_travel_disc(INT level)
{
    return tdisc[clamp_lvl(level)];
}

LONG EXPORT
rsch_gather(INT level, LONG amt)
{
    LONG pct = rsch_gather_pct(level);
    return amt + (amt * pct) / 100;
}

/* ---------------------------------------------------------------------- *
 * rsch_cost                                                              *
 *                                                                        *
 * Resource price to advance from `level` to the next tier. Costs scale    *
 * with the tier being bought; Metal is only demanded from tier 2 up.      *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
rsch_cost(INT level, LONG cost[RES_COUNT])
{
    INT i;
    LONG next = level + 1;

    for (i = 0; i < RES_COUNT; i++)
        cost[i] = 0;

    if (level < 0 || level >= RSCH_MAXLVL)
        return FALSE;

    cost[RES_WOOD]  = 20L * next;
    cost[RES_STONE] = 15L * next;
    cost[RES_METAL] = (next >= 2) ? 5L * (next - 1) : 0;
    return TRUE;
}

/* ---------------------------------------------------------------------- *
 * rsch_advance                                                           *
 * ---------------------------------------------------------------------- */
INT EXPORT
rsch_advance(struct fracplyr *p)
{
    LONG cost[RES_COUNT];
    INT  i;

    if (!rschon)
        return RADV_OFF;
    if (p->rschlvl >= RSCH_MAXLVL)
        return RADV_MAX;
    if (!rsch_cost((INT)p->rschlvl, cost))
        return RADV_MAX;

    for (i = 0; i < RES_COUNT; i++)
        if (p->res[i] < cost[i])
            return RADV_POOR;

    for (i = 0; i < RES_COUNT; i++)
        p->res[i] -= cost[i];
    p->rschlvl++;
    return RADV_OK;
}
