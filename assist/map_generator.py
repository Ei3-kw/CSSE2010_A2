import random

EMPTY = 0
B_WALL = 3
WALL = 4
DIAMOND = 5
COMPONENTS = [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,\
	B_WALL, B_WALL, WALL, WALL, WALL, WALL, WALL, WALL, DIAMOND]

i = 0
while i < 8:
	i += 1
	j = 0
	line = []
	while j < 16:
		j += 1
		line.append(random.choice(COMPONENTS))
	print(f"{line},")

