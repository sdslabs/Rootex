local class = require("game/assets/scripts/utility/middleclass")

Stack = class("Stack")

function Stack:initialize(list)
	if not list then
	  self.stack = {}
	else
	  self.stack = list
	end
end
   
function Stack:push(item)
	self.stack[#self.stack+1] = item
end

function Stack:pop()
	if #self.stack > 0 then
		return table.remove(self.stack, #self.stack)
	end
end
   
function Stack:iterator()
	return function()
		-- call pop to iterate through all items on a stack
		return self:pop()
	end
end
