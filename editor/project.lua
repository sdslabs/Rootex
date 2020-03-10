version = 1.0

project = "Rootex Editor"

window = {
    x = 100,
    y = 100,
    width = 1920,
    height = 1080,
    msaa = false,
    title = "Rootex Editor",
    isEditor = true
}

general = {
    colors = {
        accent = {r = 0.2, g = 0.5, b = 0.2, a = 1.0},
        failAccent = {r = 0.6, g = 0.0, b = 0.0, a = 1.0},
        mediumAccent = {r = 0.15, g = 0.18, b = 0.15, a = 1.0},
        heavyAccent = {r = 0.1, g = 0.15, b = 0.1, a = 1.0},
        viewportTint = {r = 1.0, g = 1.0, b = 1.0, a = 1.0},
        noneLogColor = {r = 1.0, g = 1.0, b = 1.0, a = 1.0},
        warnLogColor = {r = 0.5, g = 0.8, b = 0.8, a = 1.0},
        errorLogColor = {r = 1.0, g = 0.3, b = 0.3, a = 1.0},
        inactive = {r = 0.3, g = 0.3, b = 0.3, a = 1.0},
        success = {r = 0.0, g = 0.3, b = 0.0, a = 1.0},
        failure = {r = 0.3, g = 0.0, b = 0.0, a = 1.0}
    }
}

viewport = {
    aspectRatio = 16 / 9,
    imageTint = general.colors.viewportTint,
    borderColor = general.colors.accent
}
