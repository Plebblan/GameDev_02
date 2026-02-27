CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
SDLFLAGS = $(shell pkg-config --cflags --libs sdl3)

SRC = src/main.cpp src/arena.cpp src/ball.cpp src/player.cpp src/game.cpp
OBJ = $(SRC:.cpp=.o)
OUT = game

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(OBJ) -o $(OUT) $(SDLFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDLFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OUT)