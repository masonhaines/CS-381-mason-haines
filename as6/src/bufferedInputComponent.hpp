#ifndef INPUTCOMPONENT_HPP
#define INPUTCOMPONENT_HPP

#include "component.hpp"
#include "entity.hpp"
// #include "delegate.hpp"
#include "src/physicsComponent.hpp"
#include "src/transformComponent.hpp"
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

        auto ref3 = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref3) return; // does it exist 
        auto& render= ref3->get() ; // get values stored in reference if it exists 

        float angle = raylib::Degree(physics.heading);

        
        (*inputs)["forward"].AddPressedCallback([&physics, &render, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "W" << std::endl;
            if(selected) {
                physics.targetSpeed += 5;// thus doing 
            }
        });
        (*inputs)["backwards"].AddPressedCallback([&physics, &render, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "S" << std::endl;
            if(selected)  {
                physics.targetSpeed -= 5; // thus doing 
                
            }
        });
        (*inputs)["right"].AddPressedCallback([&physics, &transform, &angle, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "D" << std::endl;
            if(selected) {
                physics.targetHeading -= 5;
                transform.rotation.y -= angle * DEG2RAD *2;
                // if (transform.rotation.x > 45 * DEG2RAD) transform.rotation.x += 45 * DEG2RAD;
                
            }
        });
        (*inputs)["left"].AddPressedCallback([&physics, &transform, &angle, this]()-> void { //lambda function that is creating call back for action named "forward"
            
            if(selected) { 
                physics.targetHeading += 5;
                transform.rotation.y += angle * DEG2RAD *2;
                // if (transform.rotation.x > 45 * DEG2RAD) transform.rotation.x += 45 * DEG2RAD;
            }
        });
        (*inputs)["space"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
            
            if(selected) { 
                physics.speed = 0;
            }
        });
    }

   
};

#endif 