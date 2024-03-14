#ifndef TRANSFORMCOMPONENT_HPP
#define TRANSFORMCOMPONENT_HPP

#include "component.hpp" 
#include "entity.hpp" 


struct TransformComponent : public Component{
    using Component::Component;

    
   
    Vector3 position = {0,0,0};
    Vector3 scale = {1.0f, 1.0f, 1.0f};
    raylib::Color shade = RED;
    raylib::Quaternion rotation = raylib::Quaternion::Identity();

    TransformComponent(Entity& ENTITY, 
    const Vector3 POS, 
    const raylib::Quaternion& ROTATE, 
    Vector3 SCALE, raylib::Color COLOR)
        : Component(ENTITY), position(POS), rotation(ROTATE), scale(SCALE), shade(COLOR) {}
};

#endif
