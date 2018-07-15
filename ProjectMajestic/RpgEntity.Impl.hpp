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
void RpgEntity::pushTriangleVertexes(VertexArray& verts) {
	Transform trans;
	TextureInfo info = textureManager.getTextureInfo("entity_rpg");
	Vector2f center = Vector2f(getPosition() - Vector2d(0, getSize().y / 2));
	Vector2f size = Vector2f(Vector2d(7, 7)*renderIO.gameScaleFactor / 7.0 / 4.0);

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
						Color(mask, mask, mask), Vector2f(7, 0) + Vector2f(info.textureRect.left, info.textureRect.top)));
	// Left-Top
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor - trans.transformPoint(size / 2.0f)),
						Color(mask, mask, mask), Vector2f(0, 0) + Vector2f(info.textureRect.left, info.textureRect.top)));
	// Right-Bottom
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor + trans.transformPoint(size / 2.0f)),
						Color(mask, mask, mask), Vector2f(7, 7) + Vector2f(info.textureRect.left, info.textureRect.top)));
	// Left-Top
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor - trans.transformPoint(size / 2.0f)),
						Color(mask, mask, mask), Vector2f(0, 0) + Vector2f(info.textureRect.left, info.textureRect.top)));
	// Right-Bottom
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor + trans.transformPoint(size / 2.0f)),
						Color(mask, mask, mask), Vector2f(7, 5) + Vector2f(info.textureRect.left, info.textureRect.top)));
	// Left-Bottom
	verts.append(Vertex((center * (float)renderIO.gameScaleFactor + trans.transformPoint(Vector2f(-size.x, size.y) / 2.0f)),
						Color(mask, mask, mask), Vector2f(0, 7) + Vector2f(info.textureRect.left, info.textureRect.top)));

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
	// TODO Particles when RPG is flying
	if (particleClock.getElapsedTime() >= milliseconds(15)) {
		particleClock.restart();
		particleSystem.emitSmoke(getCenterPos(), 3.5, -0.1, 0.2, getRotation() + 180.0 - 8.0, getRotation() + 180.0 + 8.0, 1);
	}
}


