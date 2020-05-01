function onBegin(entity)
    number = 0
    isFailed = false
    text = entity:getTextVisual2D()
end

function onUpdate(delta, entity)
    if isFailed then
        text:setText("Wasted")
        return
    end
    number = number + 1
    text:setText(tostring(number))
end

function onFail()
    isFailed = true
end

function onEnd(entity)
end
