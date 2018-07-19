#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;

class App {

public:

	void initalaize();

	void initalaizePostWindow(RenderWindow& win);

	void onRender(sf::RenderWindow& win);

	void runImGui();

	void updateLogic(sf::RenderWindow& win);

	void handleEvent(sf::RenderWindow& win, sf::Event& e);

	void onViewportChange(RenderWindow& win);

	void onStop();

	bool hasLog;
private:

	Clock logicDeltaClock;

};

