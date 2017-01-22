all: game

game: mygame.cpp glad.c
	g++ -o game mygame.cpp glad.c -lGL -lglfw -ldl -lao -lmpg123 -lm

clean:
	rm game
