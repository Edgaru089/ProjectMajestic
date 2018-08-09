#pragma once

#include "Main.hpp"
#include "Entity.hpp"


class Particle :public Entity {
public:

	Particle(TextureInfo texture,
			 double sizeDivisor,
			 Time liveTime,
			 Vector2d onScreenSize = Vector2d(0.2, 0.2),
			 double gravity = 2.8,
			 double airFriction = .0);

	const string getEntityId() override { return "particle"; }
	TextureInfo getTextureInfo() override { return TextureInfo(wholeText.texture, textureSubRect, wholeText.id); }

	const double getFrictionDeaclc() override { return 5.0; }
	const double getGravityAclc() override { return gravity; }
	double getAirFrictionDeaclc() { return airFriction; }

	Vector2d getSize() override { return size; }

	void _updateLogic() override;



protected:

	void _getBoundingCollisionPoints(vector<Vector2d>& vec) override;

	TextureInfo wholeText;
	IntRect textureSubRect;
	Vector2d size;
	double gravity;
	double airFriction;

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
					  Time liveTimeBegin, Time liveTimeEnd,
					  Vector2d size, double gravity) :
		range(position.x, position.y, 0.0, 0.0), textureId(textureId), sizeDivisor(sizeDivisor), speed(speed), count(count),
		angleBegin(angleBegin), angleEnd(angleEnd), liveTimeBegin(liveTimeBegin), liveTimeEnd(liveTimeEnd), size(size), gravity(gravity) {}
	ParticleEmitTrace(DoubleRect range,
					  string textureId,
					  double sizeDivisor,
					  double speed,
					  int count,
					  double angleBegin, double angleEnd,
					  Time liveTimeBegin, Time liveTimeEnd,
					  Vector2d size, double gravity) :
		range(range), textureId(textureId), sizeDivisor(sizeDivisor), speed(speed), count(count),
		angleBegin(angleBegin), angleEnd(angleEnd), liveTimeBegin(liveTimeBegin), liveTimeEnd(liveTimeEnd),
		size(size), gravity(gravity) {}

	DoubleRect range;
	string textureId;
	double sizeDivisor;
	double speed;
	int count;
	double angleBegin, angleEnd;
	Time liveTimeBegin, liveTimeEnd;
	Vector2d size;
	double gravity;

};

Packet& operator << (Packet& pack, const ParticleEmitTrace& log) {
	return pack << log.range.left << log.range.top << log.range.width << log.range.height << log.textureId << log.sizeDivisor <<
		log.speed << log.count << log.angleBegin << log.angleEnd << log.liveTimeBegin.asMicroseconds() <<
		log.liveTimeEnd.asMicroseconds() << log.size.x << log.size.y << log.gravity;
}

Packet& operator >> (Packet& pack, ParticleEmitTrace& log) {
	Int64 microBegin, microEnd;
	pack >> log.range.left >> log.range.top >> log.range.width >> log.range.height >> log.textureId >> log.sizeDivisor >>
		log.speed >> log.count >> log.angleBegin >> log.angleEnd >> microBegin >> microEnd >> log.size.x >> log.size.y >> log.gravity;
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

	void emitSmoke(Vector2d position,
				   double speed = 0.4,
				   double gravity = -0.5,
				   double airFriction = 0.2,
				   double angleBegin = 180.0,
				   double angleEnd = 360.0,
				   int count = 16);

	void emitSmoke(DoubleRect position, double speed, double gravity, double airFriction, double angleBegin, double angleEnd, int count);

	void emitArrowGlow(Vector2d position);

public:

	// Emit <Count> particles with the texture <Texture> with a size of the entire
	// texture divided by <sizeDivisor> at <Position> with <Speed> at any angle
	// with a gravity modifier of <gravity> and a on-screen size of <size> blocks
	// If <isForced> is set to true, this emittion is presented on the client,
	// oterwise it is sent to the server.
	// On a server, an emittion is always presented and sent to the clients.
	void emit(Vector2d position,
			  TextureInfo texture,
			  double sizeDivisor,
			  double speed,
			  int count,
			  Time liveTimeBegin,
			  Time liveTimeEnd,
			  Vector2d size = Vector2d(0.2, 0.2),
			  double gravity = 2.8);

	// Emit <Count> particles with the texture <Texture> with a size of the entire
	// texture divided by <sizeDivisor> at a random position in rectangle <Range>
	// with <Speed> at any angle, a gravity modifier of <gravity> and a on-screen size of <size> blocks
	// If <isForced> is set to true, this emittion is presented on the client,
	// oterwise it is sent to the server.
	// On a server, an emittion is always presented and sent to the clients.
	void emit(DoubleRect range,
			  TextureInfo texture,
			  double sizeDivisor,
			  double speed,
			  int count,
			  Time liveTimeBegin,
			  Time liveTimeEnd,
			  Vector2d size = Vector2d(0.2, 0.2),
			  double gravity = 2.8);

	// Emit <Count> particles with the texture <Texture> with a size of the entire
	// texture divided by <sizeDivisor> at <Position> with <Speed> at an angle randomly seperated
	// between <AngleBegin> and <AngleEnd>
	// with a gravity modifier of <gravity> and a on-screen size of <size> blocks
	// If <isForced> is set to true, this emittion is presented on the client,
	// oterwise it is sent to the server.
	// On a server, an emittion is always presented and sent to the clients.
	void emit(Vector2d position,
			  TextureInfo texture,
			  double sizeDivisor,
			  double speed,
			  int count,
			  double angleBegin,
			  double angleEnd,
			  Time liveTimeBegin,
			  Time liveTimeEnd,
			  Vector2d size = Vector2d(0.2, 0.2),
			  double gravity = 2.8);

	// Emit <Count> particles with the texture <Texture> with a size of the entire
	// texture divided by <sizeDivisor> at a random position in rectangle <Range>
	// with <Speed> at an angle randomly seperated between <AngleBegin> and <AngleEnd>
	// with a gravity modifier of <gravity> and a on-screen size of <size> blocks
	// If <isForced> is set to true, this emittion is presented on the client,
	// oterwise it is sent to the server.
	// On a server, an emittion is always presented and sent to the clients.
	void emit(DoubleRect range,
			  TextureInfo texture,
			  double sizeDivisor,
			  double speed,
			  int count,
			  double angleBegin,
			  double angleEnd,
			  Time liveTimeBegin,
			  Time liveTimeEnd,
			  Vector2d size = Vector2d(0.2, 0.2),
			  double gravity = 2.8);

private:

	list<Particle> parts;


};


PartlcleSystem particleSystem;
