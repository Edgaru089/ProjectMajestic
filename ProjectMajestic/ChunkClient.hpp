#pragma once

#include "Main.hpp"
#include "Chunk.hpp"

class ChunkClient {
public:

	static void parseChunkData(Packet& packet);

	static void compressChunkData(Vector2i chunkId, Packet& packet);

	static void parseBlockData(Packet& packet);

	static void compressBlockData(Vector2i coord, Packet& packet);

private:

};

