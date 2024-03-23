#ifndef INPUTCOMPONENT_HPP
#define INPUTCOMPONENT_HPP

#include "component.hpp"
#include "entity.hpp"
// #include "delegate.hpp"
#include "src/physicsComponent.hpp"
// #include "src/renderingComponent.hpp"
// #include "src/transformComponent.hpp"
#include <BufferedInput.hpp>

struct bufferedComponent : public Component {
    raylib::BufferedInput* inputs; // Manager for actions 
    bool selected = false;

    

    bufferedComponent(Entity& entity, raylib::BufferedInput* Input, bool selected)
        : Component(entity), inputs(Input), selected(selected) {}

    void setup() override {

        // Check if the controlled entity is selected
        auto ref = object->GetComponent<PhysicsComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& physics = ref->get(); // get values stored in reference if it exists

        auto ref2 = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref2) return; // does it exist 
        auto& transform = ref2->get() ; // get values stored in reference if it exists 

        (*inputs)["forward"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
       
            if(selected) {
                physics.targetSpeed += 5;// thus doing 
            }
        });
        (*inputs)["backwards"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
            
            if(selected)  {
                physics.targetSpeed -= 5; // thus doing 
                
            }
        });
        (*inputs)["right"].AddPressedCallback([&physics, &transform, this]()-> void { //lambda function that is creating call back for action named "forward"

            if(selected) {
                if(physics.angularAcceleration < 10) physics.targetHeading -= 5;
                else physics.targetHeading -= 20;
            }
        });
        (*inputs)["left"].AddPressedCallback([&physics, &transform, this]()-> void { //lambda function that is creating call back for action named "forward"
            
            if(selected) { 
                if(physics.angularAcceleration < 10) physics.targetHeading += 5;
                else physics.targetHeading += 20;
                
            }
        });
        (*inputs)["space"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
            
            if(selected) { 
                physics.targetSpeed = 0;
            }
        });
    }

   
};

#endif 