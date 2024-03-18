#ifndef TRANSFORMCOMPONENT_HPP
#define TRANSFORMCOMPONENT_HPP

#include "component.hpp" 
#include <X11/extensions/randr.h>
// #include "entity.hpp" 


struct TransformComponent : public Component{
    using Component::Component;

    
   
    Vector3 position = {0, 0, 0};
    // Vector3 velocity = {0, 0, 0}; 
    Vector3 scale = {1, 1, 1};
    raylib::Color shade = WHITE;
    // float rotation = 0; 
    
    raylib::Quaternion rotation = raylib::Quaternion::Identity(); 
    // raylib::Quaternion rotation = rotation.Normalize();
    

    TransformComponent(Entity& ENTITY, 
        Vector3 POS, 
        // Vector3 VEL,
        Vector3 SCALE,
        raylib::Color COLOR, 
        // float HEADING,
        raylib::Quaternion ROTATION) 
            : Component(ENTITY), position(POS), scale(SCALE), 
        /*heading(HEADING),*/ rotation(ROTATION), shade(COLOR) {}
};

#endif
