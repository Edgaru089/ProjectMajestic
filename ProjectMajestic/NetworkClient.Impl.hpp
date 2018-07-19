#pragma once

#include "NetworkClient.hpp"


////////////////////////////////////////
bool NetworkClient::connect(IpAddress target, Uint16 port) {
	Socket::Status status = handler.getSocket().connect(target, port, seconds(10.0f));
	if (status != Socket::Done || !handler.start(false))
		return false;
	else {
		role = Client;
		notifyEntityInsert(localPlayer->getUuid(), localPlayer);
		return true;
	}
}


////////////////////////////////////////
void NetworkClient::disconnect() {
	handler.disconnect();
}


////////////////////////////////////////
void NetworkClient::getChunkCount() {
	handler.getChunkCount();
}


////////////////////////////////////////
void NetworkClient::reloadAllChunks() {
	if (!handler.isConnected())
		return;
	for (int i = 0; i < terrainManager.getChunkCount().x; i++)
		for (int j = terrainManager.getChunkCount().y - 1; j >= 0; j--) {
			handler.getChunkData(Vector2i(i, j));
		}
	handler.getLightSources();
}


////////////////////////////////////////
void NetworkClient::downloadChunk(Vector2i chunkId) {
	if (!handler.isConnected())
		return;
	handler.getChunkData(chunkId);
}


////////////////////////////////////////
void NetworkClient::uploadChunk(Vector2i chunkId) {
	if (!handler.isConnected())
		return;
	handler.sendChunkData(chunkId);
}


////////////////////////////////////////
void NetworkClient::syncPlayerPos() {
	handler.sendLocalPlayerData();
}


////////////////////////////////////////
void NetworkClient::notifyParticleEmit(ParticleEmitTrace trace) {
	handler.sendParticleEmitData(trace);
}


////////////////////////////////////////
void NetworkClient::notifyEntityInsert(Uuid id, shared_ptr<Entity> entity) {
	handler.onInsertEntity(id, entity);
}


////////////////////////////////////////
void NetworkClient::notifyEntityKill(Uuid id) {
	handler.onRemoveEntity(id);
}


////////////////////////////////////////
void NetworkClient::notifyBlockSet(Vector2i coord, string blockId) {
	handler.onSetBlock(coord, blockId);
}


////////////////////////////////////////
void NetworkClient::notifyBlockBreak(Vector2i coord) {
	handler.onBreakBlock(coord);
}


////////////////////////////////////////
void NetworkClient::notifyBlockPlace(Vector2i coord, string blockId) {
	handler.onPlaceBlock(coord,blockId);
}

