#pragma once

#include "Main.hpp"
#include "NetworkHandler.hpp"


class NetworkClient {
public:

	bool connect(IpAddress target, Uint16 port);

	void disconnect();

	bool isConnected() { return handler.isConnected(); }

public:

	void getChunkCount();

	void reloadAllChunks();

	void downloadChunk(Vector2i chunkId);

	void uploadChunk(Vector2i chunkId);

public:

	void syncPlayerPos();

	void notifyParticleEmit(ParticleEmitTrace trace);

	void notifyEntityInsert(Uuid id, Entity* entity);

	void notifyEntityKill(Uuid id);

	void notifyBlockSet(Vector2i coord, string blockId);

	void notifyBlockBreak(Vector2i coord);

	void notifyBlockPlace(Vector2i coord, string blockId);

private:

	NetworkHandler handler;

};


NetworkClient networkClient;
