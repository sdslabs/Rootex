@echo off
IF "%1"=="assets-test" (
    git submodule deinit -f game/assets
    rm -r --force .\game\assets
    rm -r --force .\.git\modules\assets
    IF "%2"=="https" (
        git config --file=.gitmodules submodule.assets.url https://gitlab.com/sdslabs/rootex-assets.git
    ) ELSE (
        git config --file=.gitmodules submodule.assets.url git@gitlab.com:sdslabs/rootex-assets.git
    )
    git config --file=.gitmodules submodule.assets.branch main
)
IF "%1"=="assets-game" (
    git submodule deinit -f game/assets
    rm -r --force .\game\assets
    rm -r --force .git\modules\assets
    IF "%2"=="https" (
        git config --file=.gitmodules submodule.assets.url https://gitlab.com/sdslabs/rootex-game.git
    ) ELSE (
        git config --file=.gitmodules submodule.assets.url git@gitlab.com:sdslabs/rootex-game.git
    )
    git config --file=.gitmodules submodule.assets.branch main
)

IF "%1"=="assets-custom" (
    git submodule deinit -f game/assets
    rm -r --force .\game\assets
    rm -r --force .git\modules\assets
    git config --file=.gitmodules submodule.assets.url %2
    git config --file=.gitmodules submodule.assets.branch main
)
echo Submodule updated. Run 'git submodule update --init --recursive --remote' for changes to reflect in game/assets