/*****************************************************************************
 *   TVARS.C                                        Fracture - Text variables  *
 *                                                                           *
 *   Registers the module's named text variables and holds their values.       *
 *                                                                           *
 *   A text variable appears in message text as the byte sequence               *
 *   0x01 <justify> <width+32> NAME 0x01 (justify N=natural, L/R/C=padded to     *
 *   the width). When a message is emitted the host replaces the sequence        *
 *   with the string returned by the variable's registered routine, so the       *
 *   sysop can move or repeat a variable anywhere in the text. The routines      *
 *   here return small static buffers the game fills through tvar_set /          *
 *   tvar_setn / tvar_player immediately before each prfmsg(); world size         *
 *   and the day's weather are computed live instead, since they are global       *
 *   and always current.                                                          *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#include <stdio.h>

#include "gcomm.h"
#include "majorbbs.h"

#include "TVARS.H"
#include "CAMP.H"
#include "WORLD.H"
#include "WEATHER.H"

#define TVBW 40                 /* room for a user-id (30) or any name/number */

static CHAR tvb[TV_COUNT][TVBW];   /* one value buffer per filled slot       */

/* One routine per filled slot - the host wants a distinct function pointer  */
/* for every registered name.                                                */
static CHAR *tvNAME(VOID)    { return tvb[TV_NAME];    }
static CHAR *tvFAME(VOID)    { return tvb[TV_FAME];    }
static CHAR *tvAP(VOID)      { return tvb[TV_AP];      }
static CHAR *tvEXPL(VOID)    { return tvb[TV_EXPL];    }
static CHAR *tvDISC(VOID)    { return tvb[TV_DISC];    }
static CHAR *tvX(VOID)       { return tvb[TV_X];       }
static CHAR *tvY(VOID)       { return tvb[TV_Y];       }
static CHAR *tvCAMP(VOID)    { return tvb[TV_CAMP];    }
static CHAR *tvCAMPX(VOID)   { return tvb[TV_CAMPX];   }
static CHAR *tvCAMPY(VOID)   { return tvb[TV_CAMPY];   }
static CHAR *tvCAP(VOID)     { return tvb[TV_CAP];     }
static CHAR *tvWOOD(VOID)    { return tvb[TV_WOOD];    }
static CHAR *tvSTONE(VOID)   { return tvb[TV_STONE];   }
static CHAR *tvMETAL(VOID)   { return tvb[TV_METAL];   }
static CHAR *tvFOOD(VOID)    { return tvb[TV_FOOD];    }
static CHAR *tvWATER(VOID)   { return tvb[TV_WATER];   }
static CHAR *tvTERRAIN(VOID) { return tvb[TV_TERRAIN]; }
static CHAR *tvRES(VOID)     { return tvb[TV_RES];     }
static CHAR *tvAMT(VOID)     { return tvb[TV_AMT];     }
static CHAR *tvGAIN(VOID)    { return tvb[TV_GAIN];    }
static CHAR *tvNEXT(VOID)    { return tvb[TV_NEXT];    }
static CHAR *tvCWOOD(VOID)   { return tvb[TV_CWOOD];   }
static CHAR *tvCSTONE(VOID)  { return tvb[TV_CSTONE];  }
static CHAR *tvCMETAL(VOID)  { return tvb[TV_CMETAL];  }
static CHAR *tvCFOOD(VOID)   { return tvb[TV_CFOOD];   }
static CHAR *tvCREW(VOID)    { return tvb[TV_CREW];    }
static CHAR *tvCREWCAP(VOID) { return tvb[TV_CREWCAP]; }
static CHAR *tvWORKER(VOID)  { return tvb[TV_WORKER];  }
static CHAR *tvTIER(VOID)    { return tvb[TV_TIER];    }
static CHAR *tvPCT(VOID)     { return tvb[TV_PCT];     }
static CHAR *tvCOST(VOID)    { return tvb[TV_COST];    }
static CHAR *tvTARGET(VOID)  { return tvb[TV_TARGET];  }
static CHAR *tvLOOT(VOID)    { return tvb[TV_LOOT];    }
static CHAR *tvDATE(VOID)    { return tvb[TV_DATE];    }
static CHAR *tvWHO(VOID)     { return tvb[TV_WHO];     }

/* Live variables - always current, no fill required.                        */
static CHAR *
tvWORLD(VOID)                      /* current world size (tiles per edge)    */
{
    static CHAR buf[12];

    sprintf(buf, "%d", wld_size());
    return buf;
}

static CHAR *
tvWX(VOID)                         /* today's weather condition name         */
{
    return (CHAR *)(wx_enabled() ? wx_wname(wx_weather()) : "");
}

static CHAR *
tvWXDESC(VOID)                     /* today's weather flavor line            */
{
    return (CHAR *)(wx_enabled() ? wx_wdesc(wx_weather()) : "");
}

static CHAR *
tvSEASON(VOID)                     /* the current season's name              */
{
    return (CHAR *)(wx_enabled() ? wx_sname(wx_season()) : "");
}

/* ---------------------------------------------------------------------- *
 * tvar_init                                                              *
 *                                                                        *
 * Register every variable with the host. The FRA_ prefix keeps the names  *
 * out of other modules' way (text variables are global to the whole BBS).  *
 * ---------------------------------------------------------------------- */
VOID EXPORT
tvar_init(VOID)
{
    register_textvar("FRA_NAME",    tvNAME);
    register_textvar("FRA_FAME",    tvFAME);
    register_textvar("FRA_AP",      tvAP);
    register_textvar("FRA_EXPL",    tvEXPL);
    register_textvar("FRA_DISC",    tvDISC);
    register_textvar("FRA_X",       tvX);
    register_textvar("FRA_Y",       tvY);
    register_textvar("FRA_CAMP",    tvCAMP);
    register_textvar("FRA_CAMPX",   tvCAMPX);
    register_textvar("FRA_CAMPY",   tvCAMPY);
    register_textvar("FRA_CAP",     tvCAP);
    register_textvar("FRA_WOOD",    tvWOOD);
    register_textvar("FRA_STONE",   tvSTONE);
    register_textvar("FRA_METAL",   tvMETAL);
    register_textvar("FRA_FOOD",    tvFOOD);
    register_textvar("FRA_WATER",   tvWATER);
    register_textvar("FRA_TERRAIN", tvTERRAIN);
    register_textvar("FRA_RES",     tvRES);
    register_textvar("FRA_AMT",     tvAMT);
    register_textvar("FRA_GAIN",    tvGAIN);
    register_textvar("FRA_NEXT",    tvNEXT);
    register_textvar("FRA_CWOOD",   tvCWOOD);
    register_textvar("FRA_CSTONE",  tvCSTONE);
    register_textvar("FRA_CMETAL",  tvCMETAL);
    register_textvar("FRA_CFOOD",   tvCFOOD);
    register_textvar("FRA_CREW",    tvCREW);
    register_textvar("FRA_CREWCAP", tvCREWCAP);
    register_textvar("FRA_WORKER",  tvWORKER);
    register_textvar("FRA_TIER",    tvTIER);
    register_textvar("FRA_PCT",     tvPCT);
    register_textvar("FRA_COST",    tvCOST);
    register_textvar("FRA_TARGET",  tvTARGET);
    register_textvar("FRA_LOOT",    tvLOOT);
    register_textvar("FRA_DATE",    tvDATE);
    register_textvar("FRA_WHO",     tvWHO);
    register_textvar("FRA_WORLD",   tvWORLD);
    register_textvar("FRA_WX",      tvWX);
    register_textvar("FRA_WXDESC",  tvWXDESC);
    register_textvar("FRA_SEASON",  tvSEASON);
}

/* ---------------------------------------------------------------------- *
 * tvar_set / tvar_setn                                                   *
 *                                                                        *
 * Fill one slot with a string or a number ahead of the prfmsg() that     *
 * shows it. Values persist until overwritten, so a fill serves every     *
 * message emitted in the same service slice.                             *
 * ---------------------------------------------------------------------- */
VOID EXPORT
tvar_set(INT slot, const CHAR *val)
{
    stzcpy(tvb[slot], (CHAR *)val, TVBW);
}

VOID EXPORT
tvar_setn(INT slot, LONG val)
{
    sprintf(tvb[slot], "%ld", val);
}

/* ---------------------------------------------------------------------- *
 * tvar_player                                                            *
 *                                                                        *
 * Fill every player-derived slot from one record - identity, score,      *
 * position, camp, and the resource stores. One call covers the status    *
 * screen, the camp view, and any message that names the player.          *
 * ---------------------------------------------------------------------- */
VOID EXPORT
tvar_player(struct fracplyr *p)
{
    tvar_set(TV_NAME, p->userid);
    tvar_setn(TV_FAME,  p->fame);
    tvar_setn(TV_AP,    (LONG)p->actpts);
    tvar_setn(TV_EXPL,  p->explores);
    tvar_setn(TV_DISC,  p->discoveries);
    tvar_setn(TV_X,     (LONG)p->posx);
    tvar_setn(TV_Y,     (LONG)p->posy);
    tvar_set(TV_CAMP,  camp_name(p->camplvl));
    tvar_setn(TV_CAMPX, (LONG)p->campx);
    tvar_setn(TV_CAMPY, (LONG)p->campy);
    tvar_setn(TV_CAP,   camp_storage(p->camplvl));
    tvar_setn(TV_WOOD,  p->res[RES_WOOD]);
    tvar_setn(TV_STONE, p->res[RES_STONE]);
    tvar_setn(TV_METAL, p->res[RES_METAL]);
    tvar_setn(TV_FOOD,  p->res[RES_FOOD]);
    tvar_setn(TV_WATER, p->res[RES_WATER]);
}
