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
 *                  machine, and handles clean shutdown. Game systems         *
 *                  (world, camp, exploration, research, economy) are added   *
 *                  as separate source files as development proceeds.         *
 *                                                                           *
 *   See LICENSE file in the project root for terms and conditions.          *
 *                                                                           *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "FRACTURE.H"

/* ---- forward declarations -------------------------------------------- */
GBOOL user_logon(VOID);              /* per-user logon supplemental routine  */
GBOOL module_main(VOID);             /* main input / menu routine            */
VOID  system_shutdown(VOID);         /* finish-up (system shutdown) routine  */
VOID  end_fracture(VOID);            /* empty marker to aid crash analysis   */

/* ---- module interface block ------------------------------------------ *
 * The host invokes these callbacks over a user's lifetime in the module. *
 * Unused hooks are NULL for now and will be filled in as systems land:    *
 * the midnight cleanup hook in particular is where Fracture's daily world *
 * evolution and Action Point reset will live.                            */
INT usrstt;                          /* per-user state id in this module     */
struct module FRACTURE = {
     "",                             /* description (set in init)            */
     user_logon,                     /* user logon supplemental routine      */
     module_main,                    /* input routine when module selected   */
     NULL,                           /* status-input routine                 */
     NULL,                           /* "injoth" routine                     */
     NULL,                           /* user logoff supplemental routine     */
     NULL,                           /* hangup (lost carrier) routine        */
     NULL,                           /* midnight cleanup (daily world tick)  */
     NULL,                           /* delete-account routine               */
     system_shutdown                 /* finish-up (system shutdown) routine  */
};

/* ---- module globals -------------------------------------------------- */
HMCVFILE fracmb;                     /* module message file handle           */
GBOOL    show_welcome;               /* sysop option: greet users at logon?  */

/* ---------------------------------------------------------------------- *
 * init__fracture                                                         *
 *                                                                        *
 * Required exported entry point. Its name must match the DLL filename.   *
 * Registers the module, opens the message file, and reads sysop options. *
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

    /* note startup in the audit log */
    shocst("Fracture", "Loaded v1.0.0 - Copyright (c) 2026 Sysop Network");
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
 * module_main                                                            *
 *                                                                        *
 * Drives the per-user menu state machine while the user is in Fracture.  *
 * Returns 1 to remain in the module, 0 to exit back to the BBS.          *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
module_main(VOID)
{
    setmbk(fracmb);

    switch (usrptr->substt) {
    case 0:
        /* first entry: show the main menu and advance the state */
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
        else if (sameas(margv[0], "l")) {
            prfmsg(STATUS);             /* "look" at the world / camp status  */
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
 * system_shutdown                                                        *
 *                                                                        *
 * Releases module resources when the system shuts down.                  *
 * ---------------------------------------------------------------------- */
VOID EXPORT
system_shutdown(VOID)
{
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
