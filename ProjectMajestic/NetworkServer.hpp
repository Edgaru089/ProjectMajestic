#pragma once

#include "Main.hpp"
#include "NetworkHandler.hpp"


class NetworkServer {
public:

	void startListen(Uint16 port);

	void stopServer();

	bool isListening() { return running; }

public:

	void notifyBlockChange(Vector2i coord);

	void notifyParitlcEmit(ParticleEmitTrace trace);

	void notifyEntityInsert(Uuid id, shared_ptr<Entity> entity);

	void notifyEntityKill(Uuid id);

	void notifyBlockSet(Vector2i coord, string id);

	void notifyBlockBreak(Vector2i coord);

	void notifyBlockPlace(Vector2i coord, string id);

private:

	void _listenThread();
	void _maintainThread();
	void _syncThread();

	thread* listen, *maintain, *sync;

	Uint16 port;
	atomic_bool running;

	list<NetworkHandler*> handlers;
	mutex handlerLock;

};


NetworkServer networkServer;
