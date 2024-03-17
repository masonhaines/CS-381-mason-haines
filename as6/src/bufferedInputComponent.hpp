#ifndef INPUTCOMPONENT_HPP
#define INPUTCOMPONENT_HPP

#include "component.hpp"
#include "entity.hpp"
// #include "delegate.hpp"
#include "src/physicsComponent.hpp"
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

        (*inputs)["forward"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "W" << std::endl;
            if(selected) physics.speed++;// thus doing 
        });
        (*inputs)["backwards"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "S" << std::endl;
            if(selected) physics.speed--;//physics.velocity.y += ; // thus doing 
        });
        (*inputs)["right"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "D" << std::endl;
            if(selected) {
                // physics.velocity.x -= 50; // thus doing 
                physics.heading += 5;
            }
        });
        (*inputs)["left"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "A" << std::endl;
            if(selected) {
                // physics.velocity.x += 50 ; // thus doing 
                physics.heading -= 5;
            }
        });
        // When space key is hit action happens
        
    }

   
};

#endif 