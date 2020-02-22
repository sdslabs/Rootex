window = {
    x = 100,
    y = 200,
    deltaX = 640,
    deltaY = 480,
    title = "Rootex"
}

testFunction = function ()
    print("Wow I was actually called this time")
    local a = TestClass()
    print("a is made")
    a:exit()
end
