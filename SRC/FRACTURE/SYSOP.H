/*****************************************************************************
 *   SYSOP.H                                             Fracture - Sysop     *
 *                                                                           *
 *   In-game administration for the door's sysop: view and edit any player,  *
 *   grant Action Points, remove a player, and reset the whole game. Every    *
 *   action is gated by the BBS "SYSOP" key, so ordinary players can never    *
 *   reach it. Driven from the main prompt by the hidden "!" command.         *
 *                                                                           *
 *   Copyright (C) 2026 Sysop Network.  All Rights Reserved.                  *
 *****************************************************************************/

#ifndef FRACTURE_SYSOP_H
#define FRACTURE_SYSOP_H

#include "gcomm.h"
#include "PLAYER.H"

/* TRUE if the current user may use the sysop tools (holds the SYSOP key).     */
GBOOL sysop_allowed(VOID);

/* Handle a "!" admin command (already split into margv[]/margc). `self` is     *
 * the calling sysop's own record, which a game reset re-initialises so the     *
 * live session stays consistent with the fresh game.                           */
VOID  do_sysop(struct fracplyr *self);

#endif /* FRACTURE_SYSOP_H */
