#include "GameCore.h"
#include "Game.h"
#include <Engine.h>
#include <Input.h>
#include <ctime>
#include <Window.h>

Game::Game()
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
}

void Game::InitGame()
{
	Info("Initializing game.");

	Srand();
	engine->GetWindow()->SetCursorLock(true);

	LoadResources();
}

void Game::LoadResources()
{
}

bool Game::OnTick(float dt)
{
	if(input->Down(Key::Alt) && input->Pressed(Key::F4))
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

	return true;
}
