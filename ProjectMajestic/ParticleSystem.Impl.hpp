#pragma once

#include "ParticleSystem.hpp"


////////////////////////////////////////
Particle::Particle(TextureInfo texture, double sizeDivisor, Time liveTime, Vector2d onScreenSize, double gravity) {
	wholeText = texture;
	size = onScreenSize;
	this->gravity = gravity;

	Vector2i size;
	int posX, posY;
	if (!isSame(sizeDivisor, 1.0)) {
		size = Vector2i(Vector2d(texture.textureRect.width, texture.textureRect.height) / sizeDivisor);
		posX = rand() % (texture.textureRect.width - size.x) + texture.textureRect.left;
		posY = rand() % (texture.textureRect.height - size.y) + texture.textureRect.top;
	}
	else {
		size = Vector2i(texture.textureRect.width, texture.textureRect.height);
		posX = texture.textureRect.left;
		posY = texture.textureRect.top;
	}

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


// TODO Network
////////////////////////////////////////
void PartlcleSystem::emitSmoke(Vector2d position, int count) {
	double sizeDivisor = 1.0, speed = 0.3, angleBegin = 180.0, angleEnd = 360.0, gravity = -0.5;
	Time liveTimeBegin = seconds(2), liveTimeEnd = seconds(4);
	Vector2d size = Vector2d(0.4, 0.4);

	if (liveTimeBegin > liveTimeEnd)
		swap(liveTimeBegin, liveTimeEnd);
	if (angleBegin > angleEnd)
		swap(angleBegin, angleEnd);
	for (int i = 0; i < count; i++) {
		TextureInfo texture = textureManager.getTextureInfo(StringParser::toStringFormatted(
			"particle_smoke_%d",
			rand(1, 8)));

		Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
		double angle = angleBegin + (angleEnd - angleBegin)*rand01();

		Particle p(texture, sizeDivisor, live, size, gravity);
		p.setPosition(position);
		p.accelerateVector(speed * 1.2 * rand01(), angle);

		parts.push_back(p);
	}
}


// Arrow Glow Particle class
// Derived from Particle
////////////////////////////////////////
class ArrowGlowParticle :public Particle {
public:
	ArrowGlowParticle(TextureInfo texture, double sizeDivisor, Time liveTime, Vector2d onScreenSize, double gravity)
		:Particle(texture, sizeDivisor, liveTime, onScreenSize, gravity) {}
	TextureInfo getTextureInfo() override {
		return textureManager.getTextureInfo(StringParser::toStringFormatted("particle_arrow_glow_%d",
			min(8, max(1, (int)(10.0f * (liveClock.getElapsedTime() / liveTime - 1.0f))))));
	}
};


////////////////////////////////////////
void PartlcleSystem::emitArrowGlow(Vector2d position) {
	emit<ArrowGlowParticle>(position, TextureInfo(),
		1.0, 0.0, 1, milliseconds(250), milliseconds(250), Vector2d(0.3, 0.3), 0.0);
}


////////////////////////////////////////
template<typename ParticleClass>
void PartlcleSystem::emit(Vector2d position, TextureInfo texture, double sizeDivisor, double speed, int count, Time liveTimeBegin, Time liveTimeEnd, Vector2d size, double gravity, bool isForced) {
	if (!isForced || role == Server) { // Networked
		networkServer.notifyParitlcEmit(ParticleEmitTrace(position, texture.id, sizeDivisor,
			speed, count, 0, 360, liveTimeBegin, liveTimeEnd, size, gravity));
		networkClient.notifyParticleEmit(ParticleEmitTrace(position, texture.id, sizeDivisor,
			speed, count, 0, 360, liveTimeBegin, liveTimeEnd, size, gravity));
	}
	if (isForced || role == Server) { // Present locally
		if (liveTimeBegin > liveTimeEnd)
			swap(liveTimeBegin, liveTimeEnd);
		for (int i = 0; i < count; i++) {
			Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
			double angle = 360.0*rand01();

			ParticleClass p(texture, sizeDivisor, live, size, gravity);
			p.setPosition(position);
			p.accelerateVector(speed * 1.2 * rand01(), angle);

			parts.push_back(p);
		}
	}
}


////////////////////////////////////////
template<typename ParticleClass>
void PartlcleSystem::emit(DoubleRect range, TextureInfo texture, double sizeDivisor, double speed, int count, Time liveTimeBegin, Time liveTimeEnd, Vector2d size, double gravity, bool isForced) {
	if (!isForced || role == Server) { // Networked
		networkServer.notifyParitlcEmit(ParticleEmitTrace(range, texture.id, sizeDivisor,
			speed, count, 0, 360, liveTimeBegin, liveTimeEnd, size, gravity));
		networkClient.notifyParticleEmit(ParticleEmitTrace(range, texture.id, sizeDivisor,
			speed, count, 0, 360, liveTimeBegin, liveTimeEnd, size, gravity));
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

			ParticleClass p(texture, sizeDivisor, live, size, gravity);
			p.setPosition(pos);
			p.accelerateVector(speed * 1.2 * rand01(), angle);

			parts.push_back(p);
		}
	}
}


////////////////////////////////////////
template<typename ParticleClass>
void PartlcleSystem::emit(Vector2d position, TextureInfo texture, double sizeDivisor, double speed, int count, double angleBegin, double angleEnd, Time liveTimeBegin, Time liveTimeEnd, Vector2d size, double gravity, bool isForced) {
	if (!isForced || role == Server) { // Networked
		networkClient.notifyParticleEmit(ParticleEmitTrace(position, texture.id, sizeDivisor, speed, count,
			angleBegin, angleEnd, liveTimeBegin, liveTimeEnd, size, gravity));
		networkServer.notifyParitlcEmit(ParticleEmitTrace(position, texture.id, sizeDivisor, speed, count,
			angleBegin, angleEnd, liveTimeBegin, liveTimeEnd, size, gravity));
	}
	if (isForced || role == Server) { // Present locally
		if (liveTimeBegin > liveTimeEnd)
			swap(liveTimeBegin, liveTimeEnd);
		if (angleBegin > angleEnd)
			swap(angleBegin, angleEnd);
		for (int i = 0; i < count; i++) {
			Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
			double angle = angleBegin + (angleEnd - angleBegin)*rand01();

			ParticleClass p(texture, sizeDivisor, live, size, gravity);
			p.setPosition(position);
			p.accelerateVector(speed * 1.2 * rand01(), angle);

			parts.push_back(p);
		}
	}
}


////////////////////////////////////////
template<typename ParticleClass>
void PartlcleSystem::emit(DoubleRect range, TextureInfo texture, double sizeDivisor, double speed, int count, double angleBegin, double angleEnd, Time liveTimeBegin, Time liveTimeEnd, Vector2d size, double gravity, bool isForced) {
	if (!isForced || role == Server) { // Networked
		networkServer.notifyParitlcEmit(ParticleEmitTrace(range, texture.id, sizeDivisor, speed, count,
			angleBegin, angleEnd, liveTimeBegin, liveTimeEnd, size, gravity));
		networkClient.notifyParticleEmit(ParticleEmitTrace(range, texture.id, sizeDivisor, speed, count,
			angleBegin, angleEnd, liveTimeBegin, liveTimeEnd, size, gravity));
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

			ParticleClass p(texture, sizeDivisor, live, size, gravity);
			p.setPosition(pos);
			p.accelerateVector(speed * 1.2 * rand01(), angle);

			parts.push_back(p);
		}
	}
}



void PartlcleSystem::emitTraced(const ParticleEmitTrace & log) {
	emit(log.range, textureManager.getTextureInfo(log.textureId), log.sizeDivisor, log.speed, log.count, log.angleBegin, log.angleEnd, log.liveTimeBegin, log.liveTimeEnd, log.size, log.gravity, true);
}

