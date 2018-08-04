#pragma once

#include "Unit.h"

struct Npc : Unit
{
	Npc() : Unit(UNIT_NPC) {}
	void Update(float dt) override;
	void Save(FileWriter& f) override;
	void Load(FileReader& f) override;
};
