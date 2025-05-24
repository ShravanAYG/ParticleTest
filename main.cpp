#include "particle.h"

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
	background.setOrigin(295.0f, 295.0f);
	background.setPosition(bgPosition);

	sf::Time t;
	sf::Clock clock, timer;
	int s, val = 255;

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

