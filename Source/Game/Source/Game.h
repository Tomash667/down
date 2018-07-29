#pragma once

#include <GameHandler.h>

class Game : public GameHandler
{
public:
	Game();
	~Game();
	int Start();

private:
	void InitLogger();
	void InitEngine();
	void InitGame();
	void LoadResources();
	bool OnTick(float dt) override;

	unique_ptr<Engine> engine;
	Scene* scene;
	Input* input;
	ResourceManager* res_mgr;
	SoundManager* sound_mgr;
};
