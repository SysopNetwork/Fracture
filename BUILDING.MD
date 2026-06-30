# Building Fracture

Fracture is a Win32 DLL module for The Major BBS v10. It is built with Microsoft
Visual Studio against the MBBS v10 Module SDK.

## Prerequisites

- **Microsoft Visual Studio 2022** with the *Desktop development with C++* workload.
- **Platform toolset v143** (ships with Visual Studio 2022).
- **Windows 10 SDK** (`WindowsTargetPlatformVersion` 10.0).
- **The Major BBS v10 Module SDK** — provides the headers (`inc/`) and import
  libraries (`lib/importlibs`, `lib/v10`) the module links against. The SDK's
  `V10MOD` sample is the structural template for this module.
- A Major BBS v10 test system for deployment and runtime testing.

## Platform / architecture

> **Build for x86 (Win32) only. Never x64.**
> The Major BBS v10 host process is 32-bit and will not load a 64-bit module.

## Build settings

The project mirrors the SDK sample's configuration:

| Setting | Value |
|---|---|
| Configuration | `Debug` or `Release` |
| Platform | `Win32` (x86) |
| Platform Toolset | `v143` |
| Windows SDK | `10.0` |
| Additional include directories | the SDK `inc` and `inc\msg\v10` folders |
| Additional library directories | the SDK `lib\importlibs` and `lib\v10` folders |
| Additional dependencies | `wgserver_lib.lib`, `galgsbl_lib.lib`, plus the `GAL*_LIB.LIB` import libraries for any subsystem used |

Build output:

- `Debug` configuration → `Debug/FRACTURE.DLL`
- `Release` configuration → `Release/FRACTURE.DLL`

Distribution builds use `Release`.

## Build steps

1. Open the Fracture solution in Visual Studio 2022.
2. Set the active configuration to **Release** and the platform to **Win32 (x86)**.
3. Build the solution. The DLL is produced in the `Release/` folder.

## Message and data files

In addition to the DLL, the module ships supporting files (see [TECHNICAL.MD](TECHNICAL.MD)):

- `.MSG` text/ANSI message source, compiled to `.MCV` at runtime by the host tooling.
- `.MDF` module definition file consumed by the host installer.
- `.DAT` Btrieve data files (with `.VIR` templates so the host can create them).

The `.H` message-number header is regenerated from the `.MSG` source whenever message
definitions change.

## Packaging a release

1. Build the **Release** configuration.
2. Create `DIST/<version>/` (for example `DIST/1.0.0/`).
3. Copy only the deployable runtime files into that folder: the DLL, `.MDF`, `.DAT`,
   `.VIR`, `.MCV`, any `.HLP` and runtime assets. Do **not** include source, project,
   or build files.
4. Ensure every deployable filename and extension is **UPPERCASE**
   (e.g. `FRACTURE.DLL`, `FRACTURE.MDF`).
5. Confirm the DLL carries correct Windows version-resource metadata (product name,
   version, copyright © Sysop Network).

## Deploying to a test system

1. Copy the built DLL and the contents of the module's `DIST/<version>/` folder into
   the Major BBS v10 installation directory.
2. Run the Menu Tree Editor and add Fracture to the menu.
3. Start the system and enter the module to test.
