#pragma once

#include "Unit.h"

struct Enemy : Unit
{
	Enemy() : Unit(UNIT_ENEMY) {}
	void Update(float dt) override;
	void Save(FileWriter& f) override;
	void Load(FileReader& f) override;
};
