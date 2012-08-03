require "conf"

snake = {
	width, height = 100, 100
	map = {}
	direction = 0
	for w = 1, width do
		for h = 1, height do
      map[w][h] = 0
    end
  end

  function move(direction)
  end
}
