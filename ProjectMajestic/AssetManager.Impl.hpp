
#include "AssetManager.hpp"


////////////////////////////////////////
bool AssetManager::loadListFile(string filename) {
	mlog << "[AssetManager] Loading asset list file: " << filename << dlog;

	ifstream fin(filename);
	if (fin.bad()) {
		mlog << Log::Error << "[AssetManager] File failed to open." << dlog;
		return false;
	}

	string str;
	string assetType;

	while (getline(fin, str)) {
		char type = str[0];
		if (type == '#') // Comment line
			continue;
		else if (type == '[') { // Set asset type
			size_t end = str.find_first_of(']');
			assetType = str.substr(1, end - 1);
		}
		else if (type == '$') { // Asset line
			string id, file;
			size_t i = 1;

			// Find first non-blank character
			while (str[i] == ' ' || str[i] == '\t')
				i++;

			// Read the asset id
			while (!isspace(str[i])) {
				id += str[i];
				i++;
			}

			// Find first non-blank character
			while (str[i] == ' ' || str[i] == '\t')
				i++;

			// Read the filename
			while (i < str.size() && !isspace(str[i])) {
				file += str[i];
				i++;
			}

			mlog << "               Loaded " << assetType << " asset: " << id << "@" << file << dlog;

			assets.insert_or_assign(id, Asset(id, file, assetType));
		}
	}

	// Load Texture Assets
	for (auto& i : assets) {
		if (i.second.type == "TEXTURE")
			textureManager.addImage(i.second.strid, i.second.filename);
	}

	return true;

}


////////////////////////////////////////
string AssetManager::getAssetFilename(string id) {
	auto p = assets.find(id);
	if (p != assets.end())
		return p->second.filename;
	else
		return "";
}


////////////////////////////////////////



////////////////////////////////////////



////////////////////////////////////////


