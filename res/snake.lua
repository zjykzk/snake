--require 'conf'

local width,height = window.width,window.height
local step = step

snake = {
  Up = 0, Down = 1, Left = 2, Right = 3,
  direction = 3,
  map={},
}

snake.line = {
  tail_ = nil, head_ = nil,
}

snake.line.push_back = function (x, y)
  local head_, tail_ = snake.line.head_, snake.line.tail_
  local n = {x_ = x, y_ = y, next_ = nil, prev_ = nil}
  if not head_ then
    snake.line.tail_ = n 
    snake.line.head_ = n
  else
    tail_.next_ = n
    n.prev_ = tail_ 
    tail_ = n
    snake.line.tail_ = n
  end
  drawRect(x, y)
  snake.map[x][y] = 1
end

snake.line.pop_front = function ()
  local head_ = snake.line.head_
  local n = head_.next_
  n.prev_ = nil
  head_.next_ = nil
  snake.line.head_ = n
  delRect()
  snake.map[n.x_][n.y_] = 0
end

snake.move = function (direction)
  x, y = snake.line.tail_.x_, snake.line.tail_.y_
  if snake.Left == direction then
    snake.direction = snake.Left
    x = x - 1
  elseif snake.Right == direction then
    snake.direction = snake.Right
    x = x + 1
  elseif snake.Up == direction then
    snake.direction = snake.Up
    y = y - 1
  elseif snake.Down == direction then
    snake.direction = snake.Down
    y = y + 1
  else
    snake.move(snake.direction)
    return
  end
  snake.line.pop_front()
  if x > width then x = 1 end
  if x < 1 then x = width end
  if y > height then y = 1 end
  if y < 1 then y = height end
  snake.line.push_back(x, y)
end

local function init()
  for w = 1, width do
  	snake.map[w] = {}
  	for h = 1, height do
  		snake.map[w][h] = 0
  	end
  end

  for x = 1, 7 do
    snake.line.push_back(x, 1)
  end
end

init()
