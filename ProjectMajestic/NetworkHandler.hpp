#pragma once

#include "Main.hpp"


struct ParticleEmitTrace;


class NetworkHandler : public Lockable {
public:

	NetworkHandler() :messageHandler(nullptr) {}
	~NetworkHandler() { disconnect(); }

	struct PeerInfo {
		int majorVersion, minorVersion, patchVersion, chunkSize;
		Vector2i chunkCount;
		string peerPlayername;
		Uuid peerUuid;
	};

	TcpSocket & getSocket() { return socket; }

	bool start(bool getChunkCount = false);

	void launchThread();

	bool isConnected() { return connected; }

	void disconnect();

	PeerInfo getPeerInfo() { return peer; }

public:

	void getChunkCount();

	void getSpawnpoints();

	void getLightSources();

	void getChunkData(Vector2i chunkId);

	void sendChunkData(Vector2i chunkId);

	void sendBlockData(Vector2i coord);

	void sendParticleEmitData(ParticleEmitTrace& trace);

	void sendEntityData();

	void sendLocalPlayerData();

	void onInsertEntity(Uuid id, Entity * entity);

	void onRemoveEntity(Uuid id);

	void onSetBlock(Vector2i coord, string blockId);

	void onBreakBlock(Vector2i coord);

	void onPlaceBlock(Vector2i coord, string blockId);

private:

	void _messageLoopThread();


	TcpSocket socket;
	atomic_bool connected;
	recursive_mutex socketLock;

	PeerInfo peer;

	thread* messageHandler;

};

