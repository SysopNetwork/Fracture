/*****************************************************************************
 *   FRACTURE.C   V1.0                The Major BBS v10 Door Game - Fracture  *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                 *
 *                                                                           *
 *   Description:   Fracture is a persistent, multiplayer world-exploration  *
 *                  and survival-strategy door game for The Major BBS v10.    *
 *                                                                           *
 *                  This file is the module entry point. It registers the     *
 *                  module with the host, wires up the per-user menu state    *
 *                  machine, and drives the core daily loop: each player      *
 *                  receives Action Points and spends them exploring.         *
 *                  Storage of player state lives in PLAYER.C; further game   *
 *                  systems (world, camp, research, economy) are added as     *
 *                  separate source files as development proceeds.            *
 *                                                                           *
 *   See LICENSE file in the project root for terms and conditions.          *
 *                                                                           *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "bbsutils.h"                /* lngrnd() - ranged random numbers     */
#include "FRACTURE.H"
#include "PLAYER.H"

/* ---- forward declarations -------------------------------------------- */
GBOOL user_logon(VOID);              /* per-user logon supplemental routine  */
GBOOL module_main(VOID);             /* main input / menu routine            */
VOID  user_deleted(CHAR *userid);    /* delete-account routine               */
VOID  do_explore(struct fracplyr *p);/* perform one Explore action           */
VOID  system_shutdown(VOID);         /* finish-up (system shutdown) routine  */
VOID  end_fracture(VOID);            /* empty marker to aid crash analysis   */

/* ---- module interface block ------------------------------------------ *
 * The host invokes these callbacks over a user's lifetime in the module. *
 * The midnight cleanup hook is still NULL: Fracture resets Action Points *
 * lazily per player (see PLAYER.C), so no server-wide nightly pass is    *
 * needed yet. It will host world evolution once the world model lands.   */
INT usrstt;                          /* per-user state id in this module     */
struct module FRACTURE = {
     "",                             /* description (set in init)            */
     user_logon,                     /* user logon supplemental routine      */
     module_main,                    /* input routine when module selected   */
     NULL,                           /* status-input routine                 */
     NULL,                           /* "injoth" routine                     */
     NULL,                           /* user logoff supplemental routine     */
     NULL,                           /* hangup (lost carrier) routine        */
     NULL,                           /* midnight cleanup (future world tick) */
     user_deleted,                   /* delete-account routine               */
     system_shutdown                 /* finish-up (system shutdown) routine  */
};

/* ---- module globals -------------------------------------------------- */
HMCVFILE fracmb;                     /* module message file handle           */
GBOOL    show_welcome;               /* sysop option: greet users at logon?  */
INT      daily_ap;                   /* sysop option: Action Points per day  */

/* ---------------------------------------------------------------------- *
 * init__fracture                                                         *
 *                                                                        *
 * Required exported entry point. Its name must match the DLL filename.   *
 * Registers the module, opens the message and data files, and reads the  *
 * sysop-configurable options.                                            *
 * ---------------------------------------------------------------------- */
VOID EXPORT
init__fracture(VOID)
{
    /* register the module with the host using the description from the MDF */
    stzcpy(FRACTURE.descrp, gmdnam("FRACTURE.MDF"), MNMSIZ);
    usrstt = register_module(&FRACTURE);

    /* open the compiled message file and read configurable options */
    fracmb = opnmsg("FRACTURE.MCV");
    show_welcome = ynopt(WELCOPT);
    daily_ap     = numopt(DAYAP, 1, 200);

    /* open (creating if needed) the persistent player data file */
    pl_open();

    /* note startup in the audit log */
    shocst("Fracture", "Loaded v1.0.0 - Copyright (C) 2026 SysopNetwork.com");
}

/* ---------------------------------------------------------------------- *
 * user_logon                                                             *
 *                                                                        *
 * Optionally greets the user as they enter the BBS (not the module).     *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
user_logon(VOID)
{
    if (show_welcome) {
        setmbk(fracmb);
        prfmsg(LOGON);
        outprf(usrnum);
        rstmbk();
    }
    return 0;
}

/* ---------------------------------------------------------------------- *
 * do_explore                                                             *
 *                                                                        *
 * Spends one Action Point to explore. Outcomes are weighted so most      *
 * explorations yield something useful (the design avoids long streaks    *
 * of failure): ~10% turn up nothing, ~75% a minor find worth a little    *
 * Fame, and ~15% a notable discovery worth more. The updated record is   *
 * persisted before returning.                                            *
 * ---------------------------------------------------------------------- */
VOID EXPORT
do_explore(struct fracplyr *p)
{
    INT roll;
    INT gain;

    if (p->actpts <= 0) {
        prfmsg(NOAP);
        return;
    }

    p->actpts--;
    p->explores++;

    roll = (INT)lngrnd(1, 100);
    if (roll <= 10) {
        prfmsg(EXPNADA);                     /* nothing of note this time     */
    }
    else if (roll <= 85) {
        gain = (INT)lngrnd(1, 3);            /* minor find                    */
        p->fame += gain;
        prfmsg(EXPRES, gain);
    }
    else {
        gain = (INT)lngrnd(10, 25);          /* notable discovery             */
        p->fame += gain;
        p->discoveries++;
        prfmsg(EXPDISC, gain);
    }

    pl_write(p);
}

/* ---------------------------------------------------------------------- *
 * module_main                                                            *
 *                                                                        *
 * Drives the per-user menu state machine while the user is in Fracture.  *
 * The current player's record is loaded fresh on every call (MBBS runs   *
 * one user's routine at a time, so a stack copy is the safe scope), and  *
 * any action that changes it saves through PLAYER.C.                      *
 * Returns 1 to remain in the module, 0 to exit back to the BBS.          *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
module_main(VOID)
{
    struct fracplyr me;              /* the current user's player record     */
    GBOOL isnew;

    setmbk(fracmb);
    isnew = pl_ensure(usaptr->userid, &me, daily_ap);

    switch (usrptr->substt) {
    case 0:
        /* first entry: greet a brand-new explorer, then show the menu */
        if (isnew)
            prfmsg(FIRSTM);
        prfmsg(usrptr->substt = MAINMEN);
        break;

    case MAINMEN:
        if (sameas(margv[0], "x")) {
            /* leave the module */
            prfmsg(EXITMSG, usaptr->userid);
            outprf(usrnum);
            rstmbk();
            return 0;
        }
        else if (sameas(margv[0], "\0") || sameas(margv[0], "")) {
            prfmsg(SHORTM);          /* empty input -> short prompt        */
        }
        else if (sameas(margv[0], "?")) {
            prfmsg(MAINMEN);           /* redisplay the full menu            */
            break;
        }
        else if (sameas(margv[0], "e")) {
            do_explore(&me);           /* spend an Action Point exploring    */
        }
        else if (sameas(margv[0], "l")) {
            /* show live status: name, Fame, Action Points, lifetime stats */
            prfmsg(STATUS, me.userid, me.fame, me.actpts,
                   me.explores, me.discoveries);
        }
        else {
            prfmsg(INVALID);            /* unrecognized selection             */
        }
        prfmsg(SHORTM);
        break;
    }

    outprf(usrnum);
    rstmbk();
    return 1;
}

/* ---------------------------------------------------------------------- *
 * user_deleted                                                           *
 *                                                                        *
 * Removes a player's saved record when their BBS account is deleted.     *
 * ---------------------------------------------------------------------- */
VOID EXPORT
user_deleted(CHAR *userid)
{
    pl_delete(userid);
}

/* ---------------------------------------------------------------------- *
 * system_shutdown                                                        *
 *                                                                        *
 * Releases module resources when the system shuts down.                  *
 * ---------------------------------------------------------------------- */
VOID EXPORT
system_shutdown(VOID)
{
    pl_close();
    clsmsg(fracmb);
}

/* ---------------------------------------------------------------------- *
 * end_fracture                                                           *
 *                                                                        *
 * Empty routine that marks the end of the module's code, which makes     *
 * GALEXCEP.OUT crash reports easier to interpret.                        *
 * ---------------------------------------------------------------------- */
VOID EXPORT
end_fracture(VOID)
{
}
