/*****************************************************************************
 *   WORLD.C                                              Fracture - World    *
 *                                                                           *
 *   Storage and generation of the game map. Wraps the DFA (Btrieve) API so  *
 *   the rest of the module reads and writes tiles as plain `struct fractile` *
 *   values addressed by (x, y).                                             *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "bbsutils.h"                /* lngrnd() - ranged random numbers     */
#include "WORLD.H"

#define WLDFIL "FRACWLD.DAT"         /* world data file                      */
#define WLDPGS 1024                  /* Btrieve page size (multiple of 512)  */

static DFAFILE *wlddat;              /* open world data file handle          */
static INT      worldsz;            /* configured world dimension           */

/* terrain display names, indexed by TER_* */
static const CHAR *ternam[TER_COUNT] = {
    "plains", "forest", "mountains", "desert", "swamp", "water", "ruins"
};

/* terrain map glyphs, indexed by TER_* */
static const CHAR terglyph[TER_COUNT] = {
    '.', 'f', '^', ':', '~', 'w', 'R'
};

/* pack an (x, y) coordinate into the tile key */
#define PACK(x, y) ((USHORT)((y) * WLDMAX + (x)))

/* ---------------------------------------------------------------------- *
 * gen_terrain (internal)                                                 *
 *                                                                        *
 * Pick a terrain type using weighted probabilities so that ordinary      *
 * ground is common and notable terrain (ruins) stays rare.               *
 * ---------------------------------------------------------------------- */
static UCHAR
gen_terrain(VOID)
{
    INT roll = (INT)lngrnd(1, 100);

    if (roll <= 35) return TER_PLAINS;     /* 35% */
    if (roll <= 60) return TER_FOREST;     /* 25% */
    if (roll <= 72) return TER_MOUNT;      /* 12% */
    if (roll <= 82) return TER_DESERT;     /* 10% */
    if (roll <= 90) return TER_SWAMP;      /*  8% */
    if (roll <= 97) return TER_WATER;      /*  7% */
    return TER_RUINS;                      /*  3% */
}

/* ---------------------------------------------------------------------- *
 * generate (internal)                                                    *
 *                                                                        *
 * Fill a freshly created world file with tiles. The central tile (every  *
 * player's starting point) is made open plains and pre-charted so the    *
 * first view is never blank.                                             *
 * ---------------------------------------------------------------------- */
static VOID
generate(VOID)
{
    struct fractile t;
    INT x, y;
    INT mid = worldsz / 2;

    dfaSetBlk(wlddat);
    for (y = 0; y < worldsz; y++) {
        for (x = 0; x < worldsz; x++) {
            t.coord   = PACK(x, y);
            t.terrain = gen_terrain();
            t.flags   = 0;
            if (x == mid && y == mid) {
                t.terrain = TER_PLAINS;
                t.flags   = TF_DISCOVERED;
            }
            dfaInsert(&t);
        }
    }
    dfaRstBlk();
}

/* ---------------------------------------------------------------------- *
 * wld_open                                                               *
 * ---------------------------------------------------------------------- */
VOID EXPORT
wld_open(INT size)
{
    GBOOL fresh;

    worldsz = size;
    fresh   = !isfile(WLDFIL);

    if (fresh) {
        struct dfaSegSpec seg;
        struct dfaKeySpec key;

        setmem(&seg, sizeof(seg), 0);
        seg.position = 0;                 /* coord is the first field         */
        seg.length   = sizeof(USHORT);
        seg.type     = DFAST_INTEGER;
        seg.flags    = 0;
        seg.nullChar = 0;

        key.flags     = 0;
        key.nSegments = 1;
        key.segs      = &seg;

        dfaCreateSpec(WLDFIL, FALSE, sizeof(struct fractile),
                      WLDPGS, 0, 0, 1, &key, NULL);
    }

    wlddat = dfaOpen(WLDFIL, sizeof(struct fractile), NULL);

    if (fresh)
        generate();
}

/* ---------------------------------------------------------------------- *
 * wld_close                                                              *
 * ---------------------------------------------------------------------- */
VOID EXPORT
wld_close(VOID)
{
    dfaClose(wlddat);
}

/* ---------------------------------------------------------------------- *
 * wld_size                                                               *
 * ---------------------------------------------------------------------- */
INT EXPORT
wld_size(VOID)
{
    return worldsz;
}

/* ---------------------------------------------------------------------- *
 * wld_get                                                                *
 *                                                                        *
 * Fetch the tile at (x, y). If the tile is missing (e.g. the configured  *
 * size grew after generation), return a blank plains tile rather than    *
 * failing.                                                                *
 * ---------------------------------------------------------------------- */
VOID EXPORT
wld_get(INT x, INT y, struct fractile *t)
{
    USHORT key = PACK(x, y);

    dfaSetBlk(wlddat);
    if (dfaAcqEQ(t, &key, 0)) {
        dfaUnlockCur();
    }
    else {
        setmem(t, sizeof(*t), 0);
        t->coord = key;
    }
    dfaRstBlk();
}

/* ---------------------------------------------------------------------- *
 * wld_put                                                                *
 * ---------------------------------------------------------------------- */
VOID EXPORT
wld_put(struct fractile *t)
{
    struct fractile cur;

    dfaSetBlk(wlddat);
    if (dfaAcqEQ(&cur, &t->coord, 0))
        dfaUpdate(t);
    else
        dfaInsert(t);
    dfaRstBlk();
}

/* ---------------------------------------------------------------------- *
 * wld_name / wld_glyph                                                   *
 * ---------------------------------------------------------------------- */
const CHAR * EXPORT
wld_name(INT terrain)
{
    if (terrain < 0 || terrain >= TER_COUNT)
        return "unknown";
    return ternam[terrain];
}

CHAR EXPORT
wld_glyph(INT terrain)
{
    if (terrain < 0 || terrain >= TER_COUNT)
        return '?';
    return terglyph[terrain];
}
