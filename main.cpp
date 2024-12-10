#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <stdio.h>
#include <vector>

float bgRadius = 295.0f;
sf::Vector2f bgPosition;

struct particle
{
	sf::Vector2f currentPosition, oldPosition, acceleration;
	float radius;

	particle(const sf::Vector2f &initialPosition, float r)
		: currentPosition(initialPosition), oldPosition(initialPosition), acceleration(0.0f, 0.0f), radius(r) {
	}

	void updatePosition(float dt) {
		const sf::Vector2f velocity = currentPosition - oldPosition;
		oldPosition = currentPosition;
		currentPosition += velocity + acceleration * dt * dt;
		acceleration = {};
	}

	void accelerate(const sf::Vector2f &a) {
		acceleration += a;
	}
};

struct solver
{
	sf::Vector2f gravity = {0.0f, 500.0f};
	std::vector<particle> m_objects;

	void update(float dt) {
		const uint32_t subSteps = 8;
		const float sub_dt = dt / subSteps;
		for (uint32_t i(subSteps); i > 0; i--) {
			applyGravity();
			applyConstraint();
			solveCollisions();
			updatePositions(sub_dt);
		}
	}

	void applyGravity() {
		for (auto &obj : m_objects) {
			obj.accelerate(gravity);
		}
	}

	void updatePositions(float dt) {
		for (auto &obj : m_objects) {
			obj.updatePosition(dt);
		}
	}

	void applyConstraint() {
		for (auto &obj : m_objects) {
			const sf::Vector2f v = obj.currentPosition - bgPosition;
			const float distance = std::sqrt(v.x * v.x + v.y * v.y);
			if (distance > bgRadius - obj.radius + 0.01f) {
				const sf::Vector2f n = v / distance;
				obj.currentPosition = bgPosition + n * (bgRadius - obj.radius);
			}
		}
	}

	void solveCollisions() {
		const uint64_t objsCount = m_objects.size();

		for (uint64_t i{0}; i < objsCount; i++) {
			particle &object1 = m_objects[i];
			for (uint64_t j{i + 1}; j < objsCount; j++) {
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

	void addParticle(const sf::Vector2f &position, float radius) {
		m_objects.emplace_back(position, radius);
	}
};

sf::CircleShape newParticle(solver &simulationSolver, const sf::Vector2f &position, float radius, sf::Color color) {
	simulationSolver.addParticle(position, radius);
	sf::CircleShape particleShape(radius);
	particleShape.setFillColor(color);
	particleShape.setOrigin(10.0f, 10.0f);
	particleShape.setPosition(position);
	return particleShape;
}

int main() {
	srand(time(NULL));
	sf::RenderWindow window(sf::VideoMode(800, 600), "Particle Simulation Test");
	window.setFramerateLimit(60);
	printf("Particle Simulation Test\n");

	solver simulationSolver;
	float radius = 10.0f;
	sf::Vector2f spawnPosition = {400.0f, 60.0f};
	std::vector<sf::CircleShape> particleShapes;

	particleShapes.push_back(newParticle(simulationSolver, spawnPosition, radius, sf::Color::Red));

	sf::CircleShape background(bgRadius);
	background.setFillColor(sf::Color::Black);
	bgPosition = {400.0f, 300.0f};
	background.setOrigin(bgRadius, bgRadius);
	background.setPosition(bgPosition);

	sf::Time t;
	sf::Clock clock, timer;
	int s, val = 255;
	float rotate = 0.0f;

	while (window.isOpen()) {
		sf::Event event;
		sf::Mouse mouse;
		sf::Vector2f mousePos;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed) {
				// radius = 4.0f + (rand() % 10);
				mousePos.x = mouse.getPosition().x - (bgPosition.x / 2 + 50 + radius);
				mousePos.y = mouse.getPosition().y - radius;
				mousePos -= background.getOrigin();
				sf::CircleShape particleShape = newParticle(simulationSolver, mousePos, radius, sf::Color::Red);
			}
		}
		t = timer.getElapsedTime();
		s = t.asMilliseconds();
		if (s >= 175) {
			timer.restart();
			radius = 4.0f + (rand() % 10);
			particleShapes.push_back(
				newParticle(simulationSolver, spawnPosition, radius, sf::Color(val, 123, 123, 255)));
		}
		float dt = clock.restart().asSeconds();

		simulationSolver.update(dt);

		window.clear(sf::Color(180, 180, 180, 255));
		window.draw(background);

		for (const auto &obj : simulationSolver.m_objects) {
			sf::CircleShape particleShape(obj.radius);
			particleShape.setFillColor(sf::Color::Red);
			particleShape.setOrigin(obj.radius, obj.radius);
			particleShape.setPosition(obj.currentPosition);
			window.draw(particleShape);
		}

		window.display();
	}

	return 0;
}
