#pragma once

class Level
{
public:
	void Update(float dt);
	void Save(FileWriter& f);
	void Load(FileReader& f);

	vector<Unit*> units;
	vector<Chest*> chests;
};
