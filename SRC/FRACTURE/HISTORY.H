/*****************************************************************************
 *   HISTORY.H                                          Fracture - History    *
 *                                                                           *
 *   The server's permanent chronicle: a shared, append-only log of the      *
 *   notable things players do. Charting new ground and raising a camp to a   *
 *   new tier are recorded here so the world accumulates a history that all    *
 *   players read as daily news. Entries are never edited or deleted, so      *
 *   physical record order is chronological - the file is read newest-first   *
 *   for the news display.                                                     *
 *                                                                           *
 *   Stored in FRACHIST.DAT (a Btrieve file keyed on an assignment sequence)  *
 *   and accessed through the DFA API.                                         *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_HISTORY_H
#define FRACTURE_HISTORY_H

#include "gcomm.h"

                                /* chronicle event types                    */
#define HE_DISCOVER 0           /*   charted a previously unknown tile      */
#define HE_BUILD    1           /*   raised a camp to a new tier            */
#define HE_COUNT    2

struct frachist {               /* one server-history event                 */
    LONG   seq;                 /*   key 0: assignment order (chronological) */
    USHORT day;                 /*   packed today() of the event            */
    UCHAR  type;                /*   HE_* event type                        */
    UCHAR  arg;                 /*   terrain type (discover) or camp tier   */
    UCHAR  x;                   /*   map column (HE_DISCOVER)               */
    UCHAR  y;                   /*   map row    (HE_DISCOVER)               */
    UCHAR  pad1;                /*   reserved (keeps the record even)       */
    UCHAR  pad2;                /*   reserved                               */
    CHAR   who[UIDSIZ];         /*   user-id of the player responsible      */
};

VOID hist_open(VOID);           /* open the chronicle, creating it if new   */
VOID hist_close(VOID);          /* close the chronicle                      */

/* Append an event to the permanent chronicle. `arg` is the terrain type for  */
/* HE_DISCOVER or the new camp tier for HE_BUILD; x/y locate a discovery.     */
VOID hist_add(INT type, const CHAR *who, INT arg, INT x, INT y);

/* Render the most recent `max` events, newest first, to the current user.    */
/* The caller must have selected the module message block (setmbk).           */
VOID hist_news(INT max);

/* Erase the entire chronicle and restart numbering (sysop game reset).       */
VOID hist_clear(VOID);

#endif /* FRACTURE_HISTORY_H */
