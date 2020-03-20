function loadEnv(file)
    local newenv = {}
    setmetatable(newenv, { __index=_G })
    loadfile(file, "bt", newenv)()
    return newenv
end
