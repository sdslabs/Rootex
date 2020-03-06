print(#Rootex)
b = Rootex.ResourceLoader.CreateTextResourceFile('LICENSE')
print(b:getString())
a = Rootex.Timer()
print(a:getTimeMs())

window = {
    x = 0,
    y = 0,
    width = 1920,
    height = 1080,
    title = "Rootex",
    msaa = true,
    postInitialize = "game/assets/config/post_initialize.lua"
}
