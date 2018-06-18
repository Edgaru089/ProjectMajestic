#pragma once

#include "Entity.hpp"


////////////////////////////////////////
void Entity::_moveX(double amount) {
	if (isSame(amount, 0))
		return;

	double move;
	bool isHit = false;

	// X-Positive
	if (amount > 0) {
		if (amount > 0.9999)
			amount = 0.9999;

		vector<Vector2d> points;
		DoubleRect bound = getHitbox();

		points.push_back(Vector2d(bound.left + bound.width, bound.top));                  // Right-Top    Point (X-Positive, Y-Negative)
		points.push_back(Vector2d(bound.left + bound.width, bound.top + bound.height));   // Right-Bottom Point (X-Positive, Y-Positive)

		// Add other mesh points to collision points
		for (double x = 0.5; x < bound.height; x += 0.5)
			points.push_back(Vector2d(bound.left + bound.width, bound.top + x));

		// Iterate through the list to find the most X-Negative-placed collision point
		move = 10000;
		bool flag = true;
		for (Vector2d i : points) {
			Block* bi = terrainManager.getBlock(TerrainManager::convertWorldPositionToBlockCoord(i));
			if (bi != nullptr && (bi->isSolid() && bi->getHitbox().contains(i))) { // Already stuck - continue
				flag = false;
				move = 0;
				isHit = true;
				continue;
			}

			Vector2d ix = i; // Moved point
			ix.x += amount;

			Vector2i blockCoord = TerrainManager::convertWorldPositionToBlockCoord(ix);
			Block* b = terrainManager.getBlock(blockCoord);

			if (b != nullptr && b->getHitbox().contains(ix)) {
				_onCollision(b);
				b->_onCollision(this);
				if (b->isSolid()) {
					ix.x = b->getHitbox().left - 1e-7; //HACK Minus EPS to avoid being treated as stuck in walls
					isHit = true;
					//mlog << "[Entity/Collision] [X-Positive] Hit: Entity {" << getUuid().toString() << "}" << dlog;
					//mlog << "       Block: (" << blockCoord.x << ", " << blockCoord.y << "), Pos: (" << i.x << ", " << i.y << ") --> (" << ix.x << ", " << ix.y << ")" << dlog;
				}
			}

			if (flag) {
				move = ix.x - i.x;
				flag = false;
			}
			else
				move = min(move, ix.x - i.x);
		}
	} // X-Negative
	else {
		if (amount < -0.9999)
			amount = -0.9999;

		vector<Vector2d> points;
		DoubleRect bound = getHitbox();

		points.push_back(Vector2d(bound.left, bound.top));                  // Left-Top    Point (X-Negative, Y-Negative)
		points.push_back(Vector2d(bound.left, bound.top + bound.height));   // Left-Bottom Point (X-Negative, Y-Positive)

		// Add other mesh points to collision points
		for (double x = 0.5; x < bound.height; x += 0.5)
			points.push_back(Vector2d(bound.left, bound.top + x));

		// Iterate through the list to find the most X-Positive-placed collision point
		move = -10000;
		bool flag = true;
		for (Vector2d i : points) {
			Block* bi = terrainManager.getBlock(TerrainManager::convertWorldPositionToBlockCoord(i));
			if (bi != nullptr && (bi->isSolid() && bi->getHitbox().contains(i))) { // Already stuck - continue
				flag = false;
				move = 0;
				isHit = true;
				continue;
			}

			Vector2d ix = i; // Moved point
			ix.x += amount;

			Vector2i blockCoord = TerrainManager::convertWorldPositionToBlockCoord(ix);
			Block* b = terrainManager.getBlock(blockCoord);
			if (b != nullptr && b->getHitbox().contains(ix)) {
				_onCollision(b);
				b->_onCollision(this);
				if (b->isSolid()) {
					ix.x = b->getHitbox().left + b->getHitbox().width + 1e-7;  //HACK Minus EPS to avoid being treated as stuck in walls
					isHit = true;
					//mlog << "[Entity / Collision] [X-Negative] Hit: Entity {" << getUuid().toString() << "}" << dlog;
					//mlog << "       Block: (" << blockCoord.x << ", " << blockCoord.y << "), Pos: (" << i.x << ", " << i.y << ") --> (" << ix.x << ", " << ix.y << ")" << dlog;
				}
			}

			if (flag) {
				move = ix.x - i.x;
				flag = false;
			}
			else
				move = max(move, ix.x - i.x);
		}
	}

	posX += move;
	if (isHit)
		vecX = 0.0;
}


////////////////////////////////////////
void Entity::_moveY(double amount) {
	if (isSame(amount, 0))
		return;

	double move;
	bool isHit = false;

	onGround = false;

	// Y-Positive
	if (amount > 0) {
		if (amount > 0.9999)
			amount = 0.9999;

		vector<Vector2d> points;
		DoubleRect bound = getHitbox();

		points.push_back(Vector2d(bound.left, bound.top + bound.height));                 // Bottom-Left  Point (X-Negative, Y-Negative)
		points.push_back(Vector2d(bound.left + bound.width, bound.top + bound.height));   // Bottom-Right Point (X-Positive, Y-Negative)

		// Add other mesh points to collision points
		for (double x = 0.5; x < bound.width; x += 0.5)
			points.push_back(Vector2d(bound.left + x, bound.top + bound.height));

		// Iterate through the list to find the most Y-Positive-placed collision point
		move = 10000;
		bool flag = true;
		for (Vector2d i : points) {
			Block* bi = terrainManager.getBlock(TerrainManager::convertWorldPositionToBlockCoord(i));
			if (bi != nullptr && (bi->isSolid() && bi->getHitbox().contains(i))) { // Already stuck - continue
				flag = false;
				move = 0;
				isHit = true;
				continue;
			}

			Vector2d ix = i; // Moved point
			ix.y += amount;

			Vector2i blockCoord = TerrainManager::convertWorldPositionToBlockCoord(ix);
			Block* b = terrainManager.getBlock(blockCoord);
			if (b != nullptr && b->getHitbox().contains(ix)) {
				_onCollision(b);
				b->_onCollision(this);
				if (b->isSolid()) {
					ix.y = b->getHitbox().top - 1e-7;  //HACK Minus EPS to avoid being treated as stuck in walls
					isHit = true;
					//mlog << "[Entity / Collision] [Y-Positive] Hit: Entity {" << getUuid().toString() << "}" << dlog;
					//mlog << "       Block: (" << blockCoord.x << ", " << blockCoord.y << "), Pos: (" << i.x << ", " << i.y << ") --> (" << ix.x << ", " << ix.y << ")" << dlog;

					// Y-Positive(Down) Specific: Check if the entity is on the ground
					if (isSame(ix.y, b->getHitbox().top))
						onGround = true;
				}
			}

			if (flag) {
				move = ix.y - i.y;
				flag = false;
			}
			else
				move = min(move, ix.y - i.y);
		}
	} // Y-Negative
	else {
		if (amount < -0.9999)
			amount = -0.9999;

		vector<Vector2d> points;
		DoubleRect bound = getHitbox();

		points.push_back(Vector2d(bound.left, bound.top));                 // Top-Left  Point (X-Negative, Y-Negative)
		points.push_back(Vector2d(bound.left + bound.width, bound.top));   // Top-Right Point (X-Positive, Y-Negative)

		// Add other mesh points to collision points
		for (double x = 0.5; x < bound.width; x += 0.5)
			points.push_back(Vector2d(bound.left + x, bound.top));

		// Iterate through the list to find the most Y-Positive-placed collision point
		move = -10000;
		bool flag = true;
		for (Vector2d i : points) {
			Block* bi = terrainManager.getBlock(TerrainManager::convertWorldPositionToBlockCoord(i));
			if (bi != nullptr && (bi->isSolid() && bi->getHitbox().contains(i))) { // Already stuck - continue
				flag = false;
				move = 0;
				isHit = true;
				continue;
			}

			Vector2d ix = i; // Moved point
			ix.y += amount;

			Vector2i blockCoord = TerrainManager::convertWorldPositionToBlockCoord(ix);
			Block* b = terrainManager.getBlock(blockCoord);
			if (b != nullptr && b->getHitbox().contains(ix)) {
				_onCollision(b);
				b->_onCollision(this);
				if (b->isSolid()) {
					ix.y = b->getHitbox().top + b->getHitbox().height + 1e-7;  //HACK Minus EPS to avoid being treated as stuck in walls
					isHit = true;
					//mlog << "[Entity / Collision] [Y-Negative] Hit: Entity {" << getUuid().toString() << "}" << dlog;
					//mlog << "       Block: (" << blockCoord.x << ", " << blockCoord.y << "), Pos: (" << i.x << ", " << i.y << ") --> (" << ix.x << ", " << ix.y << ")" << dlog;
				}
			}

			if (flag) {
				move = ix.y - i.y;
				flag = false;
			}
			else
				move = max(move, ix.y - i.y);
		}
	}

	posY += move;
	if (isHit)
		vecY = 0.0;
}


////////////////////////////////////////
void Entity::updateLogic() {
	if (!alive)
		return;

	if (onGround) {
		double minusVecX = getFrictionDeaclc()*logicIO.deltaTime.asSeconds();
		if (vecX > 0) {
			vecX -= minusVecX;
			if (vecX < 0)
				vecX = 0;
		}
		else if (vecX < 0) {
			vecX += minusVecX;
			if (vecX > 0)
				vecX = 0;
		}
	}

	vecY += getGravityAclc()*logicIO.deltaTime.asSeconds();

	// Collision detection with terrain on move
	_moveX(vecX * logicIO.deltaTime.asSeconds());
	_moveY(vecY * logicIO.deltaTime.asSeconds());

	if (!isSame(vecY, 0)) {
		angle = atan(vecY / vecX) / PI * 180.0;
		if (vecX < 0)
			angle += 180;
	}

	if (role == Server || getEntityId() == "player")
		_updateLogic();
}
