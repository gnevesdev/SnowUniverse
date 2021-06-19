# SnowUniverse
A game about orbits and the universe :ringed_planet:

[hadron13](https://github.com/hadron13) challenged me to make a game like [Orbiter's Challenge](https://omnes-ludos.itch.io/orbiters-challenge), created by himself. And I did. In 3 days.

# Building :hammer_and_wrench:
The code was tested on Windows and Linux, with GCC, Clang and MSVC compilers. However, other compilers and platforms should work just fine.

You need to install the [SDL library](https://www.libsdl.org/download-2.0.php) and download the source code. Then use your C or C++ compiler of choice to build the game. If using the Clang or MinGW compiler, `-O2` flag is recommended for redistribution.

For compiling on Windows, include the library files for SDL2 and SDL2main, putting SDL2main before. If you are using MinGW also add the mingw32 library. Like this:
```sh
# For MinGW
gcc -O2 src/*.c -lmingw32 -lSDL2main -lSDL2 -o MyBuild.exe

# For Clang and possibly others
clang -O2 src/*.c -lSDL2main -lSDL2 -o MyBuild.exe
```

On Linux and other UNIX based systems, include the math C library (comes with the C compiler, no need to install it separately). You do not need SDL2main nor mingw32 here:
```sh
# The math library is just called "m"
gcc -O2 src/*.c -lSDL2 -lm -o MyBuild
```

Remember to report any bugs on the [issues](https://github.com/gnevesdev/SnowUniverse/issues) page.

