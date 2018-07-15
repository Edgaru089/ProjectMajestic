#pragma once

#include "PlayerEntity.hpp"
#include "TerrainManager.hpp"


////////////////////////////////////////
void PlayerEntity::_updateLogic() {
	if (onLadder()) {
		onGround = true;
		vecY = 0;
		if (isAscendingLadder())
			vecY += -getMaxSpeed()*0.8;
		if (isDecendingLadder())
			vecY += getMaxSpeed()*0.8;
		Block* b = terrainManager.getBlock(Vector2i(posX, posY));
		if (b == nullptr || b->getBlockId() != "ladder") {
			onLadder() = false;
			isAscendingLadder() = false;
			isDecendingLadder() = false;
		}
	}

	if (isMovingLeft())
		if (onLadder())
			accelerate(Vector2d(-65 * logicIO.deltaTime.asSeconds(), 0));
		else if (onGround)
			accelerate(Vector2d(-75 * logicIO.deltaTime.asSeconds(), 0));
		else
			accelerate(Vector2d(-4.0*logicIO.deltaTime.asSeconds(), 0));
	if (isMovingRight())
		if (onLadder())
			accelerate(Vector2d(65 * logicIO.deltaTime.asSeconds(), 0));
		else if (onGround)
			accelerate(Vector2d(75 * logicIO.deltaTime.asSeconds(), 0));
		else
			accelerate(Vector2d(4.0*logicIO.deltaTime.asSeconds(), 0));

	if (vecX > getMaxSpeed())
		vecX = getMaxSpeed();
	if (vecX < -getMaxSpeed())
		vecX = -getMaxSpeed();

	if (wantStandup()) {
		bool ok = true;

		// Borrow collision code from Entity
		vector<Vector2d> points;
		crouched() = false;
		DoubleRect bound = getHitbox();
		crouched() = true;

		points.push_back(Vector2d(bound.left, bound.top));                 // Top-Left  Point (X-Negative, Y-Negative)
		points.push_back(Vector2d(bound.left + bound.width, bound.top));   // Top-Right Point (X-Positive, Y-Negative)

		for (Vector2d i : points) {
			Block* top = terrainManager.getBlock(TerrainManager::convertWorldPositionToBlockCoord(i));
			if (top != nullptr && top->isSolid()) {
				ok = false;
				break;
			}
		}

		if (ok) {
			wantStandup() = false;
			crouched() = false;
		}
	}

	// Collect Item
	for (auto& i : entityManager.getEntityMapList()) {
		if (i.second->getEntityId() == "item_entity") {
			if (getHitbox().intersects(i.second->getHitbox())) {
				collectItem(dynamic_cast<ItemEntity*>(i.second));
			}
		}
	}
}


////////////////////////////////////////
void PlayerEntity::jump() {
	if (isOnLadder())
		jumpOffLadder();
	else if (onGround)
		accelerate(Vector2d(0, -6));
}


////////////////////////////////////////
void PlayerEntity::moveLeft(bool state) {
	isMovingLeft() = state;
}


////////////////////////////////////////
void PlayerEntity::moveRight(bool state) {
	isMovingRight() = state;
}


////////////////////////////////////////
bool PlayerEntity::crouch(bool state) {
	crouched() = state;
	wantStandup() = false;
	if (!state) {
		bool ok = true;

		// Borrow collision code from Entity
		vector<Vector2d> points;
		crouched() = false;
		DoubleRect bound = getHitbox();
		crouched() = state;

		points.push_back(Vector2d(bound.left, bound.top));                 // Top-Left  Point (X-Negative, Y-Negative)
		points.push_back(Vector2d(bound.left + bound.width, bound.top));   // Top-Right Point (X-Positive, Y-Negative)

		for (Vector2d i : points) {
			Block* top = terrainManager.getBlock(TerrainManager::convertWorldPositionToBlockCoord(i));
			if (top != nullptr && top->isSolid()) {
				ok = false;
				break;
			}
		}

		if (ok) {
			wantStandup() = false;
			crouched() = false;
		}
		else {
			wantStandup() = true;
			crouched() = true;
		}
	}
	return crouched();
}


////////////////////////////////////////
void PlayerEntity::ascendLadder(bool state) {
	Block* b = terrainManager.getBlock(Vector2i(posX, posY));
	if (b != nullptr && b->getBlockId() == "ladder") {
		onLadder() = true;
		isAscendingLadder() = state;
	}
}


////////////////////////////////////////
void PlayerEntity::decendLadder(bool state) {
	Block* b = terrainManager.getBlock(Vector2i(posX, posY));
	if (b != nullptr && b->getBlockId() == "ladder") {
		onLadder() = true;
		isDecendingLadder() = state;
	}
}


////////////////////////////////////////
void PlayerEntity::jumpOffLadder() {
	vecY = 0;
	onLadder() = false;
}


////////////////////////////////////////
bool PlayerEntity::collectItem(ItemEntity* item) {
	//TODO Collect Item

	// First attempt to merge with items which exist
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 9; j++) {
			Dataset& d = playerInventory.slots[i][j];
			if (d["count"].getDataInt() < maxItemsPerSlot && d["item_name"].getDataString() == item->getItemName()) {
				d["count"].getDataInt()++;
				item->kill();
				return true;
			}
		}
	// Then try to place at an empty slot
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 9; j++) {
			Dataset& d = playerInventory.slots[i][j];
			if (d["item_name"].getDataString() == "") {
				d["item_name"].setData(item->getItemName());
				d["count"].getDataInt() = 1;
				for (auto& i : item->getDataset().getDatasets())
					d.getDatasets().insert(i);
				item->kill();
				return true;
			}
		}

	return false;
}

