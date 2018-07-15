#pragma once

#include "Entity.hpp"

const double grenadeDamage = 12.0;

class GrenadeEntity :public Entity {
public:

	const string getEntityId() override { return "grenade"; }
	const double getGravityAclc() override { return 3.5; }
	TextureInfo getTextureInfo() override { return textureManager.getTextureInfo("item_grenade"); }

	const bool requestEntityCollisionCallback() override { return true; }
	void _onCollideEntity(Entity* e) override;

	Vector2d getSize() override { return Vector2d(0.6, 0.6); }

	void _onCollision(Block* b) override;

private:

};
