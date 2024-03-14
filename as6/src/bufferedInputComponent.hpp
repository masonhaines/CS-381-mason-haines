// #ifndef INPUTCOMPONENT_HPP
// #define INPUTCOMPONENT_HPP

// #include "component.hpp"
// #include "entity.hpp"
// #include "delegate.hpp"
// #include <BufferedInput.hpp>

// struct bufferedComponent : public Component {
//     raylib::BufferedInput inputs; // Manager for actions 

//     bufferedComponent(Entity& entity, raylib::BufferedInput Input)
//         : Component(entity), inputs(Input) {}

//     void tick(float dt) override {
//         // Check if the controlled entity is selected
//         auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
//         if (!ref) return; // does it exist 
//         auto& transform = ref->get(); // get values stored in reference if it exists

        
//         // WASD updates plane velocity
//         if (IsKeyDown(KEY_W)) {
//             transform.velocity.y += 1; 
//         }
//         if (IsKeyDown(KEY_S)) {
//             transform.velocity.y -= 1; 
//         }
//         if (IsKeyDown(KEY_A)) {
//             transform.rotation.y += 5;
//         }
//         if (IsKeyDown(KEY_D)) {
//             transform.rotation.y -= 5; 
//         }
//         // Space sets velocity to 0!
//         if (IsKeyDown(KEY_SPACE)) {
//             transform.velocity.y = 0;
//         }
            
        
//     }
// };

// #endif 