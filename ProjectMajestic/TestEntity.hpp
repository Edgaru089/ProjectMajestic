#pragma once

#include "Main.hpp"
#include "Entity.hpp"


class TestEntity :public Entity {
public:

	const string getEntityId() override { return "test_entity"; }
	TextureInfo getTextureInfo() override { return textureManager.getTextureInfo("block_log_top"); }

	Vector2d getSize() override { return size; }
	void setSize(Vector2d size) { this->size = size; }

private:

	Vector2d size;

};

