#pragma once

#include "ArrowEntity.hpp"
#include "IO.hpp"
#include "TerrainManager.hpp"
#include "PlayerEntity.hpp"
#include "EntityManager.hpp"
#include "ItemEntity.hpp"
#include "ParticleSystem.hpp"


////////////////////////////////////////
void ArrowEntity::shoot(int damage) {
	if (localPlayer == nullptr)
		return;

	Vector2d pos = localPlayer->getEyePosition();

	Entity* e = new ArrowEntity;
	e->accelerateVector(20.0*damage / maxArrowDamage, gameIO.degreeAngle + 5.0 * (rand01() - 0.5));
	entityManager.insert(e, pos);
}


////////////////////////////////////////
void ArrowEntity::pushTriangleVertexes(VertexArray& verts) {
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
void ArrowEntity::_updateLogic() {
	if (inWall()) {
		vecX = vecY = 0;
		posX = pos0.x;
		posY = pos0.y;
		angle = angle0;

		Block* b = terrainManager.getBlock(inWallBlock);
		if (b == nullptr || !b->isSolid()) {
			// Tile Drop
			// Borrow code from Block::_onDestory()
			ItemEntity* e = new ItemEntity("item_arrow");
			// Give a random velocity
			e->accelerateVector(1.0, 180 + rand() % 180);

			entityManager.insert(e, getPosition());
			kill();
		}
	}
	else {
		// TODO Particles when arrow is flying
		if (particleClock.getElapsedTime() >= milliseconds(25)) {
			particleClock.restart();
			particleSystem.emitArrowGlow(getPosition());
		}
	}
}


////////////////////////////////////////
void ArrowEntity::_onCollision(Block* block) {
	if (!block->isSolid())
		return;
	inWall() = true;
	pos0 = Vector2d(posX, posY);
	angle0 = angle;
	inWallBlock = TerrainManager::convertChunkToWorldCoord(block->getChunkId(), block->getInChunkPosition());
}

