# Goals for 2.0

This document outlines ideas i have for a 2.0 revision that won't be happening any time soon, so consider this as an ideas document and nothing more.

The goal for these is to reduce repetition, expand feature sets while keeping syntax simple and to make the overall workflow easier to manage as manifest file sizes grow. These ideas will not break kmf 1.0 compatibility because theyre are all purely optional and non-destructive

---

## Save empty kmf blocks

KalaMove 1.0 and 1.1 bail out early when a bad kmf block is detected at parse time, this is bad if a bad block is detected in the middle of your kmf file. The goal in 2.0 is to fix this by allowing empty key values but simply skip those in handle stage, and if a path is invalid then we save as empty so it doesnt break stuff

---

## Token key

The purpose of this key is to be able to avoid retyping paths for each origin or target part that is reused

- pass a token as a path or a part of a path, the start of a token is marked with $
- tokens must use the same @ symbol for dir splitting
- tokens must be defined before used, but can be defined at any stage between any kmf blocks

---

## Support for env keys

OS-agnostic env keys work just like the token value, these tokens are restricted and you can't make new tokens with any of these names

### OS-agnostic

| Token       | Windows                                | Linux                        |
|-------------|----------------------------------------|------------------------------|
| $HOME       | C:\Users\username                      | /home/username               |
| $TEMP       | %TEMP%                                 | /tmp                         |
| $DESKTOP    | C:\Users\username\Desktop              | $HOME/Desktop                |
| $DOCUMENTS  | C:\Users\username\Documents            | $HOME/Documents              |
| $DOWNLOADS  | C:\Users\username\Downloads            | $HOME/Downloads (if present) |
| $MUSIC      | C:\Users\username\Music                | ~/Music (if present)         |
| $PICTURES   | C:\Users\username\Pictures             | ~/Pictures (if present)      |
| $VIDEOS     | C:\Users\username\Videos               | ~/Videos (if present)        |
| $PUBLIC     | C:\Users\Public                        | /usr/share or /srv/public or ~/Public |
| $CONFIG     | C:\Users\username\AppData\Roaming      | $HOME/.config                |
| $BIN        | $PROGRAMFILES (defaults to $BINX64)    | /usr/local/bin               |
| $BINX64     | $PROGRAMFILES                          | /usr/local/bin               |
| $BINX32     | $PROGRAMFILESX86                       | /usr/local/bin               |
| $BINLOCAL   | $LOCALAPPDATA\Programs                 | /usr/local/bin               |
	
### Windows-specific

| Token               | Path                                                                 |
|---------------------|----------------------------------------------------------------------|
| $APPDATA            | C:\Users\username\AppData\Roaming                                   |
| $LOCALAPPDATA       | C:\Users\username\AppData\Local                                     |
| $LOCALLOW           | C:\Users\username\AppData\LocalLow                                  |
| $PROGRAMFILES       | C:\Program Files                                                    |
| $PROGRAMFILESX86    | C:\Program Files (x86)                                              |
| $COMMONPROGRAMFILES | C:\Program Files\Common Files or C:\Program Files (x86)\Common Files |

### Linux-specific

| Token            | Path             |
|------------------|------------------|
| $XDG_CONFIG_HOME | $HOME/.config    |
| $XDG_CACHE_HOME  | $HOME/.cache     |
| $XDG_DATA_HOME   | $HOME/.local/share |
| $USR_LOCAL       | /usr/local       |
| $ETC             | /etc             |

---

## Print key

The purpose of this key is to be able to print log messages to KalaMove for debugging or information

- print keys only accept a single value - all content after 'print: ' is what is actually printed to KalaMove output
- print keys accept tokens and can print the value of a token, the token must be defined before the print key
- print keys can be placed anywhere and are saved to the equivalent block where the print key was placed above, then its ran in the handle stage

--- 

## Run key

The purpose of this key is to be able to run external batch/shell/powershell scripts when you need to do an action in between a stage

- run keys only accept a single value - the path to the script file
- run keys dont accept extensions so that the key can remain os-agnostic and run the same way on both windows and linux, instead the parser checks internally if the value leads to a batch or shell script and then runs that
- run keys can be placed anywhere and are saved to the equivalent block where the run key was placed above, then its ran in the handle stage