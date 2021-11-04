CC=g++
CFLAGS= -c -Wall
SDL2= -w -lSDL2

all: ParticleExplosion

ParticleExplosion: main.o engine.o renderer.o window.o pixel.o swarm.o
	$(CC) main.o engine.o renderer.o window.o pixel.o swarm.o $(SDL2) -o ParticleExplosion

main.o: src/main.cpp
	$(CC) $(CFLAGS) -c src/main.cpp

engine.o: src/engine.cpp
	$(CC) $(CFLAGS) src/engine.cpp

renderer.o: src/renderer.cpp
	$(CC) $(CFLAGS) src/renderer.cpp

window.o: src/window.cpp
	$(CC) $(CFLAGS) src/window.cpp

pixel.o: src/pixel.cpp
	$(CC) $(CFLAGS) src/pixel.cpp

swarm.o: src/swarm.cpp
	$(CC) $(CFLAGS) src/swarm.cpp

clean:
	rm *.o ParticleExplosion
