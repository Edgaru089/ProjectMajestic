
#include "App.hpp"

#include "LogSystem.hpp"

#include "Scene.hpp"

#include "GameScene.hpp"
#include "TestScene.hpp"
#include "IO.hpp"

#include "TextSystem.hpp"


////////////////////////////////////////
void App::initalaizePostWindow(RenderWindow& win) {
	for (pair<const string, Scene*>& i : sceneMapper)
		i.second->postWindowInitalaize(win);

	currentScene->start(win);
	logicDeltaClock.restart();
}


////////////////////////////////////////
void App::initalaize() {

	sceneMapper["GameScene"] = new GameScene();
	sceneMapper["TestScene"] = new TestScene();

	for (pair<const string, Scene*>& i : sceneMapper)
		i.second->preWindowInitalaize();

	currentScene = sceneMapper["TestScene"];

	text.loadFromFile("lang.list");

	hasLog = false;
}


////////////////////////////////////////
void App::onRender(RenderWindow& win) {

	currentScene->onRender(win);

}


////////////////////////////////////////
void App::runImGui() {

	//////////////////// Log Window ////////////////////
	if (hasLog) {
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		if (imgui::Begin("Logs", NULL, ImGuiWindowFlags_MenuBar)) {
			//////// Menu Bar ////////
			static bool follow = true;
			if (imgui::BeginMenuBar()) {
				if (imgui::BeginMenu("Controls")) {
					if (imgui::MenuItem("Clear"))
						dlog.clearBuffer();
					imgui::Separator();
					imgui::MenuItem("Follow the end of log     ", NULL, &follow);
					imgui::EndMenu();
				}
				imgui::EndMenuBar();
			}

			//////// Text Area ////////
			imgui::BeginChild("DigitLogScroll", Vector2i(0, 0), true);
			static float size;
			string total;
			for (const string& i : dlog.getBuffers())
				total += (i + '\n');
			imgui::TextUnformatted(total.c_str());
			if (size != imgui::GetScrollMaxY() && follow)
				imgui::SetScrollY(imgui::GetScrollMaxY());
			size = imgui::GetScrollMaxY();
			imgui::EndChild();
		}
		imgui::End();
	}

	currentScene->runImGui();
}


////////////////////////////////////////
void App::updateLogic(RenderWindow& win) {

	logicIO.deltaTime = logicDeltaClock.restart();

	// Update key states
	// Keyboard
	for (int i = 0; i < Keyboard::KeyCount; i++) {
		bool state = Keyboard::isKeyPressed((Keyboard::Key)i);
		if (state) {
			if (logicIO.keyboardState[i] == LogicIO::Released || logicIO.keyboardState[i] == LogicIO::JustReleased)
				logicIO.keyboardState[i] = LogicIO::JustPressed;
			else
				logicIO.keyboardState[i] = LogicIO::Pressed;
		}
		else {
			if (logicIO.keyboardState[i] == LogicIO::Pressed || logicIO.keyboardState[i] == LogicIO::JustPressed)
				logicIO.keyboardState[i] = LogicIO::JustReleased;
			else
				logicIO.keyboardState[i] = LogicIO::Released;
		}
	}
	// Mouse
	for (int i = 0; i < Mouse::ButtonCount; i++) {
		bool state = Mouse::isButtonPressed((Mouse::Button)i);
		if (state) {
			if (logicIO.mouseState[i] == LogicIO::Released || logicIO.mouseState[i] == LogicIO::JustReleased)
				logicIO.mouseState[i] = LogicIO::JustPressed;
			else
				logicIO.mouseState[i] = LogicIO::Pressed;
		}
		else {
			if (logicIO.mouseState[i] == LogicIO::Pressed || logicIO.mouseState[i] == LogicIO::JustPressed)
				logicIO.mouseState[i] = LogicIO::JustReleased;
			else
				logicIO.mouseState[i] = LogicIO::Released;
		}
	}

	currentScene->updateLogic(win);

	Scene::updateScene(win);
}


////////////////////////////////////////
void App::handleEvent(RenderWindow& win, Event& e) {
	currentScene->handleEvent(win, e);
}


////////////////////////////////////////
void App::onViewportChange(RenderWindow& win) {

}


////////////////////////////////////////
void App::onStop() {
	currentScene->stop();
}


