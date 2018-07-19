#pragma once

#include "GrenadeItem.hpp"
#include "EntityManager.hpp"
#include "GrenadeEntity.hpp"
#include "PlayerEntity.hpp"


////////////////////////////////////////
void GrenadeItem::_onRightReleased() {
	slotDataset["count"].getDataInt()--;
	if (slotDataset["count"].getDataInt() <= 0)
		slotDataset["item_name"].getDataString() = ""s;
	
	GrenadeEntity* e = new GrenadeEntity;
	e->accelerateVector(8.0, gameIO.degreeAngle);
	entityManager.insert(e, localPlayer->getEyePosition() + Vector2d(.0, e->getSize().y / 2.0));
}

