# KalaMove

**KalaMove** is a project that grew from a system of many batch files, CMake post build commands and lots of manual file moving into a file movement system of its own. KMF is the official file format for KalaMoveFile files used by KalaMove.exe, the executable of this system.

**KalaMove** is natively supported on Windows and Linux because it mostly uses C++ standard libraries and no OS-specific libraries. The only external library it uses is [logging.hpp](https://github.com/KalaKit/KalaHeaders/blob/main/logging.hpp) for logging messages to the console.

## How to use 

The first official .kmf version is 1.0 and it is the standard format used for all current KalaKit and Lost Empire Entertainment file distribution systems.

Old versions are not guaranteed to work with newer .kmf files.

To move files with KalaMove.exe simply create a text file and set its extension to .kmf and write your content inside it. Follow 'MANIFEST.kmf' or the example section below to see how to use .kmf files correctly with KalaMove.exe. The details section below explains syntax rules and other info in more depth.

---

## Example

```
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
```

---

## Details

These are the important rules you must follow when using KalaMove and creating .kmf files.

- all paths must be relative to the parent directory of KalaMove.exe
- use '@' for paths
- use '/' or any number of it for comments
- 'origin' is where content is copied from
- 'target' is where content is copied to, it supports multiple paths, separate each path with ', '
- 'overwrite' is used to toggle if overwrite is allowed or not
- each move block MUST have 'origin', 'target' and 'overwrite' keys and the key must end with ': '
- any invalid or non-existing origin stops the whole program and you must fix it
- any invalid or non-existing target is ignored and KalaMove simply skips it and tries to paste to the next target path