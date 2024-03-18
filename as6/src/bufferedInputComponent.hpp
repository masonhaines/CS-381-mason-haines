#ifndef INPUTCOMPONENT_HPP
#define INPUTCOMPONENT_HPP

#include "component.hpp"
#include "entity.hpp"
// #include "delegate.hpp"
#include "src/physicsComponent.hpp"
#include "src/renderingComponent.hpp"
#include "src/transformComponent.hpp"
#include <BufferedInput.hpp>

struct bufferedComponent : public Component {
    raylib::BufferedInput* inputs; // Manager for actions 
    bool selected = false;


    bufferedComponent(Entity& entity, raylib::BufferedInput* Input)
        : Component(entity), inputs(Input) {}

 

    void setup() override {
        // Check if the controlled entity is selected
        auto ref = object->GetComponent<PhysicsComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& physics = ref->get(); // get values stored in reference if it exists

        auto ref2 = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref2) return; // does it exist 
        auto& transform = ref2->get() ; // get values stored in reference if it exists
        // auto [axis, angle] = transform.rotation.ToAxisAngle(); // gets quaternion
        transform.rotation.Normalize();



        (*inputs)["forward"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "W" << std::endl;
            if(selected) 
                physics.speed++;// thus doing 
                
        });
        (*inputs)["backwards"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "S" << std::endl;
            if(selected) 
                physics.speed--; // thus doing 
                
        });
        (*inputs)["right"].AddPressedCallback([&transform, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "D" << std::endl;
            if(selected) {
               // Rotate right (clockwise)
               transform.rotation.ToEuler();
                // transform.rotation = QuaternionMultiply(transform.rotation, QuaternionFromAxisAngle({0, 0, 0}, -1.0f));
                // transform.rotation.x += .5;
                transform.rotation.y += .50 * DEG2RAD;
                
            }
        });
        (*inputs)["left"].AddPressedCallback([&transform, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "A" << std::endl;
            if(selected) {
                // Rotate left (counter-clockwise)
                // transform.rotation.ToEuler();
                
                // transform.rotation
                // transform.rotation
                // transform.rotation
                // transform.rotation
                // transform.rotation = QuaternionMultiply(transform.rotation, QuaternionFromAxisAngle({0, 0, 0}, 1.0f));
                // transform.rotation.x -= .5;
                transform.rotation.y += 50 * DEG2RAD;
                
                
            }
        });
        // When space key is hit action happens
        
    }

   
};

#endif 