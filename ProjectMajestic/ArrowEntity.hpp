#pragma once

#include "Entity.hpp"

const int maxArrowDamage = 10;

class ArrowEntity :public Entity {
public:

	static void shoot(int damage = maxArrowDamage);

public:

	ArrowEntity() { inWall() = false; }

	const string getEntityId() override { return "arrow"; }
	const bool requestSpeicalRendering() override { return true; }
	void pushTriangleVertexes(VertexArray& verts) override;

	const double getGravityAclc() override { return 2.0; }

	Vector2d getSize() override { return Vector2d(0.2, 0.2); }

	void _updateLogic() override;
	void _onCollision(Block* block) override;

private:

	Clock particleClock;

	bool& inWall() { return getData("in_wall").getDataBool(); };

	Vector2d pos0;
	Vector2i inWallBlock;
	double angle0;

};

