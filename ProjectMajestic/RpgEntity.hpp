#pragma once

#include "Entity.hpp"
#include "PlayerEntity.hpp"

const double rpgExplodeForce = 25.0;

class RpgEntity :public Entity {
public:

	static void shoot(double force = rpgExplodeForce,
					  Vector2d position = localPlayer->getEyePosition(),
					  double degree = gameIO.degreeAngle);

public:

	RpgEntity() {}

	const string getEntityId() override { return "rpg"; }
	const bool requestSpeicalRendering() override { return true; }
	void pushTriangleVertexes(VertexArray& verts) override;

	const bool requestEntityCollisionCallback() override { return true; }
	void _onCollideEntity(Entity* e) override;

	const double getGravityAclc() override { return 1.5; }

	Vector2d getSize() override { return Vector2d(0.2, 0.2); }

	void _updateLogic() override;
	void _onCollision(Block* block) override;

private:

	Clock particleClock;

	double force;

};

