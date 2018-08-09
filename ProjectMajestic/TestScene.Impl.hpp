
#include "TestScene.hpp"

#include "ChunkProvider.hpp"
#include "TerrainManager.hpp"
#include "Uuid.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"
#include "TestEntity.hpp"
#include "ParticleSystem.hpp"
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


void TestScene::postWindowInitalaize(RenderWindow& win) {
	imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("font_minecraft").c_str(),
											 16, nullptr, imgui::GetIO().Fonts->GetGlyphRangesDefault());
	imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("courier_new").c_str(),
											 13, nullptr, imgui::GetIO().Fonts->GetGlyphRangesDefault());
	imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("courier_new_bold").c_str(),
											 13, nullptr, imgui::GetIO().Fonts->GetGlyphRangesDefault());
										 //imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("source_han_sans").c_str(),
											 //16, nullptr, imgui::GetIO().Fonts->GetGlyphRangesChinese());
	imgui::GetIO().Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Dengb.ttf",
											 13, nullptr, imgui::GetIO().Fonts->GetGlyphRangesChinese());
	imgui::SFML::UpdateFontTexture();

	prov.setup(Vector2u(3, 2), 5);
}


////////////////////////////////////////
void TestScene::start(RenderWindow & win) {
	testEntity = Uuid::nil();

	localPlayer = make_shared<PlayerEntity>();
	localPlayer->setIsLocalPlayer(true);
	localPlayer->setHealth(localPlayer->getMaxHealth());
	entityManager.insert(localPlayer, Vector2d(prov.getSpawnPoints()[0]) + Vector2d(0.5, 1 - 1e-7));

	gameIO.ruleExplosionDamagesTerrain = true;
	showDebugInfo = false;
	showExtraImGuiWindows = true;

	role = Server;
}


////////////////////////////////////////
void TestScene::onRender(RenderWindow & win) {
	AUTOLOCK_ALL_SYSTEM;

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
	sp.setPosition(fmod(renderIO.gameScenePosOffset.x, renderIO.gameScaleFactor * 1.5) - renderIO.gameScaleFactor * 1.5,
				   fmod(renderIO.gameScenePosOffset.y, renderIO.gameScaleFactor * 1.5) - renderIO.gameScaleFactor * 1.5);
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
	terrainManager.getLightMask(totalTriangles); terrainMaskSize = totalTriangles.getVertexCount()
		- terrainListSize;
	entityManager.getRenderList(totalTriangles); entityListSize = totalTriangles.getVertexCount()
		- terrainListSize - terrainMaskSize;
	particleSystem.getRenderList(totalTriangles); particleListSize = totalTriangles.getVertexCount()
		- terrainListSize - terrainMaskSize - entityListSize;
	totalVertSize = totalTriangles.getVertexCount();

	win.draw(totalTriangles, textureManager.getBindingTexture());

	for (auto i : entityManager.getEntityMapList())
		if (i.second != nullptr)
			win.draw(renderRect(i.second->getHitbox()*renderIO.gameScaleFactor));

	Vector2i mousePos = TerrainManager::convertScreenPixelToWorldBlockCoord(Mouse::getPosition(win));
	shared_ptr<Block> b = terrainManager.getBlock(mousePos);
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

	if (localPlayer != nullptr) {
		VertexArray verts;
		Vector2d eye = localPlayer->getEyePosition();
		//verts.append(Vertex(Vector2f(eye.x, eye.y - win.getSize().y / (float)renderIO.gameScaleFactor / 2.0f) * (float)renderIO.gameScaleFactor, Color::White));
		//verts.append(Vertex(Vector2f(eye.x, eye.y + win.getSize().y / (float)renderIO.gameScaleFactor / 2.0f) * (float)renderIO.gameScaleFactor, Color::White));
		//verts.append(Vertex(Vector2f(eye.x - win.getSize().x / (float)renderIO.gameScaleFactor / 2.0f, eye.y) * (float)renderIO.gameScaleFactor, Color::White));
		//verts.append(Vertex(Vector2f(eye.x + win.getSize().x / (float)renderIO.gameScaleFactor / 2.0f, eye.y) * (float)renderIO.gameScaleFactor, Color::White));
		verts.append(Vertex(Vector2f(eye) * (float)renderIO.gameScaleFactor, Color::White));
		verts.append(Vertex(Vector2f(TerrainManager::convertScreenPixelToWorldCoord(Mouse::getPosition(win))) * (float)renderIO.gameScaleFactor, Color::White));
		verts.setPrimitiveType(PrimitiveType::Lines);
		win.draw(verts);
	}

	win.setView(View(FloatRect(0, 0, win.getSize().x, win.getSize().y)));
	VertexArray verts;
	Vector2i pos = Mouse::getPosition(win);
	//verts.append(Vertex(Vector2f(pos.x, 0)));
	//verts.append(Vertex(Vector2f(pos.x, win.getView().getSize().y)));
	//verts.append(Vertex(Vector2f(0, pos.y)));
	//verts.append(Vertex(Vector2f(win.getView().getSize().x, pos.y)));
	verts.append(Vertex(Vector2f(pos.x, pos.y - 15)));
	verts.append(Vertex(Vector2f(pos.x, pos.y + 15)));
	verts.append(Vertex(Vector2f(pos.x - 15, pos.y)));
	verts.append(Vertex(Vector2f(pos.x + 15, pos.y)));
	verts.setPrimitiveType(PrimitiveType::Lines);
	win.draw(verts);

}


////////////////////////////////////////
bool TestScene::_sendMousePressedToHandItem(Mouse::Button button) {
	bool flag = false;
	const string& name = playerInventory.slots[0][playerInventory.cursorId]["item_name"];
	if (name.substr(0, 5) == "item_") {
		shared_ptr<Item> item = itemAllocator.allocate(name.substr(5), playerInventory.slots[0][playerInventory.cursorId]);
		if (item != nullptr && (button == Mouse::Left ? item->_onLeftPressed() : item->_onRightPressed()))
			flag = true;
	}
	return flag;
}


////////////////////////////////////////
void TestScene::_sendMouseReleasedToHandItem(Mouse::Button button) {
	const string& name = playerInventory.slots[0][playerInventory.cursorId]["item_name"];
	if (name.substr(0, 5) == "item_") {
		shared_ptr<Item> item = itemAllocator.allocate(name.substr(5), playerInventory.slots[0][playerInventory.cursorId]);
		if (item != nullptr) {
			if (button == Mouse::Left)
				item->_onLeftReleased();
			else
				item->_onRightReleased();
		}
	}
}


////////////////////////////////////////
void TestScene::updateLogic(RenderWindow & win) {
	AUTOLOCK_ALL_SYSTEM;

	if (localPlayer != nullptr) {
		Vector2d mouseBlock = TerrainManager::convertScreenPixelToWorldCoord(Mouse::getPosition(win));
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
	if (!imgui::GetIO().WantCaptureMouse && (logicIO.mouseState[Mouse::Left] == LogicIO::JustPressed))
		if (!_sendMousePressedToHandItem(Mouse::Left))
			terrainManager.breakBlock(TerrainManager::convertScreenPixelToWorldBlockCoord(Mouse::getPosition(win)));
	if (logicIO.mouseState[Mouse::Left] == LogicIO::JustReleased)
		_sendMouseReleasedToHandItem(Mouse::Left);
	if (!imgui::GetIO().WantCaptureMouse && (logicIO.mouseState[Mouse::Right] == LogicIO::JustPressed)) {
		const string& str = playerInventory.slots[0][playerInventory.cursorId]["item_name"].getDataString();
		bool sendRightClick = true;
		Vector2i coord = TerrainManager::convertScreenPixelToWorldBlockCoord(Mouse::getPosition(win));
		shared_ptr<Block> b = terrainManager.getBlock(coord);
		if (str != "") {
			if (str.substr(0, 6) == "block_")
				terrainManager.placeBlock(coord, str.substr(6));
			else if (str.substr(0, 5) == "item_") {
				shared_ptr<Item> item = itemAllocator.allocate(str.substr(5), playerInventory.slots[0][playerInventory.cursorId]);
				if (item != nullptr)
					sendRightClick = !item->_onRightPressed();
			}
		}
		if (sendRightClick) {
			if (b != nullptr)
				b->_onRightClick();
		}
	}
	if (logicIO.mouseState[Mouse::Right] == LogicIO::JustReleased)
		_sendMouseReleasedToHandItem(Mouse::Right);
	if (!imgui::GetIO().WantCaptureMouse && (logicIO.mouseState[Mouse::Middle] == LogicIO::JustPressed)) {
		if (!localPlayer->isAlive()) {
			localPlayer = make_shared<PlayerEntity>();
			localPlayer->setHealth(localPlayer->getMaxHealth());
			localPlayer->setIsLocalPlayer(true);
			entityManager.insert(localPlayer, TerrainManager::convertScreenPixelToWorldCoord(Mouse::getPosition(win)));
		}
		else {
			if (testEntity == Uuid()) {
				shared_ptr<TestEntity> te = make_shared<TestEntity>();
				te->setHealth(te->getMaxHealth());
				testEntity = entityManager.insert(te, TerrainManager::convertScreenPixelToWorldCoord(Mouse::getPosition(win)));
			}
			else {
				shared_ptr<TestEntity> te = make_shared<TestEntity>();
				te->setHealth(te->getMaxHealth());
				entityManager.insert(te, TerrainManager::convertScreenPixelToWorldCoord(Mouse::getPosition(win)));
			}
				//entityManager.explode(TerrainManager::convertScreenPixelToWorldCoord(Mouse::getPosition(win)),
				//					  12.0, false);
			//particleSystem.emitSmoke(TerrainManager::convertScreenPixelToWorldCoord(Mouse::getPosition(win)));
			//for (int i = 1; i <= 1000; i++) {
			//	// Tile Drop
			//	ItemEntity* e = new ItemEntity("block_stone");
			//	// Give a random velocity
			//	e->accelerateVector(1.0, 180 + rand() % 180);

			//	entityManager.insert(e, Vector2d(TerrainManager::convertScreenPixelToWorldCoord(Mouse::getPosition(win))) + Vector2d(0.5, 0.8));
			//}
		}
	}

	// Keyboard controls
	if (!imgui::GetIO().WantCaptureKeyboard &&
		logicIO.keyboardState[Keyboard::F1] == LogicIO::JustPressed)
		showExtraImGuiWindows = !showExtraImGuiWindows;
	if (!imgui::GetIO().WantCaptureKeyboard &&
		logicIO.keyboardState[Keyboard::F3] == LogicIO::JustPressed)
		app->hasLog = showDebugInfo = !showDebugInfo;
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
	if (!imgui::GetIO().WantCaptureKeyboard &&
		Keyboard::isKeyPressed(Keyboard::Space))
		localPlayer->jump();

	terrainManager.updateLogic();
	particleSystem.updateLogic();
	entityManager.updateLogic();
	playerInventory.updateLogic();
	uiManager.updateLogic();

	if (logicIO.keyboardState[Keyboard::E] == LogicIO::JustPressed && !imgui::GetIO().WantCaptureKeyboard)
		uiManager.changeUI(make_shared<PlayerInventoryUI>());
	if (logicIO.keyboardState[Keyboard::Escape] == LogicIO::JustPressed && !imgui::GetIO().WantCaptureKeyboard)
		uiManager.changeUI(make_shared<PlayerInventoryUI>());
}


////////////////////////////////////////
void TestScene::handleEvent(RenderWindow & win, Event & event) {
	AUTOLOCK_ALL_SYSTEM;

	if (event.type == Event::MouseWheelScrolled) {
		if (Mouse::isButtonPressed(Mouse::Left))
			_sendMouseReleasedToHandItem(Mouse::Left);
		if (Mouse::isButtonPressed(Mouse::Right))
			_sendMouseReleasedToHandItem(Mouse::Right);
		if (event.mouseWheelScroll.delta < 0) {
			playerInventory.addCursorId();
		}
		else /*if (event.mouseWheelScroll.delta > 0)*/ {
			playerInventory.minusCursorId();
		}
		if (Mouse::isButtonPressed(Mouse::Left))
			_sendMousePressedToHandItem(Mouse::Left);
		if (Mouse::isButtonPressed(Mouse::Right))
			_sendMousePressedToHandItem(Mouse::Right);
	}
}


////////////////////////////////////////
void TestScene::runImGui() {
	AUTOLOCK_ALL_SYSTEM;

	if (showExtraImGuiWindows) {
		imgui::Begin("TestEntity");
		if (testEntity == Uuid::nil())
			imgui::Text("TextEntity == nullptr");
		else {
			shared_ptr<Entity> e = entityManager.getEntity(testEntity);
			if (e == nullptr)
				testEntity = Uuid();
			else {
				TestEntity& te = dynamic_cast<TestEntity&>(*e);
				imgui::Text("Health: %d / %d", te.getHealth(), te.getMaxHealth());
			}
		}
		imgui::End();

		imgui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		imgui::ShowFontSelector("Fonts");
		imgui::Text(u8"Innovation In China 中国智造，惠及全球 1234567890");
		static char langFile[128] = {"lang-zh-Hans.list"};
		if (imgui::InputText("Language File", langFile, sizeof(langFile), ImGuiInputTextFlags_EnterReturnsTrue))
			text.loadFromFile(langFile);
		static float value = renderIO.gameScaleFactor;
		imgui::SliderFloat("GameScaleFactor", &value, 16, 64);
		renderIO.gameScaleFactor = value;
		imgui::Checkbox("Explosion damages terrain", &gameIO.ruleExplosionDamagesTerrain);
		if (imgui::Button("Break!"))
			for (auto& k : terrainManager.getChunks()) {
				Vector2i off = k.first*chunkSize;
				shared_ptr<Chunk> c = k.second;
				for (int i = 0; i < chunkSize; i++)
					for (int j = 0; j < chunkSize; j++) {
						if (c->getBlock(Vector2i(i, j)) == nullptr)
							continue;
						if (c->getBlock(Vector2i(i, j))->getBlockId() != "bedrock")
							terrainManager.breakBlock(off + Vector2i(i, j));
					}
			}
		imgui::SameLine();
		if (imgui::Button("Update lighting"))
			terrainManager.requestLightingUpdate();
		imgui::Image(*textureManager.getBindingTexture());
		imgui::End();

		imgui::ShowDemoWindow();
	}

	if (showDebugInfo) {
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
		Vector2i posB = TerrainManager::convertScreenPixelToWorldBlockCoord(pos);
		Vector2d posC = TerrainManager::convertScreenPixelToWorldCoord(pos);
		Vector2i chunkId = TerrainManager::convertWorldCoordToChunkId(posB);
		Vector2i inChunkC = TerrainManager::convertWorldCoordToInChunkCoord(posB);
		shared_ptr<Chunk> c = terrainManager.getChunk(chunkId);
		shared_ptr<Block> b = terrainManager.getBlock(posB);
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

		imgui::Text("Tick Time");
		imgui::Text("  Render");
		Time values[] = {appRenderTime, runImGuiTime, imGuiRenderTime, imGuiUpdateTime};
		string names[] = {"App->OnRender() %6dMs", "App->RunImGui() %6dMs", "ImGui::Render() %6dMs", "ImGui::Update() %6dMs"};
		for (int i = 0; i < 4; i++) {
			imgui::Text(names[i].c_str(), values[i].asMicroseconds());
			if (values[i] != Time::Zero) {
				imgui::SameLine();
				imgui::ProgressBar(1.0f, ImVec2(values[i].asMicroseconds() / 10.0f, 13.0f), "");
			}
		}
		imgui::Text("App->UpdateLogic() %3dMs", logicThreadTickTime.asMicroseconds());
		if (logicThreadTickTime != Time::Zero) {
			imgui::SameLine();
			imgui::ProgressBar(1.0f, ImVec2(logicThreadTickTime.asMicroseconds() / 10.0f, 13.0f), "");
		}
		LEAVE_IMGUI_DEBUG;
	}

	uiManager.runImGui();

	playerInventory.runImGui();
}


////////////////////////////////////////
void TestScene::stop() {

}


////////////////////////////////////////
