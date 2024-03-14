#ifndef TRANSFORMCOMPONENT_HPP
#define TRANSFORMCOMPONENT_HPP

#include "component.hpp" // Include the Component header


struct TransformComponent : public Component{
    using Component::Component;
    raylib::Vector3 position = {0, 0, 0};
    raylib::Quaternion rotation = raylib::Quaternion::Identity();
};

#endif
