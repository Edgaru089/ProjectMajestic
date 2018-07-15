#pragma once

#include "GrenadeEntity.hpp"
#include "EntityManager.hpp"
#include "PlayerEntity.hpp"


////////////////////////////////////////
void GrenadeEntity::_onCollideEntity(Entity* e) {
	if (dynamic_cast<Mob*>(e) != nullptr)
		if (e->getUuid() != localPlayer->getUuid()) {
			entityManager.explode(getPosition(), grenadeDamage, false);
			kill();
		}
}


////////////////////////////////////////
void GrenadeEntity::_onCollision(Block* b) {
	if (b->isSolid()) {
		entityManager.explode(getPosition(), grenadeDamage, false);
		kill();
	}
}

