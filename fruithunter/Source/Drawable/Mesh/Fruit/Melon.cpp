#include "Melon.h"

Melon::Melon(float3 pos) : Fruit(pos) {
	loadAnimated("Melon", 1);
	m_nrOfFramePhases = 6;
	m_meshAnim.setFrameTargets(0, 0);
	m_rollSpeed = 5;
	m_fruitType = MELON;

	setCollisionData(getPosition(),
		m_meshAnim.getBoundingBoxHalfSizes().y); // sets to sphere with radius same as y halfsize;
	setScale(0.5);
	changeState(AI::State::PASSIVE);
	setStartPosition(pos);
	ErrorLogger::log("Melon:: going to second home");
	m_secondWorldHome = m_worldHome + float3(3.f, 0.0, 3.0f);
	m_direction = m_position - m_secondWorldHome;
	m_direction.Normalize();
	// m_velocity = float3(1.f);
	m_rollAnimationSpeed = 2;
}

void Melon::behaviorPassive(float3 playerPosition) {

	m_worldHome.y = m_position.y;
	m_secondWorldHome.y = m_position.y;
	float3 toHome = m_worldHome - m_position;
	if (toHome.Length() < 0.1f || toHome.Length() > (m_worldHome - m_secondWorldHome).Length()) {
		ErrorLogger::log("Melon:: going to second home");
		m_direction = m_secondWorldHome - m_position;
		lookTo(m_secondWorldHome);
		m_direction.Normalize();
	}
	toHome = m_secondWorldHome - m_position;
	if (toHome.Length() < 0.1f) {
		m_direction = m_worldHome - m_position;
		ErrorLogger::log("Melon:: going to home");

		lookTo(m_worldHome);
		m_direction.Normalize();
	}

	if ((playerPosition - m_position).Length() < 8.0f) {
		changeState(ACTIVE);
	}
}

void Melon::behaviorActive(float3 playerPosition) {
	ErrorLogger::log("Melon:: Doing active.");

	float3 toPlayer = m_position - playerPosition;
	float3 sideStep = toPlayer.Cross(float3(0.0f, 1.0f, 0.0f));
	if (toPlayer.Length() > 5.f) {
		sideStep -= toPlayer;
	}
	else if (toPlayer.Length() < 5.f) {

		sideStep += toPlayer;
	}
	m_direction = sideStep;
	// lookTo(m_direction);
	// pathfinding(m_position, sideStep - m_position);


	if ((playerPosition - m_position).Length() > 4.0f) {
		changeState(PASSIVE);
	}
}

void Melon::behaviorCaught(float3 playerPosition) {
	ErrorLogger::log("Melon:: Doing caught.");
	m_direction = playerPosition - m_position; // run to player
	m_direction.Normalize();

	if ((playerPosition - m_position).Length() < 1.0f) {
		// delete yourself
		ErrorLogger::log("Melon:: is picked up");
	}
}

void Melon::roll(float dt) { rotateX(dt * m_rollAnimationSpeed); }

// TODO, deside is we are going to use m_direction or m_velocity or both for movement.
void Melon::move(float dt) {
	if (!m_availablePath.empty() && m_currentState == ACTIVE) {
		m_direction = (m_availablePath.back() - m_position);
		m_availablePath.pop_back();
	}
	// updateAnimated(dt);
	m_direction.y;
	m_position += m_direction * dt;
	setPosition(m_position);
}


void Melon::update(float dt, float3 playerPosition, TerrainManager* terrain) {
	doBehavior(playerPosition);
	updateAnimated(dt);
	move(dt);
	m_position.y = terrain->getHeightFromPosition(m_position);
}
//
// void Melon::circulateAround(float3 playerPosition) {
//
//	float3 playerToMelon = m_position - playerPosition;
//	// playerToMelon.Transform();
//	float angle = 3.1416 * 0.25; // 8th of halfcircle
//
//	// rotate to find target
//	/*float3 rotated = (float4x4::CreateRotationY(angle).
//					  float4(playerToMelon.x, playerToMelon.y, playerToMelon.z, 1.0f))
//						 .xyz;*/
//	float3 target = playerToMelon;
//
//
//
//	ErrorLogger::log("inside circulateAround, target: (" + std::to_string(target.x) + " , " +
//					 std::to_string(target.y) + " , " + std::to_string(target.z) + ")");
//	pathfinding(m_position, playerPosition);
//}

void Melon::updateAnimated(float dt) {
	m_frameTime += dt;
	if (m_frameTime > 2) {
		m_frameTime = 0.f;
		setDestination();
		lookTo(m_destinationAnimationPosition);
	}
	float3 tempDir(m_destinationAnimationPosition - getPosition());
	tempDir.Normalize();
	roll(dt);
}

void Melon::setRollSpeed(float rollSpeed) { m_rollAnimationSpeed = rollSpeed; }
