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

#define HISTFIL "FRACHIST.DAT"       /* chronicle data file                  */
#define HISTPGS 1024                 /* Btrieve page size (multiple of 512)  */

static DFAFILE *histdat;             /* open chronicle data file handle      */
static LONG     nextseq;             /* sequence number for the next entry   */

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
    struct frachist h;

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

    /* resume the sequence after the most recent (highest physical) entry */
    nextseq = 1;
    dfaSetBlk(histdat);
    if (dfaStepHI(&h))
        nextseq = h.seq + 1;
    dfaRstBlk();
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
            switch (h.type) {
            case HE_DISCOVER:
                prfmsg(NEWSDSC, ncdate(h.day), h.who,
                       wld_name(h.arg), (INT)h.x, (INT)h.y);
                break;
            case HE_BUILD:
                prfmsg(NEWSBLD, ncdate(h.day), h.who, camp_name(h.arg));
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
