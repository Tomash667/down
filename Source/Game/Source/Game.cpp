#include "GameCore.h"
#include "Game.h"
#include <Engine.h>
#include <Input.h>
#include <ctime>
#include <Window.h>
#include <SceneNode.h>
#include <Scene.h>
#include <ResourceManager.h>
#include <Camera.h>
#include <MeshInstance.h>
#include "PhysicalWorld.h"

Game::Game() : draw_phy(false)
{
}

Game::~Game()
{
}

int Game::Start()
{
	engine.reset(new Engine);

	InitLogger();

	try
	{
		InitEngine();
	}
	catch(cstring err)
	{
		engine->ShowError(Format("Failed to initialize engine: %s", err));
		return 1;
	}

	try
	{
		InitGame();
	}
	catch(cstring err)
	{
		engine->ShowError(Format("Failed to initialize game: %s", err));
		return 2;
	}

	try
	{
		engine->Run();
	}
	catch(cstring err)
	{
		engine->ShowError(Format("Fatal error when running game: %s", err));
		return 3;
	}
	catch(...)
	{
		engine->ShowError("Fatal unknown error when running game.");
		return 3;
	}

	return 0;
}

void Game::InitLogger()
{
	// log to text file and to console in debug build
	FileLogger* file_logger = new FileLogger("log.txt");
	if(DEBUG_BOOL)
	{
		MultiLogger* multi_logger = new MultiLogger;
		multi_logger->Add(file_logger);
		multi_logger->Add(new ConsoleLogger);
		Logger::Set(multi_logger);
	}
	else
		Logger::Set(file_logger);

	// log start
	time_t t = time(0);
	tm t2;
	localtime_s(&t2, &t);
	Info("Down Adventure");
	Info("Date: %04d-%02d-%02d", t2.tm_year + 1900, t2.tm_mon + 1, t2.tm_mday);
}

void Game::InitEngine()
{
	Info("Initializing engine.");

	Window* window = engine->GetWindow();
	window->SetTitle("Down Adventure");

	engine->Init(this);

	scene = engine->GetScene();
	input = engine->GetInput();
	res_mgr = engine->GetResourceManager();
	sound_mgr = engine->GetSoundManager();

	phy_world.reset(new PhysicalWorld);
	phy_world->Init();

	scene->SetDebugDrawHandler(delegate<void(DebugDrawer*)>(this, &Game::OnDebugDraw));
	scene->SetDebugDrawEnabled(true);
}

void Game::InitGame()
{
	Info("Initializing game.");

	Srand();
	engine->GetWindow()->SetCursorLock(true);

	player = new SceneNode;
	player->mesh = res_mgr->GetMesh("units/human.qmsh");
	player->mesh_inst = new MeshInstance(player->mesh);
	player->mesh_inst->Play("stoi", 0, 0);
	player->mesh_inst->SetToEnd();
	player->pos = Vec3(0, 0, 0);
	player->rot = Vec3(0, 0, 0);
	player_rot_to = 0;
	player_ani = ANI_STAND;
	scene->Add(player);

	SceneNode* floor = new SceneNode;
	floor->mesh = res_mgr->GetMesh("floor.qmsh");
	floor->pos = Vec3::Zero;
	floor->rot = Vec3::Zero;
	scene->Add(floor);

	cam = scene->GetCamera();
	cam->from = Vec3(2, 2, 2);
	cam->to = Vec3(0, 0, 0);
	cam_rot = Vec2(0, 4.47908592f);

	SceneNode* box = new SceneNode;
	box->mesh = res_mgr->GetMesh("box.qmsh");
	box->pos = Vec3(3, 0, 0);
	box->rot = Vec3::Zero;
	scene->Add(box);

	phy_world->AddBoxCollider();
}

bool Game::OnTick(float dt)
{
	if((input->Down(Key::Alt) && input->Pressed(Key::F4))
		|| input->Pressed(Key::Escape))
		return false;
	
	Window* window = engine->GetWindow();
	if(input->Down(Key::Alt) && input->Pressed(Key::Enter))
	{
		bool fullscreen = !window->IsFullscreen();
		window->SetFullscreen(fullscreen);
		//config->SetBool("fullscreen", fullscreen);
		//config->Save();
	}

	if(input->Pressed(Key::U))
		window->SetCursorLock(!window->IsCursorLocked());

#ifdef _DEBUG
	if(input->Pressed(Key::N0))
		draw_phy = !draw_phy;
#endif

	UpdatePlayer(dt);
	UpdateCamera(dt);

	return true;
}

void Game::OnDebugDraw(DebugDrawer* debug_drawer)
{
	if(draw_phy)
		phy_world->Draw(debug_drawer);
}

void Game::UpdatePlayer(float dt)
{
	Animation ani = ANI_STAND;

	int mov = 0;
	if(input->Down(Key::W))
		mov += 10;
	if(input->Down(Key::S))
		mov -= 10;
	if(input->Down(Key::A))
		mov -= 1;
	if(input->Down(Key::D))
		mov += 1;

	// rotate player to look at same dir as camera
	const float rot_speed = 7.f * dt;
	float dif = AngleDiff(player->rot.y, player_rot_to);
	if(dif < rot_speed)
	{
		player->rot.y = player_rot_to;
		dif = 0;
	}
	else
	{
		float arc = ShortestArc(player->rot.y, player_rot_to);
		player->rot.y = Clip(player->rot.y + Sign(arc) * rot_speed);
		dif -= rot_speed;
		if(arc > 0)
			ani = ANI_ROTATE_RIGHT;
		else
			ani = ANI_ROTATE_LEFT;
	}

	if(mov != 0)
	{
		float dir;
		bool back = false;
		switch(mov)
		{
		case 1: // right
			dir = PI * 0 / 4;
			break;
		case 11: // forward right
			dir = PI * 1 / 4;
			break;
		case 10: // forward
			dir = PI * 2 / 4;
			break;
		case 9: // forward left
			dir = PI * 3 / 4;
			break;
		case -1: // left
			dir = PI * 4 / 4;
			break;
		case -11: // backward left
			dir = PI * 5 / 4;
			back = true;
			break;
		case -10: // backward
			dir = PI * 6 / 4;
			back = true;
			break;
		case -9: // backward right
			dir = PI * 7 / 4;
			back = true;
			break;
		}

		player_rot_to = cam_rot.x;
		
		if(dif < PI / 4)
		{
			dir += player->rot.y - PI / 2;
			bool run = !back && !input->Down(Key::Shift);

			const float speed = run ? 7.f : 2.5f;
			player->pos += Vec3(cos(dir), 0, sin(dir)) * (speed * dt);

			if(run)
				ani = ANI_RUN;
			else if(back)
				ani = ANI_WALK_BACK;
			else
				ani = ANI_WALK;
		}
	}

	if(ani != player_ani)
	{
		player_ani = ani;
		switch(ani)
		{
		case ANI_STAND:
			player->mesh_inst->Play("stoi", 0, 0);
			break;
		case ANI_ROTATE_LEFT:
			player->mesh_inst->Play("w_lewo", 0, 0);
			break;
		case ANI_ROTATE_RIGHT:
			player->mesh_inst->Play("w_prawo", 0, 0);
			break;
		case ANI_WALK:
			player->mesh_inst->Play("idzie", 0, 0);
			break;
		case ANI_WALK_BACK:
			player->mesh_inst->Play("idzie", PLAY_BACK, 0);
			break;
		case ANI_RUN:
			player->mesh_inst->Play("biegnie", 0, 0);
			break;
		}
	}

	player->mesh_inst->Update(dt);
}

void Game::UpdateCamera(float dt)
{
	const float h = 1.7f;
	const float dist = 1.5f;
	const Vec2 angle_limits = Vec2(3.24f, 5.75f);

	Int2 dif = input->GetMouseDif();
	cam_rot.x = Clip(cam_rot.x - float(dif.x) / 400);
	cam_rot.y = angle_limits.Clamp(cam_rot.y - float(dif.y) / 400);

	Vec3 to = player->pos;
	to.y += h;

	Vec3 ray(0, -dist, 0);
	Matrix mat = Matrix::Rotation(-cam_rot.x - PI / 2, cam_rot.y, 0);
	ray = Vec3::Transform(ray, mat);

	Vec3 from = to + ray;

	/*if(reset)
	{
		cam->from = from;
		cam->to = to;
		reset = false;
	}
	else
	{
		float d = 1.0f - exp(log(0.5f) * springiness * dt);
		cam->from += (from - cam->from) * d;
		cam->to += (to - cam->to) * d;
	}*/

	cam->from = from;
	cam->to = to;
}
