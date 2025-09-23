# Move Project

The purpose of this project is to move files from KalaWindow to several external targets efficiently and quickly so we can avoid CMake and batch script hell. This ensures each target repository has the latest up to date 

It is not required to be used unless you want the exact same hierarchy structure as Lost Empire Entertainment uses for its development and testing of KalaWindow and the target repositories. If any target directories are missing then they are ignored.

Current targets:

- Circuit Chan
- Elypso Engine
- Elypso Hub
- Hinnamasin (internal project)

Required hierarchy:

> Note: All names are case-sensitive and must match except the projects root folder names

```
/projects_root
	/MoveProject (this repository root)
	/KalaWindow
	/Elypso-engine
	/Elypso-hub
	/Circuit-chan
```