CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
TARGET = game
SOURCES = main.cpp Game.cpp Player.cpp Map.cpp Event.cpp SaveSystem.cpp Item.cpp MiniGame.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
