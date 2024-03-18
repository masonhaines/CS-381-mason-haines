#ifndef TRANSFORMCOMPONENT_HPP
#define TRANSFORMCOMPONENT_HPP

#include "component.hpp" 
// #include <X11/extensions/randr.h>
// #include "entity.hpp" 


struct TransformComponent : public Component{
    using Component::Component;

    raylib::Vector3 position = {0, 0, 0};
    // Vector3 velocity = {0, 0, 0}; 
    raylib::Vector3 scale = {1, 1, 1};
    raylib::Color shade = WHITE;
    raylib::Vector3 rotation = {};
    float heading = 0; 
    
    // raylib::Quaternion rotation = raylib::Quaternion::Identity(); 
    // raylib::Quaternion rotation = rotation.Normalize();
    
    TransformComponent(Entity& ENTITY, 
        raylib::Vector3& POS, 
        raylib::Vector3 SCALE,
        raylib::Color COLOR, 
        raylib::Vector3& ROTATION,
        float HEADING
        ) 
            : Component(ENTITY), position(POS), scale(SCALE), 
        rotation(ROTATION), heading(HEADING), shade(COLOR) {}
};

#endif
