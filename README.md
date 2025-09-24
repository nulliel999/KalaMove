# KalaMove

**KalaMove** is a project that grew from a system of many batch and shell files, CMake post build commands and lots of manual file moving into a file movement system of its own. `.kmf` is the official file format for **KalaMoveFile** files used by **KalaMove**, the executable of this file move system. The first official `.kmf` version is 1.0 and it is the standard format used for distributing all **KalaKit** and **Lost Empire Entertainment** files. 

**KalaMove** is natively supported on **Windows** and **Linux** because it mostly uses C++ standard libraries and no OS-specific libraries. Only two external libraries are used and they can be found in `/include/external`.

---

## How to use 

To move files with **KalaMove** simply create a text file in the same directory as the executable and set its extension to `.kmf` and write your content inside it. The same manifest `.kmf` file is also reusable across **Windows** and **Linux** because it uses an OS-agnostic directory separator `@` instead of the common `\\` or `\` or `/` variants.  The example section at the bottom of this **README** file shows how to write a simple `.kmf` file.

### Core Keys

All available keys you can use in `.kmf` files.

| Key          | Description |
|--------------|-------------|
| `origin`     | Defines the source path where content is copied from, the origin path must always exist |
| `target`     | Defines the destination path(s). Supports multiple paths, separated by `, `. Invalid values are skipped, one value must always exist |
| `overwrite`  | Toggles whether existing files at the target should be overwritten (`yes` or `no`), the overwrite value must always exist |

### Rules

All syntax rules you must follow when creating `.kmf` files.

| Rule          | Description |
|---------------|-------------|
| Path rule     | All paths in `.kmf` files must be relative to the **KalaMove** executable |
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