#include "Fruit.h"
#include "Input.h"

void Fruit::setStartPosition(float3 pos) {
	setPosition(pos);
	m_startPos = pos;
	m_heightPos = pos;
	m_destinationPos = pos;
	m_nextDestinationPos = pos;
}

void Fruit::setNextDestination(float3 nextDest) { m_nextDestinationPos = nextDest; }

void Fruit::lookTo(float3 lookAt) { setRotation(float3(0.f, findRequiredRotation(lookAt), 0.f)); }

float Fruit::findRequiredRotation(float3 lookAt) {
	float rot = 0.f;

	float dx = lookAt.x - getPosition().x;
	float dz = lookAt.z - getPosition().z;
	if (dx != 0)
		rot = -atan(dz / dx);
	else
		rot = 0;

	if (dx < 0)
		rot = 3.1416f + rot;
	return rot + 3.14f * 0.5f;
}

void Fruit::setDestination() {
	m_destinationPos = m_nextDestinationPos;
	m_startPos = getPosition();
	m_heightPos = XMVectorLerp(m_startPos, m_destinationPos, 0.5f);
	m_heightPos.y += 1.f;
}

Fruit::Fruit(float3 pos) : Entity() {
	setStartPosition(pos);
	m_nrOfFramePhases = 0;
	m_currentFramePhase = 0;
	m_frameTime = 0.0f;
}
