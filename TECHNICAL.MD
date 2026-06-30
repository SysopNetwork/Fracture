# Fracture — Technical Reference

This document describes the technical design of Fracture, a module for The Major BBS
v10. It is maintained alongside the implementation: sections describing code that does
not yet exist are marked **(planned)** and are filled in as the corresponding systems
are built.

## 1. Module model

A Major BBS v10 module is a Win32 DLL loaded by the BBS host. The host drives the
module through a fixed set of callbacks registered at initialization.

### Entry point

- The module exposes one required exported routine, `init__fracture`. Its name must
  match the DLL filename (`FRACTURE.DLL`). It registers the module, opens the message
  file, opens data files, and reads sysop configuration options.
- All routines are exported so crash reports (`GALEXCEP.OUT`) resolve to readable
  symbol names.

### The module interface block

Initialization registers a `struct module` with the host via `register_module()`. Its
fields are callbacks invoked by the host:

| Callback | Role in Fracture |
|---|---|
| user logon | per-user setup when a user enters the BBS |
| main input routine | the module's menu/state machine while a user is inside it |
| status-input / injoth | reserved |
| user logoff | per-user teardown |
| hangup | handle lost carrier |
| **midnight cleanup** | **daily world evolution and Action Point reset** |
| delete-account | remove a deleted user's records from module data files |
| shutdown | close message and data files cleanly |

`register_module()` returns a per-user state id; the main routine advances the user
through menu states using `usrptr->substt`, parsing input from `margv[]`.

### Daily tick

The midnight cleanup callback is the heart of Fracture's "living world": it resets each
player's Action Points, regenerates a configurable share of unexplored tiles, rolls the
day's weather and events, and advances research and worker production. Keeping all
time-based world changes in this single hook keeps the daily evolution deterministic
and testable.

## 2. Presentation (messages and ANSI)

User-facing text is **not** hardcoded. It lives in a `.MSG` source file as named
entries, optionally carrying embedded ANSI escape sequences for color. At runtime the
module opens the compiled message file and emits entries by name:

- `opnmsg()` opens the compiled `.MCV`; `setmbk()` / `rstmbk()` select and restore the
  active message block.
- `prfmsg()` formats a message (with `printf`-style substitution) into the user's
  output buffer; `outprf()` flushes it to the user.
- Boolean sysop options are declared in the `.MSG` (the `... B` form) and read with
  `ynopt()`; this is the primary mechanism for sysop configuration toggles.

A `.H` header maps message names to their numeric ids and is regenerated from the
`.MSG` whenever messages change. The UI targets 80x24 and degrades gracefully to plain
ASCII when color is unavailable.

## 3. Persistence

Fracture stores state in **Btrieve data files accessed through the DFA API** — there is
no SQL database. Each major game system owns one or more `.DAT` files, shipped with a
`.VIR` template so the host can create the file on first run. The `.MDF` declares the
Btrieve page size and file count.

Typical DFA usage pattern:

- `dfaOpen()` at init, `dfaClose()` at shutdown.
- `dfaSetBlk()` / `dfaRstBlk()` to select and restore the active data block around an
  operation.
- `dfaAcqEQ()` to look up a record by key, `dfaInsert()` / `dfaDelete()` to modify, and
  `dfaStepLO()` / `dfaStepNX()` to iterate.

### Data model (planned)

Each logical entity maps to a record struct in a `.DAT` file. The intended set of data
files, derived from the game design, includes: players, player camps, world tiles,
buildings, resource stores, inventory, research, workers, marketplace listings,
contracts, mail, events, artifacts, discoveries, history, weather, and statistics.
Records are kept lean and keyed for the access patterns above; redundant data is
avoided. The concrete record layouts are documented here as each system is implemented.

## 4. Source organization (planned)

Source lives under `SRC/`. Each major game system gets its own source files and its
own data file(s) so new content can be added without modifying existing systems — the
core engine (module lifecycle, daily tick, menu dispatch, persistence helpers) is kept
separate from the individual feature systems (world, camp, exploration, research,
buildings, economy, events, missions, workers, combat). This separation is the basis
for the project's expandability goal.

## 5. Configuration

Gameplay tunables are exposed to the sysop rather than hardcoded: world size, daily
Action Points, weather frequency, PvP and raiding toggles, marketplace tax, research
speed, resource production rates, event frequency, season length, and camp limits. New
systems are expected to surface their balance parameters from the start. The mechanism
(message-file options and/or a configuration file) is documented per setting as it is
implemented.

---

*Fracture is developed and maintained by Mark Laudenbach at Sysop Network.*
