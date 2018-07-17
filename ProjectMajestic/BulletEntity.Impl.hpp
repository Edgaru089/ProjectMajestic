#pragma once

#include "BulletEntity.hpp"
#include "EntityManager.hpp"
#include "Mob.hpp"


////////////////////////////////////////
void BulletEntity::shoot(double damage, double speed, Vector2d position, double degree) {
	BulletEntity* b = new BulletEntity;
	b->damage = damage;
	b->accelerateVector(speed, degree);
	entityManager.insert(b, position + Vector2d(.0, b->getSize().y / 2.0));
}


////////////////////////////////////////
void BulletEntity::_onCollideEntity(Entity* e) {
	if (!isAlive())
		return;
	if (e->getUuid() != localPlayer->getUuid()) {
		Mob* mob = dynamic_cast<Mob*>(e);
		if (mob != nullptr) {
			mob->harm(damage, getPosition());
			kill();
		}
	}
}


////////////////////////////////////////
void BulletEntity::_onCollision(Block* b) {
	if (!isAlive())
		return;
	if (b->isSolid())
		kill();
}

