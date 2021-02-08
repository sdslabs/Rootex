VAR money = 100
VAR has_knife = true
VAR name = "Katy"

{ money }
~ money++
{ money }
~ money += 100
{ money }
~ money -= 50
{ money }
~ money--
{ money }
~ money = 500
~ temp coins = 20

Hello, { name }! Do you have { money } bucks? { coins } pennies may be?
{ has_knife: Nope| Yeap}.