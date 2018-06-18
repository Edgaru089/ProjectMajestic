#pragma once

#include "ParticleSystem.hpp"


////////////////////////////////////////
Particle::Particle(TextureInfo texture, double sizeDivisor, Time liveTime) {
	wholeText = texture;

	Vector2i size = Vector2i(Vector2d(texture.textureRect.width, texture.textureRect.height) / sizeDivisor);
	int posX, posY;

	posX = rand() % (texture.textureRect.width - size.x) + texture.textureRect.left;
	posY = rand() % (texture.textureRect.height - size.y) + texture.textureRect.top;

	textureSubRect = IntRect(posX, posY, size.x, size.y);

	this->liveTime = liveTime;
	liveClock.restart();

	alive = true;
}


////////////////////////////////////////
void Particle::_updateLogic() {
	if (liveClock.getElapsedTime() > liveTime)
		kill();
}


////////////////////////////////////////
void PartlcleSystem::updateLogic() {
	for (auto i = parts.begin(); i != parts.end();) {
		if (!i->isAlive())
			i = parts.erase(i);
		else {
			i->updateLogic();
			i++;
		}
	}
}


////////////////////////////////////////
void PartlcleSystem::getRenderList(VertexArray& verts) {
	for (auto e = parts.begin(); e != parts.end();) {
		if (!e->isAlive())
			e = parts.erase(e);
		else {
			// Borrow EntityManager's code
			if (e->getHitbox().intersects(renderIO.viewRectBlock)) {
				TextureInfo tex = e->getTextureInfo();
				Vector2d center = e->getPosition()*renderIO.gameScaleFactor;
				double width = e->getSize().x*renderIO.gameScaleFactor, height = e->getSize().y*renderIO.gameScaleFactor;

				Vector2i chunk = TerrainManager::convertWorldCoordToChunkId(Vector2i(e->getPosition().x, e->getPosition().y));
				Vector2i inCc = TerrainManager::convertWorldCoordToInChunkCoord(Vector2i(e->getPosition().x, e->getPosition().y));
				Chunk* c = terrainManager.getChunk(chunk);

				Uint8 mask;
				if (c != nullptr)
					mask = 63 + 192.0*((double)c->lightLevel[inCc.x][inCc.y] / maxLightingLevel);
				else
					mask = 63;

				// Left-Top
				verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - height),
					Color(mask, mask, mask),
					Vector2f(tex.textureRect.left, tex.textureRect.top)
				));
				// Right-Top
				verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - height),
					Color(mask, mask, mask),
					Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top)
				));
				// Left-Bottom
				verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y),
					Color(mask, mask, mask),
					Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)
				));
				// Right-Top
				verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - height),
					Color(mask, mask, mask),
					Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top)
				));
				// Left-Bottom
				verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y),
					Color(mask, mask, mask),
					Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)
				));
				// Right-Bottom
				verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y),
					Color(mask, mask, mask),
					Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top + tex.textureRect.height)
				));
			}
			e++;
		}
	}
}


////////////////////////////////////////
void PartlcleSystem::emit(Vector2d position, TextureInfo texture, double sizeDivisor, double speed, int count, Time liveTimeBegin, Time liveTimeEnd, bool isForced) {
	if (!isForced || role == Server) { // Networked
		networkServer.notifyParitlcEmit(ParticleEmitTrace(position, texture.id, sizeDivisor,
			speed, count, 0, 360, liveTimeBegin, liveTimeEnd));
		networkClient.notifyParticleEmit(ParticleEmitTrace(position, texture.id, sizeDivisor,
			speed, count, 0, 360, liveTimeBegin, liveTimeEnd));
	}
	if (isForced || role == Server) { // Present locally
		if (liveTimeBegin > liveTimeEnd)
			swap(liveTimeBegin, liveTimeEnd);
		for (int i = 0; i < count; i++) {
			Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
			double angle = 360.0*rand01();

			Particle p(texture, sizeDivisor, live);
			p.setPosition(position);
			p.accelerateVector(speed, angle);

			parts.push_back(p);
		}
	}
}


////////////////////////////////////////
void PartlcleSystem::emit(DoubleRect range, TextureInfo texture, double sizeDivisor, double speed, int count, Time liveTimeBegin, Time liveTimeEnd, bool isForced) {
	if (!isForced || role == Server) { // Networked
		networkServer.notifyParitlcEmit(ParticleEmitTrace(range, texture.id, sizeDivisor,
			speed, count, 0, 360, liveTimeBegin, liveTimeEnd));
		networkClient.notifyParticleEmit(ParticleEmitTrace(range, texture.id, sizeDivisor,
			speed, count, 0, 360, liveTimeBegin, liveTimeEnd));
	}
	if (isForced || role == Server) { // Present locally
		if (liveTimeBegin > liveTimeEnd)
			swap(liveTimeBegin, liveTimeEnd);
		for (int i = 0; i < count; i++) {
			Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
			double angle = 360.0*rand01();
			Vector2d pos;

			pos.x = range.left + range.width*rand01();
			pos.y = range.top + range.height*rand01();

			Particle p(texture, sizeDivisor, live);
			p.setPosition(pos);
			p.accelerateVector(speed, angle);

			parts.push_back(p);
		}
	}
}


////////////////////////////////////////
void PartlcleSystem::emit(Vector2d position, TextureInfo texture, double sizeDivisor, double speed, int count, double angleBegin, double angleEnd, Time liveTimeBegin, Time liveTimeEnd, bool isForced) {
	if (!isForced || role == Server) { // Networked
		networkClient.notifyParticleEmit(ParticleEmitTrace(position, texture.id, sizeDivisor, speed, count,
			angleBegin, angleEnd, liveTimeBegin, liveTimeEnd));
		networkServer.notifyParitlcEmit(ParticleEmitTrace(position, texture.id, sizeDivisor, speed, count,
			angleBegin, angleEnd, liveTimeBegin, liveTimeEnd));
	}
	if (isForced || role == Server) { // Present locally
		if (liveTimeBegin > liveTimeEnd)
			swap(liveTimeBegin, liveTimeEnd);
		if (angleBegin > angleEnd)
			swap(angleBegin, angleEnd);
		for (int i = 0; i < count; i++) {
			Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
			double angle = angleBegin + (angleEnd - angleBegin)*rand01();

			Particle p(texture, sizeDivisor, live);
			p.setPosition(position);
			p.accelerateVector(speed, angle);

			parts.push_back(p);
		}
	}
}


////////////////////////////////////////
void PartlcleSystem::emit(DoubleRect range, TextureInfo texture, double sizeDivisor, double speed, int count, double angleBegin, double angleEnd, Time liveTimeBegin, Time liveTimeEnd, bool isForced) {
	if (!isForced || role == Server) { // Networked
		networkServer.notifyParitlcEmit(ParticleEmitTrace(range, texture.id, sizeDivisor, speed, count,
			angleBegin, angleEnd, liveTimeBegin, liveTimeEnd));
		networkClient.notifyParticleEmit(ParticleEmitTrace(range, texture.id, sizeDivisor, speed, count,
			angleBegin, angleEnd, liveTimeBegin, liveTimeEnd));
	}
	if (isForced || role == Server) { // Present locally
		if (liveTimeBegin > liveTimeEnd)
			swap(liveTimeBegin, liveTimeEnd);
		if (angleBegin > angleEnd)
			swap(angleBegin, angleEnd);
		for (int i = 0; i < count; i++) {
			Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
			double angle = angleBegin + (angleEnd - angleBegin)*rand01();
			Vector2d pos;

			pos.x = range.left + range.width*rand01();
			pos.y = range.top + range.height*rand01();

			Particle p(texture, sizeDivisor, live);
			p.setPosition(pos);
			p.accelerateVector(speed, angle);

			parts.push_back(p);
		}
	}
}



void PartlcleSystem::emitTraced(const ParticleEmitTrace & log) {
	emit(log.range, textureManager.getTextureInfo(log.textureId), log.sizeDivisor, log.speed, log.count, log.angleBegin, log.angleEnd, log.liveTimeBegin, log.liveTimeEnd, true);
}

