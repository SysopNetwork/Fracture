/*****************************************************************************
 *   MARKET.H                                          Fracture - Market      *
 *                                                                           *
 *   The trading post: a server-wide exchange where players convert between   *
 *   gathered resources and Fame. Selling a resource earns Fame; buying one    *
 *   spends it. Prices are set by the sysop (a base sell and a base buy rate)   *
 *   and weighted per resource so scarce materials are worth more, with the     *
 *   buy rate above the sell rate so there is no free arbitrage.                *
 *                                                                           *
 *   This is a priced exchange, not a player-to-player order book, so it        *
 *   needs no data file of its own - trades settle immediately against the      *
 *   acting player's record. Player-posted listings are a natural next step.    *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_MARKET_H
#define FRACTURE_MARKET_H

#include "gcomm.h"
#include "PLAYER.H"                  /* struct fracplyr, RES_* via RESOURCE.H */

                                /* market_sell / market_buy results         */
#define MKT_OK      0
#define MKT_OFF     1           /*   trading disabled by the sysop          */
#define MKT_BADRES  2           /*   resource selector out of range         */
#define MKT_BADQTY  3           /*   quantity was zero or negative          */
#define MKT_NORES   4           /*   not enough of the resource to sell     */
#define MKT_NOFAME  5           /*   not enough Fame to buy any             */
#define MKT_FULL    6           /*   camp storage already full for buying   */

/* Open the exchange and latch the sysop settings: `enabled` master toggle,    */
/* `sellbase` Fame earned per unit sold and `buybase` Fame paid per unit       */
/* bought, both before the per-resource weight is applied.                     */
VOID  market_open(GBOOL enabled, INT sellbase, INT buybase);

GBOOL market_enabled(VOID);

LONG  market_sellprice(INT restype); /* Fame per unit when selling restype  */
LONG  market_buyprice(INT restype);  /* Fame per unit when buying restype   */

/* Sell up to `qty` units of `restype` from *p. On MKT_OK, *fame receives the  */
/* Fame earned. The caller persists *p (pl_write) on MKT_OK.                    */
INT   market_sell(struct fracplyr *p, INT restype, LONG qty, LONG *fame);

/* Buy up to `qty` units of `restype` for *p, limited by Fame on hand and camp */
/* storage room. On MKT_OK, *got receives the units actually bought and *fame   */
/* the Fame spent. The caller persists *p (pl_write) on MKT_OK.                 */
INT   market_buy(struct fracplyr *p, INT restype, LONG qty, LONG *got, LONG *fame);

#endif /* FRACTURE_MARKET_H */
