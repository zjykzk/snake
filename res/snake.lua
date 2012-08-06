--require 'conf'

local width,height = window.width,window.height
local step = step

snake = {
  Up = 0, Down = 1, Left = 2, Right = 3,
  direction = 3,
  map={},
  freshPoint = { x = 0, y = 0 }
}

snake.line = {
  tail_ = nil, head_ = nil, length_ = 0
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
  Canvas.drawRect(x, y)
  snake.map[x][y] = 1
  snake.line.length_ = snake.line.length_ + 1
end

snake.line.pop_front = function ()
  local head_ = snake.line.head_
  local n = head_.next_
  n.prev_ = nil
  head_.next_ = nil
  snake.line.head_ = n
  Canvas.delRect()
  snake.map[n.x_][n.y_] = 0
  snake.line.length_ = snake.line.length_ - 1
end

local function calcFreshPoint()
  n = math.random(width * height - snake.line.length_)
  for w = 1, width do
  	for h = 1, height do
  		if snake.map[w][h] == 0 then
        n = n - 1
        if n == 0 then return w, h end
      end
    end
  end
  return 0, 0
end

snake.move = function (direction)
  sumDir = direction + snake.direction
  if  sumDir == snake.Down + snake.Up or sumDir == snake.Left + snake.Right then
    direction = snake.direction
  end

  x, y = snake.line.tail_.x_, snake.line.tail_.y_
  if snake.Left == direction then
    x = x - 1
  elseif snake.Right == direction then
    x = x + 1
  elseif snake.Up == direction then
    y = y - 1
  elseif snake.Down == direction then
    y = y + 1
  else
    snake.move(snake.direction)
    return
  end

  snake.direction = direction
  if x > width then x = 1 end
  if x < 1 then x = width end
  if y > height then y = 1 end
  if y < 1 then y = height end

  if x == snake.freshPoint.x and y == snake.freshPoint.y then
    snake.freshPoint.x, snake.freshPoint.y = calcFreshPoint()
    Canvas.drawFreshPoint(snake.freshPoint.x, snake.freshPoint.y)
    snake.line.push_back(x, y)
    return
  end

  if snake.map[x][y] == 1 then
  	Canvas.over()
  	return 
  end

  snake.line.pop_front()
  snake.line.push_back(x, y)
end

function init()
  for w = 1, width do
  	snake.map[w] = {}
  	for h = 1, height do
  		snake.map[w][h] = 0
  	end
  end

  snake.line.tail_, snake.line.head_, snake.line.length_ = nil, nil, 0

  for x = 1, 7 do
    snake.line.push_back(x, 1)
  end
  snake.freshPoint.x, snake.freshPoint.y = calcFreshPoint()
  Canvas.drawFreshPoint(snake.freshPoint.x, snake.freshPoint.y)
end

init()
