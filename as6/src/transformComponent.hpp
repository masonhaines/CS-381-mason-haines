#ifndef TRANSFORMCOMPONENT_HPP
#define TRANSFORMCOMPONENT_HPP

#include "component.hpp" // Include the Component header
#include "entity.hpp" // Include the Entity header


struct TransformComponent : public Component{
    using Component::Component;

    
   
    Vector3 position = {};
    raylib::Quaternion rotation = raylib::Quaternion::Identity();

    TransformComponent(Entity& ENTITY, Vector3 POS, const raylib::Quaternion& ROTATE) 
        : Component(ENTITY), position(POS), rotation(ROTATE) {}
};

#endif
