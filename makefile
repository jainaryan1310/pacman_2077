CC = g++
CFLAGS = -I. -Wall
TARGET = main
THREAD = -lpthread
LIBS = sdl2-config --cflags --libs

all: $(TARGET)


$(TARGET): $(TARGET).cpp
	$(CC) $(TARGET).cpp maze_render.cpp objects.cpp font.cpp client_udp.cpp server_udp.cpp network.cpp menu.cpp list.cpp -o game `$(LIBS)` -lSDL2_ttf $(THREAD) $(CFLAGS)


clean:
	$(RM) game