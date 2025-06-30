## BunnyGamePatch

A Rabi-Ribi patch to improve the local co-op experience by addressing the following game bugs:

- Load player 1 amulet charge and BP value from the save file for other players instead of resetting to zero on each save reload/game over.
- Killing enemies now heals all players, not just player 1.

## Installation

Download dll files from the latest GitHub [release](https://github.com/Sinamer/BunnyGamePatch/releases/latest) and put them into the game folder.

## Building

You'll need a bash shell and gcc compiler to run the makefile and output the dll-s.

Navigate to the `BunnyGamePatch` directory and run `make`.

The `build` directory will contain the resulting dll files.