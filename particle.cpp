#include "particle.h"

#include <cmath>

float bgRadius = 295.0f;
sf::Vector2f bgPosition;

particle::particle(const sf::Vector2f &initialPosition, float r)
	: currentPosition(initialPosition), oldPosition(initialPosition), acceleration(0.0f, 0.0f), radius(r) {
}

void particle::updatePosition(float dt) {
	const sf::Vector2f velocity = currentPosition - oldPosition;
	oldPosition = currentPosition;
	currentPosition += velocity + acceleration * dt * dt;
	acceleration = {};
}

void particle::accelerate(const sf::Vector2f &a) {
	acceleration += a;
}

solver::solver() : gravity(0.0f, 500.0f) {
}

void solver::update(float dt) {
	const uint32_t subSteps = 8;
	const float sub_dt = dt / subSteps;
	for (uint32_t i = subSteps; i > 0; --i) {
		applyGravity();
		applyConstraint();
		solveCollisions();
		updatePositions(sub_dt);
	}
}

void solver::applyGravity() {
	for (auto &obj : m_objects) {
		obj.accelerate(gravity);
	}
}

void solver::updatePositions(float dt) {
	for (auto &obj : m_objects) {
		obj.updatePosition(dt);
	}
}

void solver::applyConstraint() {
	for (auto &obj : m_objects) {
		const sf::Vector2f v = obj.currentPosition - bgPosition;
		const float distance = std::sqrt(v.x * v.x + v.y * v.y);
		if (distance > bgRadius - obj.radius + 0.01f) {
			const sf::Vector2f n = v / distance;
			obj.currentPosition = bgPosition + n * (bgRadius - obj.radius);
		}
	}
}

void solver::solveCollisions() {
	const uint64_t objsCount = m_objects.size();

	for (uint64_t i = 0; i < objsCount; ++i) {
		particle &object1 = m_objects[i];
		for (uint64_t j = i + 1; j < objsCount; ++j) {
			particle &object2 = m_objects[j];
			const sf::Vector2f collisionAxis = object1.currentPosition - object2.currentPosition;
			const float dist2 = (collisionAxis.x * collisionAxis.x + collisionAxis.y * collisionAxis.y);
			const float minDist = object1.radius + object2.radius;

			if (dist2 < minDist * minDist) {
				const float dist = std::sqrt(dist2);
				const sf::Vector2f n = collisionAxis / dist;
				const float delta = minDist - dist;

				object1.currentPosition += 0.5f * delta * n;
				object2.currentPosition -= 0.5f * delta * n;
			}
		}
	}
}

void solver::addParticle(const sf::Vector2f &position, float radius) {
	m_objects.emplace_back(position, radius);
}

sf::CircleShape newParticle(solver &simulationSolver, const sf::Vector2f &position, float radius, sf::Color color) {
	simulationSolver.addParticle(position, radius);
	sf::CircleShape particleShape(radius);
	particleShape.setFillColor(color);
	particleShape.setOrigin(10.0f, 10.0f);
	particleShape.setPosition(position);
	return particleShape;
}

