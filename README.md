# KalaMove

**KalaMove** is a project that grew from a system of many batch and shell files, CMake post build commands and lots of manual file moving into a file movement system of its own. KMF is the official file format for KalaMoveFile files used by KalaMove.exe, the executable of this file move system. Only two external libraries are used and they can be found in `/include/external`.

**KalaMove** is natively supported on Windows and Linux because it mostly uses C++ standard libraries and no OS-specific libraries. The same manifest .kmf file is also reusable across Windows and Linux because it uses OS-agnostic path separator `@` instead of the common `\\` or `\` or `/`. 

---

## How to use 

The first official .kmf version is 1.0 and it is the standard format used for all current KalaKit and Lost Empire Entertainment file distribution systems.

To move files with KalaMove.exe simply create a text file and set its extension to .kmf and write your content inside it. The example section at the bottom of this README shows how to write the keys and how to follow the rules.

### Core Keys

All available keys you can use in .kmf files.

| Key          | Description |
|--------------|-------------|
| `origin`     | Defines the source path where content is copied from, the origin path must always exist |
| `target`     | Defines the destination path(s). Supports multiple paths, separated by `, `. Invalid values are skipped, one value must always exist |
| `overwrite`  | Toggles whether existing files at the target should be overwritten (`yes` or `no`), the overwrite value must always exist |

### Rules

All syntax rules you must follow when creating .kmf files.

| Rule          | Description |
|---------------|-------------|
| Path rule     | All paths in .kmf files must be relative to the **KalaMove** executable |
| Block rule    | Each move block must contain `origin`, `target`, and `overwrite` keys, and each key must end with `: ` |
| `@`           | Use `@` as the directory separator to ensure cross-platform compatibility |
| `..@`, `.@`   | Use `..@` to reference the parent directory and `.@` for the current directory |
| `/`           | Lines starting with `/` (or multiple `/`) are treated as comments |

---

## Example

```
//kmf version must always be at the top.
//comments and empty lines are ignored so you can comment as much as you want
#KMF VERSION 1.0

//
// This example .kmf file copies the KalaWindow readme file and the
// glm folder within its _external_shared folder to the target
// Elypso engine _external_shared folders
//

//file example
origin: KalaWindow@readme.md
target: Elypso-engine@_external_shared@KalaWindow@readme.md
overwrite: yes

//folder example
origin: KalaWindow@_external_shared@glm
target: Elypso-engine@_external_shared@glm
overwrite: no

//parent directory example
origin: ..@readme.md
target: ..@Elypso-engine@readme.md
overwrite: no

//current directory example
origin: .@readme.md
target: .@Elypso-engine@readme.md
overwrite: no
```