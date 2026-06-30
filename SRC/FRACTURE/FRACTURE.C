/*****************************************************************************
 *   FRACTURE.C   V1.0                The Major BBS v10 Door Game - Fracture  *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                 *
 *                                                                           *
 *   Description:   Fracture is a persistent, multiplayer world-exploration  *
 *                  and survival-strategy door game for The Major BBS v10.    *
 *                                                                           *
 *                  This file is the module entry point and the per-user      *
 *                  menu loop. It moves the player across the tile map        *
 *                  (WORLD.C), searches tiles for finds, and renders the      *
 *                  local view. Player state is stored by PLAYER.C. Further   *
 *                  systems (camp, research, economy) are added as separate   *
 *                  source files as development proceeds.                     *
 *                                                                           *
 *   See LICENSE file in the project root for terms and conditions.          *
 *                                                                           *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "bbsutils.h"                /* lngrnd() - ranged random numbers     */
#include "FRACTURE.H"
#include "PLAYER.H"
#include "WORLD.H"
#include "RESOURCE.H"

/* ---- forward declarations -------------------------------------------- */
GBOOL user_logon(VOID);              /* per-user logon supplemental routine  */
GBOOL module_main(VOID);             /* main input / menu routine            */
VOID  user_deleted(CHAR *userid);    /* delete-account routine               */
VOID  do_move(struct fracplyr *p, INT dx, INT dy, const CHAR *dir);
VOID  do_search(struct fracplyr *p); /* search the current tile              */
VOID  do_look(struct fracplyr *p);   /* show the local map and tile          */
VOID  system_shutdown(VOID);         /* finish-up (system shutdown) routine  */
VOID  end_fracture(VOID);            /* empty marker to aid crash analysis   */

/* ---- module interface block ------------------------------------------ */
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
INT      world_sz;                   /* sysop option: world dimension        */

/* ---------------------------------------------------------------------- *
 * init__fracture                                                         *
 *                                                                        *
 * Required exported entry point. Its name must match the DLL filename.   *
 * Registers the module and opens the message and data files.            *
 * ---------------------------------------------------------------------- */
VOID EXPORT
init__fracture(VOID)
{
    stzcpy(FRACTURE.descrp, gmdnam("FRACTURE.MDF"), MNMSIZ);
    usrstt = register_module(&FRACTURE);

    fracmb = opnmsg("FRACTURE.MCV");
    show_welcome = ynopt(WELCOPT);
    daily_ap     = numopt(DAYAP, 1, 200);
    world_sz     = numopt(WLDSIZ, 8, WLDMAX);

    pl_open();                       /* persistent players                   */
    wld_open(world_sz);              /* the map (generated on first run)     */

    shocst("Fracture", "Loaded v1.0.0 - Copyright (C) 2026 SysopNetwork.com");
}

/* ---------------------------------------------------------------------- *
 * user_logon                                                             *
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
 * do_look                                                                *
 *                                                                        *
 * Render the player's surroundings: a header naming the current tile,    *
 * then a local map centered on the player. Charted tiles show a terrain  *
 * glyph, the player is '@', unexplored tiles are '?', and area beyond    *
 * the world edge is blank.                                               *
 * ---------------------------------------------------------------------- */
#define VIEWW 4                      /* columns shown each side of player    */
#define VIEWH 3                      /* rows shown above/below player        */

VOID EXPORT
do_look(struct fracplyr *p)
{
    struct fractile here;
    struct fractile t;
    CHAR row[2 * (2 * VIEWW + 1) + 1];
    INT vx, vy, i;
    const CHAR *name;

    wld_get(p->posx, p->posy, &here);
    name = (here.flags & TF_DISCOVERED) ? wld_name(here.terrain) : "unknown";
    prfmsg(LOOKHD, (INT)p->posx, (INT)p->posy, name);

    prf("\r\n");
    for (vy = (INT)p->posy - VIEWH; vy <= (INT)p->posy + VIEWH; vy++) {
        i = 0;
        for (vx = (INT)p->posx - VIEWW; vx <= (INT)p->posx + VIEWW; vx++) {
            CHAR ch;
            if (vx < 0 || vy < 0 || vx >= wld_size() || vy >= wld_size())
                ch = ' ';
            else if (vx == (INT)p->posx && vy == (INT)p->posy)
                ch = '@';
            else {
                wld_get(vx, vy, &t);
                ch = (t.flags & TF_DISCOVERED) ? wld_glyph(t.terrain) : '?';
            }
            row[i++] = ch;
            row[i++] = ' ';
        }
        row[i] = '\0';
        prf("      %s\r\n", row);
    }
    prfmsg(LEGEND);
}

/* ---------------------------------------------------------------------- *
 * do_move                                                                *
 *                                                                        *
 * Travel one tile in a compass direction for one Action Point, then show *
 * the new surroundings. Movement is blocked at the edge of the world.    *
 * ---------------------------------------------------------------------- */
VOID EXPORT
do_move(struct fracplyr *p, INT dx, INT dy, const CHAR *dir)
{
    INT nx, ny;

    if (p->actpts <= 0) {
        prfmsg(NOAP);
        return;
    }

    nx = (INT)p->posx + dx;
    ny = (INT)p->posy + dy;
    if (nx < 0 || ny < 0 || nx >= wld_size() || ny >= wld_size()) {
        prfmsg(EDGE);
        return;
    }

    p->actpts--;
    p->posx = (UCHAR)nx;
    p->posy = (UCHAR)ny;
    pl_write(p);

    prfmsg(MOVED, dir);
    do_look(p);
}

/* ---------------------------------------------------------------------- *
 * do_search                                                              *
 *                                                                        *
 * Spend one Action Point searching the current tile. Charting a tile for *
 * the first time is the big payoff (it reveals the terrain and awards    *
 * Fame). Every search then gathers whatever the terrain yields, so the   *
 * land is a renewable source of materials.                               *
 * ---------------------------------------------------------------------- */
VOID EXPORT
do_search(struct fracplyr *p)
{
    struct fractile t;
    INT gain;
    INT rtype, ramt;

    if (p->actpts <= 0) {
        prfmsg(NOAP);
        return;
    }

    p->actpts--;
    p->explores++;
    wld_get(p->posx, p->posy, &t);

    if (!(t.flags & TF_DISCOVERED)) {
        t.flags |= TF_DISCOVERED;
        wld_put(&t);
        p->discoveries++;
        gain = (INT)lngrnd(8, 20);
        p->fame += gain;
        prfmsg(NEWTILE, wld_name(t.terrain), gain);
    }

    res_yield(t.terrain, &rtype, &ramt);
    if (ramt > 0) {
        p->res[rtype] += ramt;
        prfmsg(GATHER, ramt, res_name(rtype));
    }
    else {
        prfmsg(EXPNADA);
    }

    pl_write(p);
}

/* ---------------------------------------------------------------------- *
 * module_main                                                            *
 *                                                                        *
 * Per-user menu state machine. The player's record is loaded fresh each  *
 * call (MBBS runs one user's routine at a time), the starting position   *
 * is placed at the center of the map on first ever entry, and any action *
 * that changes the record saves through PLAYER.C.                        *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
module_main(VOID)
{
    struct fracplyr me;
    GBOOL isnew;

    setmbk(fracmb);
    isnew = pl_ensure(usaptr->userid, &me, daily_ap);

    if (!me.started) {
        me.posx    = (UCHAR)(wld_size() / 2);
        me.posy    = (UCHAR)(wld_size() / 2);
        me.started = 1;
        pl_write(&me);
    }

    switch (usrptr->substt) {
    case 0:
        if (isnew)
            prfmsg(FIRSTM);
        prfmsg(usrptr->substt = MAINMEN);
        break;

    case MAINMEN:
        if (sameas(margv[0], "x")) {
            prfmsg(EXITMSG, usaptr->userid);
            outprf(usrnum);
            rstmbk();
            return 0;
        }
        else if (sameas(margv[0], "\0") || sameas(margv[0], "")) {
            prfmsg(SHORTM);
        }
        else if (sameas(margv[0], "?")) {
            prfmsg(MAINMEN);
            break;
        }
        else if (sameas(margv[0], "n")) {
            do_move(&me, 0, -1, "north");
        }
        else if (sameas(margv[0], "s")) {
            do_move(&me, 0, 1, "south");
        }
        else if (sameas(margv[0], "e")) {
            do_move(&me, 1, 0, "east");
        }
        else if (sameas(margv[0], "w")) {
            do_move(&me, -1, 0, "west");
        }
        else if (sameas(margv[0], "r")) {
            do_search(&me);
        }
        else if (sameas(margv[0], "l")) {
            do_look(&me);
        }
        else if (sameas(margv[0], "t")) {
            prfmsg(STATUS, me.userid, me.fame, me.actpts,
                   me.explores, me.discoveries, (INT)me.posx, (INT)me.posy,
                   me.res[RES_WOOD], me.res[RES_STONE], me.res[RES_METAL],
                   me.res[RES_FOOD], me.res[RES_WATER]);
        }
        else {
            prfmsg(INVALID);
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
 * ---------------------------------------------------------------------- */
VOID EXPORT
user_deleted(CHAR *userid)
{
    pl_delete(userid);
}

/* ---------------------------------------------------------------------- *
 * system_shutdown                                                        *
 * ---------------------------------------------------------------------- */
VOID EXPORT
system_shutdown(VOID)
{
    wld_close();
    pl_close();
    clsmsg(fracmb);
}

/* ---------------------------------------------------------------------- *
 * end_fracture                                                           *
 * ---------------------------------------------------------------------- */
VOID EXPORT
end_fracture(VOID)
{
}
