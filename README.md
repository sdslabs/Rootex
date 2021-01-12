# Rootex

<img src="/rootex/assets/rootex.png" width=400 />

[![MIT License](https://img.shields.io/apm/l/atomic-design-ui.svg?)](https://github.com/tterb/atomic-design-ui/blob/master/LICENSEs)
![C/C++ CI](https://github.com/sdslabs/Rootex/workflows/C/C++%20CI/badge.svg)
[![Documentation Status](https://readthedocs.org/projects/rootex/badge/?version=latest)](https://rootex.readthedocs.io/en/latest/?badge=latest)
[![Discord](https://discordapp.com/api/guilds/758961084337618944/embed.png)](https://discord.gg/dXkVEgTPu9)

## Contents

* [What is Rootex?](#what)
* [Why is Rootex?](#why)
* [How do I use Rootex?](#setup)
* [How can I contribute?](#how)
* [License](#license)

## <a name=what>What is Rootex?

Rootex is an advanced C++ 3D game engine powering an in-production game yet to be announced. Rootex is planned to support a free 2.5D local multiplayer physics based sidescroller game to be shipped on Windows and using the DirectX 11 Graphics API.

This game will be a product of a join effort of SDSLabs and [Design Studio](https://designstudio.cc/).

<img src="/rootex/assets/editor.png"/>

### Features

* Based on the popular Entity-Component-System architecture
  * Allows performance benefits due to better CPU cache usage. 
* Full editor GUI implemented in ImGui
  * Similar to Unreal Engine's editor for added familiarity and ease of use.
* 3D DirectX 11 renderer
  * Dynamic Phong-model lighting 
  * CPU particle effects
  * High resolution textures and other popular rendering effects.
* Physics engine powered by Bullet 3D, allowing for flexible, realistic and easy-to-work-with physics in game.
* Audio engine supporting 3D sound attenuation and stereo sound.
* Lua scripting engine with functionality exposed for easy game curation in engine.
* Applies the event-based programming paradigm for better maitainability for game code.

## <a name=why>Why is Rootex?

Rootex is the direct successor of [Rubeus, our 2D Game Engine](https://github.com/sdslabs/Rubeus). Rubeus Engine is now being preserved and any newer features will be planned for Rootex and be implemented in Rootex instead of Rubeus. The game being developed will be announced soon. 

## <a name=setup>How do I use Rootex?

Rootex runs only on Windows and there are no plans to port it to other platforms.

1. Install [Visual Studio 2019 or Visual Studio 2017](https://visualstudio.microsoft.com/vs/), [CMake build system](https://cmake.org/download/) and [OpenAL](https://www.openal.org/).
2. Install Visual Studio Desktop C++ development pack (or anything similar, since C++ is no longer a default language since at least Visual Studio 19)
3. Run `generate_cache.bat /19` for VS 2019 or `generate_cache.bat /17` for VS 2017.
4. Use `build.bat` to build Rootex.

Now you can start reading the [documentation](https://rootex.readthedocs.io/) and build games on Rootex!

## <a name=how>How can I contribute?

Read [here](CONTRIBUTING.md) to know our contribution guidelines. Join our [Discord server](https://discord.gg/dXkVEgTPu9) or optionally ping us at chat.sdslabs.co to get guidance. You can start with setting up Rootex on your Windows machine and try solving a few bugs listed here: https://github.com/sdslabs/Rootex/issues

## <a name=license>License

This project is under the MIT license
