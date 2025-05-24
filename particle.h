#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>

extern float bgRadius;
extern sf::Vector2f bgPosition;

struct particle {
    sf::Vector2f currentPosition, oldPosition, acceleration;
    float radius;

    particle(const sf::Vector2f &initialPosition, float r);
    void updatePosition(float dt);
    void accelerate(const sf::Vector2f &a);
};

struct solver {
    sf::Vector2f gravity;
    std::vector<particle> m_objects;

    solver();
    void update(float dt);
    void applyGravity();
    void updatePositions(float dt);
    void applyConstraint();
    void solveCollisions();
    void addParticle(const sf::Vector2f &position, float radius);
};

sf::CircleShape newParticle(solver &simulationSolver, const sf::Vector2f &position, float radius, sf::Color color);

#endif // PARTICLE_H

