#pragma once

#include "Main.hpp"
#include "Entity.hpp"
#include "Mob.hpp"


class TestEntity :public Mob {
public:

	const string getEntityId() override { return "test_entity"; }
	TextureInfo getTextureInfo() override { return textureManager.getTextureInfo("block_log_top"); }

	const int getMaxHealth() override { return 50; }

	Vector2d getSize() override { return Vector2d(2.5, 2.5); }

private:

	Vector2d size;

};

