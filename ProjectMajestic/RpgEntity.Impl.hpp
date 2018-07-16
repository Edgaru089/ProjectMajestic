#pragma once

#include "RpgEntity.hpp"
#include "IO.hpp"
#include "EntityManager.hpp"
#include "ParticleSystem.hpp"
#include "Mob.hpp"


////////////////////////////////////////
void RpgEntity::shoot(double force, Vector2d position, double degree) {
	RpgEntity* e = new RpgEntity;
	e->force = force;
	e->accelerateVector(15.0, degree);
	entityManager.insert(e, position);
}


////////////////////////////////////////
void RpgEntity::_onCollision(Block* block) {
	if (!isAlive())
		return;
	if (!block->isSolid())
		return;
	entityManager.explode(getCenterPos(), force);
	kill();
}


////////////////////////////////////////
void RpgEntity::_onCollideEntity(Entity* e) {
	if (!isAlive())
		return;
	Mob* mob = dynamic_cast<Mob*>(e);
	if (mob == nullptr)
		return;

	entityManager.explode(getCenterPos(), force);
	kill();
}


////////////////////////////////////////
void RpgEntity::_updateLogic() {
	// Particles when RPG is flying
	if (particleClock.getElapsedTime() >= milliseconds(15)) {
		particleClock.restart();
		particleSystem.emitSmoke(getCenterPos(), 5.0, -0.1, 0.2, getRotation() + 180.0 - 8.0, getRotation() + 180.0 + 8.0, 1);
	}
}


