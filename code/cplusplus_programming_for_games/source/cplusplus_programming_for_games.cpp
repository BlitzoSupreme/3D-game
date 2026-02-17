#include "GameLoop.h"
#undef main

int main(void) {
	GameLoop* g = new GameLoop();
	g->init();
	while (g->handleInput()) {
		g->update();
		g->draw();
	}
	g->clean();
	delete g;
	return 0;
}