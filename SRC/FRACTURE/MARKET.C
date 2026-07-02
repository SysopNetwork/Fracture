/*****************************************************************************
 *   MARKET.C                                          Fracture - Market      *
 *                                                                           *
 *   The trading post: convert resources to Fame and back at sysop-set rates. *
 *   Prices are a base rate (sell and buy, set by the sysop) times a fixed      *
 *   per-resource weight, so a unit of Metal is worth more than a unit of       *
 *   Water. Buying costs more than selling returns, which keeps the exchange     *
 *   from being a free Fame pump. Trades settle at once against the acting       *
 *   player, so there is no data file here.                                     *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "MARKET.H"
#include "CAMP.H"
#include "RESOURCE.H"

static GBOOL mkton;                  /* sysop master toggle                  */
static INT   sell_base;              /* base Fame per unit sold              */
static INT   buy_base;               /* base Fame per unit bought            */

/* Per-resource value weight, indexed by RES_*. Scarcer materials are worth   */
/* more. Wood/Stone/Water are common (1), Food matters for upkeep (2), Metal   */
/* is the rare prize (3).                                                     */
static const INT resweight[RES_COUNT] = {
    1,  /* RES_WOOD  */
    1,  /* RES_STONE */
    3,  /* RES_METAL */
    2,  /* RES_FOOD  */
    1   /* RES_WATER */
};

/* ---------------------------------------------------------------------- *
 * market_open                                                            *
 * ---------------------------------------------------------------------- */
VOID EXPORT
market_open(GBOOL enabled, INT sellbase, INT buybase)
{
    mkton     = enabled;
    sell_base = sellbase;
    buy_base  = buybase;
}

GBOOL EXPORT
market_enabled(VOID)
{
    return mkton;
}

/* ---------------------------------------------------------------------- *
 * price helpers                                                          *
 * ---------------------------------------------------------------------- */
LONG EXPORT
market_sellprice(INT restype)
{
    if (restype < 0 || restype >= RES_COUNT)
        return 0;
    return (LONG)sell_base * resweight[restype];
}

LONG EXPORT
market_buyprice(INT restype)
{
    if (restype < 0 || restype >= RES_COUNT)
        return 0;
    return (LONG)buy_base * resweight[restype];
}

/* ---------------------------------------------------------------------- *
 * market_sell                                                            *
 * ---------------------------------------------------------------------- */
INT EXPORT
market_sell(struct fracplyr *p, INT restype, LONG qty, LONG *fame)
{
    LONG price;

    if (!mkton)
        return MKT_OFF;
    if (restype < 0 || restype >= RES_COUNT)
        return MKT_BADRES;
    if (qty <= 0)
        return MKT_BADQTY;
    if (p->res[restype] < qty)
        return MKT_NORES;

    price = market_sellprice(restype);
    p->res[restype] -= qty;
    *fame = qty * price;
    p->fame += *fame;
    return MKT_OK;
}

/* ---------------------------------------------------------------------- *
 * market_buy                                                             *
 *                                                                        *
 * Buys as much of `qty` as the player's Fame and camp storage allow.     *
 * Room in storage is capped by the camp tier; Fame caps how many units    *
 * can actually be paid for. Whichever is smaller wins.                    *
 * ---------------------------------------------------------------------- */
INT EXPORT
market_buy(struct fracplyr *p, INT restype, LONG qty, LONG *got, LONG *fame)
{
    LONG price, room, afford, buy, cap;

    if (!mkton)
        return MKT_OFF;
    if (restype < 0 || restype >= RES_COUNT)
        return MKT_BADRES;
    if (qty <= 0)
        return MKT_BADQTY;

    cap  = camp_storage(p->camplvl);
    room = cap - p->res[restype];
    if (room <= 0)
        return MKT_FULL;

    price = market_buyprice(restype);
    if (price <= 0)
        return MKT_OFF;                  /* prices not configured            */
    afford = p->fame / price;
    if (afford <= 0)
        return MKT_NOFAME;

    buy = qty;
    if (buy > room)   buy = room;        /* storage limits the buy           */
    if (buy > afford) buy = afford;      /* Fame limits the buy              */

    *got  = buy;
    *fame = buy * price;
    p->fame        -= *fame;
    p->res[restype] += buy;
    return MKT_OK;
}
