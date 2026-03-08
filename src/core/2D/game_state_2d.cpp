#include "core/2D/game_state_2d.h"

namespace bacon
{
	GameState2D::GameState2D()
	{
		assets = new AssetManager2D();
		scene = new Scene2D();
		renderer = new Renderer2D(800, 600);
	}

	GameState2D::~GameState2D()
	{
		cleanup();
	}

	void GameState2D::cleanup()
	{
		delete assets;
		delete scene;
		delete renderer;
	}
}
