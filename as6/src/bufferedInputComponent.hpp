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

    // bufferedComponent(Entity& entity)
    //     : Component(entity) {}

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
            if(selected) physics.velocity.x -= 50; // thus doing 
        });
        (*inputs)["left"].AddPressedCallback([&physics, this]()-> void { //lambda function that is creating call back for action named "forward"
            // std::cout << "A" << std::endl;
            if(selected) {
                physics.velocity.x += 50 ; // thus doing 
                // physics.
            }
        });
        // When space key is hit action happens
        
    }

    void tick(float dt) override {
        // Check if the controlled entity is selected
        auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transform = ref->get(); // get values stored in reference if it exists

       

        


        // // WASD updates plane velocity
        // if (IsKeyDown(KEY_W)) {
        //     transform.velocity.y += 1; 
        // }
        // if (IsKeyDown(KEY_S)) {
        //     transform.velocity.y -= 1; 
        // }
        // if (IsKeyDown(KEY_A)) {
        //     transform.rotation.y += 5;
        // }
        // if (IsKeyDown(KEY_D)) {
        //     transform.rotation.y -= 5; 
        // }
        // // Space sets velocity to 0!
        // if (IsKeyDown(KEY_SPACE)) {
        //     transform.velocity.y = 0;
        // }
            
        
    }
};

#endif 