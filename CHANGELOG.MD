# Changelog

All notable changes to Fracture are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [1.0.0] - 2026-07-03

First release: the complete core game — a persistent tile world with daily
midnight evolution, exploration and first discoveries, camp building, resources,
workers and passive production, weather and seasons, a research track, a trading
post, player raiding, a server chronicle with daily news, a Fame leaderboard,
in-game sysop tools, and a freely sysop-editable message file (every dynamic
value is a named text variable).

### Changed
- Every dynamic value in the message file — names, amounts, costs, coordinates,
  the weather — is a **named text variable** (`FRA_FAME`, `FRA_TARGET`,
  `FRA_WX`, ...) substituted by the host wherever it appears in the text,
  replacing the positional `%s`/`%ld` printf tokens. Sysops can therefore
  reword, reorder, repeat, or drop any value when editing message text without
  breaking the module — previously the token order had to match the code's
  argument list exactly. World size and the day's weather and season are
  computed live, so they are always current wherever the text mentions them.
  New source pair `TVARS.C`/`TVARS.H`; message names, types, and order are
  unchanged. ADR 0015 records the design.
- The main menu and the compact command bar were rebuilt as aligned grids. The
  full menu is now two columns (half its former height) and lists **U**
  Upgrade; the compact bar is a four-column grid with every tab starting at a
  fixed column, its rows themed — world, camp, progress/rivals, then help/exit —
  instead of the old ragged 4/4/4/2 wrap that left Board and Upgrade dangling.
  Message names, types, and order were preserved, so the numbering is unchanged.
- The location/map screen and every command prompt gained consistent spacing: a
  single blank line after the location header, after the legend, and before the
  menu. The menu itself now carries the leading gap, so the separation is uniform
  on every screen.
- Raid targets (`A name`) now match case-insensitively, so a rival's name need only
  be spelled right, not cased right (`A himmey` finds `Himmey`).
- The status screen (`T`) and the how-to-play text now show the real configured
  world size (`N x N`) instead of a hardcoded value.
- A new player's camp/home tile is charted on first entry (and when an
  out-of-bounds player is relocated home), so it is never shown as "unknown" —
  previously visible after the world was grown past its original size.
- The screens were widened to use the full 80-column display: banners, rules, and
  the menu tab bar now span ~74 columns (was 52), the tabs are spread out, and the
  status and camp screens gained a blank line above and below their fields. Message
  names, types, and argument order were preserved.
- Visual overhaul: every player-facing screen was restyled in a consistent
  shaded block-art ANSI look (CP437 `░▒▓█` banners and gradient rules, `▐▌`
  half-block menu tabs, a fixed cyan/white/yellow palette). The main menu,
  welcome, status, camp, location header, news chronicle, and weather line now
  share one visual language. Message names, types, and argument order were
  preserved, so no code changed and no rebuild is required (the host recompiles
  the `.MSG` to `.MCV` at startup). The `.MSG` is now generated from
  `TOOLS/GENMSG.PY`; ADR 0009 records the style. Sysops can still reword the
  human-readable text.
- The midnight cleanup routine now advances weather and seasons alongside the
  world's terrain evolution, and logs a second audit-trail line naming the new
  day's condition and season (summary `Fracture's skies have turned`).
- The boot-time audit-trail entry now has personality: the console summary reads
  `Fracture's here. Act impressed.` with detail
  `Fracture v1.0.0 just joined the chat! Hold my juicebox.` (was just `Fracture`).
- The midnight world-evolution audit entry got the same treatment: summary
  `Fracture reshuffled the world`, detail
  `N unexplored tile(s) reborn in the dark while you slept.` (the count keeps it
  verifiable).

### Fixed
- A sysop `! reset` could later crash the door with a Btrieve duplicate-key error
  (status 5) on `FRACHIST.DAT`. The history file resumed its sequence counter from
  the physically-last record, which stopped matching the highest sequence once the
  reset began deleting entries, so a later insert reused an existing key. The next
  sequence number is now derived from the true maximum key by scanning the file,
  which is correct regardless of physical layout and self-heals an affected file on
  the next open.
- The host's output more-prompt (`(N)onstop, (Q)uit, or (C)ontinue?`) could
  interrupt the game's own full-screen redraws for users with a short screen-pause
  length, because the line counter carries across screen clears. The module now
  runs nonstop while a player is in the game and restores their setting on exit,
  logoff, or hangup, so screens no longer page mid-draw.
- Pressing Enter at the main prompt drew the menu — and the `Choose:` line —
  twice. The empty-input case now relies on the single shared prompt redraw like
  every other command path.
- The first-time welcome (`FIRSTM`) was being wiped immediately by the main
  menu's screen-clear, so it never showed. The menu no longer clears the screen
  itself (the entry message clears instead), so the welcome — and the new
  arrival quips — now display above the menu.
- A player whose stored position (or camp) fell outside the current world
  bounds — which happens when the world size changes after they have started —
  was stranded: the local map rendered blank and movement was blocked, though
  other actions still worked. The main routine now brings such a player home
  (re-founding an out-of-bounds camp at the map center) on entry. The `WLDSIZ`
  help text was corrected to describe how changing the size affects an existing
  world.

### Added
- Ten more sysop options, exposing balance values that were fixed in code:
  Fame rewards (`FAMEDISC` for first discoveries, `FAMEBLD` per camp tier
  built, `RADFAME` for a winning raid), action costs (`TRAVAP` per tile
  travelled, `SRCHAP` per search), the worker recruit cost (`WRKCWOOD`,
  `WRKCFOOD`), the camp tier required to raid or be raided (`RADMIN`), and
  the sizes of the news view and the Fame leaderboard (`NEWSMAX`, `BRDMAX`).
  The raid screens now name the required camp tier — and the raid-win Fame —
  from the live settings instead of fixed text. In the configuration editor
  all options appear under a single "Fracture: configurable options" section
  and all message text under a single "Fracture: module text" section (the
  editor shows one grouping per level per file).
- In-game sysop tools (`SYSOP.C`/`.H`): live administration gated by the BBS
  **SYSOP** key, reached from the main prompt with a hidden **!** command that is
  invisible to players. `! find <name>` shows a player's full record; `! set <name>
  <field> <value>` edits Fame, Action Points, resources, camp or research tier, or
  position; `! ap <name|ALL> <n>` grants Action Points; `! wipe <name> YES` deletes
  a player; `! reset CONFIRM` wipes every player, regenerates the world, and clears
  the news. The two destructive commands require a literal `YES`/`CONFIRM` token,
  and names match case-insensitively. Admin output uses `prf` only, so no
  message-file entries were added. New data helpers `pl_wipeall`, `pl_ap_all`,
  `pl_leaderboard`, `wld_reset`, and `hist_clear`. ADR 0014 records the design.
- Fame leaderboard: **F** shows the top explorers by Fame, the caller marked with
  their rank beneath. It reads the roster and sorts in memory, and costs no Action
  Points. Listed in both menus (the compact bar also now surfaces the existing
  **U** Upgrade command).
- Batch travel: **`<dir> [R] [count]`** (e.g. `W R 3`) walks several tiles in one
  direction, optionally searching each, stopping at the world edge or when Action
  Points run out, then redrawing the map once with a one-line trip summary (tiles
  charted, Fame, resources gathered, Action Points spent). A plain single step is
  unchanged.
- How-to-play screen: **?** now opens a one-page guide — the world, how Action
  Points work, what each command does, and that Look, Status, Help, and the Board
  cost no Action Points. (Previously `?` only reprinted the command list.)
- Player raiding (`RAID.C`/`.H`): the first player-versus-player interaction.
  **A** lists raidable rivals (players who have built past a Tent, with their camp
  tier and Fame); **A name** raids one. The clash rolls each side's camp tier plus
  research plus a die, the defender favored on home ground; win and you carry off a
  sysop-set share of the target's resources (capped by your storage) plus Fame, lose
  and the Action Points you spent are the cost. Both raider and target must have a
  Cabin or better, so newcomers are safe and can neither raid nor be raided. A raided
  player sees a "RAIDED!" notice on their next login. No new data file (the raid
  counter reuses a spare record byte). Sysop options: `RADOPT` (on/off), `RADCOST`
  (Action Points per raid, default 5), `RADLOOT` (percent looted on a win, default
  25). ADR 0013 records the model.
- Research track (`RESEARCH.C`/`.H`): a five-tier knowledge ladder (Untrained →
  Sharpened Tools → Field Survey → Logistics → Master Craft) that permanently
  improves how a player plays. Each tier raises gather yield (up to +100%) and, from
  the Logistics tier, shaves an Action Point off travel. **K** shows the research
  bench (the whole ladder with your tier marked and the next tier's cost); **K R**
  advances one tier for a scaling resource cost (Wood and Stone, Metal from tier 2).
  The tier is stored in a spare byte of the player record, so there is no new data
  file and existing players start untrained. Sysop option `RSCHOPT` (on/off). ADR
  0012 records the model.
- Trading post (`MARKET.C`/`.H`): a server-wide exchange for turning resources into
  Fame and back. **M** shows the post — a sell and buy price per resource (each
  weighted by scarcity: Metal ×3, Food ×2, the rest ×1) and how much you hold;
  **M S n q** sells and **M B n q** buys (n = 1–5, q = amount). Selling earns Fame,
  buying spends it, with the buy rate kept at or above the sell rate so trading is
  never a free source of Fame, and buys are capped by Fame on hand and camp storage.
  Fame is now spendable and resources have a floor value. No new data file — trades
  settle at once against the acting player. Sysop options: `MKTOPT` (on/off),
  `MKTSELL` (base Fame per unit sold, default 2), `MKTBUY` (base Fame per unit
  bought, default 3). ADR 0011 records the model.
- Workers and passive production (`WORKER.C`/`.H`): players house a crew of NPC
  workers at their camp who produce resources on their own — one day's worth for
  each new day the player logs in. Five worker types each work one resource
  (Forester→Wood, Miner→Stone, Prospector→Metal, Farmer→Food, Water Carrier→Water).
  A new crew screen (**P**) shows the roster and capacity; **P 1**..**P 5** recruits
  a worker for a Wood+Food cost. Camp tier caps crew size (Tent houses none, up to
  16 at a Citadel), giving upgrades a second purpose. The day's weather scales
  output and the whole crew eats Food, so a lean larder or a heat wave bites into
  production. Crew rosters live in a fifth Btrieve file (`FRACWRK.DAT`, one record
  per player). Sysop options: `WRKOPT` (on/off), `WRKPROD` (units per worker per
  day), `WRKFOOD` (Food per worker per day). ADR 0010 records the model.
- Weather and seasons (`WEATHER.C`/`.H`): one server-wide weather condition per
  day (Clear, Rain, Fog, Storm, Snow, Heat Wave), rolled at midnight and biased by
  a four-season cycle (Spring → Summer → Autumn → Winter). Weather is more than
  flavor — rain makes gathering generous while fog, snow, heat, and storms make it
  stingy, and storms and snow add an Action Point to the cost of travel. The
  current condition and season are shown on entry, on the status screen (**T**),
  and with the news (**H**). State lives in a fourth Btrieve file (`FRACWX.DAT`, a
  single record) and catches up lazily if a midnight was missed. Sysop options:
  `WXOPT` (master on/off, default YES) and `SEASLEN` (days per season, default 7,
  1..90). ADR 0008 records the model.
- Arrival quips: returning players are greeted on entry with one of a dozen
  short, irreverent one-liners chosen at random (sysop-editable as `QUIP1`..`QUIP12`
  text blocks; add or reword freely). New explorers still get the first-time intro.
- Initial project scaffolding: repository directory structure (`SRC/`, `DIST/`,
  `DOCS/`, `DOCS/ADR/`, `TESTS/`, `TOOLS/`, `BUILD/`).
- Project documentation: `README.MD`, `TECHNICAL.MD`, `BUILDING.MD`, `CHANGELOG.MD`.
- MIT `LICENSE`.
- `.gitignore` and `.gitattributes` covering Visual Studio/MSBuild artifacts,
  internal development documents, and environment-specific files.
- Visual Studio solution (`Fracture.sln`) and the `FRACTURE` module project
  (Win32/x86, toolset v143), with the SDK location supplied via a git-ignored
  `MBBSSDK.props` (template: `MBBSSDK.props.example`).
- Module entry point (`FRACTURE.C`/`.H`): registers the module, opens its message
  file, reads a sysop logon-greeting option, and runs a keyboard-driven main menu
  (Look / Exit) as the foundation for the game systems to come.
- Module support files: `FRACTURE.MSG` (ANSI message source), `FRACTURE.MDF`
  (module definition), `FRACTURE_EXP.DEF` (export alias), and `FRACTURE.rc`
  (Windows version resource: product, version, copyright).
- Persistent player records (`PLAYER.C`/`.H`): one Btrieve record per player in
  `FRACTURE.DAT`, keyed on the user-id and accessed via the DFA API. The data
  file is created programmatically on first run (no shipped virgin file).
- Core daily loop: each player receives a sysop-configurable number of Action
  Points per day (`DAYAP` option, default 20), refilled lazily on the first
  visit of a new day.
- First real action — **Explore**: spends one Action Point on a weighted-random
  outcome (mostly minor finds, occasional notable discoveries) that awards Fame
  and updates lifetime stats. A live **status** screen shows name, Fame, Action
  Points remaining, and exploration/discovery totals.
- ADR 0002: player record shape and the lazy daily Action Point reset.
- World map (`WORLD.C`/`.H`): a square tile grid in `FRACWLD.DAT` (Btrieve,
  keyed on a packed coordinate), generated once on first run with weighted
  terrain. Sysop-configurable size (`WLDSIZ` option, default 40, 8..64).
- Player position on the map (starts at the center), with travel in the four
  compass directions (**N/S/E/W**, 1 Action Point each), bounded by the world
  edge.
- Tile-based searching (**R**): charting an unexplored tile reveals its terrain
  and awards Fame; re-searching charted ground yields occasional minor finds.
- Local map view (**L**): an ANSI minimap centered on the player showing charted
  terrain glyphs, unexplored tiles, and a legend. Status (**T**) now shows the
  player's coordinates.
- ADR 0003: the world map model (coordinate packing, storage, generation).
- Resources (`RESOURCE.C`/`.H`): a core set of five materials (Wood, Stone,
  Metal, Food, Water) gathered by searching tiles. Each terrain has a
  characteristic yield; counts are kept per player and shown on the status
  screen. ADR 0004 records the resource model.
- Camp system (`CAMP.C`/`.H`): each player has one camp, founded at the map
  center, that upgrades through six tiers (Tent → Cabin → Outpost → Settlement →
  Fort → Citadel). The camp tier sets per-resource storage capacity; gathering is
  capped to it. A camp screen (**C**) shows the tier, location, capacity, and the
  next upgrade cost; **U** spends resources to build the next tier and awards
  Fame. Status shows the current camp and storage cap. ADR 0005 records the camp
  and storage model.
- Server history and daily news (`HISTORY.C`/`.H`): a permanent, server-wide
  chronicle in `FRACHIST.DAT` (Btrieve, append-only). Charting an unexplored tile
  (a server-wide first discovery) and raising a camp to a new tier are recorded
  for every player. A news view (**H**) shows the most recent events newest-first,
  making the shared world feel inhabited. ADR 0006 records the history and news
  model.
- Daily world evolution: the module's midnight cleanup routine (`mcurou`) now runs
  during the nightly BBS cleanup and regenerates a sysop-configurable percentage
  (`REGENPCT` option, default 5, 0..100) of the tiles no player has charted yet,
  with fresh terrain. Charted ground, camps, and structures are preserved; setting
  the option to 0 freezes the world. The result is logged to the audit trail. ADR
  0007 records the model.
