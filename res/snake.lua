snake = {
  width = 100,
  height = 100,

  Up = 0,
  Down = 1,
  Left = 2,
  Right = 3,

  x = 100, y = 100,
  map={}
}
snake.move = function (direction)
  x, y = snake.x, snake.y
  if snake.Left == direction then
    x = x - 10
  elseif snake.Right == direction then
    x = x + 10
  elseif snake.Up == direction then
    y = y - 10
  elseif snake.Down == direction then
    y = y + 10
  else
    x = x + 10
  end
  delRect()
  drawRect(x, y)
  snake.x, snake.y = x, y
end

for w = 1, snake.width do
	snake.map[w] = {}
  for h = 1, snake.height do
    snake.map[w][h] = 0
  end
end

drawRect(snake.x,snake.y)
drawRect(snake.x+10,snake.y)
