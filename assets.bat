@echo off
IF "%1"=="assets-test" (
    git submodule deinit -f game/assets
    rm -rf game/assets
    rm -rf .git/modules/assets
    git config --file=.gitmodules submodule.assets.url https://gitlab.com/sdslabs/rootex-assets.git
    git config --file=.gitmodules submodule.assets.branch main
    git submodule update --init --recursive --remote
)
IF "%1"=="assets-game" (
    git submodule deinit -f game/assets
    rm -rf game/assets
    rm -rf .git/modules/assets
    git config --file=.gitmodules submodule.assets.url https://gitlab.com/sdslabs/rootex-game.git
    git config --file=.gitmodules submodule.assets.branch main
    git submodule update --init --recursive --remote
)