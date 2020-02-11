#include "PlayState.h"
#include "ErrorLogger.h"
#include "Renderer.h"
#include "Quad.h"
#include <iostream>
#include <string>

using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;

void PlayState::initialize() {
	m_name = "Play State";

	m_terrain.initilize("heightmap1.png", XMINT2(50, 50), XMINT2(10, 10));
	m_terrain.setScale(float3(1, 0.25, 1) * 25);

	m_entity.load("sphere");
	m_entity.setScale(0.1f);

	m_player.initialize();
	m_player.setPosition(float3(2.f, 0.f, 0.2f));

	m_bow.loadAnimated("Bow", 3);
	m_bow.setPosition(float3(2.f, 0.f, 0.f));

	m_apple.setPosition(float3(5.0f, 10.0f, 5.0f));
	m_apple.setScale(0.3f);
}

void PlayState::update() {
	float3 pos = m_player.getPosition();
	float3 normal = m_terrain.getNormalFromPosition(pos.x, pos.z);
	float h = m_terrain.getHeightFromPosition(pos.x, pos.z);
	m_player.update(0.017f, h + 0.5f, normal);
	m_timer.update();
	float dt = m_timer.getDt();
	m_bow.updateAnimated(dt);

	m_apple.updateAnimated(dt);
	m_apple.update(dt,
		m_terrain.getHeightFromPosition(m_apple.getPosition().x, m_apple.getPosition().z),
		m_terrain.getNormalFromPosition(m_apple.getPosition().x, m_apple.getPosition().z), pos);

	float x = 5.0f + sin(m_timer.getTimePassed() * 0.6f);
	float z = 5.0f + cos(m_timer.getTimePassed() * 0.6f);
	float y = m_terrain.getHeightFromPosition(x, z);
	float3 appleDestination = float3(x, y, z);

	m_apple.setNextDestination(appleDestination);
}

void PlayState::handleEvent() { return; }

void PlayState::pause() {
	ErrorLogger::log(m_name + " pause() called.");
	AudioHandler::pauseAmbient();
}

void PlayState::draw() {
	m_player.draw();

	m_terrain.draw();

	float3 p = m_player.getPosition();
	float3 d = m_player.getForward() * 10;
	m_entity.setPosition(p + d);
	m_entity.draw();
	m_terrain.castRay(p, d);
	m_entity.setPosition(p);
	m_entity.draw();

	// Text
	float t = m_timer.getTimePassed();
	Vector4 col = Vector4(.5f, abs(cos(t)), abs(sin(t)), 1.f);
	m_textRenderer.draw("Time: " + std::to_string(t), Vector2(400., 75.), col);

	// Apple
	m_apple.draw_animate();

	// Bow
	m_bow.draw_animate();
}

void PlayState::play() {
	Input::getInstance()->setMouseModeRelative();
	AudioHandler::startPlayAmbient();
	ErrorLogger::log(m_name + " play() called.");
}
