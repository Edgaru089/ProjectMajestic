#pragma once

#include "Main.hpp"

// Handled by game scene (GameScene or TestScene)
struct RenderIO {

	// Size of a block, in pixels
	double gameScaleFactor;

	// Logical on-screen position of point (0, 0)
	// Handled by game scene
	Vector2d gameScenePosOffset;

	// Rectangle covering current rendering area
	// Handled by game scene
	FloatRect viewRect;
	DoubleRect viewRectBlock;

};

// Handled by scene manager ( App class )
struct LogicIO {

	enum KeyState {
		Released,
		JustPressed,
		Pressed,
		JustReleased
	};

	// Elapsed time before last logic update
	Time deltaTime;

	// Error message returned by last failed operation
	string lastError;

	// State of all the keys (pressed or released) in the last frame
	// Set by SFML coding (Keyboard::Key or Mouse::Button)
	KeyState keyboardState[Keyboard::KeyCount];
	KeyState mouseState[Mouse::ButtonCount];

};

// Handled by game scene (GameScene or TestScene)
struct GameIO {

	// Offset angle from the X-axis to the line between the player and the cursor
	// in clockwise pattern
	double degreeAngle;

};

// Enum representing the networking role of the local game
enum NetworkRole {
	Server,  // Server / Local Game (logic processor)
	Client   // Client (sends commands to the server for processing)
};

RenderIO renderIO;
LogicIO logicIO;
GameIO gameIO;

NetworkRole role;
