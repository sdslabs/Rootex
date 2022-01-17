@echo off
IF "%1"=="assets-test" (
    git submodule deinit -f game/assets
    rm -r --force .\game\assets
    rm -r --force .\.git\modules\assets
    git config --file=.gitmodules submodule.assets.url https://gitlab.com/sdslabs/rootex-assets.git
    git config --file=.gitmodules submodule.assets.branch main
    git submodule update --init --recursive --remote
    cd game/assets
    git remote add upstream git@gitlab.com:sdslabs/rootex-assets.git
)
IF "%1"=="assets-game" (
    git submodule deinit -f game/assets
    rm -r --force .\game\assets
    rm -r --force .git\modules\assets
    git config --file=.gitmodules submodule.assets.url https://gitlab.com/sdslabs/rootex-game.git
    git config --file=.gitmodules submodule.assets.branch main
    git submodule update --init --recursive --remote
    cd game/assets
    git remote add upstream git@gitlab.com:sdslabs/rootex-game.git
)

IF "%1"=="assets-custom" (
    git submodule deinit -f game/assets
    rm -r --force .\game\assets
    rm -r --force .git\modules\assets
    git config --file=.gitmodules submodule.assets.url %2
    git config --file=.gitmodules submodule.assets.branch main
    git submodule update --init --recursive --remote
)