/*****************************************************************************
 *   SYSOP.C                                             Fracture - Sysop     *
 *                                                                           *
 *   In-game administration, reached from the main prompt with the hidden    *
 *   "!" command and gated by the BBS "SYSOP" key. A sysop can inspect and    *
 *   edit any player, hand out Action Points, delete a player, or reset the   *
 *   whole game (wipe players, regenerate the world, clear the news).         *
 *                                                                           *
 *   Commands are plain inline words, matching the rest of the module         *
 *   (e.g. the market "M s b 5" form):                                        *
 *                                                                           *
 *      !                          show this menu                             *
 *      ! find <name>              show a player's full record                *
 *      ! set  <name> <field> <v>  edit one field                            *
 *      ! ap   <name|ALL> <n>      set Action Points                         *
 *      ! wipe <name> YES          delete a player                          *
 *      ! reset CONFIRM            wipe ALL players, world and news          *
 *                                                                           *
 *   Output uses prf() directly (like the market and raid list screens), so  *
 *   these admin views add no entries to the message file.                   *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"                /* haskey, margv/margc, prf, today       */
#include "SYSOP.H"
#include "PLAYER.H"
#include "WORLD.H"
#include "RESOURCE.H"
#include "CAMP.H"
#include "RESEARCH.H"
#include "HISTORY.H"

extern INT daily_ap;                 /* configured daily Action Points (FRACTURE.C) */

#define SCLR "\x1b[2J\x1b[H"         /* clear screen and home the cursor      */

/* ---------------------------------------------------------------------- *
 * sysop_allowed                                                          *
 * ---------------------------------------------------------------------- */
GBOOL EXPORT
sysop_allowed(VOID)
{
    return (GBOOL)haskey("SYSOP");
}

/* Parse a non-negative integer from a command word (digits only).         */
static LONG
sys_num(const CHAR *s)
{
    LONG v = 0;
    if (s == NULL)
        return 0;
    while (*s >= '0' && *s <= '9') {
        if (v < 100000000L)              /* clamp before LONG can overflow    */
            v = v * 10 + (*s - '0');
        s++;
    }
    return v;
}

/* ---------------------------------------------------------------------- *
 * sys_menu                                                               *
 * ---------------------------------------------------------------------- */
static VOID
sys_menu(VOID)
{
    prf(SCLR "\n  \x1b[1;36m== FRACTURE SYSOP ==\x1b[0;37m\n\n");
    prf("  \x1b[1;37m! find \x1b[0;37m<name>                 show a player's record\n");
    prf("  \x1b[1;37m! set  \x1b[0;37m<name> <field> <value>  edit one field\n");
    prf("         \x1b[1;30mfields: fame ap wood stone metal food water camp rsch x y\x1b[0;37m\n");
    prf("  \x1b[1;37m! ap   \x1b[0;37m<name|ALL> <n>          set Action Points\n");
    prf("  \x1b[1;37m! wipe \x1b[0;37m<name> YES              delete a player\n");
    prf("  \x1b[1;37m! reset CONFIRM\x1b[0;37m                wipe ALL players, world & news\n");
}

/* ---------------------------------------------------------------------- *
 * sys_find                                                               *
 * ---------------------------------------------------------------------- */
static VOID
sys_find(const CHAR *name)
{
    struct fracplyr p;

    if (name == NULL || !name[0]) {
        prf("  usage: ! find <name>\n");
        return;
    }
    if (!pl_load_ci(name, &p)) {
        prf("  \x1b[1;31mNo player named '%s'.\x1b[0;37m\n", name);
        return;
    }

    prf(SCLR "\n  \x1b[1;37m%s\x1b[0;37m\n\n", p.userid);
    prf("  Fame \x1b[1;33m%ld\x1b[0;37m   AP \x1b[1;32m%d\x1b[0;37m   "
        "Camp \x1b[1;37m%d\x1b[0;37m (%s)   Research \x1b[1;37m%d\x1b[0;37m\n",
        p.fame, p.actpts, (INT)p.camplvl, camp_name(p.camplvl), (INT)p.rschlvl);
    prf("  Pos (\x1b[1;37m%d,%d\x1b[0;37m)   Camp home (\x1b[1;37m%d,%d\x1b[0;37m)\n",
        (INT)p.posx, (INT)p.posy, (INT)p.campx, (INT)p.campy);
    prf("  Explored \x1b[1;37m%ld\x1b[0;37m   Discovered \x1b[1;37m%ld\x1b[0;37m   "
        "Raids taken \x1b[1;37m%d\x1b[0;37m\n",
        p.explores, p.discoveries, (INT)p.raidhit);
    prf("  Wood \x1b[1;37m%ld\x1b[0;37m  Stone \x1b[1;37m%ld\x1b[0;37m  "
        "Metal \x1b[1;37m%ld\x1b[0;37m  Food \x1b[1;37m%ld\x1b[0;37m  "
        "Water \x1b[1;37m%ld\x1b[0;37m\n",
        p.res[RES_WOOD], p.res[RES_STONE], p.res[RES_METAL],
        p.res[RES_FOOD], p.res[RES_WATER]);
}

/* ---------------------------------------------------------------------- *
 * sys_set                                                                *
 * ---------------------------------------------------------------------- */
static VOID
sys_set(const CHAR *name, const CHAR *field, const CHAR *valstr)
{
    struct fracplyr p;
    LONG v;

    if (name == NULL || field == NULL || valstr == NULL || !valstr[0]) {
        prf("  usage: ! set <name> <field> <value>\n");
        return;
    }
    if (!pl_load_ci(name, &p)) {
        prf("  \x1b[1;31mNo player named '%s'.\x1b[0;37m\n", name);
        return;
    }
    v = sys_num(valstr);

    if      (sameas((CHAR *)field, "fame"))  p.fame = v;
    else if (sameas((CHAR *)field, "ap"))    p.actpts = (INT)v;
    else if (sameas((CHAR *)field, "wood"))  p.res[RES_WOOD]  = v;
    else if (sameas((CHAR *)field, "stone")) p.res[RES_STONE] = v;
    else if (sameas((CHAR *)field, "metal")) p.res[RES_METAL] = v;
    else if (sameas((CHAR *)field, "food"))  p.res[RES_FOOD]  = v;
    else if (sameas((CHAR *)field, "water")) p.res[RES_WATER] = v;
    else if (sameas((CHAR *)field, "camp"))
        p.camplvl = (UCHAR)(v > CAMP_MAXLVL ? CAMP_MAXLVL : v);
    else if (sameas((CHAR *)field, "rsch"))
        p.rschlvl = (UCHAR)(v > RSCH_MAXLVL ? RSCH_MAXLVL : v);
    else if (sameas((CHAR *)field, "x"))
        p.posx = (UCHAR)(v >= wld_size() ? wld_size() - 1 : v);
    else if (sameas((CHAR *)field, "y"))
        p.posy = (UCHAR)(v >= wld_size() ? wld_size() - 1 : v);
    else {
        prf("  \x1b[1;31mUnknown field '%s'.\x1b[0;37m\n", field);
        return;
    }

    pl_write(&p);
    prf("  \x1b[1;32mSet %s.%s = %ld\x1b[0;37m\n", p.userid, field, v);
}

/* ---------------------------------------------------------------------- *
 * sys_ap                                                                 *
 * ---------------------------------------------------------------------- */
static VOID
sys_ap(const CHAR *name, const CHAR *nstr)
{
    struct fracplyr p;
    INT n;

    if (name == NULL || nstr == NULL || !nstr[0]) {
        prf("  usage: ! ap <name|ALL> <n>\n");
        return;
    }
    n = (INT)sys_num(nstr);

    if (sameas((CHAR *)name, "all")) {
        pl_ap_all(n);
        prf("  \x1b[1;32mSet every player's AP to %d.\x1b[0;37m\n", n);
        return;
    }
    if (!pl_load_ci(name, &p)) {
        prf("  \x1b[1;31mNo player named '%s'.\x1b[0;37m\n", name);
        return;
    }
    p.actpts = n;
    pl_write(&p);
    prf("  \x1b[1;32mSet %s AP to %d.\x1b[0;37m\n", p.userid, n);
}

/* ---------------------------------------------------------------------- *
 * sys_wipe                                                               *
 * ---------------------------------------------------------------------- */
static VOID
sys_wipe(const CHAR *name, const CHAR *confirm)
{
    struct fracplyr p;

    if (name == NULL || !name[0]) {
        prf("  usage: ! wipe <name> YES\n");
        return;
    }
    if (!pl_load_ci(name, &p)) {
        prf("  \x1b[1;31mNo player named '%s'.\x1b[0;37m\n", name);
        return;
    }
    if (confirm == NULL || !sameas((CHAR *)confirm, "yes")) {
        prf("  \x1b[1;33mThis deletes %s. To proceed: ! wipe %s YES\x1b[0;37m\n",
            p.userid, p.userid);
        return;
    }
    pl_delete(p.userid);
    prf("  \x1b[1;32mDeleted player %s.\x1b[0;37m\n", p.userid);
}

/* ---------------------------------------------------------------------- *
 * sys_reset                                                              *
 *                                                                        *
 * The full game reset: every player is deleted, the world is regenerated *
 * and the news is cleared. The sysop's own record is wiped along with     *
 * the rest, so re-found them as a fresh explorer at the map centre to     *
 * keep the live session consistent with the new game. Best run when no    *
 * one else is in the module.                                              *
 * ---------------------------------------------------------------------- */
static VOID
sys_reset(struct fracplyr *self, const CHAR *confirm)
{
    CHAR uid[UIDSIZ];

    if (confirm == NULL || !sameas((CHAR *)confirm, "confirm")) {
        prf("  \x1b[1;31mThis WIPES every player, regenerates the world and clears "
            "the news.\x1b[0;37m\n");
        prf("  \x1b[1;33mTo proceed, type: ! reset CONFIRM\x1b[0;37m\n");
        return;
    }

    pl_wipeall();
    wld_reset();
    hist_clear();

    stzcpy(uid, self->userid, UIDSIZ);
    setmem(self, sizeof(*self), 0);
    stzcpy(self->userid, uid, UIDSIZ);
    self->lastday = today();
    self->actpts  = daily_ap;
    self->posx = self->campx = (UCHAR)(wld_size() / 2);
    self->posy = self->campy = (UCHAR)(wld_size() / 2);
    self->started = 1;
    pl_write(self);

    prf(SCLR "\n  \x1b[1;32mGame reset complete.\x1b[0;37m  "
        "Players wiped, world regenerated, news cleared.\n");
}

/* ---------------------------------------------------------------------- *
 * do_sysop                                                               *
 * ---------------------------------------------------------------------- */
VOID EXPORT
do_sysop(struct fracplyr *self)
{
    const CHAR *sub = (margc > 1) ? margv[1] : "";

    if      (margc <= 1)                     sys_menu();
    else if (sameas((CHAR *)sub, "find"))    sys_find(margc > 2 ? margv[2] : NULL);
    else if (sameas((CHAR *)sub, "set"))     sys_set(margc > 2 ? margv[2] : NULL,
                                                     margc > 3 ? margv[3] : NULL,
                                                     margc > 4 ? margv[4] : NULL);
    else if (sameas((CHAR *)sub, "ap"))      sys_ap(margc > 2 ? margv[2] : NULL,
                                                    margc > 3 ? margv[3] : NULL);
    else if (sameas((CHAR *)sub, "wipe"))    sys_wipe(margc > 2 ? margv[2] : NULL,
                                                      margc > 3 ? margv[3] : NULL);
    else if (sameas((CHAR *)sub, "reset"))   sys_reset(self, margc > 2 ? margv[2] : NULL);
    else                                     sys_menu();
}
