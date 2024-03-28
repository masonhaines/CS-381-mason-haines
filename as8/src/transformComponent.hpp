#ifndef TRANSFORMCOMPONENT_HPP
#define TRANSFORMCOMPONENT_HPP

#include "component.hpp" 

struct TransformComponent : public Component{
    using Component::Component;

    raylib::Vector3 position = {0, 0, 0};
    raylib::Vector3 scale = {1, 1, 1};
    raylib::Vector3 rotation;
    
    TransformComponent(Entity& ENTITY, 
        raylib::Vector3& POS, 
        raylib::Vector3& SCALE,
        raylib::Vector3& ROTATION
        ) 
            : Component(ENTITY), position(POS), scale(SCALE), 
        rotation(ROTATION) {}
};

#endif
