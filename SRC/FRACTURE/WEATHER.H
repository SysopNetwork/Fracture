/*****************************************************************************
 *   WEATHER.H                                        Fracture - Weather      *
 *                                                                           *
 *   The living world's daily mood: one weather condition affects the whole   *
 *   server each day, and the year turns through four seasons that bias which  *
 *   weather is likely. Both advance once per night from the midnight cleanup  *
 *   hook, alongside the world's terrain evolution.                            *
 *                                                                           *
 *   Weather is more than flavor: it scales what a search gathers and can add  *
 *   to the Action-Point cost of travel. Seasons only shift the odds - they    *
 *   have no direct mechanical effect of their own yet.                        *
 *                                                                           *
 *   The single server-wide state (today's date, season, weather, and how far  *
 *   into the season we are) lives in FRACWX.DAT: one record, keyed on a        *
 *   constant, accessed through the DFA API.                                    *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_WEATHER_H
#define FRACTURE_WEATHER_H

#include "gcomm.h"

                                /* seasons, in the order they cycle         */
#define SEA_SPRING 0
#define SEA_SUMMER 1
#define SEA_AUTUMN 2
#define SEA_WINTER 3
#define SEA_COUNT  4

                                /* weather conditions                       */
#define WX_CLEAR 0
#define WX_RAIN  1
#define WX_FOG   2
#define WX_STORM 3
#define WX_SNOW  4
#define WX_HEAT  5
#define WX_COUNT 6

/* Open the weather state, creating FRACWX.DAT on first run and rolling an     */
/* opening condition so day one is never blank. `enabled` is the sysop master  */
/* toggle; `seaslen` is the number of days a season lasts. If a midnight passed */
/* while the host was down, the state is aged once on open (lazy catch-up).     */
VOID  wx_open(GBOOL enabled, INT seaslen);
VOID  wx_close(VOID);

/* The daily tick: age the season (advancing to the next when it runs out) and */
/* roll a fresh weather condition biased by the new season. Run once per night  */
/* from the midnight cleanup hook.                                              */
VOID  wx_advance(VOID);

GBOOL wx_enabled(VOID);         /* is weather turned on by the sysop?       */
INT   wx_weather(VOID);         /* current condition (WX_CLEAR when off)    */
INT   wx_season(VOID);          /* current season (SEA_*)                   */

const CHAR *wx_wname(INT weather);  /* weather display name                 */
const CHAR *wx_wdesc(INT weather);  /* one-line weather flavor              */
const CHAR *wx_sname(INT season);   /* season display name                  */

/* Action Points needed to travel one tile under today's weather (always >= 1; */
/* storms and snow add to it). Returns 1 when weather is disabled.             */
INT   wx_travcost(VOID);

/* Adjust a base gather amount for today's weather (rain helps, fog/heat/etc.  */
/* hurt). Returns the amount unchanged when weather is disabled; a find is      */
/* never reduced all the way to nothing unless the modifier is zero.           */
INT   wx_yield(INT amount);

#endif /* FRACTURE_WEATHER_H */
