
#include "EntityManager.hpp"
#include "ParticleSystem.hpp"
#include "Mob.hpp"
#include "NetworkClient.hpp"
#include "NetworkServer.hpp"
#include <typeinfo>


////////////////////////////////////////
void EntityManager::initalaize() {

}


////////////////////////////////////////
void EntityManager::updateLogic() {
	AUTOLOCKABLE(*this);
	for (auto i = entities.begin(); i != entities.end();) {
		if (i->second == nullptr || !i->second->isAlive()) {
			if (i->second != nullptr) {
				networkServer.notifyEntityKill(i->second->getUuid());
				delete i->second;
			}
			i = entities.erase(i);
		}
		else {
			i->second->updateLogic();
			i++;
		}
	}
	for (auto i : entities) {
		if (i.second->requestEntityCollisionCallback() && i.second->isAlive())
			for (auto j : entities)
				if (j.first != i.first&&j.second->getHitbox().intersects(i.second->getHitbox()))
					i.second->_onCollideEntity(j.second);
	}

	if (role == Client)
		networkClient.syncPlayerPos();
}


////////////////////////////////////////
void EntityManager::explode(Vector2d position, double force, bool damageTerrain) {
	// Damage
	for (auto& i : entities) {
		Mob* m = dynamic_cast<Mob*>(i.second);
		if (m != nullptr) {
			// Use the center point for distance calculations
			double dist = getDis(m->getCenterPos(), position);
			double damage = force - dist * dist * 1.1;
			if (damage > 0)
				m->harm(damage, position, damage / 5.0);
		}
	}

	// TODO Terrain destruction
	if (damageTerrain) {
		for (auto& k : terrainManager.getChunks()) {
			Vector2i off = k.first*chunkSize;
			Chunk* c = k.second;
			for (int i = 0; i < chunkSize; i++)
				for (int j = 0; j < chunkSize; j++) {
					if (c->getBlock(Vector2i(i, j)) == nullptr)
						continue;
					Vector2d blockCenter = Vector2d(off + Vector2i(i, j)) + Vector2d(.5, .5);
					if (sqr(getDis(blockCenter, position)*1.6) < force)
						terrainManager.breakBlock(off + Vector2i(i, j));
				}
		}
	}

	// Particles
	particleSystem.emitSmoke(position, 1.2 * force, -4.0, 0.002, 0.0, 360.0, 7 * force);
	particleSystem.emitSmoke(position, 1.7 * force, -4.0, 0.002, 0.0, 360.0, 0.5 * force);
}


////////////////////////////////////////
void EntityManager::getRenderList(VertexArray& verts) {
	AUTOLOCKABLE(*this);
	for (auto i : entities) {
		if (i.second == nullptr || !i.second->isAlive() || !i.second->getHitbox().intersects(renderIO.viewRectBlock))
			continue;

		if (i.second->requestSpeicalRendering()) {
			i.second->pushTriangleVertexes(verts);
			continue;
		}

		Entity* e = i.second;
		TextureInfo tex = e->getTextureInfo();
		Vector2d center = e->getPosition()*renderIO.gameScaleFactor;
		double width = e->getSize().x*renderIO.gameScaleFactor, height = e->getSize().y*renderIO.gameScaleFactor;

		Vector2i chunk = TerrainManager::convertWorldCoordToChunkId(Vector2i(e->getPosition().x, e->getPosition().y));
		Vector2i inCc = TerrainManager::convertWorldCoordToInChunkCoord(Vector2i(e->getPosition().x, e->getPosition().y));
		Chunk* c = terrainManager.getChunk(chunk);

		Uint8 mask;
		if (c != nullptr)
			mask = 63 + 192.0*((double)c->lightLevel[inCc.x][inCc.y] / maxLightingLevel);
		else
			mask = 63;

		// Left-Top
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - height),
							Color(mask, mask, mask),
							Vector2f(tex.textureRect.left, tex.textureRect.top)
		));
		// Right-Top
		verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - height),
							Color(mask, mask, mask),
							Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top)
		));
		// Left-Bottom
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y),
							Color(mask, mask, mask),
							Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)
		));
		// Right-Top
		verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - height),
							Color(mask, mask, mask),
							Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top)
		));
		// Left-Bottom
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y),
							Color(mask, mask, mask),
							Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)
		));
		// Right-Bottom
		verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y),
							Color(mask, mask, mask),
							Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top + tex.textureRect.height)
		));
	}
}


////////////////////////////////////////
Uuid EntityManager::insert(Entity * entity, Vector2d position) {
	Uuid id = Uuid::get();
	entity->setUuid(id);
	entity->setPosition(position);
	lock();
	entities.insert_or_assign(id, entity);
	unlock();
	entity->onCreate();
	networkServer.notifyEntityInsert(id, entity);
	return id;
}


////////////////////////////////////////
void EntityManager::insert(Uuid id, Entity * entity) {
	entity->setUuid(id);
	lock();
	entities.insert_or_assign(id, entity);
	unlock();
	entity->onCreate();
}


////////////////////////////////////////
Entity * EntityManager::getEntity(Uuid id) {
	AUTOLOCKABLE(*this);
	auto i = entities.find(id);
	if (i == entities.end() || i->second == nullptr)
		return nullptr;
	else
		return i->second;
}

