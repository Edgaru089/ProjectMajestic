#pragma once

#include "Main.hpp"
#include "TextureManager.hpp"


class AssetManager {
public:

	struct Asset {
		Asset() {}
		Asset(string id, string filename, string type) :
			strid(id), filename(filename), type(type) {}

		string strid, filename, type;
	};

	bool loadListFile(string filename = "assets.list");

	string getAssetFilename(string id);

private:

	map<string, Asset> assets;

};

AssetManager assetManager;
