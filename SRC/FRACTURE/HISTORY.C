/*****************************************************************************
 *   HISTORY.C                                          Fracture - History    *
 *                                                                           *
 *   Storage and presentation of the server's shared chronicle. Wraps the     *
 *   DFA (Btrieve) API so the rest of the module records events as plain       *
 *   calls and renders the news without touching data-file mechanics.          *
 *                                                                           *
 *   Records are only ever inserted, so the file's physical order is the       *
 *   order events happened. The news view walks that order in reverse (newest  *
 *   first) using the DFA "step" operations.                                   *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "FRACTURE.H"
#include "WORLD.H"
#include "CAMP.H"
#include "HISTORY.H"
#include "TVARS.H"

#define HISTFIL "FRACHIST.DAT"       /* chronicle data file                  */
#define HISTPGS 1024                 /* Btrieve page size (multiple of 512)  */

static DFAFILE *histdat;             /* open chronicle data file handle      */
static LONG     nextseq;             /* sequence number for the next entry   */

/* ---------------------------------------------------------------------- *
 * hist_maxseq (internal)                                                 *
 *                                                                        *
 * Return the highest sequence number in the file, or 0 if empty. This    *
 * scans every record rather than trusting the physically-last one: once  *
 * hist_clear can delete entries, physical order no longer matches key    *
 * order, so the last physical record is not necessarily the highest seq  *
 * (a wrong guess here would hand out a duplicate key on the next insert). *
 * ---------------------------------------------------------------------- */
static LONG
hist_maxseq(VOID)
{
    struct frachist h;
    LONG mx = 0;

    dfaSetBlk(histdat);
    if (dfaStepLO(&h)) {
        do {
            if (h.seq > mx)
                mx = h.seq;
        } while (dfaStepNX(&h));
    }
    dfaRstBlk();
    return mx;
}

/* ---------------------------------------------------------------------- *
 * hist_open                                                              *
 *                                                                        *
 * Opens the chronicle, creating it on first run with a single integer    *
 * key on the assignment sequence. The next sequence number is resumed    *
 * from the most recent existing entry so numbering survives restarts.    *
 * ---------------------------------------------------------------------- */
VOID EXPORT
hist_open(VOID)
{
    if (!isfile(HISTFIL)) {
        struct dfaSegSpec seg;       /* the sequence key segment             */
        struct dfaKeySpec key;       /* the single key built from it         */

        setmem(&seg, sizeof(seg), 0);
        seg.position = 0;            /* seq is the first field                */
        seg.length   = sizeof(LONG);
        seg.type     = DFAST_INTEGER;
        seg.flags    = 0;
        seg.nullChar = 0;

        key.flags     = 0;
        key.nSegments = 1;
        key.segs      = &seg;

        dfaCreateSpec(HISTFIL, FALSE, sizeof(struct frachist),
                      HISTPGS, 0, 0, 1, &key, NULL);
    }

    histdat = dfaOpen(HISTFIL, sizeof(struct frachist), NULL);

    /* resume the sequence after the highest existing entry (by key, not by  *
     * physical position - see hist_maxseq)                                   */
    nextseq = hist_maxseq() + 1;
}

/* ---------------------------------------------------------------------- *
 * hist_close                                                             *
 * ---------------------------------------------------------------------- */
VOID EXPORT
hist_close(VOID)
{
    dfaClose(histdat);
}

/* ---------------------------------------------------------------------- *
 * hist_add                                                               *
 *                                                                        *
 * Append an event to the permanent chronicle. The event keeps just the   *
 * structured facts (who, when, what kind, terrain/tier, location); the    *
 * news view turns those into readable lines, so the wording stays in the  *
 * sysop-editable message file.                                            *
 * ---------------------------------------------------------------------- */
VOID EXPORT
hist_add(INT type, const CHAR *who, INT arg, INT x, INT y)
{
    struct frachist h;

    setmem(&h, sizeof(h), 0);
    h.seq  = nextseq++;
    h.day  = today();
    h.type = (UCHAR)type;
    h.arg  = (UCHAR)arg;
    h.x    = (UCHAR)x;
    h.y    = (UCHAR)y;
    stzcpy(h.who, (CHAR *)who, UIDSIZ);

    dfaSetBlk(histdat);
    dfaInsert(&h);
    dfaRstBlk();
}

/* ---------------------------------------------------------------------- *
 * hist_news                                                              *
 *                                                                        *
 * Render up to `max` of the most recent chronicle entries, newest first. *
 * Because entries are only ever inserted, the highest physical record is  *
 * the latest event and stepping to the previous record walks backward in  *
 * time. ncdate() formats the stored packed date for display.              *
 * ---------------------------------------------------------------------- */
VOID EXPORT
hist_news(INT max)
{
    struct frachist h;
    INT shown = 0;

    prfmsg(NEWSHDR);

    dfaSetBlk(histdat);
    if (dfaStepHI(&h)) {
        do {
            tvar_set(TV_DATE, ncdate(h.day));
            tvar_set(TV_WHO, h.who);
            switch (h.type) {
            case HE_DISCOVER:
                tvar_set(TV_TERRAIN, wld_name(h.arg));
                tvar_setn(TV_X, (LONG)h.x);
                tvar_setn(TV_Y, (LONG)h.y);
                prfmsg(NEWSDSC);
                break;
            case HE_BUILD:
                tvar_set(TV_CAMP, camp_name(h.arg));
                prfmsg(NEWSBLD);
                break;
            }
            shown++;
        } while (shown < max && dfaStepPR(&h));
    }
    dfaRstBlk();

    if (shown == 0)
        prfmsg(NEWSNON);
    prfmsg(NEWSFTR);
}

/* ---------------------------------------------------------------------- *
 * hist_clear                                                             *
 *                                                                        *
 * Delete every chronicle entry and restart the sequence, for the sysop   *
 * "reset game" command. Re-fetch the lowest record each pass so the       *
 * cursor is never left dangling on a just-deleted record.                 *
 * ---------------------------------------------------------------------- */
VOID EXPORT
hist_clear(VOID)
{
    struct frachist h;

    dfaSetBlk(histdat);
    while (dfaStepLO(&h)) {
        dfaAcqEQ(NULL, &h.seq, 0);
        dfaDelete();
    }
    dfaRstBlk();
    /* restart at 1 if the file truly emptied; if any record survived, resume *
     * past it so we never hand out a duplicate key (see hist_maxseq)          */
    nextseq = hist_maxseq() + 1;
}
