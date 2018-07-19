
#include "ChunkProvider.hpp"


////////////////////////////////////////
void EmptyChunkProvider::_loadChunk(Vector2i chunkId, Chunk& chunk) {
	chunk.id = chunkId;

	chunk._resize(chunkSize, chunkSize);

	for (int i = 0; i < chunkSize; i++)
		for (int j = 0; j < chunkSize; j++) {
			chunk.blocks[i][j] = make_shared<Stone>();
			chunk.blocks[i][j]->setInChunkPosition(i, j);
			chunk.blocks[i][j]->setChunkId(chunkId);
		}

}

