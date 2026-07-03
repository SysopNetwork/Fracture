# Fracture — Technical Reference

This document describes the technical design of Fracture, a module for The Major BBS
v10, as implemented.

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
| **midnight cleanup** | **daily world evolution, weather, and seasons** |
| delete-account | remove a deleted user's records from module data files |
| shutdown | close message and data files cleanly |

`register_module()` returns a per-user state id; the main routine advances the user
through menu states using `usrptr->substt`, parsing input from `margv[]`.

### Daily tick

Server-wide daily change runs in the midnight cleanup callback: it regenerates a
configurable share of unexplored tiles (charted ground and camps are preserved) and
rolls the new day's weather, advancing the season cycle. Both results are logged to
the audit trail.

Per-player daily change is deliberately **lazy** instead: Action Points refill, and
worker production settles, on a player's first entry of a new day. This keeps the
midnight routine short regardless of the player count and works correctly for
players who skip days. The weather system also catches up lazily if a midnight tick
was missed (for example, when the BBS was down overnight).

## 2. Presentation (messages and ANSI)

User-facing text is **not** hardcoded. It lives in a `.MSG` source file as named
entries, optionally carrying embedded ANSI escape sequences for color. At runtime the
module opens the compiled message file and emits entries by name:

- `opnmsg()` opens the compiled `.MCV`; `setmbk()` / `rstmbk()` select and restore the
  active message block.
- `prfmsg()` formats a message into the user's output buffer; `outprf()` flushes
  it to the user.
- Dynamic values in message text are **named text variables** (`TVARS.C`,
  ADR 0015): the module registers each value as `FRA_<name>` via
  `register_textvar()`, and the host substitutes it wherever the name appears in
  the text. The sysop can move, repeat, or drop these tokens freely when
  rewording messages. The module fills a variable just before emitting a message
  that uses it; world size and the day's weather/season are computed live.
- Boolean sysop options are declared in the `.MSG` (the `... B` form) and read with
  `ynopt()`; this is the primary mechanism for sysop configuration toggles.

A `.H` header maps message names to their numeric ids and is regenerated from the
`.MSG` whenever messages change. The UI targets 80x24, drawn in a consistent shaded
block-art ANSI style (CP437 `░▒▓█` banners, `▐▌` menu tabs, a fixed palette) recorded
in ADR 0009.

`FRACTURE.MSG` is **generated** by `TOOLS/GENMSG.PY` — edit the generator, not the
`.MSG` text, when changing screens in source control. (Sysops remain free to reword
the human-readable text of a deployed `.MSG`.) The host recompiles the `.MSG` to
`.MCV` at startup, so no compiled message file is shipped. The `.MSG` and `.MDF`
must keep CRLF line endings, or the host's configuration editor will not list the
module's options (pinned via `.gitattributes`).

## 3. Persistence

Fracture stores state in **Btrieve data files accessed through the DFA API** — there is
no SQL database. Each major game system owns its own `.DAT` file, created
programmatically on first run with `dfaCreateSpec()` (no `.VIR` templates are
shipped). The `.MDF` declares the Btrieve page size and file count.

Typical DFA usage pattern:

- `dfaOpen()` at init, `dfaClose()` at shutdown.
- `dfaSetBlk()` / `dfaRstBlk()` to select and restore the active data block around an
  operation.
- `dfaAcqEQ()` to look up a record by key, `dfaInsert()` / `dfaDelete()` to modify, and
  `dfaStepLO()` / `dfaStepNX()` to iterate.

### Data files

| File | Owner | Contents | Key |
|---|---|---|---|
| `FRACTURE.DAT` | `PLAYER.C` | one record per player: Fame, Action Points, position, camp tier/location, resources, research tier, lifetime stats | user-id |
| `FRACWLD.DAT` | `WORLD.C` | one record per world tile: terrain, charted flag | packed x/y coordinate |
| `FRACHIST.DAT` | `HISTORY.C` | append-only server chronicle: first discoveries, camp builds; read newest-first for the news view | sequence number |
| `FRACWX.DAT` | `WEATHER.C` | a single record: date, season, day's weather, days into the season | fixed |
| `FRACWRK.DAT` | `WORKER.C` | one record per player: worker crew counts and last production day | user-id |

Camp, resource, and research state live inside the player record rather than in
separate files — they are one-to-one with the player and always read together.
Records are kept lean and keyed for the access patterns above; the sequence counter
for the chronicle is derived from the true maximum key at open (not the physically
last record), so the file self-heals after deletions.

## 4. Source organization

Source lives under `SRC/FRACTURE/`, one pair of files per game system, so new
content can be added without modifying existing systems:

| Files | System |
|---|---|
| `FRACTURE.C/.H` | module lifecycle, menu dispatch, command parsing, sysop options |
| `PLAYER.C/.H` | player records, daily Action Point reset, leaderboard |
| `WORLD.C/.H` | tile world: generation, travel, search/chart, minimap, midnight regeneration |
| `RESOURCE.C/.H` | the five materials and per-terrain gather yields |
| `CAMP.C/.H` | camp tiers, storage caps, upgrades |
| `HISTORY.C/.H` | server chronicle and the daily news view |
| `WEATHER.C/.H` | weather conditions and the season cycle |
| `WORKER.C/.H` | NPC worker crews and passive production |
| `MARKET.C/.H` | the trading post (resources ↔ Fame) |
| `RESEARCH.C/.H` | the five-tier research track |
| `RAID.C/.H` | player raiding |
| `SYSOP.C/.H` | hidden in-game sysop tools (`!`), gated by the BBS SYSOP key |

Design decisions for each system are recorded as ADRs in `DOCS/ADR/`.

## 5. Configuration

Every gameplay tunable is a standard MBBS configuration option declared in the
`.MSG` (booleans read with `ynopt()`, numbers with `numopt()`), edited by the sysop
in the host's configuration editor:

| Option | Meaning | Default | Range |
|---|---|---|---|
| `WELCOPT` | greet players at logon | YES | B |
| `DAYAP` | Action Points granted per player per day | 20 | 1–200 |
| `APFREE` | refill Action Points on every entry (casual/test mode) | NO | B |
| `WLDSIZ` | world size, tiles per edge | 40 | 8–64 |
| `REGENPCT` | unexplored tiles regenerated each midnight (%) | 5 | 0–100 |
| `WXOPT` | enable weather and seasons | YES | B |
| `SEASLEN` | days per season | 7 | 1–90 |
| `WRKOPT` | enable workers and production | YES | B |
| `WRKPROD` | resource units produced per worker per day | 2 | 0–50 |
| `WRKFOOD` | Food eaten per worker per day | 1 | 0–50 |
| `MKTOPT` | enable the trading post | YES | B |
| `MKTSELL` | Fame earned per resource unit sold | 2 | 0–100 |
| `MKTBUY` | Fame charged per resource unit bought | 3 | 0–100 |
| `RSCHOPT` | enable the research track | YES | B |
| `RADOPT` | enable player raiding | YES | B |
| `RADCOST` | Action Points per raid | 5 | 0–200 |
| `RADLOOT` | percent of resources looted on a winning raid | 25 | 0–100 |

Setting `REGENPCT` to 0 freezes the world; each major system's on/off switch
(`WXOPT`, `WRKOPT`, `MKTOPT`, `RSCHOPT`, `RADOPT`) removes it from play cleanly. New
systems are expected to surface their balance parameters from the start.

## 6. Sysop tools

A hidden `!` command at the main prompt — available only to users holding the BBS
**SYSOP** key, and invisible to everyone else — provides live administration:
`! find` inspects a player record, `! set` edits its fields, `! ap` grants Action
Points (to one player or `ALL`), `! wipe <name> YES` deletes a player, and
`! reset CONFIRM` wipes all players, regenerates the world, and clears the news.
Destructive commands require their literal confirmation token. ADR 0014 records the
design.

---

*Fracture is developed and maintained by Mark Laudenbach at Sysop Network.*
