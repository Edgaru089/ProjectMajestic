#pragma once

#include "NetworkServer.hpp"


////////////////////////////////////////
void NetworkServer::startListen(Uint16 port) {
	if (running)
		return;

	this->port = port;
	running = true;
	handlers.clear();

	role = Server;

	listen = new thread(&NetworkServer::_listenThread, this);
	maintain = new thread(&NetworkServer::_maintainThread, this);
	sync = new thread(&NetworkServer::_syncThread, this);
}


////////////////////////////////////////
void NetworkServer::stopServer() {
	if (!running)
		return;

	running = false;

	listen->join();
	maintain->join();
	sync->join();
	delete listen;
	delete maintain;
	delete sync;
	listen = nullptr;
	maintain = nullptr;
	sync = nullptr;

	for (auto i : handlers) {
		i->disconnect();
		delete i;
	}
	handlers.clear();
}


////////////////////////////////////////
void NetworkServer::notifyBlockChange(Vector2i coord) {
	AUTOLOCK(handlerLock);
	for (auto handler : handlers) {
		handler->sendBlockData(coord);
	}
}


////////////////////////////////////////
void NetworkServer::notifyParitlcEmit(ParticleEmitTrace trace) {
	AUTOLOCK(handlerLock);
	for (auto handler : handlers) {
		handler->sendParticleEmitData(trace);
	}
}


////////////////////////////////////////
void NetworkServer::notifyEntityInsert(Uuid id, shared_ptr<Entity> entity) {
	AUTOLOCK(handlerLock);
	for (auto handler : handlers) {
		handler->onInsertEntity(id, entity);
	}
}


////////////////////////////////////////
void NetworkServer::notifyEntityKill(Uuid id) {
	AUTOLOCK(handlerLock);
	for (auto handler : handlers) {
		handler->onRemoveEntity(id);
	}
}


////////////////////////////////////////
void NetworkServer::notifyBlockSet(Vector2i coord, string id) {
	AUTOLOCK(handlerLock);
	for (auto handler : handlers) {
		handler->onSetBlock(coord, id);
	}
}


////////////////////////////////////////
void NetworkServer::notifyBlockPlace(Vector2i coord, string id) {
	AUTOLOCK(handlerLock);
	for (auto handler : handlers) {
		handler->onPlaceBlock(coord, id);
	}
}


////////////////////////////////////////
void NetworkServer::_listenThread() {
	NetworkHandler* handler;
	TcpListener listener;

	listener.listen(port);
	listener.setBlocking(false);

	handler = new NetworkHandler;
	while (running) {
		if (listener.accept(handler->getSocket()) == Socket::Done) {
			if (handler->start(false)) {
				AUTOLOCK(handlerLock);
				handlers.push_back(handler);
				handler = new NetworkHandler;
			}
		}
		else
			sleep(milliseconds(50));
	}

	delete handler;
	listener.close();

}


////////////////////////////////////////
void NetworkServer::_maintainThread() {
	while (running) {
		handlerLock.lock();
		for (auto i = handlers.begin(); i != handlers.end();) {
			if (!(*i)->isConnected()) {
				delete (*i);
				i = handlers.erase(i);
			}
			else
				i++;
		}
		handlerLock.unlock();
		sleep(milliseconds(200));
	}
}


////////////////////////////////////////
void NetworkServer::_syncThread() {
	Clock cl;
	while (running) {
		handlerLock.lock();
		for (auto i : handlers) {
			i->sendEntityData();
		}
		handlerLock.unlock();
		Time t = cl.restart();
		if (t < microseconds(16666))
			sleep(microseconds(16666) - t);
	}
}

