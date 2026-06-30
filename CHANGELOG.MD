# Changelog

All notable changes to Fracture are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Added
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
