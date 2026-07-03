/*****************************************************************************
 *   PLAYER.C                                            Fracture - Players   *
 *                                                                           *
 *   Storage layer for persistent player records. Wraps the DFA (Btrieve)    *
 *   API so the rest of the module works with plain `struct fracplyr`        *
 *   values and never touches data-file mechanics directly.                  *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "PLAYER.H"

#define PLYRFIL "FRACTURE.DAT"       /* player data file                     */
#define PLYRPGS 1024                 /* Btrieve page size (multiple of 512)  */
#define PL_RAIDMAX 25                /* cap the raid list (bounds prfbuf use) */
#define PL_ALLMAX 128                /* cap for a bulk/board pass (ample)     */

static DFAFILE *plyrdat;             /* open player data file handle         */

/* ---------------------------------------------------------------------- *
 * pl_open                                                                *
 *                                                                        *
 * Opens the player data file, creating it on first run. The file is      *
 * created programmatically (no shipped virgin file is required) with a    *
 * single string key on the user-id at the start of the record.           *
 * ---------------------------------------------------------------------- */
VOID EXPORT
pl_open(VOID)
{
    if (!isfile(PLYRFIL)) {
        struct dfaSegSpec seg;       /* the user-id key segment              */
        struct dfaKeySpec key;       /* the single key built from it         */

        setmem(&seg, sizeof(seg), 0);
        seg.position = 0;            /* user-id is the first field            */
        seg.length   = UIDSIZ;
        seg.type     = DFAST_STRING;
        seg.flags    = 0;
        seg.nullChar = 0;

        key.flags     = DFAKF_MODIFYABLE;
        key.nSegments = 1;
        key.segs      = &seg;

        dfaCreateSpec(PLYRFIL, FALSE, sizeof(struct fracplyr),
                      PLYRPGS, 0, 0, 1, &key, NULL);
    }
    plyrdat = dfaOpen(PLYRFIL, sizeof(struct fracplyr), NULL);
}

/* ---------------------------------------------------------------------- *
 * pl_close                                                               *
 * ---------------------------------------------------------------------- */
VOID EXPORT
pl_close(VOID)
{
    dfaClose(plyrdat);
}

/* ---------------------------------------------------------------------- *
 * pl_read (internal)                                                     *
 *                                                                        *
 * Reads the record for userid into *p. Returns TRUE if found. The record *
 * is released again immediately, since this is a read-only lookup.       *
 * ---------------------------------------------------------------------- */
static GBOOL
pl_read(const CHAR *userid, struct fracplyr *p)
{
    GBOOL found;

    dfaSetBlk(plyrdat);
    found = dfaAcqEQ(p, userid, 0);
    if (found)
        dfaUnlockCur();          /* read-only: release the record we locked */
    dfaRstBlk();
    return found;
}

/* ---------------------------------------------------------------------- *
 * pl_write                                                               *
 *                                                                        *
 * Saves *p: updates the existing record, or inserts it if new.           *
 * ---------------------------------------------------------------------- */
VOID EXPORT
pl_write(struct fracplyr *p)
{
    struct fracplyr cur;             /* scratch buffer for the lookup        */

    dfaSetBlk(plyrdat);
    if (dfaAcqEQ(&cur, p->userid, 0))
        dfaUpdate(p);
    else
        dfaInsert(p);
    dfaRstBlk();
}

/* ---------------------------------------------------------------------- *
 * pl_ensure                                                              *
 *                                                                        *
 * Loads the player for userid, creating the record on first visit and    *
 * refilling Action Points when a new calendar day has started (a lazy    *
 * daily reset keyed on the packed value returned by today()).            *
 * Returns TRUE when a brand-new explorer was just created.               *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
pl_ensure(const CHAR *userid, struct fracplyr *p, INT dailyap)
{
    GBOOL isnew = 0;

    if (!pl_read(userid, p)) {
        /* first time this user has entered Fracture */
        setmem(p, sizeof(*p), 0);
        stzcpy(p->userid, (CHAR *)userid, UIDSIZ);
        p->lastday = today();
        p->actpts  = dailyap;
        pl_write(p);
        isnew = 1;
    }
    else if (p->lastday != today()) {
        /* a new day has begun since the player last acted: refill */
        p->lastday = today();
        p->actpts  = dailyap;
        pl_write(p);
    }
    return isnew;
}

/* ---------------------------------------------------------------------- *
 * pl_load                                                                *
 *                                                                        *
 * Public read-only fetch by user-id (no create, no AP refill), for       *
 * reading and modifying another player such as a raid target.            *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
pl_load(const CHAR *userid, struct fracplyr *p)
{
    return pl_read(userid, p);
}

/* ---------------------------------------------------------------------- *
 * pl_load_ci                                                            *
 *                                                                        *
 * Like pl_load, but matches the user-id case-insensitively by scanning   *
 * the file, so a raider need not reproduce the target's exact casing     *
 * ("himmey" finds "Himmey"). Fills *p with the record in its stored      *
 * form (so callers get the canonical user-id back). Returns TRUE on a    *
 * match. A linear scan is fine here: the roster is small and a raid is    *
 * an occasional action.                                                   *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
pl_load_ci(const CHAR *userid, struct fracplyr *p)
{
    struct fracplyr scan;
    GBOOL found = 0;

    dfaSetBlk(plyrdat);
    if (dfaStepLO(&scan)) {
        do {
            if (sameas(scan.userid, (CHAR *)userid)) {
                *p    = scan;
                found = 1;
                break;
            }
        } while (dfaStepNX(&scan));
    }
    dfaRstBlk();
    return found;
}

/* ---------------------------------------------------------------------- *
 * pl_raidlist                                                            *
 *                                                                        *
 * Walk every player record and print the ones a raider may target: not   *
 * the caller, and with a camp of at least `mincamp`. Returns the count.   *
 * ---------------------------------------------------------------------- */
INT EXPORT
pl_raidlist(const CHAR *selfid, INT mincamp)
{
    struct fracplyr p;
    INT shown = 0;

    dfaSetBlk(plyrdat);
    if (dfaStepLO(&p)) {
        do {
            if ((INT)p.camplvl >= mincamp && !sameas(p.userid, (CHAR *)selfid)) {
                prf("  \x1b[1;31m*\x1b[0;37m \x1b[1;37m%-20s\x1b[0;37m "
                    "camp \x1b[1;37m%d\x1b[0;37m   Fame \x1b[1;33m%ld\x1b[0;37m\r\n",
                    p.userid, (INT)p.camplvl, p.fame);
                shown++;
            }
            /* stop once the list is full, so a big roster cannot overrun the  *
             * prf output buffer (the news view caps itself the same way)      */
        } while (shown < PL_RAIDMAX && dfaStepNX(&p));
    }
    dfaRstBlk();
    return shown;
}

/* ---------------------------------------------------------------------- *
 * pl_leaderboard                                                        *
 *                                                                        *
 * Print the highest-Fame players, marking the caller, and return the     *
 * caller's own rank. Records are read into a lean local array (just the   *
 * fields the board shows) and insertion-sorted by Fame; the roster is     *
 * small so an in-memory sort is simplest.                                *
 * ---------------------------------------------------------------------- */
INT EXPORT
pl_leaderboard(const CHAR *selfid, INT max)
{
    struct lbent { CHAR uid[UIDSIZ]; LONG fame; UCHAR camp; };
    struct lbent b[PL_ALLMAX], tmp;
    struct fracplyr p;
    INT n = 0, i, j, selfrank = 0;

    dfaSetBlk(plyrdat);
    if (dfaStepLO(&p)) {
        do {
            stzcpy(b[n].uid, p.userid, UIDSIZ);
            b[n].fame = p.fame;
            b[n].camp = p.camplvl;
        } while (++n < PL_ALLMAX && dfaStepNX(&p));
    }
    dfaRstBlk();

    for (i = 1; i < n; i++) {             /* insertion sort, Fame descending    */
        tmp = b[i];
        j = i - 1;
        while (j >= 0 && b[j].fame < tmp.fame) {
            b[j + 1] = b[j];
            j--;
        }
        b[j + 1] = tmp;
    }

    for (i = 0; i < n; i++)               /* find the caller's rank, even off-list */
        if (sameas(b[i].uid, (CHAR *)selfid)) {
            selfrank = i + 1;
            break;
        }

    for (i = 0; i < n && i < max; i++) {
        if (sameas(b[i].uid, (CHAR *)selfid))
            prf("  \x1b[1;33m%2d. %-20s Fame %ld   camp %d   <- you\x1b[0;37m\n",
                i + 1, b[i].uid, b[i].fame, (INT)b[i].camp);
        else
            prf("  \x1b[1;37m%2d.\x1b[0;37m %-20s Fame \x1b[1;33m%ld\x1b[0;37m"
                "   camp \x1b[1;37m%d\x1b[0;37m\n",
                i + 1, b[i].uid, b[i].fame, (INT)b[i].camp);
    }
    return selfrank;
}

/* ---------------------------------------------------------------------- *
 * pl_delete                                                              *
 *                                                                        *
 * Removes a player's record (called when the BBS account is deleted).    *
 * ---------------------------------------------------------------------- */
VOID EXPORT
pl_delete(const CHAR *userid)
{
    dfaSetBlk(plyrdat);
    if (dfaAcqEQ(NULL, userid, 0))
        dfaDelete();
    dfaRstBlk();
}

/* ---------------------------------------------------------------------- *
 * pl_wipeall                                                            *
 *                                                                        *
 * Delete every player record - the destructive half of a sysop game     *
 * reset. Re-fetch the lowest record each pass so the cursor is never     *
 * left pointing at a record we just removed.                             *
 * ---------------------------------------------------------------------- */
VOID EXPORT
pl_wipeall(VOID)
{
    struct fracplyr p;

    dfaSetBlk(plyrdat);
    while (dfaStepLO(&p)) {
        dfaAcqEQ(NULL, p.userid, 0);
        dfaDelete();
    }
    dfaRstBlk();
}

/* ---------------------------------------------------------------------- *
 * pl_ap_all                                                             *
 *                                                                        *
 * Set every player's Action-Point pool to `ap` (sysop "give AP to all"). *
 * ---------------------------------------------------------------------- */
VOID EXPORT
pl_ap_all(INT ap)
{
    struct fracplyr p;
    CHAR ids[PL_ALLMAX][UIDSIZ];
    INT  n = 0, i;

    /* Phase 1: collect user-ids with a read-only walk, so phase 2's keyed     *
     * writes never modify the file mid-traversal (stepping + modifying is      *
     * the pattern the world code deliberately avoids).                         */
    dfaSetBlk(plyrdat);
    if (dfaStepLO(&p)) {
        do {
            stzcpy(ids[n], p.userid, UIDSIZ);
        } while (++n < PL_ALLMAX && dfaStepNX(&p));
    }
    dfaRstBlk();

    /* Phase 2: fetch and rewrite each by key (the proven pl_write pattern).    */
    for (i = 0; i < n; i++) {
        if (pl_read(ids[i], &p)) {
            p.actpts = ap;
            pl_write(&p);
        }
    }
}
