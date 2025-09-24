# KalaMove

**KalaMove** is a project that grew from a system of many batch and shell files, CMake post build commands and lots of manual file moving into a file movement system of its own. KMF is the official file format for KalaMoveFile files used by KalaMove.exe, the executable of this file move system.

**KalaMove** is natively supported on Windows and Linux because it mostly uses C++ standard libraries and no OS-specific libraries. The same manifest .kmf file is also reusable across Windows and Linux because it uses OS-agnostic path separator '@' instead of the common '\\' or '\' or '/'. Only two external libraries are used and they can be found in '/include/external'.

## How to use 

The first official .kmf version is 1.0 and it is the standard format used for all current KalaKit and Lost Empire Entertainment file distribution systems.

Old versions are not guaranteed to work with newer .kmf files.

To move files with KalaMove.exe simply create a text file and set its extension to .kmf and write your content inside it. Follow 'MANIFEST.kmf' or the example section below to see how to use .kmf files correctly with KalaMove.exe. The details section below explains syntax rules and other info in more depth.

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

---

## Details

These are the important rules and details you should know when creating or modifying .kmf files.

- 'origin' is where content is copied from  
- 'target' is where content is copied to, it supports multiple paths, separated by ', '  
- 'overwrite' is used to toggle if overwrite is allowed or not  
- each move block MUST have 'origin', 'target' and 'overwrite' keys and the key must end with ': '  
- use '@' for directory splitting  
- get the parent directory with '..@' and the current directory with '.@'  
- all paths are relative to the KalaMove executable  
- use '/' or any number of it for comments  
- overwrite key must have 'yes' or 'no' as its value  
- any invalid or non-existing origin stops the whole program and you must fix it  
- any invalid or non-existing target is ignored and KalaMove simply skips it and tries to paste to the next target path  
