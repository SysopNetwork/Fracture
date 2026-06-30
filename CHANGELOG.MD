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
  (module definition, no Btrieve files yet), `FRACTURE_EXP.DEF` (export alias),
  and `FRACTURE.rc` (Windows version resource: product, version, copyright).
