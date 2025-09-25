# KalaMove

**KalaMove** is an executable that uses `.kmf` files that are designed as a lightweight, strict, and repeatable alternative to batch/shell scripts, CMake commands, or even full installers when all you need is reliable file movement. **KalaMove** is natively supported on **Windows** and **Linux** because it mostly uses C++ standard libraries and no OS-specific libraries. Only three external headers from [KalaHeaders](https://github.com/kalakit/kalaheaders) are used and they can be found in `/include/external`.

### Why `.kmf` over scripting?  

| Advantage                  | Explanation |
|----------------------------|-------------|
| **Strict & Repeatable**    | Every `.kmf` file follows the same format with strict rules. No surprises, no “works on my machine” problems - execution is always consistent. |
| **Cross-Platform by Design** | A single `.kmf` manifest works on both **Windows** and **Linux**, thanks to the OS-agnostic `@` separator. |
| **Readable & Maintainable** | `.kmf` files are plain text with human-friendly keys like `origin`, `target`, and `overwrite`. No need to debug fragile scripts. |
| **Multiple Targets**       | A single `origin` can copy to multiple `target` locations in one block — something most batch/shell/CMake setups handle poorly. |
| **Safe by Default**        | Invalid origins immediately stop execution, preventing data loss. Invalid targets are skipped safely without halting everything. |

### Why `.kmf` over installers?  

Traditional installers (NSIS, Inno Setup, MSI, etc.) are often **overkill** if you only need to move files. They come with:  
- OS lock-in (Windows-only).  
- Complex scripting languages.  
- Extra features (registry, compression, uninstallers) you may not need.  

**KalaMove** provides a simpler alternative:  

| Feature                    | KalaMove |
|----------------------------|----------|
| **Cross-platform**         | Yes - `.kmf` works on both Windows & Linux |
| **Executable size**        | <150 KB standalone `.exe` |
| **Transparency**           | Users can read `.kmf` files directly |
| **Zero setup**             | Drop the `.exe` anywhere and run it |
| **Focused scope**          | File movement only - no bloat |

### Real-World Use Cases  

- **Game Engine Development**  
  Keep shared headers, shaders, or assets synced between frameworks (`KalaWindow`) and dependent projects (`Elypso Engine`, `Elypso Hub`, `CircuitChan`).  

- **Multi-Repository Workflows**  
  Maintain 5–10 repositories that rely on the same master library. One `.kmf` ensures everything is copied consistently.  

- **End-User Distribution**  
  Instead of creating a full installer, ship your software with a `.kmf` manifest. The user runs `KalaMove.exe`, and files are placed correctly - repeatable, always the same.  

- **CI/CD Pipelines**  
  Use `.kmf` in automated build systems to copy outputs into test directories or package folders with strict validation.  

---

## How to use 

To handle files with **KalaMove** simply create a text file in the same directory as the executable and set its extension to `.kmf` and write your content inside it. The example section at the bottom of this **README** file shows how to write a simple `.kmf` file. All relative paths are built internally relative to their `.kmf` file.

### Run types

You can run **KalaMove** manually or from your console. by running its executable manually which gets `.kmf` files from the same folder as the exe itself is at, or by passing `.kmf` paths to **KalaMove** executable from your console as parameters.

| Key          | Description |
|--------------|-------------|
| `manual`     | Picks up all `.kmf` files from the same folder where the exe is run |
| `console`    | Accepts `.kmf` file paths as parameters from the console, allowing you to run them from outside the exe folder |

### Core Keys

All available keys you can use in `.kmf` files.

| Key          | Description |
|--------------|-------------|
| `origin`     | Defines the source path where content is copied from, the origin path must always exist |
| `target`     | Defines the destination path(s). Supports multiple paths, separated by `, `. Invalid values are skipped, one value must always exist |
| `action`     | Handle origin and target paths with chosen action type |

### Action types

All available action types you can use for the action keyword.

| Action      | Description |
|-------------|-------------|
| `copy`      | Copy origin to target path(s), skip if target exists |
| `forcecopy` | Copy origin to target path(s), overwrite if target exists |
| `move`      | Force copy origin to all targets except last, then move origin to last target |
| `rename`    | Rename each target to use origin’s stem, origin must be a valid path |
| `delete`    | Delete all target paths, origin is validated but not used |
| `create`    | Create a new directory at chosen target paths, origin is validated but not used |

### Syntax rules

All syntax rules you must follow when creating `.kmf` files.

| Rule          | Description |
|---------------|-------------|
| Name rule     | Origin name does not need to match with target names as long as extensions match and file-file or dir-dir matches |
| Block rule    | Each move block must contain `origin`, `target`, and `action` keys, and each key must end with `: ` |
| `@@`          | Use `@@` for absolute paths, relative path to executable is used if no `@@` was added at the front of a path. Using absolute paths naturally makes the .kmf file not cross platform because of how absolute paths work differently on Windows and Linux |
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
// This example .kmf file shows how to use the three different actions
// and how to handle relative, absolute, parent and current directory.
//

//file example
origin: KalaWindow@readme.md
target: Elypso-engine@_external_shared@KalaWindow@readme.md
action: copy

//folder example
origin: KalaWindow@_external_shared@glm
target: Elypso-engine@_external_shared@glm
action: copy

//absolute path example
origin: @@C:@Users@greenlaser@Documents@_cpp@KalaWindow@readme.md
target: @@C:@Users@greenlaser@Documents@_cpp@Elypso-engine@_external_shared@KalaWindow@readme.md
action: copy

//relative parent directory example
origin: ..@readme.md
target: ..@Elypso-engine@readme.md
action: copy

//relative current directory example
origin: .@readme.md
target: .@Elypso-engine@readme.md
action: copy
```