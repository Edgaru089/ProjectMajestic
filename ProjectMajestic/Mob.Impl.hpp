#pragma once

#include "Mob.hpp"
#include "ParticleSystem.hpp"


////////////////////////////////////////
void Mob::kill() {
	particleSystem.emitSmoke(getHitbox(), 1.2, -.6, .2, 180, 360, 15);
	Vector2d t = getSize();
	double s = (t.x + t.y) / 2.0;
	particleSystem.emit(getHitbox(), getTextureInfo(), 5, 1.5f, 40, seconds(2), seconds(3), Vector2d(s, s)*.2);
	alive = false;
	_onKill();
}

