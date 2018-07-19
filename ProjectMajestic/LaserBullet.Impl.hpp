#pragma once

#include "LaserBullet.hpp"
#include "ParticleSystem.hpp"


////////////////////////////////////////
void LaserBullet::shoot(double damage, double speed, double knockbackFactor, Vector2d position, double degree) {
	LaserBullet* b = new LaserBullet;
	b->damage = damage;
	b->knockback = knockbackFactor;
	b->accelerateVector(speed, degree);
	entityManager.insert(b, position + Vector2d(.0, b->getSize().y / 2.0));
}


////////////////////////////////////////
void LaserBullet::_updateLogic() {
	//if (particleClock.getElapsedTime() > milliseconds(30)) {
	//	particleClock.restart();
		particleSystem.emit(getCenterPos(), textureManager.getTextureInfo("particle_laser_glow"), 1.0, .0, 1, seconds(.2f), seconds(.2f), Vector2d(.3, .3), .0);
	//}
}


////////////////////////////////////////
void LaserBullet::_onCollideEntity(Entity* e) {
	// Borrow modified code from BulletEntity::_onCollideEntity()
	if (!isAlive())
		return;
	if (e->getUuid() != localPlayer->getUuid() && hitMobs.find(e->getUuid()) == hitMobs.end()) {
		Mob* mob = dynamic_cast<Mob*>(e);
		if (mob != nullptr) {
			mob->harm(damage, getPosition(), knockback);
			hitMobs.insert(mob->getUuid());
		}
	}
}

