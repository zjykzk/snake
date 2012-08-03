snake = {
  width = 300,
  height = 300,

  Up = 0,
  Down = 1,
  Left = 2,
  Right = 3,
  Step = 10,

  x = 100, y = 100,
  map={}
}

snake.move = function (direction)
  x, y = snake.x, snake.y
  if snake.Left == direction then
    x = x - snake.Step
  elseif snake.Right == direction then
    x = x + snake.Step
  elseif snake.Up == direction then
    y = y - snake.Step
  elseif snake.Down == direction then
    y = y + snake.Step
  else
    x = x + snake.Step
  end
  delRect()
  if x > snake.width then x = 100 end
  if x < 0 then x = snake.width end
  if y > snake.height then y = 100 end
  if y < 0 then y = snake.height end
  drawRect(x, y)
  snake.x, snake.y = x, y
end

for w = 1, snake.width do
	snake.map[w] = {}
  for h = 1, snake.height do
    snake.map[w][h] = 0
  end
end

local function init()
	for i = 0, 6 do
		drawRect(snake.x + i * snake.Step, snake.y)
	end
	snake.x = snake.x + 6 * snake.Step
end

init()
