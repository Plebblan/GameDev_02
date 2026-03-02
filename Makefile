CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
SDLFLAGS = $(shell pkg-config --cflags --libs sdl3 sdl3-ttf sdl3-mixer)

SRC = src/main.cpp src/arena.cpp src/ball.cpp src/player.cpp src/game.cpp src/menu.cpp 
OBJ = $(SRC:.cpp=.o)
OUT = game

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(OBJ) -o $(OUT) $(SDLFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDLFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OUT)