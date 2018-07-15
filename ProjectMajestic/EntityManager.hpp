#pragma once

#include "Entity.hpp"
#include "IO.hpp"
#include "Uuid.hpp"


class EntityManager : public Lockable {
public:

	friend class Entity;

	void initalaize();

	void updateLogic();

	// Triangles
	void getRenderList(VertexArray& verts);

public:

	Uuid insert(Entity* entitiy, Vector2d position);

	// Does not call Entity::onCreate()
	void insert(Uuid id, Entity * entity);

	Entity* getEntity(Uuid id);

	map<Uuid, Entity*>& getEntityMapList() { return entities; }

public:

	// Summon an explosion
	void explode(Vector2d position, double force, bool damageTerrain = gameIO.ruleExplosionDamagesTerrain);

private:

	map<Uuid, Entity*> entities;

};


EntityManager entityManager;
