#pragma once

#include <GameHandler.h>

enum Animation
{
	ANI_STAND,
	ANI_ROTATE_LEFT,
	ANI_ROTATE_RIGHT,
	ANI_WALK,
	ANI_WALK_BACK,
	ANI_RUN
};

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
	bool OnTick(float dt) override;
	void OnDebugDraw(DebugDrawer* debug_drawer);
	void UpdatePlayer(float dt);
	void UpdateCamera(float dt);
	void Save(FileWriter& f);
	void Load(FileReader& f);

	unique_ptr<Engine> engine;
	Scene* scene;
	Input* input;
	ResourceManager* res_mgr;
	SoundManager* sound_mgr;

	unique_ptr<PhysicalWorld> phy_world;
	bool draw_phy;

	SceneNode* player;
	Camera* cam;
	Vec2 cam_rot;
	float player_rot_to;
	Animation player_ani;
};
