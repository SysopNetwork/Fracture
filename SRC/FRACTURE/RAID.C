/*****************************************************************************
 *   RAID.C                                                Fracture - Raiding  *
 *                                                                           *
 *   Player-versus-player raiding. A raid pits the attacker's camp tier and     *
 *   research plus a die roll against the defender's, the defender getting a      *
 *   home-ground edge. Win and the raider carries off a share of every resource   *
 *   the target holds (capped by the raider's own storage) and a little Fame;      *
 *   lose and the Action Points spent are the price. Either way the target is       *
 *   marked so they learn of it on their next login.                              *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "bbsutils.h"                /* lngrnd() - ranged random numbers     */
#include "RAID.H"
#include "CAMP.H"
#include "RESOURCE.H"

static GBOOL raidon;                 /* sysop master toggle                  */
static INT   ap_cost;                /* Action Points per raid               */
static INT   loot_pct;               /* percent of each resource looted      */

#define RAID_FAME 5                  /* Fame for a successful raid           */

/* ---------------------------------------------------------------------- *
 * open / accessors                                                       *
 * ---------------------------------------------------------------------- */
VOID EXPORT
raid_open(GBOOL enabled, INT apcost, INT lootpct)
{
    raidon   = enabled;
    ap_cost  = apcost;
    loot_pct = lootpct;
}

GBOOL EXPORT
raid_enabled(VOID)
{
    return raidon;
}

INT EXPORT
raid_apcost(VOID)
{
    return ap_cost;
}

/* ---------------------------------------------------------------------- *
 * raid_do                                                                *
 * ---------------------------------------------------------------------- */
VOID EXPORT
raid_do(struct fracplyr *atk, const CHAR *targetid, struct raidout *out)
{
    struct fracplyr def;
    INT i;

    setmem(out, sizeof(*out), 0);

    if (!raidon)                              { out->outcome = RD_OFF;   return; }
    if ((INT)atk->camplvl < RAID_MINCAMP)     { out->outcome = RD_WEAK;  return; }
    if (sameas((CHAR *)targetid, atk->userid)){ out->outcome = RD_SELF;  return; }
    if (!pl_load(targetid, &def))             { out->outcome = RD_NOTGT; return; }

    stzcpy(out->target, def.userid, UIDSIZ);
    if ((INT)def.camplvl < RAID_MINCAMP)      { out->outcome = RD_PROT;  return; }
    if (atk->actpts < ap_cost)                { out->outcome = RD_NOAP;  return; }

    atk->actpts -= ap_cost;                   /* the raid is mounted        */

    /* clash: camp tier + research tier + a die roll; defender defends home  */
    out->atkroll = (INT)atk->camplvl + (INT)atk->rschlvl + (INT)lngrnd(1, 6);
    out->defroll = (INT)def.camplvl + (INT)def.rschlvl + (INT)lngrnd(1, 6) + 1;

    if (out->atkroll > out->defroll) {
        LONG cap   = camp_storage(atk->camplvl);
        LONG total = 0;
        for (i = 0; i < RES_COUNT; i++) {
            LONG take = def.res[i] * loot_pct / 100;
            LONG room = cap - atk->res[i];    /* raider storage caps the haul */
            if (take > room) take = room;
            if (take < 0)    take = 0;
            def.res[i]  -= take;
            atk->res[i] += take;
            total += take;
        }
        out->loot = total;
        atk->fame += RAID_FAME;
        out->outcome = RD_WIN;
    }
    else {
        out->outcome = RD_REPEL;
    }

    if (def.raidhit < 250)                     /* the victim will notice      */
        def.raidhit++;
    pl_write(&def);
}
