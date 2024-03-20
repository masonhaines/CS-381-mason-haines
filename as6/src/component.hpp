#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <memory>
#include "raylib-cpp.hpp"

struct Component {
    struct Entity* object; // object prototype

    // Component() = default; 

    Component(struct Entity& e) : object(&e) {} //constructor 

    virtual void setup() {}
    virtual void cleanup() {}
    virtual void tick(float dt) {}
};

#endif // COMPONENT_HPP
