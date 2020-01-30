Entity = {
    Components = {
        TransformComponent = { 
            m_Position = { x = 0, y = 0, z = 0 },
            m_Rotation = { yaw = 0, pitch = 0, roll = 0 },
            m_Scale = { x = 0.5, y = 0.5, z = 0.5 }
        },
        DiffuseVisualComponent = {
            resFile = "game/assets/sphere.obj",
            texturePath = "game/assets/test/yellow.png",
        },
        PointLightComponent = {
            attLin = 0.045,
            attQuad = 0.0075,
            attConst = 1.0,
            range = 10.0,
            diffuseIntensity = 2.0,
            diffuseColor = {r = 0.5, g = 0.7, b = 1.0, a = 1.0},
            ambientColor = {r = 0.1, g = 0.2, b = 0.3, a = 1.0},
        },
    }
}
