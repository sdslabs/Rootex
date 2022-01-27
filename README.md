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

Rootex is an advanced C++ 3D game engine powering an in-production game yet to be announced. The game will finally ship on Windows and use DirectX 11.

Find the upcoming features and the game's [public Trello board here!](https://trello.com/b/ES4oR0Gs/rootex-game)

This game will be a product of a join effort of SDSLabs and [Design Studio](https://www.instagram.com/ds_iitr/?hl=en).

<img src="/rootex/assets/editor.png"/>

### Features

* Based on the popular Entity-Component-System architecture
  * Allows performance benefits due to better CPU cache usage. However over-optimizations are not the goal
  * Uses an impure ECS where components can have functions
* Full editor GUI implemented in ImGui
  * Similar interface as other popular game engine editors for added familiarity and ease of use
  * Readable asset files (100% JSON)
* 3D DirectX 11 renderer
  * Dynamic and static Phong-model lighting
  * High resolution textures for Diffuse, Normal, Specular and Light mapping
  * Post processing effects like gaussian blur, bloom, Adaptive SSAO, FXAA, including ad-hoc sepia, black-and-white, tonemapping
  * Mildly configurable CPU based particle effects
  * Effekseer Particle effects integration available for high quality VFX
  * Environment effects like Sky sphere, sky reflections, refractions and depth fog
  * Supports basic transform and skeletal animations
  * Automatic LOD (level-of-detail) generation for 3D models and animations
  * Custom materials using custom HLSL shaders
  * And few more things...
* RmlUi integration which allows writing/debugging UI overlays in an HTML/CSS-like language and Lua
* Physics engine powered by Bullet Physics 3, allows an easy-to-work-with interface for making physics simulations
* OpenAL-Soft based audio engine supporting 3D attenuation and stereo sound
* Lua scripting engine with functionality exposed for easy game curation
  * Uses an object-oriented approach with Lua scripting for making intuitive scope declarations and reducing garbage generation
  * Lua debugger integration
  * Equipped with a tweening API provided by [flux](https://github.com/rxi/flux)
* Applies the event-based programming paradigm for better maitainability of game code
* Being developed for an actual game
  * Some game-specific features are also present like the Inky [Lua runtime integration](https://github.com/astrochili/narrator/) which allows writing dialogue in the [Ink language](https://www.inklestudios.com/ink/) and running them inside Rootex.
* Documentation

## <a name=why>Why is Rootex?

Rootex is the direct successor of [Rubeus, our 2D Game Engine](https://github.com/sdslabs/Rubeus). Rubeus Engine is not being maintained and is being preserved. Newer features will only be planned for Rootex. The game being developed will be announced soon as well.

## <a name=setup>How do I use Rootex?

Rootex runs only on Windows and there are no plans to port it to other platforms.

1. Install [Visual Studio 2019 or Visual Studio 2017](https://visualstudio.microsoft.com/vs/), [CMake build system](https://cmake.org/download/).
2. Install Visual Studio Desktop C++ development pack (or anything similar, since C++ is no longer a default language since at least Visual Studio 19)
3. Run `generate_cache.bat /19` for VS 2019 or `generate_cache.bat /17` for VS 2017.
4. Use `build.bat` to build Rootex.
 
Assets Workflow :
 
1. Assets are stored in separate repositories and added as git submodules. The testing assets are stored at https://gitlab.com/sdslabs/rootex-assets while the assets for the game are stored at https://gitlab.com/sdslabs/rootex-game.
2. The `assets.bat` script has two subcommands to make switching of the submodules between the testing and game repository easier. Use `assets.bat assets-test` to switch to the test repository and `assets.bat assets-game` to switch to the game repository. The submodule will have two remote urls set. The `origin` points to the HTTPS url while `upstream` points to the SSH url.
3. You can also add your own repository as an assets submodule. Use `assets.bat assets-custom <url>` for that. If setting a custom url, use an HTTPS url here and then set an SSH remote afterwards.
 
**__WARNING__** : Running `assets.bat` will delete the `game/assets/` folder. Be sure to backup any unsaved progress before running it.

Now you can start reading the [documentation](https://rootex.readthedocs.io/) and build games on Rootex!

> **_NOTE:_**  If you get the error `dxgidebug.dll not loaded` while opening the Rootex Editor, install *Graphics Tools* by following this [guide](https://docs.microsoft.com/en-us/windows/uwp/gaming/use-the-directx-runtime-and-visual-studio-graphics-diagnostic-features).

## <a name=how>How can I contribute?

Read [here](CONTRIBUTING.md) to know our contribution guidelines. Join our [Discord server](https://discord.gg/dXkVEgTPu9) or optionally ping us at chat.sdslabs.co to get guidance. You can start with setting up Rootex on your Windows machine and try solving a few bugs listed here: https://github.com/sdslabs/Rootex/issues

## <a name=license>License

This project is under the MIT license. See `THIRDPARTY.md` for thirdparty license notices.
