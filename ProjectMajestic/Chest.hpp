#pragma once

#include "Main.hpp"
#include "Block.hpp"


class Chest :public Block {
public:

	Chest();

	const string getBlockId() override { return "chest"; }
	DoubleRect getHitbox() override;

	const bool requestSpeicalRendering() override { return true; }
	void pushTriangleVertexes(VertexArray& verts) override;

	void _onRightClick() override;
	void _onDestroy(Entity* destroyer) override;


	Dataset chestSlots[4][9];

};
