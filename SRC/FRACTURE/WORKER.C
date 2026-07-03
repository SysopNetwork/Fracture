/*****************************************************************************
 *   WORKER.C                                          Fracture - Workers     *
 *                                                                           *
 *   The crew system: NPC workers who passively produce resources. Each        *
 *   player's roster (a count per worker type plus the day production last      *
 *   ran) is one record in FRACWRK.DAT, keyed on the user-id via the DFA API.  *
 *                                                                           *
 *   Production runs one day at a time, lazily, on the first entry of a new     *
 *   day - the same pattern the daily Action-Point reset uses - so there is no  *
 *   mass sweep of every player at midnight and long absences do not compound   *
 *   into windfalls. The day's weather scales output, and the crew eats Food.   *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "WORKER.H"
#include "CAMP.H"
#include "RESOURCE.H"
#include "WEATHER.H"

#define WRKFIL "FRACWRK.DAT"         /* crew data file                       */
#define WRKPGS 1024                  /* Btrieve page size (matches the MDF)  */

struct fracwrk {                     /* one player's crew                    */
    CHAR   userid[UIDSIZ];           /*   key 0: BBS user-id                  */
    USHORT prodday;                  /*   packed today() production last ran  */
    UCHAR  count[WRK_COUNT];         /*   workers of each type                */
    UCHAR  pad;                      /*   reserved (keeps the record even)    */
};

static DFAFILE *wrkdat;              /* open crew data file handle           */
static GBOOL    wrkon;               /* sysop master toggle                  */
static INT      prod_per;           /* base units per worker per day         */
static INT      food_up;            /* Food eaten per worker per day          */
static INT      cost_wood;          /* Wood cost to recruit one worker        */
static INT      cost_food;          /* Food cost to recruit one worker        */

/* worker display names, indexed by WRK_* */
static const CHAR *wrknam[WRK_COUNT] = {
    "Forester", "Miner", "Prospector", "Farmer", "Water Carrier"
};

/* the resource each worker type produces, indexed by WRK_* */
static const INT wrkres[WRK_COUNT] = {
    RES_WOOD, RES_STONE, RES_METAL, RES_FOOD, RES_WATER
};

/* ---------------------------------------------------------------------- *
 * wrk_open                                                               *
 * ---------------------------------------------------------------------- */
VOID EXPORT
wrk_open(GBOOL enabled, INT perday, INT foodup, INT costwood, INT costfood)
{
    wrkon     = enabled;
    prod_per  = perday;
    food_up   = foodup;
    cost_wood = costwood;
    cost_food = costfood;

    if (!isfile(WRKFIL)) {
        struct dfaSegSpec seg;       /* the user-id key segment              */
        struct dfaKeySpec key;

        setmem(&seg, sizeof(seg), 0);
        seg.position = 0;            /* user-id is the first field            */
        seg.length   = UIDSIZ;
        seg.type     = DFAST_STRING;
        seg.flags    = 0;
        seg.nullChar = 0;

        key.flags     = DFAKF_MODIFYABLE;
        key.nSegments = 1;
        key.segs      = &seg;

        dfaCreateSpec(WRKFIL, FALSE, sizeof(struct fracwrk),
                      WRKPGS, 0, 0, 1, &key, NULL);
    }
    wrkdat = dfaOpen(WRKFIL, sizeof(struct fracwrk), NULL);
}

/* ---------------------------------------------------------------------- *
 * wrk_close                                                              *
 * ---------------------------------------------------------------------- */
VOID EXPORT
wrk_close(VOID)
{
    dfaClose(wrkdat);
}

/* ---------------------------------------------------------------------- *
 * wrk_load (internal)                                                    *
 *                                                                        *
 * Read the crew record for userid into *w. Returns TRUE if one exists;    *
 * otherwise *w is zeroed with the user-id set, ready to be inserted.      *
 * ---------------------------------------------------------------------- */
static GBOOL
wrk_load(const CHAR *userid, struct fracwrk *w)
{
    GBOOL found;

    dfaSetBlk(wrkdat);
    found = dfaAcqEQ(w, (VOID *)userid, 0);
    if (found)
        dfaUnlockCur();
    dfaRstBlk();

    if (!found) {
        setmem(w, sizeof(*w), 0);
        stzcpy(w->userid, (CHAR *)userid, UIDSIZ);
    }
    return found;
}

/* ---------------------------------------------------------------------- *
 * wrk_save (internal)                                                    *
 * ---------------------------------------------------------------------- */
static VOID
wrk_save(struct fracwrk *w)
{
    struct fracwrk cur;

    dfaSetBlk(wrkdat);
    if (dfaAcqEQ(&cur, w->userid, 0))
        dfaUpdate(w);
    else
        dfaInsert(w);
    dfaRstBlk();
}

/* ---------------------------------------------------------------------- *
 * simple accessors                                                       *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
wrk_enabled(VOID)
{
    return wrkon;
}

const CHAR * EXPORT
wrk_name(INT type)
{
    if (type < 0 || type >= WRK_COUNT)
        return "?";
    return wrknam[type];
}

INT EXPORT
wrk_resource(INT type)
{
    if (type < 0 || type >= WRK_COUNT)
        return RES_FOOD;
    return wrkres[type];
}

INT EXPORT
wrk_perday(VOID)
{
    return prod_per;
}

INT EXPORT
wrk_costwood(VOID)
{
    return cost_wood;
}

INT EXPORT
wrk_costfood(VOID)
{
    return cost_food;
}

/* ---------------------------------------------------------------------- *
 * wrk_counts                                                             *
 * ---------------------------------------------------------------------- */
INT EXPORT
wrk_counts(const CHAR *userid, INT count[WRK_COUNT])
{
    struct fracwrk w;
    INT i, total = 0;

    wrk_load(userid, &w);
    for (i = 0; i < WRK_COUNT; i++) {
        count[i] = (INT)w.count[i];
        total += count[i];
    }
    return total;
}

/* ---------------------------------------------------------------------- *
 * wrk_hire                                                               *
 * ---------------------------------------------------------------------- */
INT EXPORT
wrk_hire(struct fracplyr *p, INT type)
{
    struct fracwrk w;
    INT i, total = 0;

    if (!wrkon)
        return WHIRE_OFF;
    if (type < 0 || type >= WRK_COUNT)
        return WHIRE_BAD;

    wrk_load(p->userid, &w);
    for (i = 0; i < WRK_COUNT; i++)
        total += (INT)w.count[i];

    if (total >= camp_workers(p->camplvl))
        return WHIRE_FULL;
    if (p->res[RES_WOOD] < cost_wood || p->res[RES_FOOD] < cost_food)
        return WHIRE_POOR;

    p->res[RES_WOOD] -= cost_wood;
    p->res[RES_FOOD] -= cost_food;
    w.count[type]++;
    if (w.prodday == 0)
        w.prodday = (USHORT)today();   /* new crew: production starts tomorrow */
    wrk_save(&w);
    return WHIRE_OK;
}

/* ---------------------------------------------------------------------- *
 * wrk_daily                                                              *
 * ---------------------------------------------------------------------- */
INT EXPORT
wrk_daily(struct fracplyr *p, GBOOL *hungry)
{
    struct fracwrk w;
    LONG cap;
    INT  i, crew = 0, gained = 0, need;

    *hungry = 0;
    if (!wrkon)
        return 0;

    /* No record, or production already ran today: nothing to do.           */
    if (!wrk_load(p->userid, &w))
        return 0;
    if (w.prodday == (USHORT)today())
        return 0;

    cap = camp_storage(p->camplvl);

    for (i = 0; i < WRK_COUNT; i++) {
        INT  n = (INT)w.count[i];
        INT  out;
        INT  r;
        LONG before;

        if (n <= 0)
            continue;
        crew += n;

        out = wx_yield(n * prod_per);     /* the day's weather scales output */
        if (out <= 0)
            continue;

        r      = wrkres[i];
        before = p->res[r];
        p->res[r] += out;
        if (p->res[r] > cap)
            p->res[r] = cap;              /* camp storage is the cap         */
        gained += (INT)(p->res[r] - before);
    }

    /* the whole crew eats, after the farmers have brought Food in           */
    need = crew * food_up;
    if (p->res[RES_FOOD] >= need) {
        p->res[RES_FOOD] -= need;
    }
    else {
        p->res[RES_FOOD] = 0;
        if (crew > 0)
            *hungry = 1;
    }

    w.prodday = (USHORT)today();
    wrk_save(&w);
    return gained;
}

/* ---------------------------------------------------------------------- *
 * wrk_delete                                                             *
 * ---------------------------------------------------------------------- */
VOID EXPORT
wrk_delete(const CHAR *userid)
{
    dfaSetBlk(wrkdat);
    if (dfaAcqEQ(NULL, (VOID *)userid, 0))
        dfaDelete();
    dfaRstBlk();
}
