#pragma once

#include "Entity.hpp"
#include "PlayerEntity.hpp"
#include "Mob.hpp"

class BulletEntity :public Entity {
public:

	static void shoot(double damage = 5,
					  Vector2d position = localPlayer->getEyePosition(),
					  double degree = gameIO.degreeAngle);

public:

	const double getGravityAclc() override { return .0; }
	const string getEntityId() override { return "bullet"; }

	const bool requestEntityCollisionCallback() override { return true; }
	void _onCollideEntity(Entity* e) override;

	virtual void _onCollision(Block* block);

private:

	double damage;

};
