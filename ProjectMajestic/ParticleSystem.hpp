#pragma once

#include "Main.hpp"
#include "Entity.hpp"


class Particle :public Entity {
public:

	Particle(TextureInfo texture, double sizeDivisor, Time liveTime);

	const string getEntityId() override { return "particle"; }
	TextureInfo getTextureInfo() override { return TextureInfo(wholeText.texture, textureSubRect, wholeText.id); }

	const double getFrictionDeaclc() override { return 5.0; }

	Vector2d getSize() override { return Vector2d(0.2, 0.2); }

	void _updateLogic() override;

private:

	TextureInfo wholeText;
	IntRect textureSubRect;

	Time liveTime;
	Clock liveClock;

};

struct ParticleEmitTrace {

	ParticleEmitTrace() {}
	ParticleEmitTrace(Vector2d position,
		string textureId,
		double sizeDivisor,
		double speed,
		int count,
		double angleBegin, double angleEnd,
		Time liveTimeBegin, Time liveTimeEnd) :
		range(position.x, position.y, 0.0, 0.0), textureId(textureId), sizeDivisor(sizeDivisor), speed(speed), count(count),
		angleBegin(angleBegin), angleEnd(angleEnd), liveTimeBegin(liveTimeBegin), liveTimeEnd(liveTimeEnd) {}
	ParticleEmitTrace(DoubleRect range,
		string textureId,
		double sizeDivisor,
		double speed,
		int count,
		double angleBegin, double angleEnd,
		Time liveTimeBegin, Time liveTimeEnd) :
		range(range), textureId(textureId), sizeDivisor(sizeDivisor), speed(speed), count(count),
		angleBegin(angleBegin), angleEnd(angleEnd), liveTimeBegin(liveTimeBegin), liveTimeEnd(liveTimeEnd) {}

	DoubleRect range;
	string textureId;
	double sizeDivisor;
	double speed;
	int count;
	double angleBegin, angleEnd;
	Time liveTimeBegin, liveTimeEnd;

};

Packet& operator << (Packet& pack,const ParticleEmitTrace& log) {
	return pack << log.range.left << log.range.top << log.range.width << log.range.height << log.textureId << log.sizeDivisor <<
		log.speed << log.count << log.angleBegin << log.angleEnd << log.liveTimeBegin.asMicroseconds() <<
		log.liveTimeEnd.asMicroseconds();
}

Packet& operator >> (Packet& pack, ParticleEmitTrace& log) {
	Int64 microBegin, microEnd;
	pack >> log.range.left >> log.range.top >> log.range.width >> log.range.height >> log.textureId >> log.sizeDivisor >>
		log.speed >> log.count >> log.angleBegin >> log.angleEnd >> microBegin >> microEnd;
	log.liveTimeBegin = microseconds(microBegin);
	log.liveTimeEnd = microseconds(microEnd);
	return pack;
}


class PartlcleSystem : public Lockable {
public:

	void updateLogic();
	void getRenderList(VertexArray& verts); // Triangles

	list<Particle>& getParticleList() { return parts; }

public:

	// Emit <Count> particles with the texture <Texture> with a size of the entire
	// texture divided by <sizeDivisor> at <Position> with <Speed> at any angle
	// If <isForced> is set to true, this emittion is presented on the client,
	// oterwise it is sent to the server.
	// On a server, an emittion is always presented and sent to the clients.
	void emit(Vector2d position, TextureInfo texture, double sizeDivisor, double speed, int count, Time liveTimeBegin, Time liveTimeEnd, bool isForced = false);

	// Emit <Count> particles with the texture <Texture> with a size of the entire
	// texture divided by <sizeDivisor> at a random position in rectangle <Range>
	// with <Speed> at any angle
	// If <isForced> is set to true, this emittion is presented on the client,
	// oterwise it is sent to the server.
	// On a server, an emittion is always presented and sent to the clients.
	void emit(DoubleRect range, TextureInfo texture, double sizeDivisor, double speed, int count, Time liveTimeBegin, Time liveTimeEnd, bool isForced = false);

	// Emit <Count> particles with the texture <Texture> with a size of the entire
	// texture divided by <sizeDivisor> at <Position> with <Speed> at an angle randomly seperated
	// between <AngleBegin> and <AngleEnd>
	// If <isForced> is set to true, this emittion is presented on the client,
	// oterwise it is sent to the server.
	// On a server, an emittion is always presented and sent to the clients.
	void emit(Vector2d position, TextureInfo texture, double sizeDivisor, double speed, int count, double angleBegin, double angleEnd, Time liveTimeBegin, Time liveTimeEnd, bool isForced = false);

	// Emit <Count> particles with the texture <Texture> with a size of the entire
	// texture divided by <sizeDivisor> at a random position in rectangle <Range>
	// with <Speed> at an angle randomly seperated between <AngleBegin> and <AngleEnd>
	// If <isForced> is set to true, this emittion is presented on the client,
	// oterwise it is sent to the server.
	// On a server, an emittion is always presented and sent to the clients.
	void emit(DoubleRect range, TextureInfo texture, double sizeDivisor, double speed, int count, double angleBegin, double angleEnd, Time liveTimeBegin, Time liveTimeEnd, bool isForced = false);

public:

	// This emit is always forced, i.e., it is always presented locally and
	// never sent across the network on a client, and both presented and sent
	// to the clients on a server.
	void emitTraced(const ParticleEmitTrace& log);

private:

	list<Particle> parts;


};


PartlcleSystem particleSystem;
