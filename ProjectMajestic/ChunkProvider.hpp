#pragma once

#include "Main.hpp"
#include "Chunk.hpp"
#include "Stone.hpp"
#include "TerrainManager.hpp"


class ChunkProvider {
public:

	void loadChunk(Vector2i chunkId, Chunk& chunk) {
		_loadChunk(chunkId, chunk);
	}

private:

	virtual void _loadChunk(Vector2i chunkId, Chunk& chunk) = 0;

};


class EmptyChunkProvider :public ChunkProvider {
public:
private:

	void _loadChunk(Vector2i chunkId, Chunk& chunk) override;

};


class ChunkProviderWrapper :public ChunkProvider {
public:

	ChunkProviderWrapper(function<void(Vector2i, Chunk&)> loader) :loader(loader) {}

private:

	void _loadChunk(Vector2i chunkId, Chunk& chunk) override {
		loader(chunkId, chunk);
	}

	function<void(Vector2i, Chunk&)> loader;

};
