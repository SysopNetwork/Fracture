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
#include "CAMP.H"
#include "HISTORY.H"
#include "WEATHER.H"
#include "WORKER.H"

/* ---- forward declarations -------------------------------------------- */
GBOOL user_logon(VOID);              /* per-user logon supplemental routine  */
GBOOL user_logoff(VOID);             /* per-user logoff / hangup routine     */
GBOOL module_main(VOID);             /* main input / menu routine            */
VOID  frac_nostop(VOID);             /* force nonstop output while in-module  */
VOID  frac_restore(VOID);            /* restore the user's screen-pause pref  */
VOID  user_deleted(CHAR *userid);    /* delete-account routine               */
VOID  do_move(struct fracplyr *p, INT dx, INT dy);
VOID  do_search(struct fracplyr *p); /* search the current tile              */
VOID  do_look(struct fracplyr *p);   /* show the local map and tile          */
VOID  do_camp(struct fracplyr *p);   /* show the camp and its next upgrade   */
VOID  do_upgrade(struct fracplyr *p);/* spend resources to upgrade the camp  */
VOID  do_news(VOID);                 /* show the server-wide news chronicle  */
VOID  do_crew(struct fracplyr *p);   /* show the crew & production screen     */
VOID  do_hire(struct fracplyr *p, CHAR *sel); /* recruit a worker by number   */
VOID  show_weather(VOID);            /* print today's weather/season line    */
VOID  world_midnight(VOID);          /* midnight cleanup: evolve the world   */
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
     user_logoff,                    /* user logoff supplemental routine     */
     user_logoff,                    /* hangup (lost carrier) routine        */
     world_midnight,                 /* midnight cleanup: daily world tick   */
     user_deleted,                   /* delete-account routine               */
     system_shutdown                 /* finish-up (system shutdown) routine  */
};

/* ---- module globals -------------------------------------------------- */
HMCVFILE fracmb;                     /* module message file handle           */
GBOOL    show_welcome;               /* sysop option: greet users at logon?  */
INT      daily_ap;                   /* sysop option: Action Points per day  */
INT      world_sz;                   /* sysop option: world dimension        */
INT      regen_pct;                  /* sysop option: % unexplored tiles     */
                                     /* regenerated each midnight            */
GBOOL    weather_on;                 /* sysop option: weather & seasons?     */
INT      season_len;                 /* sysop option: days per season        */
GBOOL    workers_on;                 /* sysop option: workers & production?  */
INT      wrk_prod;                   /* sysop option: units per worker/day   */
INT      wrk_food;                   /* sysop option: Food per worker/day    */

/* Fracture paints its own full-screen pages, so while a user is in the game *
 * we suppress the host's output more-prompt ("(N)onstop,(Q)uit,(C)ontinue?")*
 * by forcing their page-break length to 0 (nonstop). The pause counter      *
 * carries across screen clears, so without this it trips mid-redraw. The     *
 * user's real setting is saved per channel and restored on exit / logoff.    */
static CHAR sav_scnbrk[MAXNTERM];    /* saved usaptr->scnbrk, per channel     */
static CHAR brk_forced[MAXNTERM];    /* 1 while we hold this user at nonstop   */

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
    regen_pct    = numopt(REGENPCT, 0, 100);
    weather_on   = ynopt(WXOPT);
    season_len   = numopt(SEASLEN, 1, 90);
    workers_on   = ynopt(WRKOPT);
    wrk_prod     = numopt(WRKPROD, 0, 50);
    wrk_food     = numopt(WRKFOOD, 0, 50);

    pl_open();                       /* persistent players                   */
    wld_open(world_sz);              /* the map (generated on first run)     */
    hist_open();                     /* the shared server chronicle          */
    wx_open(weather_on, season_len); /* today's weather and the season cycle */
    wrk_open(workers_on, wrk_prod, wrk_food); /* the crew / production system */

    /* summary <= 32 chars, detail <= 63 - both show on the audit trail     */
    shocst("Fracture's here. Act impressed.",
           "Fracture v1.0.0 just joined the chat! Hold my juicebox.");
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
 * frac_nostop / frac_restore                                             *
 *                                                                        *
 * Suppress the host's automatic output more-prompt while a user is in    *
 * the game. frac_nostop() saves the user's page-break length once, then  *
 * forces it to 0 (nonstop) on every input tick. frac_restore() puts the  *
 * saved value back when the user leaves (X, logoff, or hangup) so their  *
 * screen-pause preference is untouched everywhere else on the BBS.       *
 * ---------------------------------------------------------------------- */
VOID EXPORT
frac_nostop(VOID)
{
    if (!brk_forced[usrnum]) {
        sav_scnbrk[usrnum] = usaptr->scnbrk;
        brk_forced[usrnum] = 1;
    }
    usaptr->scnbrk = 0;                   /* 0 = nonstop (no page breaks)      */
}

VOID EXPORT
frac_restore(VOID)
{
    if (brk_forced[usrnum]) {
        usaptr->scnbrk = sav_scnbrk[usrnum];
        brk_forced[usrnum] = 0;
    }
}

/* ---------------------------------------------------------------------- *
 * user_logoff                                                            *
 *                                                                        *
 * Runs on logoff and on hangup. Its only job is to restore the screen-   *
 * pause preference if the user dropped out without pressing X.           *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
user_logoff(VOID)
{
    frac_restore();
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
 * Travel one tile by (dx, dy) for one Action Point, then redraw the new  *
 * surroundings. Movement is blocked at the edge of the world. The map     *
 * redraw clears the screen, so no separate "you moved" line is printed -   *
 * it would only be erased by that clear (and would trip the page pause).   *
 * ---------------------------------------------------------------------- */
VOID EXPORT
do_move(struct fracplyr *p, INT dx, INT dy)
{
    INT nx, ny;
    INT cost = wx_travcost();            /* storms and snow raise the toll   */

    if (p->actpts < cost) {
        prfmsg(NOAP);
        return;
    }

    nx = (INT)p->posx + dx;
    ny = (INT)p->posy + dy;
    if (nx < 0 || ny < 0 || nx >= wld_size() || ny >= wld_size()) {
        prfmsg(EDGE);
        return;
    }

    p->actpts -= cost;
    p->posx = (UCHAR)nx;
    p->posy = (UCHAR)ny;
    pl_write(p);

    do_look(p);                          /* the recentered map is the feedback */
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
        /* the tile is shared, so this player is its first discoverer */
        hist_add(HE_DISCOVER, p->userid, t.terrain,
                 (INT)p->posx, (INT)p->posy);
    }

    res_yield(t.terrain, &rtype, &ramt);
    ramt = wx_yield(ramt);                /* weather makes the land generous */
    if (ramt > 0) {                       /* or stingy for the day           */
        LONG cap    = camp_storage(p->camplvl);
        LONG before = p->res[rtype];
        p->res[rtype] += ramt;
        if (p->res[rtype] > cap)
            p->res[rtype] = cap;          /* camp storage is the cap        */
        if (p->res[rtype] > before)
            prfmsg(GATHER, (INT)(p->res[rtype] - before), res_name(rtype));
        else
            prfmsg(STORFUL);              /* already at capacity            */
    }
    else {
        prfmsg(EXPNADA);
    }

    pl_write(p);
}

/* ---------------------------------------------------------------------- *
 * do_camp                                                                *
 *                                                                        *
 * Show the player's camp: its tier, location, and storage capacity, then *
 * either the cost of the next upgrade or a note that it is fully built.   *
 * ---------------------------------------------------------------------- */
VOID EXPORT
do_camp(struct fracplyr *p)
{
    LONG cost[RES_COUNT];

    prfmsg(CAMPVW, camp_name(p->camplvl), (INT)p->campx, (INT)p->campy,
           camp_storage(p->camplvl));

    if (camp_cost(p->camplvl, cost))
        prfmsg(CAMPUPC, camp_name(p->camplvl + 1),
               cost[RES_WOOD], cost[RES_STONE], cost[RES_METAL]);
    else
        prfmsg(CAMPMAX);
}

/* ---------------------------------------------------------------------- *
 * do_upgrade                                                             *
 *                                                                        *
 * Upgrade the camp to the next tier if the player can pay for it. The    *
 * cost is deducted, the tier rises, and the building project earns Fame. *
 * ---------------------------------------------------------------------- */
VOID EXPORT
do_upgrade(struct fracplyr *p)
{
    LONG cost[RES_COUNT];
    INT  i;
    INT  gain;

    if (!camp_cost(p->camplvl, cost)) {
        prfmsg(CAMPMAX);
        return;
    }

    for (i = 0; i < RES_COUNT; i++) {
        if (p->res[i] < cost[i]) {
            prfmsg(CANTAFF);
            return;
        }
    }

    for (i = 0; i < RES_COUNT; i++)
        p->res[i] -= cost[i];

    p->camplvl++;
    gain = 10 * (INT)p->camplvl;          /* building projects earn Fame    */
    p->fame += gain;
    pl_write(p);

    prfmsg(CAMPUP, camp_name(p->camplvl), gain);
    hist_add(HE_BUILD, p->userid, p->camplvl, (INT)p->campx, (INT)p->campy);
}

/* ---------------------------------------------------------------------- *
 * do_news                                                                *
 *                                                                        *
 * Show the server-wide chronicle: the most recent discoveries and        *
 * building projects from every player, newest first. This is the shared  *
 * "living world" view that makes the server feel inhabited.              *
 * ---------------------------------------------------------------------- */
#define NEWSMAX 12               /* most recent entries shown in the news    */

VOID EXPORT
do_news(VOID)
{
    hist_news(NEWSMAX);
    show_weather();                      /* today's forecast under the news  */
}

/* ---------------------------------------------------------------------- *
 * show_weather                                                           *
 *                                                                        *
 * Print the one-line report of today's weather and the current season.   *
 * Silent when the sysop has weather disabled. The caller must already     *
 * have selected the module message block (setmbk).                        *
 * ---------------------------------------------------------------------- */
VOID EXPORT
show_weather(VOID)
{
    INT wx;

    if (!wx_enabled())
        return;

    wx = wx_weather();
    prfmsg(WXLINE, wx_wname(wx), wx_wdesc(wx), wx_sname(wx_season()));
}

/* ---------------------------------------------------------------------- *
 * do_crew                                                                *
 *                                                                        *
 * Show the player's crew: one row per worker type with how many they      *
 * have and what those workers produce each day, then the capacity their   *
 * camp allows and how to recruit. The rows are drawn here (like the map)   *
 * because they are dynamic; the frame and footer come from the message    *
 * file so their wording stays sysop-editable.                             *
 * ---------------------------------------------------------------------- */
VOID EXPORT
do_crew(struct fracplyr *p)
{
    INT count[WRK_COUNT];
    INT total, per, i;

    if (!wrk_enabled()) {
        prf("\x1b[2J\r\n  \x1b[1;31mThe crew system is disabled here.\x1b[0;37m\r\n");
        return;
    }

    total = wrk_counts(p->userid, count);
    per   = wrk_perday();

    prfmsg(CREWHD);
    for (i = 0; i < WRK_COUNT; i++) {
        prf("  \x1b[1;33m[%d]\x1b[0;37m \x1b[1;37m%-13s\x1b[0;37m makes "
            "\x1b[1;37m%-6s\x1b[0;37m  have \x1b[1;32m%d\x1b[0;37m"
            "   \x1b[1;33m+%d\x1b[0;37m/day\r\n",
            i + 1, wrk_name(i), res_name(wrk_resource(i)),
            count[i], count[i] * per);
    }
    prfmsg(CREWFT, total, camp_workers(p->camplvl),
           WRK_COSTWOOD, WRK_COSTFOOD);
}

/* ---------------------------------------------------------------------- *
 * do_hire                                                                *
 *                                                                        *
 * Recruit one worker of the type named by a 1..5 selection (the second    *
 * word of the "P n" command). Reports the outcome and, on success, saves   *
 * the resource cost deducted by wrk_hire.                                  *
 * ---------------------------------------------------------------------- */
VOID EXPORT
do_hire(struct fracplyr *p, CHAR *sel)
{
    INT type;

    if (sel == NULL || sel[0] < '1' || sel[0] > '0' + WRK_COUNT) {
        prfmsg(HIREBAD);
        return;
    }
    type = sel[0] - '1';

    switch (wrk_hire(p, type)) {
    case WHIRE_OK:
        pl_write(p);
        prfmsg(HIRED, wrk_name(type));
        break;
    case WHIRE_FULL:
        prfmsg(HIREFUL);
        break;
    case WHIRE_POOR:
        prfmsg(HIRECST, WRK_COSTWOOD, WRK_COSTFOOD);
        break;
    default:
        prfmsg(HIREBAD);
        break;
    }
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
    frac_nostop();                   /* keep our full-screen pages from paging */
    isnew = pl_ensure(usaptr->userid, &me, daily_ap);

    if (!me.started) {
        /* place the player and found their camp at the center of the map */
        me.posx    = (UCHAR)(wld_size() / 2);
        me.posy    = (UCHAR)(wld_size() / 2);
        me.campx   = me.posx;
        me.campy   = me.posy;
        me.camplvl = 0;                  /* start with a Tent              */
        me.started = 1;
        pl_write(&me);
    }
    else if (me.posx  >= wld_size() || me.posy  >= wld_size() ||
             me.campx >= wld_size() || me.campy >= wld_size()) {
        /* The player (or their camp) is outside the current world bounds -  *
         * this happens when the sysop changes the world size after a player *
         * has started. Bring them home: re-found an out-of-bounds camp at   *
         * the map center, then place the player on their (now valid) camp.  */
        UCHAR ctr = (UCHAR)(wld_size() / 2);
        if (me.campx >= wld_size() || me.campy >= wld_size()) {
            me.campx = ctr;
            me.campy = ctr;
        }
        if (me.posx >= wld_size() || me.posy >= wld_size()) {
            me.posx = me.campx;
            me.posy = me.campy;
        }
        pl_write(&me);
    }

    switch (usrptr->substt) {
    case 0:
        if (isnew)
            prfmsg(FIRSTM);                  /* new explorers get the intro */
        else
            prfmsg((INT)lngrnd(QUIP1, QUIPN));/* veterans get a random quip */
        {
            /* the crew works one day for each new day the player returns */
            GBOOL hungry = 0;
            INT   made   = wrk_daily(&me, &hungry);
            if (made > 0 || hungry)
                pl_write(&me);
            if (made > 0)
                prfmsg(PRODSUM, made);
            if (hungry)
                prfmsg(PRODHUN);
        }
        show_weather();                      /* then today's forecast       */
        prfmsg(usrptr->substt = MAINMEN);
        break;

    case MAINMEN:
        if (sameas(margv[0], "x")) {
            prfmsg(EXITMSG, usaptr->userid);
            outprf(usrnum);
            rstmbk();
            frac_restore();          /* hand the user's screen-pause back      */
            return 0;
        }
        else if (sameas(margv[0], "\0") || sameas(margv[0], "")) {
            /* bare Enter: fall through and let the prompt below redraw once  *
             * (printing SHORTM here too would double the menu)               */
        }
        else if (sameas(margv[0], "?")) {
            prf("\x1b[2J");                  /* MAINMEN no longer self-clears */
            prfmsg(MAINMEN);
            break;
        }
        else if (sameas(margv[0], "n")) {
            do_move(&me, 0, -1);
        }
        else if (sameas(margv[0], "s")) {
            do_move(&me, 0, 1);
        }
        else if (sameas(margv[0], "e")) {
            do_move(&me, 1, 0);
        }
        else if (sameas(margv[0], "w")) {
            do_move(&me, -1, 0);
        }
        else if (sameas(margv[0], "r")) {
            do_search(&me);
        }
        else if (sameas(margv[0], "l")) {
            do_look(&me);
        }
        else if (sameas(margv[0], "c")) {
            do_camp(&me);
        }
        else if (sameas(margv[0], "u")) {
            do_upgrade(&me);
        }
        else if (sameas(margv[0], "h")) {
            do_news();
        }
        else if (sameas(margv[0], "p")) {
            if (margc > 1)
                do_hire(&me, margv[1]);      /* "P n" recruits a worker     */
            else
                do_crew(&me);                /* "P" shows the crew screen   */
        }
        else if (sameas(margv[0], "t")) {
            prfmsg(STATUS, me.userid, me.fame, me.actpts,
                   me.explores, me.discoveries, (INT)me.posx, (INT)me.posy,
                   camp_name(me.camplvl), camp_storage(me.camplvl),
                   me.res[RES_WOOD], me.res[RES_STONE], me.res[RES_METAL],
                   me.res[RES_FOOD], me.res[RES_WATER]);
            show_weather();                  /* today's forecast on status   */
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
    wrk_delete(userid);
}

/* ---------------------------------------------------------------------- *
 * world_midnight                                                         *
 *                                                                        *
 * The module's midnight cleanup routine, run once by the host during the *
 * nightly BBS cleanup. This is where the world evolves: a sysop-set       *
 * percentage of unexplored tiles are regenerated so returning players     *
 * find fresh ground to chart. The result is logged to the audit trail.   *
 * ---------------------------------------------------------------------- */
VOID EXPORT
world_midnight(VOID)
{
    INT changed = wld_evolve(regen_pct);
    /* summary <= 32 chars, detail <= 63 (count keeps it verifiable)        */
    shocst("Fracture reshuffled the world",
           "%d unexplored tile(s) reborn in the dark while you slept.",
           changed);

    /* Roll the new day's weather and age the season on the same tick.       */
    if (weather_on) {
        wx_advance();
        shocst("Fracture's skies have turned",
               "%s dawns over the %s world today.",
               wx_wname(wx_weather()), wx_sname(wx_season()));
    }
}

/* ---------------------------------------------------------------------- *
 * system_shutdown                                                        *
 * ---------------------------------------------------------------------- */
VOID EXPORT
system_shutdown(VOID)
{
    wld_close();
    pl_close();
    hist_close();
    wx_close();
    wrk_close();
    clsmsg(fracmb);
}

/* ---------------------------------------------------------------------- *
 * end_fracture                                                           *
 * ---------------------------------------------------------------------- */
VOID EXPORT
end_fracture(VOID)
{
}
