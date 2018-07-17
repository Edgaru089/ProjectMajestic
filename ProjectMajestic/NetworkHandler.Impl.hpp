#pragma once

#include "NetworkHandler.hpp"
#include "ChunkClient.hpp"
#include "IO.hpp"
#include "TerrainProvider.hpp"
#include "ParticleSystem.hpp"
#include "EntityAllocator.hpp"


////////////////////////////////////////
bool NetworkHandler::start(bool client) {

	Packet packet;
	string welcome;
	Socket::Status send, receive;

	mlog << "[Network/Handshake] Peer: " << socket.getRemoteAddress().toString() << ":" << socket.getRemotePort() << dlog;

	packet << "Hello"s << majorVersion << minorVersion << patchVersion << chunkSize;
	send = socket.send(packet); packet.clear();
	receive = socket.receive(packet);

	if (send != Socket::Done || receive != Socket::Done) {
		logicIO.lastError = "Socket Disconnected";
		return false;
	}

	packet >> welcome >> peer.majorVersion >> peer.minorVersion >> peer.patchVersion >> peer.chunkSize;

	if (peer.majorVersion != majorVersion || peer.minorVersion != minorVersion) {
		logicIO.lastError = StringParser::toStringFormatted("Client/Server Version Mismatch, Local: %d.%d.%d, Remote: %d.%d.%d",
															majorVersion, minorVersion, patchVersion, peer.majorVersion, peer.minorVersion, peer.patchVersion);
		mlog << Log::Error << "[Network/Handshake Error] " << logicIO.lastError << dlog;
		socket.disconnect();
		return false;
	}

	if (peer.chunkSize != chunkSize) {
		logicIO.lastError = StringParser::toStringFormatted("Client/Server Chunk Size Mismatch, Local: %d, Remote: %d",
															chunkSize, peer.chunkSize);
		mlog << Log::Error << "[Network/Handshake Error] " << logicIO.lastError << dlog;
		socket.disconnect();
		return false;
	}

	if (client) {
		mlog << "[Network] Acquiring chunk count && spawnpoints" << dlog;
		packet.clear();
		packet << "GETCHUNKCNT";
		socket.send(packet); packet.clear();
		socket.receive(packet);

		string command;
		Vector2i chunkCnt;
		packet >> command >> chunkCnt.x >> chunkCnt.y;
		terrainManager.setChunkCount(chunkCnt);
		mlog << "          Acquired: (" << chunkCnt.x << ", " << chunkCnt.y << ")" << dlog;

		packet.clear();
		packet << "GETSPAWN";
		socket.send(packet); packet.clear();
		socket.receive(packet);

		size_t size;
		packet >> command >> size;
		for (int i = 1; i <= size; i++) {
			Vector2i pos;
			packet >> pos.x >> pos.y;
			terrainManager.getSpawnpoints().push_back(pos);
		}
		terrainManager.getSpawnpoints().shrink_to_fit();

		mlog << "          Acquired " << size << " spawnpoints." << dlog;
	}

	mlog << "[Network/Handshake] Handshake Success" << dlog;

	connected = true;

	launchThread();

	return true;
}


////////////////////////////////////////
void NetworkHandler::launchThread() {
	if (messageHandler == nullptr)
		messageHandler = new thread(&NetworkHandler::_messageLoopThread, this);
}

void NetworkHandler::disconnect() {
	if (!connected)
		return;
	socket.disconnect();
	connected = false;
	if (messageHandler != nullptr) {
		messageHandler->join();
		delete messageHandler;
		messageHandler = nullptr;
	}
}


#define BEGIN_IF_COMMAND(str) if (command == str) 
#define IF_COMMAND(str) else if (command == str)

#define CHECK_CONNECT \
if(!connected)\
return

#define PACKET_COMMAND(packetName, command) \
Packet packetName; \
packetName << command

#define CHECKED_SEND(packetName)\
socketLock.lock();\
Socket::Status stat = socket.send(pack);\
socketLock.unlock();\
if (stat == Socket::Disconnected)\
connected = false

#define CHECKED_SEND_COMMAND(command)\
PACKET_COMMAND(pack,command);\
CHECKED_SEND(pack)

////////////////////////////////////////
void NetworkHandler::_messageLoopThread() {
	Packet pack;
	Socket::Status stat;

	IpAddress ip = socket.getRemoteAddress();
	Uint16 port = socket.getRemotePort();

	while (connected && (stat = socket.receive(pack)) != Socket::Disconnected) {
		if (stat != Socket::Done)
			continue;

		string command;
		pack >> command;

		// PART I --- UNDERLYING API
		BEGIN_IF_COMMAND("GETCHUNKCNT") {
			Packet ret;
			ret << "CHUNKCNT" << terrainManager.getChunkCount().x << terrainManager.getChunkCount().y;
			socket.send(ret);
		}
		IF_COMMAND("CHUNKCNT") {
			Vector2i cnt;
			pack >> cnt.x >> cnt.y;
			terrainManager.setChunkCount(cnt);
		}
		IF_COMMAND("GETPLAYER") {
			PACKET_COMMAND(ret, "PLAYER");
			if (localPlayer != nullptr)
				ret << localPlayer->getUuid() << localPlayer->getPlayerName();
			else
				ret << Uuid::nil() << ""s;
			CHECKED_SEND(ret);
		}
		IF_COMMAND("PLAYER") {
			pack >> peer.peerUuid >> peer.peerPlayername;
		}
		IF_COMMAND("GETSPAWN") {
			PACKET_COMMAND(ret, "SPAWN");
			terrainManager.lock();
			ret << terrainManager.getSpawnpoints().size();
			for (auto& i : terrainManager.getSpawnpoints())
				ret << i.x << i.y;
			terrainManager.unlock();
			CHECKED_SEND(ret);
		}
		IF_COMMAND("SPAWN") {
			size_t size;
			pack >> size;
			terrainManager.lock();
			for (int i = 1; i <= size; i++) {
				Vector2i pos;
				pack >> pos.x >> pos.y;
				terrainManager.getSpawnpoints().push_back(pos);
			}
			terrainManager.getSpawnpoints().shrink_to_fit();
			terrainManager.unlock();
		}
		IF_COMMAND("GETCHUNK") {
			Vector2i chunkId;
			pack >> chunkId.x >> chunkId.y;
			sendChunkData(chunkId);
		}
		IF_COMMAND("CHUNKDATA") {
			ChunkClient::parseChunkData(pack);
		}
		IF_COMMAND("GETBLOCK") {
			Vector2i coord;
			pack >> coord.x >> coord.y;
			sendBlockData(coord);
		}
		IF_COMMAND("BLOCKDATA") {
			ChunkClient::parseBlockData(pack);
		}

		// PART II --- CLIENT CONTROLS
		IF_COMMAND("PARTICLE") {
			// Does the same thing; it is left for ParticleSystem::emit to tell whether to
			// present locally or to send across the network
			ParticleEmitTrace log;
			pack >> log;
			AUTOLOCKABLE(particleSystem);
			particleSystem.emitTraced(log);
		}
		IF_COMMAND("SETBLOCK") {
			// Same as above; just call
			Vector2i coord;
			string id;
			pack >> coord.x >> coord.y >> id;
			AUTOLOCKABLE(terrainManager);
			terrainManager.setBlock(coord, id, true);
		}
		IF_COMMAND("BREAKBLOCK") {
			// Same as above; just call
			Vector2i coord;
			pack >> coord.x >> coord.y;
			AUTOLOCKABLE(terrainManager);
			terrainManager.breakBlock(coord, nullptr, true);
		}
		IF_COMMAND("PLACEBLOCK") {
			// Same as above; just call
			Vector2i coord;
			string id;
			pack >> coord.x >> coord.y >> id;
			AUTOLOCKABLE(terrainManager);
			terrainManager.placeBlock(coord, id, nullptr, true);
		}
		IF_COMMAND("INSENTITY") {
			Uuid id;
			string str;
			Vector2d pos, vel;
			pack >> id >> str;
			pack >> pos.x >> pos.y >> vel.x >> vel.y;
			Entity* e = entityAllocator.allocate(str);
			if (e != nullptr) {
				e->setUuid(id);
				e->setPosition(pos);
				e->setVelocity(vel);
				pack >> e->getDataset();
				AUTOLOCKABLE(entityManager);
				entityManager.insert(id, e);
			}
		}
		IF_COMMAND("DELENTITY") {
			Uuid id;
			pack >> id;
			entityManager.lock();
			Entity* e = entityManager.getEntity(id);
			if (e != nullptr)
				e->kill();
			entityManager.unlock();
		}
		IF_COMMAND("ENTITY") {
			vector<Uuid> ids;
			vector<string> eids;
			vector<Vector2d> pos, vec;
			vector<Dataset> datas;
			size_t cnt;

			pack >> cnt;
			ids.resize(cnt); eids.resize(cnt); pos.resize(cnt); vec.resize(cnt); datas.resize(cnt);
			for (int i = 0; i < cnt; i++)
				pack >> ids[i] >> eids[i] >> pos[i].x >> pos[i].y >> vec[i].x >> vec[i].y >> datas[i];

			entityManager.lock();
			for (int i = 0; i < cnt; i++) {
				Entity* e = entityManager.getEntity(ids[i]);
				if (e == nullptr) {
					// Allocate a new entity
					Entity* ex = entityAllocator.allocate(eids[i]);
					if (ex != nullptr) {
						ex->setUuid(ids[i]);
						ex->setPosition(pos[i]);
						ex->setVelocity(vec[i]);
						ex->getDataset() = datas[i];
						ex->setData("remote", true);
						entityManager.insert(ids[i], ex);
					}
				}
				else {
					e->setPosition(pos[i]);
					e->setVelocity(vec[i]);
					e->getDataset() = datas[i];
					e->setData("remote", true);
				}
			}
			entityManager.unlock();
		}

	}
	dlog.logf(Log::Info, "[NetworkHandler] Peer %s:%d disconnected.", ip.toString().c_str(), port);
	connected = false;
	role = Server; // HACK When disconected, always set role to server
}


////////////////////////////////////////
void NetworkHandler::getChunkCount() {
	CHECK_CONNECT;
	CHECKED_SEND_COMMAND("GETCHUNKCNT");
}


////////////////////////////////////////
void NetworkHandler::getSpawnpoints() {
	CHECK_CONNECT;
	CHECKED_SEND_COMMAND("GETSPAWN");
}


////////////////////////////////////////
void NetworkHandler::getLightSources() {
	mlog << "[NetworkHandler] Requesting light sources" << dlog;
	CHECK_CONNECT;
	CHECKED_SEND_COMMAND("GETLIGHTS");
}


////////////////////////////////////////
void NetworkHandler::getChunkData(Vector2i chunkId) {
	CHECK_CONNECT;
	PACKET_COMMAND(pack, "GETCHUNK");
	pack << chunkId.x << chunkId.y;
	CHECKED_SEND(pack);
}


////////////////////////////////////////
void NetworkHandler::sendChunkData(Vector2i chunkId) {
	CHECK_CONNECT;
	PACKET_COMMAND(pack, "CHUNKDATA");
	ChunkClient::compressChunkData(chunkId, pack);
	CHECKED_SEND(pack);
}


////////////////////////////////////////
void NetworkHandler::sendBlockData(Vector2i coord) {
	CHECK_CONNECT;
	PACKET_COMMAND(pack, "BLOCKDATA");
	ChunkClient::compressBlockData(coord, pack);
	CHECKED_SEND(pack);
}


////////////////////////////////////////
void NetworkHandler::sendParticleEmitData(ParticleEmitTrace & trace) {
	CHECK_CONNECT;
	PACKET_COMMAND(pack, "PARTICLE");
	pack << trace;
	CHECKED_SEND(pack);
}


////////////////////////////////////////
void NetworkHandler::sendEntityData() {
	CHECK_CONNECT;
	PACKET_COMMAND(pack, "ENTITY");

	entityManager.lock();

	auto& m = entityManager.getEntityMapList();
	pack << m.size();

	for (auto& i : m) {
		pack << i.first;
		pack << i.second->getEntityId();
		pack << i.second->getPosition().x << i.second->getPosition().y;
		pack << i.second->getVelocity().x << i.second->getVelocity().y;
		pack << i.second->getDataset();
	}

	entityManager.unlock();

	CHECKED_SEND(pack);
}


////////////////////////////////////////
void NetworkHandler::sendLocalPlayerData() {
	if (localPlayer != nullptr) {
		CHECK_CONNECT;
		PACKET_COMMAND(pack, "ENTITY");

		entityManager.lock();

		pack << 1;
		pack << localPlayer->getUuid();
		pack << localPlayer->getEntityId();
		pack << localPlayer->getPosition().x << localPlayer->getPosition().y;
		pack << localPlayer->getVelocity().x << localPlayer->getVelocity().y;
		pack << localPlayer->getDataset();

		entityManager.unlock();

		CHECKED_SEND(pack);
	}
}


////////////////////////////////////////
void NetworkHandler::onInsertEntity(Uuid id, Entity* entity) {
	CHECK_CONNECT;
	PACKET_COMMAND(pack, "INSENTITY");

	entityManager.lock();

	pack << id << entity->getEntityId();
	pack << entity->getPosition().x << entity->getPosition().y;
	pack << entity->getVelocity().x << entity->getVelocity().y;
	pack << entity->getDataset();

	entityManager.unlock();

	CHECKED_SEND(pack);
}


////////////////////////////////////////
void NetworkHandler::onRemoveEntity(Uuid id) {
	CHECK_CONNECT;
	PACKET_COMMAND(pack, "DELENTITY");
	pack << id;
	CHECKED_SEND(pack);
}


////////////////////////////////////////
void NetworkHandler::onSetBlock(Vector2i coord, string blockId) {
	CHECK_CONNECT;
	PACKET_COMMAND(pack, "SETBLOCK");
	pack << coord.x << coord.y << blockId;
	CHECKED_SEND(pack);
}


////////////////////////////////////////
void NetworkHandler::onBreakBlock(Vector2i coord) {
	CHECK_CONNECT;
	PACKET_COMMAND(pack, "BREAKBLOCK");
	pack << coord.x << coord.y;
	CHECKED_SEND(pack);
}


////////////////////////////////////////
void NetworkHandler::onPlaceBlock(Vector2i coord, string blockId) {
	CHECK_CONNECT;
	PACKET_COMMAND(pack, "PLACEBLOCK");
	pack << coord.x << coord.y << blockId;
	CHECKED_SEND(pack);
}

