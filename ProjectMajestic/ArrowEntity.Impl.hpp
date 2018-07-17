#pragma once

#include "ArrowEntity.hpp"
#include "IO.hpp"
#include "TerrainManager.hpp"
#include "PlayerEntity.hpp"
#include "EntityManager.hpp"
#include "ItemEntity.hpp"
#include "ParticleSystem.hpp"
#include "Mob.hpp"


////////////////////////////////////////
void ArrowEntity::shoot(double damage, Vector2d position, double degree) {
	ArrowEntity* e = new ArrowEntity;
	e->damage = damage;
	e->accelerateVector(20.0 * damage / maxArrowDamage, degree + 5.0 * (rand01() - 0.5));
	entityManager.insert(e, position + Vector2d(.0, e->getSize().y / 2.0));
}


////////////////////////////////////////
void ArrowEntity::_pushTriangleVertexes(VertexArray& verts) {
	Transform trans;
	TextureInfo info = textureManager.getTextureInfo("entity_arrow");
	Vector2f center = Vector2f(getPosition() - Vector2d(0, getSize().y / 2));
	Vector2f size = Vector2f(Vector2d(16, 5)*renderIO.gameScaleFactor*0.2*0.25);

	Vector2i chunk = TerrainManager::convertWorldCoordToChunkId(Vector2i(getPosition().x, getPosition().y));
	Vector2i inCc = TerrainManager::convertWorldCoordToInChunkCoord(Vector2i(getPosition().x, getPosition().y));
	Chunk* c = terrainManager.getChunk(chunk);

	Uint8 mask;
	if (c != nullptr)
		mask = 63 + 192.0*((double)c->lightLevel[inCc.x][inCc.y] / maxLightingLevel);
	else
		mask = 63;

	trans.rotate(getRotation());

	// Right-Top
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor + trans.transformPoint(Vector2f(size.x, -size.y) / 2.0f)),
						Color(mask, mask, mask), Vector2f(16, 0) + Vector2f(info.textureRect.left, info.textureRect.top)));
					// Left-Top
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor - trans.transformPoint(size / 2.0f)),
						Color(mask, mask, mask), Vector2f(0, 0) + Vector2f(info.textureRect.left, info.textureRect.top)));
					// Right-Bottom
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor + trans.transformPoint(size / 2.0f)),
						Color(mask, mask, mask), Vector2f(16, 5) + Vector2f(info.textureRect.left, info.textureRect.top)));
					// Left-Top
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor - trans.transformPoint(size / 2.0f)),
						Color(mask, mask, mask), Vector2f(0, 0) + Vector2f(info.textureRect.left, info.textureRect.top)));
					// Right-Bottom
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor + trans.transformPoint(size / 2.0f)),
						Color(mask, mask, mask), Vector2f(16, 5) + Vector2f(info.textureRect.left, info.textureRect.top)));
					// Left-Bottom
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor + trans.transformPoint(Vector2f(-size.x, size.y) / 2.0f)),
						Color(mask, mask, mask), Vector2f(0, 5) + Vector2f(info.textureRect.left, info.textureRect.top)));

}


////////////////////////////////////////
void ArrowEntity::_onCollision(Block* block) {
	if (inEntity() || inWall())
		return;
	if (!block->isSolid())
		return;
	inWall() = true;
	pos0 = Vector2d(posX, posY);
	angle0 = angle;
	inWallBlock = TerrainManager::convertChunkToWorldCoord(block->getChunkId(), block->getInChunkPosition());
}


////////////////////////////////////////
void ArrowEntity::_onCollideEntity(Entity* e) {
	if (inEntity() || inWall())
		return;
	Mob* mob = dynamic_cast<Mob*>(e);
	if (mob == nullptr)
		return;
	mob->harm(damage, getPosition());

	inEntity() = true;
	pos0 = Vector2d(posX, posY) - e->getPosition();
	angle0 = angle;
	inEntityId = e->getUuid();
}


////////////////////////////////////////
void ArrowEntity::_dropItem() {
	// Tile Drop
	// Borrow code from Block::_onDestory()
	ItemEntity* e = new ItemEntity("item_arrow");
	// Give a random velocity
	e->accelerateVector(1.0, 180 + rand() % 180);

	entityManager.insert(e, getPosition());
	kill();
}


////////////////////////////////////////
void ArrowEntity::_updateLogic() {
	if (inWall()) {
		vecX = vecY = 0;
		posX = pos0.x;
		posY = pos0.y;
		angle = angle0;

		Block* b = terrainManager.getBlock(inWallBlock);
		if (b == nullptr || !b->isSolid())
			_dropItem();
	}
	else if (inEntity()) {
		Entity* e = entityManager.getEntity(inEntityId);
		if (e == nullptr)
			_dropItem();
		else {
			vecX = vecY = 0;
			posX = pos0.x + e->getPosition().x;
			posY = pos0.y + e->getPosition().y;
			angle = angle0;
		}
	}
	else {
		// Particles when arrow is flying
		if (particleClock.getElapsedTime() >= milliseconds(25)) {
			particleClock.restart();
			particleSystem.emitArrowGlow(getCenterPos());
		}
	}
}

