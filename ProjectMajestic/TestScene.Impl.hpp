
#include "TestScene.hpp"

#include "ChunkProvider.hpp"
#include "TerrainManager.hpp"
#include "Uuid.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"
#include "TestEntity.hpp"
#include "ParticleSystem.hpp"
#include "NetworkServer.hpp"
#include "NetworkClient.hpp"
#include "EntityAllocator.hpp"
#include "InGameUI.hpp"
#include "PlayerInventory.hpp"


////////////////////////////////////////
void TestScene::preWindowInitalaize() {

	mlog << "PREWindowInitalaize" << dlog;

	assetManager.loadListFile();

	textureManager.bindTexture();

	blockAllocator.initalaize();
	entityAllocator.initalaize();
	itemAllocator.initalaize();

	background.loadFromFile(assetManager.getAssetFilename("background_stone"));
	background.setRepeated(true);

	renderIO.gameScaleFactor = 24.0;
	renderIO.gameScenePosOffset = Vector2d(0, 0);

	localPlayer = nullptr;

	//terrainManager.loadChunk(Vector2i(0, 0), EmptyChunkProvider());
	//terrainManager.loadChunk(Vector2i(0, 1), EmptyChunkProvider());
	//terrainManager.loadChunk(Vector2i(1, 0), EmptyChunkProvider());
	//terrainManager.loadChunk(Vector2i(1, 1), EmptyChunkProvider());
	//terrainManager.loadChunk(Vector2i(0, 2), EmptyChunkProvider());
	//terrainManager.loadChunk(Vector2i(1, 2), EmptyChunkProvider());
	//terrainManager.loadChunk(Vector2i(2, 2), EmptyChunkProvider());
	//terrainManager.loadChunk(Vector2i(2, 1), EmptyChunkProvider());
	//terrainManager.loadChunk(Vector2i(2, 0), EmptyChunkProvider());

	//	terrainManager.poseLight(Vector2i(12, 12), 11);
	//	terrainManager.poseLight(Vector2i(19, 19), 8);

	//	terrainManager.poseLight(Vector2i(1, 3), 11);

	Uuid::seed(time(nullptr));

	win.setKeyRepeatEnabled(false);

	blocks.push_back("torch");
	blocks.push_back("stone");
	blocks.push_back("log");
	blocks.push_back("gravel");
	blocks.push_back("bedrock");

	curBlock = blocks[0];

}


////////////////////////////////////////
void TestScene::start(RenderWindow & win) {
	imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("font_minecraft").c_str(),
		16, nullptr, imgui::GetIO().Fonts->GetGlyphRangesDefault());
	imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("courier_new").c_str(),
		13, nullptr, imgui::GetIO().Fonts->GetGlyphRangesDefault());
	imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("courier_new_bold").c_str(),
		13, nullptr, imgui::GetIO().Fonts->GetGlyphRangesDefault());
	//imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("source_han_sans").c_str(),
		//16, nullptr, imgui::GetIO().Fonts->GetGlyphRangesChinese());
	//imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("font_msyh").c_str(),
		//16, nullptr, imgui::GetIO().Fonts->GetGlyphRangesChinese());
	imgui::SFML::UpdateFontTexture();

	prov.setup(Vector2u(3, 2), 5);

	localPlayer = new PlayerEntity();
	entityManager.insert(localPlayer, Vector2d(prov.getSpawnPoints()[0]) + Vector2d(0.5, 1 - 1e-7));

	role = Server;
}


////////////////////////////////////////
void TestScene::onRender(RenderWindow & win) {
	AUTOLOCKABLE_NAMED(terrainManager, tml);
	AUTOLOCKABLE_NAMED(particleSystem, psl);
	AUTOLOCKABLE_NAMED(entityManager, eml);

	// Place screen offset calculations here to obtain maxinum synchronization
	if (localPlayer != nullptr) {
		Vector2d onScrPlayer = localPlayer->getPosition()*renderIO.gameScaleFactor;
		renderIO.gameScenePosOffset = Vector2d(win.getSize()) / 2.0 - onScrPlayer;
	}

	Clock drawTime;

	Sprite sp;
	sp.setTexture(background);
	sp.setScale(renderIO.gameScaleFactor / background.getSize().x * 1.5, renderIO.gameScaleFactor / background.getSize().y * 1.5);
	sp.setTextureRect(IntRect(0, 0, (win.getView().getSize().x + 2 * renderIO.gameScaleFactor * 1.5) / sp.getScale().x,
		(win.getView().getSize().y + 2 * renderIO.gameScaleFactor * 1.5) / sp.getScale().y));
	sp.setPosition(modDouble(renderIO.gameScenePosOffset.x, renderIO.gameScaleFactor * 1.5) - renderIO.gameScaleFactor * 1.5,
		modDouble(renderIO.gameScenePosOffset.y, renderIO.gameScaleFactor * 1.5) - renderIO.gameScaleFactor * 1.5);
	win.draw(sp);

	renderIO.viewRect = FloatRect(-renderIO.gameScenePosOffset.x, -renderIO.gameScenePosOffset.y, win.getSize().x, win.getSize().y);
	renderIO.viewRectBlock = DoubleRect(
		renderIO.viewRect.left / renderIO.gameScaleFactor,
		renderIO.viewRect.top / renderIO.gameScaleFactor,
		renderIO.viewRect.width / renderIO.gameScaleFactor,
		renderIO.viewRect.height / renderIO.gameScaleFactor);
	win.setView(View(renderIO.viewRect));

	VertexArray totalTriangles;

	totalTriangles.setPrimitiveType(PrimitiveType::Triangles);
	terrainManager.getRenderList(totalTriangles); terrainListSize = totalTriangles.getVertexCount();
	terrainManager.getLightMask(totalTriangles);
	// Append extra triangles covering the outside if required
	// Left (and two corners on the left)
	Vector2d rightBottomT = Vector2d(terrainManager.getChunkCount()) * 16.0 * renderIO.gameScaleFactor;
	if (renderIO.viewRect.left < 0) {
		totalTriangles.append(Vertex(Vector2f(0, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Right-Top
		totalTriangles.append(Vertex(Vector2f(renderIO.viewRect.left, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		totalTriangles.append(Vertex(Vector2f(0, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		totalTriangles.append(Vertex(Vector2f(renderIO.viewRect.left, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		totalTriangles.append(Vertex(Vector2f(0, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		totalTriangles.append(Vertex(Vector2f(renderIO.viewRect.left, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Left-Bottom
	}
	// Right (and two corners on the right)
	if (renderIO.viewRect.left + renderIO.viewRect.width > rightBottomT.x) {
		totalTriangles.append(Vertex(Vector2f(renderIO.viewRect.left + renderIO.viewRect.width, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Right-Top
		totalTriangles.append(Vertex(Vector2f(rightBottomT.x, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		totalTriangles.append(Vertex(Vector2f(renderIO.viewRect.left + renderIO.viewRect.width, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		totalTriangles.append(Vertex(Vector2f(rightBottomT.x, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		totalTriangles.append(Vertex(Vector2f(renderIO.viewRect.left + renderIO.viewRect.width, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		totalTriangles.append(Vertex(Vector2f(rightBottomT.x, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Left-Bottom
	}
	// Top
	if (renderIO.viewRect.top < 0) {
		totalTriangles.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), renderIO.viewRect.top), Color(0, 0, 0, 192))); // Right-Top
		totalTriangles.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		totalTriangles.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), 0), Color(0, 0, 0, 192))); // Right-Bottom
		totalTriangles.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		totalTriangles.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), 0), Color(0, 0, 0, 192))); // Right-Bottom
		totalTriangles.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), 0), Color(0, 0, 0, 192))); // Left-Bottom
	}
	// Bottom
	if (renderIO.viewRect.top + renderIO.viewRect.height > rightBottomT.y) {
		totalTriangles.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), rightBottomT.y), Color(0, 0, 0, 192))); // Right-Top
		totalTriangles.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), rightBottomT.y), Color(0, 0, 0, 192))); // Left-Top
		totalTriangles.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		totalTriangles.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), rightBottomT.y), Color(0, 0, 0, 192))); // Left-Top
		totalTriangles.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		totalTriangles.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Left-Bottom
	}
	terrainMaskSize = totalTriangles.getVertexCount()
		- terrainListSize;
	particleSystem.getRenderList(totalTriangles); particleListSize = totalTriangles.getVertexCount()
		- terrainListSize - terrainMaskSize;
	entityManager.getRenderList(totalTriangles); entityListSize = totalTriangles.getVertexCount()
		- terrainListSize - terrainMaskSize - particleListSize;
	totalVertSize = totalTriangles.getVertexCount();

	win.draw(totalTriangles, textureManager.getBindingTexture());

	for (auto i : entityManager.getEntityMapList())
		if (i.second != nullptr)
			win.draw(renderRect(i.second->getHitbox()*renderIO.gameScaleFactor));

	Vector2i mousePos = TerrainManager::convertScreenPixelToWorldBlockCoord(Vector2d(Mouse::getPosition(win)));
	Block* b = terrainManager.getBlock(mousePos);
	if (b == nullptr) {
		Vector2d center = Vector2d(mousePos.x + 0.5, mousePos.y + 0.5)*renderIO.gameScaleFactor;
		double width = renderIO.gameScaleFactor;
		VertexArray verts;
		// Left-Top
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - width / 2.0)));
		// Right-Top
		verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - width / 2.0)));
		// Right-Bottom
		verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y + width / 2.0)));
		// Left-Bottom
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y + width / 2.0)));
		// Left-Top
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - width / 2.0)));

		verts.setPrimitiveType(PrimitiveType::LinesStrip);
		win.draw(verts);
	}
	else
		win.draw(renderRect(b->getHitbox()*renderIO.gameScaleFactor));
	/*
		win.setView(View(FloatRect(0, 0, win.getSize().x, win.getSize().y)));

		VertexArray verts;
		Vector2i pos = Mouse::getPosition(win);
		verts.append(Vertex(Vector2f(pos.x, 0)));
		verts.append(Vertex(Vector2f(pos.x, win.getView().getSize().y)));
		verts.append(Vertex(Vector2f(0, pos.y)));
		verts.append(Vertex(Vector2f(win.getView().getSize().x, pos.y)));
		verts.setPrimitiveType(PrimitiveType::Lines);
		win.draw(verts);
	*/
}


////////////////////////////////////////
void TestScene::updateLogic(RenderWindow & win) {
	AUTOLOCKABLE_NAMED(terrainManager, tml);
	AUTOLOCKABLE_NAMED(particleSystem, psl);
	AUTOLOCKABLE_NAMED(entityManager, eml);

	if (localPlayer != nullptr) {
		Vector2d mouseBlock = TerrainManager::convertScreenPixelToWorldCoord(Vector2d(Mouse::getPosition(win)));
		Vector2d eyePos = localPlayer->getEyePosition();
		gameIO.degreeAngle = atan((mouseBlock.y - eyePos.y) / (mouseBlock.x - eyePos.x)) * 180.0 / PI;
		if (mouseBlock.x - eyePos.x < 0)
			gameIO.degreeAngle += 180.0;
		if (gameIO.degreeAngle > 360.0)
			gameIO.degreeAngle -= 360.0;
		if (gameIO.degreeAngle < 0.0)
			gameIO.degreeAngle += 360.0;
	}

	// Manage keys and game/player controls
	// Mouse controls
	if (!imgui::GetIO().WantCaptureMouse && (logicIO.mouseState[Mouse::Left] == LogicIO::JustPressed)) {
		bool breakFlag = true;
		const string& name = playerInventory.slots[0][playerInventory.cursorId]["item_name"];
		if (name.substr(0, 5) == "item_") {
			Item* item = itemAllocator.allocate(name.substr(5), playerInventory.slots[0][playerInventory.cursorId]);
			if (item != nullptr && item->_onLeftPressed())
				breakFlag = false;
			if (item != nullptr)
				delete item;
		}
		if (breakFlag)
			terrainManager.breakBlock(TerrainManager::convertScreenPixelToWorldBlockCoord(Vector2d(Mouse::getPosition(win))));
	}
	if (logicIO.mouseState[Mouse::Left] == LogicIO::JustReleased) {
		const string& name = playerInventory.slots[0][playerInventory.cursorId]["item_name"];
		if (name.substr(0, 5) == "item_") {
			Item* item = itemAllocator.allocate(name.substr(5), playerInventory.slots[0][playerInventory.cursorId]);
			if (item != nullptr) {
				item->_onLeftReleased();
				delete item;
			}
		}
	}
	if (!imgui::GetIO().WantCaptureMouse && (logicIO.mouseState[Mouse::Right] == LogicIO::JustPressed)) {
		const string& str = playerInventory.slots[0][playerInventory.cursorId]["item_name"].getDataString();
		bool sendRightClick = true;
		Vector2i coord = TerrainManager::convertScreenPixelToWorldBlockCoord(Vector2d(Mouse::getPosition(win)));
		Block* b = terrainManager.getBlock(coord);
		if (str != "") {
			if (str.substr(0, 6) == "block_")
				terrainManager.placeBlock(coord, str.substr(6));
			else if (str.substr(0, 5) == "item_") {
				Item* item = itemAllocator.allocate(str.substr(5), playerInventory.slots[0][playerInventory.cursorId]);
				if (item != nullptr) {
					sendRightClick = !item->_onRightPressed();
					delete item;
				}
			}
		}
		if (sendRightClick) {
			if (b != nullptr)
				b->_onRightClick();
		}
	}
	if (logicIO.mouseState[Mouse::Right] == LogicIO::JustReleased) {
		const string& name = playerInventory.slots[0][playerInventory.cursorId]["item_name"];
		if (name.substr(0, 5) == "item_") {
			Item* item = itemAllocator.allocate(name.substr(5), playerInventory.slots[0][playerInventory.cursorId]);
			if (item != nullptr) {
				item->_onRightReleased();
				delete item;
			}
		}
	}
	if (!imgui::GetIO().WantCaptureMouse && (logicIO.mouseState[Mouse::Middle] == LogicIO::JustPressed))
		if (localPlayer == nullptr) {
			localPlayer = new PlayerEntity();
			entityManager.insert(localPlayer, TerrainManager::convertScreenPixelToWorldCoord(Vector2d(Mouse::getPosition(win))));
		}

	// Keyboard controls
	if (!imgui::GetIO().WantCaptureKeyboard &&
		(logicIO.keyboardState[Keyboard::A] == LogicIO::JustPressed || logicIO.keyboardState[Keyboard::Left] == LogicIO::JustPressed))
		localPlayer->moveLeft(true);
	if (logicIO.keyboardState[Keyboard::A] == LogicIO::JustReleased || logicIO.keyboardState[Keyboard::Left] == LogicIO::JustReleased)
		localPlayer->moveLeft(false);
	if (!imgui::GetIO().WantCaptureKeyboard &&
		(logicIO.keyboardState[Keyboard::D] == LogicIO::JustPressed || logicIO.keyboardState[Keyboard::Right] == LogicIO::JustPressed))
		localPlayer->moveRight(true);
	if (logicIO.keyboardState[Keyboard::D] == LogicIO::JustReleased || logicIO.keyboardState[Keyboard::Right] == LogicIO::JustReleased)
		localPlayer->moveRight(false);
	if (!imgui::GetIO().WantCaptureKeyboard &&
		(Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)))
		localPlayer->ascendLadder(true);
	if (logicIO.keyboardState[Keyboard::W] == LogicIO::JustReleased || logicIO.keyboardState[Keyboard::Up] == LogicIO::JustReleased)
		localPlayer->ascendLadder(false);
	if (!imgui::GetIO().WantCaptureKeyboard &&
		(Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)))
		localPlayer->decendLadder(true);
	if (logicIO.keyboardState[Keyboard::S] == LogicIO::JustReleased || logicIO.keyboardState[Keyboard::Down] == LogicIO::JustReleased)
		localPlayer->decendLadder(false);
	if (!imgui::GetIO().WantCaptureKeyboard &&
		(logicIO.keyboardState[Keyboard::LControl] == LogicIO::JustPressed))
		localPlayer->crouch(true);
	if (logicIO.keyboardState[Keyboard::LControl] == LogicIO::JustReleased)
		localPlayer->crouch(false);
	if (Keyboard::isKeyPressed(Keyboard::Space))
		localPlayer->jump();

	terrainManager.updateLogic();
	particleSystem.updateLogic();
	entityManager.updateLogic();
	uiManager.updateLogic();

	if (logicIO.keyboardState[Keyboard::E] == LogicIO::JustPressed && !imgui::GetIO().WantCaptureKeyboard)
		uiManager.changeUI(new PlayerInventoryUI);
	if (logicIO.keyboardState[Keyboard::Escape] == LogicIO::JustPressed && !imgui::GetIO().WantCaptureKeyboard)
		uiManager.changeUI(new PlayerInventoryUI);
}


////////////////////////////////////////
void TestScene::handleEvent(RenderWindow & win, Event & event) {
	AUTOLOCKABLE_NAMED(terrainManager, tml);
	AUTOLOCKABLE_NAMED(particleSystem, psl);
	AUTOLOCKABLE_NAMED(entityManager, eml);

	if (event.type == Event::MouseWheelScrolled) {
		if (event.mouseWheelScroll.delta < 0)
			playerInventory.addCursorId();
		if (event.mouseWheelScroll.delta > 0)
			playerInventory.minusCursorId();
	}
}


////////////////////////////////////////
void TestScene::runImGui() {
	AUTOLOCKABLE_NAMED(terrainManager, tml);
	AUTOLOCKABLE_NAMED(particleSystem, psl);
	AUTOLOCKABLE_NAMED(entityManager, eml);

	imgui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	imgui::ShowFontSelector("Fonts");
	imgui::Text(u8"Innovation In China �й����죬�ݼ�ȫ�� 1234567890");
	static float value = renderIO.gameScaleFactor;
	imgui::SliderFloat("GameScaleFactor", &value, 24, 64);
	renderIO.gameScaleFactor = value;
	imgui::End();

	imgui::ShowDemoWindow();

	ENTER_IMGUI_DEBUG;

	imgui::Text("%s %s / Version %d.%d.%d %s / Compile Time %s",
		projectCode.c_str(), projectSuffix.c_str(), majorVersion, minorVersion, patchVersion, releaseStage.c_str(), compileTime.c_str());
#ifdef USE_ASYNC_RENDERING
	imgui::Text("State: Async-Rendering, TPS: %d, EPS: %d, FPS: %d", logicTickPerSecond, eventTickPerSecond, framePerSecond);
#else
	imgui::Text("State: Mono-Thread, FPS: %d", eventTickPerSecond);
#endif // USE_ASYNC_RENDERING
	imgui::Text("Local Role: %s", role == Server ? "Server" : "Client");
	imgui::Text("Entity Count: %d", entityManager.getEntityMapList().size());
	imgui::Text("Particle Count: %d", particleSystem.getParticleList().size());

	imgui::Text("PlayerEntity Info");
	if (localPlayer != nullptr) {
		imgui::Text("  Uuid: {%s}", localPlayer->getUuid().toString().c_str());
		imgui::Text("  Position: (%.3lf, %.3lf)", localPlayer->getPosition().x, localPlayer->getPosition().y);
		imgui::Text("  Velocity: (%.3lf, %.3lf)", localPlayer->getVelocity().x, localPlayer->getVelocity().y);
		imgui::Text("  EyePosition: (%.3lf, %.3lf)", localPlayer->getEyePosition().x, localPlayer->getEyePosition().y);
		imgui::Text("  IsOnGround: %s", localPlayer->isOnGround() ? "True" : "False");
		imgui::Text("  IsOnLadder: %s", localPlayer->isOnLadder() ? "True" : "False");
		imgui::Text("  IsCrouched: %s", localPlayer->isCrouched() ? "True" : "False");
		imgui::Text("  WantStandup: %s", localPlayer->isStuck() ? "True" : "False");
	}
	else
		imgui::Text("  localPlayer == nullptr");

	imgui::Text("Mouse Cursor Info");
	Vector2i pos = Mouse::getPosition(win);
	Vector2i posB = TerrainManager::convertScreenPixelToWorldBlockCoord(Vector2d(pos));
	Vector2d posC = TerrainManager::convertScreenPixelToWorldCoord(Vector2d(pos));
	Vector2i chunkId = TerrainManager::convertWorldCoordToChunkId(posB);
	Vector2i inChunkC = TerrainManager::convertWorldCoordToInChunkCoord(posB);
	Chunk* c = terrainManager.getChunk(chunkId);
	Block* b = terrainManager.getBlock(posB);
	imgui::Text("  OnScreen Pos: (%d, %d)", pos.x, pos.y);
	imgui::Text("  World Pos: (%.3lf, %.3lf)", posC.x, posC.y);
	imgui::Text("  DegreeAngle: %.3lf", gameIO.degreeAngle);
	imgui::Text("  World Block Pos: (%d, %d)", posB.x, posB.y);
	imgui::Text("  InChunk Pos: Chunk(%d, %d), InChunk(%d, %d)", chunkId.x, chunkId.y, inChunkC.x, inChunkC.y);
	imgui::Text("  BlockType: %s", b == nullptr ? "nullptr" : b->getBlockId().c_str());
	if (c != nullptr)
		imgui::Text("  Light Level: %d", c->lightLevel[inChunkC.x][inChunkC.y]);
	if (b != nullptr) {
		for (auto& i : b->getDataset().getDatasets()) {
			imgui::Text("  %s:", i.first.c_str()); imgui::SameLine();
			if (i.second.getType() == Data::Integer)
				imgui::Text("%d", i.second.getDataInt());
			else if (i.second.getType() == Data::String)
				imgui::Text("%s", i.second.getDataString().c_str());
			else if (i.second.getType() == Data::Bool) {
				if (i.second.getDataBool())
					imgui::PushStyleColor(ImGuiCol_Text, Color::Green);
				else
					imgui::PushStyleColor(ImGuiCol_Text, Color::Red);
				imgui::Text("%s", i.second.getDataBool() ? "true" : "false");
				imgui::PopStyleColor();
			}
			else if (i.second.getType() == Data::Uuid)
				imgui::Text("{%s}", i.second.getDataUuid().toString());
		}
	}

	imgui::Text("Item In Hand");
	imgui::Text("  CursorId: %d", playerInventory.cursorId);
	Dataset& slot = playerInventory.slots[0][playerInventory.cursorId];
	for (auto& i : slot.getDatasets()) {
		imgui::Text("  %s:", i.first.c_str()); imgui::SameLine();
		if (i.second.getType() == Data::Integer)
			imgui::Text("%d", i.second.getDataInt());
		else if (i.second.getType() == Data::String)
			imgui::Text("%s", i.second.getDataString().c_str());
		else if (i.second.getType() == Data::Bool) {
			if (i.second.getDataBool())
				imgui::PushStyleColor(ImGuiCol_Text, Color::Green);
			else
				imgui::PushStyleColor(ImGuiCol_Text, Color::Red);
			imgui::Text("%s", i.second.getDataBool() ? "true" : "false");
			imgui::PopStyleColor();
		}
		else if (i.second.getType() == Data::Uuid)
			imgui::Text("{%s}", i.second.getDataUuid().toString());
	}

	imgui::Text("Light Sources"); imgui::SameLine();
	auto& lights = terrainManager.getLightSources();
	imgui::Text("Count: %d", lights.size());
	for (auto& i : lights) {
		imgui::Text("  {%08x.} (%d, %d) %d", i.first.sc1, i.second.first.x, i.second.first.y, i.second.second);
	}

	imgui::Text("TerrainRender:  %d verts (%d tri)",
		terrainListSize, terrainListSize / 3);
	imgui::Text("TerrainLight:   %d verts (%d tri)",
		terrainMaskSize, terrainMaskSize / 3);
	imgui::Text("ParticleRender: %d verts (%d tri)",
		particleListSize, particleListSize / 3);
	imgui::Text("EntityRender:   %d verts (%d tri)",
		entityListSize, entityListSize / 3);
	imgui::Text("Total:          %d verts (%d tri)",
		totalVertSize, totalVertSize / 3);

	LEAVE_IMGUI_DEBUG;

	imgui::Begin("Server", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	static int ports;
	imgui::InputInt("Port", &ports);
	imgui::SameLine();
	if (!networkServer.isListening()) {
		if (imgui::Button("Start"))
			networkServer.startListen(ports);
	}
	else {
		if (imgui::Button("Stop"))
			networkServer.stopServer();
	}
	imgui::End();

	imgui::Begin("Client", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	static int portc;
	static char ip[256];
	imgui::InputText("Ip Address", ip, 256);
	imgui::InputInt("Port", &portc);
	imgui::SameLine();
	if (imgui::Button("Kill player")) {
		if (localPlayer != nullptr) {
			localPlayer->kill();
			localPlayer = nullptr;
		}
	}
	if (!networkClient.isConnected()) {
		if (imgui::Button("Connect"))
			networkClient.connect(IpAddress(ip), portc);
	}
	else {
		if (imgui::Button("Get Chunk Count")) {
			networkClient.getChunkCount();
		}
		imgui::SameLine();
		if (imgui::Button("Clear Terrain")) {
			terrainManager.clear();
		}
		imgui::SameLine();
		if (imgui::Button("Reload Terrain")) {
			networkClient.reloadAllChunks();
		}
	}
	imgui::End();

	uiManager.runImGui();

	playerInventory.runImGui();
}


////////////////////////////////////////
void TestScene::stop() {

}


////////////////////////////////////////