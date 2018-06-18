#pragma once

#include "Main.hpp"
#include "Scene.hpp"

#include "TextureManager.hpp"
#include "Chunk.hpp"
#include "AssetManager.hpp"
#include "TestEntity.hpp"
#include "PlayerEntity.hpp"
#include "TerrainProvider.hpp"

class TestScene :public Scene {
public:

	const string getSceneName() override {
		return "TestScene";
	}

	void preWindowInitalaize() override;

	void start(RenderWindow& win) override;

	void onRender(RenderWindow& win) override;

	void updateLogic(RenderWindow& win) override;

	void handleEvent(RenderWindow& win, Event& event) override;

	void runImGui() override;

	void stop() override;


private:

	Uuid mouseLightId;
	Vector2i mouseWorldCoord;

	Texture background;

	vector<string> blocks;
	string curBlock;

	TerrainProvider prov;

	int terrainListSize, terrainMaskSize, particleListSize, entityListSize, totalVertSize;

};

