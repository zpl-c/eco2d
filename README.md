<div align="center">
    <a href="https://github.com/zpl-c/zpl"><img src="https://user-images.githubusercontent.com/2182108/111983468-d5593e80-8b12-11eb-9c59-8c78ecc0504e.png" alt="eco2d" /></a>
</div>

<br />

<div align="center">
    <a href="https://discord.gg/2fZVEym"><img src="https://img.shields.io/discord/354670964400848898?color=7289DA&style=for-the-badge" alt="discord" /></a>
</div>

<br />
<div align="center">
  Small C99 2D game engine with a focus on prototyping.
</div>

<div align="center">
  <sub>
    Brought to you by <a href="https://github.com/zaklaus">@zaklaus</a>,
      <a href="https://github.com/inlife">@inlife</a>
    and <strong>contributors</strong>
  </sub>
</div>

# Introduction
zpl.eco2d is a small framework/game engine made out of curiosity. It attempts to bridge several libraries to create a playable sandbox with ease of extensibility and with performance in mind. The goal is not to make a generic 2D game engine but to build a game prototype that anyone can use to build various experiments.

The game runs on top of [raylib](https://raylib.com/) technologies and makes use of the [zpl](https://zpl.pw/) ecosystem alongside the **cwpack** library for data serialization. The game logic and lifecycle are maintained using [flecs](https://github.com/SanderMertens/flecs/) library and its suite of tools that help us improve the development efficiency.

It was built with networked game sessions in mind from the ground up and therefore provided two significant ways to play the game:
* Networked game (UDP) - networked sessions benefit from the use of [enet](https://github.com/zpl-c/enet/) library.
* Local Only sessions - Data is streamed via local buffers.

In both cases, the game engine does not differentiate between these two options and makes the concept of Server <>client infrastructure entirely abstract. As a result, gameplay logic is only tied to living entities, where the entity might represent a networked client / local controller.

All data is transferred via packets fully automated by our serialization rule system, which uses the **cwpack** library in the background.

Data is streamed in or out using the [librg](https://github.com/zpl-c/librg/) library, thanks to which all clients only receive data relevant to their location.

On top of that, the game client can also spin up multiple client heads and cycle between them, which allows us to easily debug new features locally without having to start the Server physically.

In the abstract sense, we call the Server the game master hosting all gameplay rules and features, while the Client represents a terminal sending key inputs and receiving data to render.

## Major things to do
* More believable world generation.
* Networking implementation, the communication part is done, but there's currently no way to start a headless server or connect to it anyhow.
* Improved rendering - the current world structure does not allow for layered blocks, but it's something worth looking into.
* UI and visual effects

# Build the project
We use CMake to generate project files and manage builds.
You can do the following on the command line to create and build this project:
```sh
git clone https://github.com/zpl-c/eco2d.git
cd eco2d
cmake -S . -B build
cmake --build build
```

Run the following command to see all the options:
```sh
Windows:
build\Debug\eco2d.exe -?

Linux:
build\eco2d.exe -?
```
