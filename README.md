# cpp-platformer-collab

## How to play
clone the repo:\
`git clone https://github.com/ojace8143/cpp-platformer-collab`

compile it\
`make`\
(requires SFML3+ to compile, you may have to change `Makefile` to compile successfully)

then run the binary\
`./bin/platformer`\
or:\
`make run`

### Config.hpp
config.hpp has options you can change to alter player settings, camera settings, window settings, and costmetics. also includes some debugs. once you make these changes:\
`make clean` (to remove compiled binaries) then `make` to recompile. then run the binary as normal.\
or:\
`make clean run` (which will run it after you recompile)

## TODO list
- [x] get a basic layout
- [x] make a starting script for SFML
- [x] Smooth movement
- [x] Deltatime movement
- [x] Make platform struct
- [x] Collision
- [x] Make sure everything has a config in config.hpp
- [x] multiple platforms
- [x] Scrolling (with wasd)
- [x] plan stuff (toby lock in)
- [ ] background (probably next)
- [x] make text struct
- [x] lerping
- [ ] checkpoints??? 3/1 <- yes/no (level 2 will have less checkpoints)
- [ ] parallax background
- [x] game states
- [ ] gameplay loop
- [x] title screen and pause screen
- [x] menus (i'll count it since its functional)
- [x] respawns after death
- [x] die if you fall off the map
- [ ] hire playtesters
- [x] lock in
- [ ] cooler name (think of a name for the game)
- [x] level select AREA that you get dropped into (daylen 200iq)
- [x] nerf spike hitboxes
- [ ] 

## Notes

i MIGHT have just fallen in love with cpp.....

atp just making an entire game engine (when i'm done i'll make a fork with the actual engine stuff)

deadline in 2 weeks

## Ai use
VERY limited ai use. Only used for chore tasks like renaming things, and debugs. All code written by me except for renaming tasks as I have said.
