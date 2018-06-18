
#include "TextureManager.hpp"


////////////////////////////////////////
void TextureManager::addImage(string id, Image & image) {
	images[id] = image;
}


////////////////////////////////////////
void TextureManager::addImage(string id, string filename) {
	images[id].loadFromFile(filename);
}


////////////////////////////////////////
void TextureManager::bindTexture() {
	mlog << "[TextureManager] Binding texture..." << dlog;

	if (texture != nullptr)
		clear();

	int offX, offY, sizeX, sizeY;
	int maxHeight;

	offX = offY = sizeX = sizeY = maxHeight = 0;

	for (pair<const string, Image>& i : images) {

		// New line
		if (offX + i.second.getSize().x > maxImageWidth) {
			offX = 0;
			offY += maxHeight;
			maxHeight = 0;
		}

		rects.insert(pair<string, IntRect>(i.first, IntRect(offX, offY, i.second.getSize().x, i.second.getSize().y)));

		sizeX = max(sizeX, (int)(offX + i.second.getSize().x));
		sizeY = max(sizeY, (int)(offY + i.second.getSize().y));
		maxHeight = max(maxHeight, (int)(i.second.getSize().y));

		mlog << "                 Texture Id: " << i.first << ", Pos(" <<
			offX << ", " << offY << "), Size: (" << i.second.getSize().x << ", " <<
			i.second.getSize().y << ")" << dlog;

		offX += i.second.getSize().x;
	}

	Image* image = new Image();
	image->create(sizeX, sizeY, Color::Transparent);

	for (pair<const string, Image>& i : images) {
		const IntRect& rect = rects[i.first];
		for (int x = 0; x < rect.width; x++) {
			for (int y = 0; y < rect.height; y++) {
				image->setPixel(x + rect.left, y + rect.top, i.second.getPixel(x, y));
			}
		}
	}

	texture = new Texture();
	texture->loadFromImage(*image);

	mlog << "[TextureManager] Binding done." << dlog;
}


////////////////////////////////////////
TextureInfo TextureManager::getTextureInfo(string id) {
	if (texture == nullptr)
		return TextureInfo();
	else {
		auto i = rects.find(id);
		if (i != rects.end())
			return TextureInfo(texture, i->second, id);
		else
			return TextureInfo();
	}
}


////////////////////////////////////////
void TextureManager::clear() {
	if (texture != nullptr)
		delete texture;
	texture = nullptr;
	images.clear();
	rects.clear();
}


////////////////////////////////////////
Texture* TextureManager::getBindingTexture() {
	return texture;
}
