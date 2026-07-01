/*****************************************************************************
 *   WEATHER.C                                        Fracture - Weather      *
 *                                                                           *
 *   Server-wide daily weather and the season cycle behind it. One record in  *
 *   FRACWX.DAT holds the whole of it (date, season, weather, days into the    *
 *   season); every player reads the same condition for the day. The state     *
 *   turns over once a night from the midnight cleanup hook.                    *
 *                                                                           *
 *   Weather names, flavor, and the balance tables (which weather each season   *
 *   favors, how much it helps or hurts gathering, and what it adds to travel)  *
 *   live here as plain C tables so they can be tuned in one place.             *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "bbsutils.h"                /* lngrnd() - ranged random numbers     */
#include "WEATHER.H"

#define WXFIL "FRACWX.DAT"           /* weather/season state file            */
#define WXPGS 1024                   /* Btrieve page size (matches the MDF)  */
#define WXKEY 1                      /* the single record's constant key     */

struct fracwx {                      /* the one server-wide weather record   */
    USHORT id;                       /*   key 0: always WXKEY (single row)    */
    USHORT day;                      /*   packed today() this weather is for  */
    UCHAR  season;                   /*   current season (SEA_*)              */
    UCHAR  weather;                  /*   current weather (WX_*)              */
    USHORT seasday;                  /*   days elapsed in the current season  */
};

static DFAFILE *wxdat;               /* open state file handle               */
static GBOOL    wxon;                /* sysop master toggle                  */
static INT      seasonlen;           /* configured days per season           */
static struct fracwx cur;            /* cached copy of the one record        */

/* season display names, indexed by SEA_* */
static const CHAR *seanam[SEA_COUNT] = {
    "Spring", "Summer", "Autumn", "Winter"
};

/* weather display names, indexed by WX_* */
static const CHAR *wxnam[WX_COUNT] = {
    "Clear", "Rain", "Fog", "Storm", "Snow", "Heat Wave"
};

/* one-line weather flavor, indexed by WX_* */
static const CHAR *wxdsc[WX_COUNT] = {
    "the skies are calm",
    "steady rain feeds the land",
    "a heavy fog smothers the ground",
    "storms lash the open world",
    "snow blankets everything",
    "a blistering heat bakes the land"
};

/* Weather likelihood by season: a weight per condition. Rows sum to 100 for   */
/* readability but need not - a roll is taken against the row total. Snow is a  */
/* winter/autumn thing; heat belongs to spring/summer; every season can be     */
/* clear. Tuning the world's mood happens right here.                          */
static const UCHAR wxbias[SEA_COUNT][WX_COUNT] = {
    /*              Clear Rain Fog Storm Snow Heat */
    /* Spring */  {   30,  35, 20,   10,   0,   5 },
    /* Summer */  {   40,  15, 10,   10,   0,  25 },
    /* Autumn */  {   30,  25, 25,   15,   5,   0 },
    /* Winter */  {   25,   5, 20,   15,  35,   0 }
};

/* Extra Action Points a single move costs under each weather (base cost is 1). */
static const INT wxtrav[WX_COUNT] = { 0, 0, 0, 1, 1, 0 };

/* Percentage applied to what a search gathers under each weather. Rain makes   */
/* the land generous; fog, storms, snow, and heat make it stingy.               */
static const INT wxyld[WX_COUNT]  = { 100, 130, 70, 60, 80, 75 };

/* ---------------------------------------------------------------------- *
 * roll_weather (internal)                                                *
 *                                                                        *
 * Pick a weather condition for a season using that season's weight row.   *
 * ---------------------------------------------------------------------- */
static UCHAR
roll_weather(INT season)
{
    INT total = 0;
    INT r, i;

    if (season < 0 || season >= SEA_COUNT)
        return WX_CLEAR;

    for (i = 0; i < WX_COUNT; i++)
        total += wxbias[season][i];
    if (total <= 0)
        return WX_CLEAR;

    r = (INT)lngrnd(1, total);
    for (i = 0; i < WX_COUNT; i++) {
        r -= wxbias[season][i];
        if (r <= 0)
            return (UCHAR)i;
    }
    return WX_CLEAR;
}

/* ---------------------------------------------------------------------- *
 * wx_save (internal)                                                     *
 *                                                                        *
 * Persist the cached record, inserting it on first write and updating it  *
 * thereafter.                                                             *
 * ---------------------------------------------------------------------- */
static VOID
wx_save(VOID)
{
    struct fracwx tmp;
    USHORT key = WXKEY;

    dfaSetBlk(wxdat);
    if (dfaAcqEQ(&tmp, &key, 0))
        dfaUpdate(&cur);
    else
        dfaInsert(&cur);
    dfaRstBlk();
}

/* ---------------------------------------------------------------------- *
 * wx_open                                                                *
 * ---------------------------------------------------------------------- */
VOID EXPORT
wx_open(GBOOL enabled, INT seaslen)
{
    GBOOL  fresh;
    USHORT key = WXKEY;

    wxon      = enabled;
    seasonlen = (seaslen < 1) ? 1 : seaslen;
    fresh     = !isfile(WXFIL);

    if (fresh) {
        struct dfaSegSpec seg;
        struct dfaKeySpec skey;

        setmem(&seg, sizeof(seg), 0);
        seg.position = 0;                 /* id is the first field            */
        seg.length   = sizeof(USHORT);
        seg.type     = DFAST_INTEGER;
        seg.flags    = 0;
        seg.nullChar = 0;

        skey.flags     = 0;
        skey.nSegments = 1;
        skey.segs      = &seg;

        dfaCreateSpec(WXFIL, FALSE, sizeof(struct fracwx),
                      WXPGS, 0, 0, 1, &skey, NULL);
    }

    wxdat = dfaOpen(WXFIL, sizeof(struct fracwx), NULL);

    dfaSetBlk(wxdat);
    if (dfaAcqEQ(&cur, &key, 0)) {
        dfaUnlockCur();
        dfaRstBlk();
    }
    else {
        dfaRstBlk();
        /* first run: open in Spring and roll the opening condition         */
        setmem(&cur, sizeof(cur), 0);
        cur.id      = WXKEY;
        cur.season  = SEA_SPRING;
        cur.seasday = 0;
        cur.day     = (USHORT)today();
        cur.weather = roll_weather(cur.season);
        wx_save();
    }

    /* Lazy catch-up: if the stored day is not today, a nightly turnover was  *
     * missed (the host was down at midnight). Age the state once so returning *
     * players still meet fresh weather, mirroring the daily AP-reset pattern. */
    if (cur.day != (USHORT)today())
        wx_advance();
}

/* ---------------------------------------------------------------------- *
 * wx_close                                                               *
 * ---------------------------------------------------------------------- */
VOID EXPORT
wx_close(VOID)
{
    dfaClose(wxdat);
}

/* ---------------------------------------------------------------------- *
 * wx_advance                                                             *
 * ---------------------------------------------------------------------- */
VOID EXPORT
wx_advance(VOID)
{
    cur.seasday++;
    if (cur.seasday >= (USHORT)seasonlen) {
        cur.seasday = 0;
        cur.season  = (UCHAR)((cur.season + 1) % SEA_COUNT);
    }
    cur.weather = roll_weather(cur.season);
    cur.day     = (USHORT)today();
    wx_save();
}

/* ---------------------------------------------------------------------- *
 * simple state accessors                                                 *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
wx_enabled(VOID)
{
    return wxon;
}

INT EXPORT
wx_weather(VOID)
{
    return wxon ? (INT)cur.weather : WX_CLEAR;
}

INT EXPORT
wx_season(VOID)
{
    return (INT)cur.season;
}

const CHAR * EXPORT
wx_wname(INT weather)
{
    if (weather < 0 || weather >= WX_COUNT)
        return "unknown";
    return wxnam[weather];
}

const CHAR * EXPORT
wx_wdesc(INT weather)
{
    if (weather < 0 || weather >= WX_COUNT)
        return "";
    return wxdsc[weather];
}

const CHAR * EXPORT
wx_sname(INT season)
{
    if (season < 0 || season >= SEA_COUNT)
        return "unknown";
    return seanam[season];
}

/* ---------------------------------------------------------------------- *
 * wx_travcost                                                            *
 * ---------------------------------------------------------------------- */
INT EXPORT
wx_travcost(VOID)
{
    if (!wxon)
        return 1;
    return 1 + wxtrav[cur.weather];
}

/* ---------------------------------------------------------------------- *
 * wx_yield                                                               *
 * ---------------------------------------------------------------------- */
INT EXPORT
wx_yield(INT amount)
{
    INT pct, r;

    if (!wxon || amount <= 0)
        return amount;

    pct = wxyld[cur.weather];
    r   = (amount * pct) / 100;
    if (r < 1 && pct > 0)
        r = 1;                            /* weather rarely zeroes a find   */
    return r;
}
