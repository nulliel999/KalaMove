# Move Project

The purpose of this project is to move files from [KalaWindow](https://github.com/kalakit/kalawindow) to several external targets efficiently and quickly so we can avoid CMake and batch script hell. This ensures each target repository has the latest up to date content from KalaWindow

You do not need to use this executable for normal operation of KalaWindow or any other software or libraries made by Lost Empire Entertainment unless you want the exact same hierarchy structure as Lost Empire Entertainment uses for its development and testing of KalaWindow and the target repositories. If any target directories are missing then they are ignored.

Current targets:

- Circuit Chan
- Elypso Engine
- Elypso Hub
- Hinnamasin (internal project)

Required hierarchy:

> Note: All names are case-sensitive and must match, except folders named 'example'

> Note: each target folder requires _external_shared folder inside itself so that KalaWindow content can be copied to it correctly

```
/example
	/MoveProject                     (this repository root)
		/targets.txt                 (lists each project folder name)
		/MoveProject.exe             (the executable itself)
		/example/MoveProject         (optional sub-folder location)
		/example/example/MoveProject (optional sub-folder location)
	/KalaWindow                      (several files are copied from here)
		/_external_shared            (all external libraries are copied from here)
	/example
		/_external_shared
	/example
		/_external_shared
	and so on...
```