#include "PlayState.hpp"
#include "ErrorLogger.hpp"
#include "Renderer.hpp"
#include "Quad.hpp"

PlayState PlayState::m_playState;
void PlayState::init() {
	m_name = "Play State";
	m_quad.init();
}

void PlayState::update() { ErrorLogger::log(m_name + " update() called."); }

void PlayState::handleEvent(int event) { return; }

void PlayState::pause() { ErrorLogger::log(m_name + " pause() called."); }

void PlayState::draw() {
	m_quad.draw();
	ErrorLogger::log(m_name + " draw() called.");
}

void PlayState::play() { ErrorLogger::log(m_name + " play() called."); }
