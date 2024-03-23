#ifndef RENDERINGCOMPONENT_HPP
#define RENDERINGCOMPONENT_HPP

#include "component.hpp" 
#include "entity.hpp" 
#include "src/bufferedInputComponent.hpp"
#include "src/physicsComponent.hpp"
#include "transformComponent.hpp" 
// #include <iostream>

template<typename T>
    concept Transformer = requires( T t, raylib::Transform m) {
        {t.operator()(m)}->std::convertible_to<raylib::Transform>;
    };


struct RenderingComponent : public Component{

    void DrawModel(raylib::Model& model, Transformer auto transformer) {

        auto ref3 = object->GetComponent<bufferedComponent>(); // get optional reference to transform component 
        if (!ref3) return; // does it exist 
        auto& buffer = ref3->get() ; // get values stored in reference if it exists
        
        
        raylib::Transform backupTransform = model.transform; // Save the model's transform
        model.transform = transformer(backupTransform); // Update the model's transform based on the transformation function
        // model.GetTransformedBoundingBox().Draw(raylib::Color(BLACK));
        model.Draw({});
        if (buffer.selected) {
            model.GetTransformedBoundingBox().Draw(raylib::Color(BLACK));
        }
        model.transform = backupTransform; // Restore the original transform
        
    }


    raylib::Model model;

    RenderingComponent(Entity& e, raylib::Model model = {}) : Component(e), model(std::move(model)) {}
    
    void tick(float dt) override {
        
        auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transformC = ref->get(); // get values stored in reference if it exists

        auto ref1 = object->GetComponent<PhysicsComponent>(); // get optional ref1erence to transform component 
        if (!ref1) return; // does it exist 
        auto& physics = ref1->get(); // get values stored in reference if it exists

        auto ref3 = object->GetComponent<bufferedComponent>(); // get optional reference to transform component 
        if (!ref3) return; // does it exist 
        auto& buffer = ref3->get() ; // get values stored in reference if it exists

        // if (buffer.selected) {
        //     model.GetTransformedBoundingBox().Draw(raylib::Color(BLACK));
        // }
        // model.Draw(transformC.position, transformC.rotation.Normalize(), 0.0f);
        // DrawModelEx(model, transformC.position, transformC.rotation, physics.heading, transformC.scale, PINK);
        // if (buffer.selected) {
        //     model.GetTransformedBoundingBox().Draw(raylib::Color(BLACK));
        // }
        // model.Draw(
        //     transformC.position, 
        // transformC.rotation, 
        // transformC.rotation.x,
        // transformC.scale, 
        // PINK);
        

        DrawModel(model, [&transformC](raylib::Transform t) -> raylib::Transform {
            return t.Translate(transformC.position).
            RotateXYZ(transformC.rotation.x, transformC.rotation.y,transformC.rotation.z).
            Scale(transformC.scale.x, transformC.scale.y, transformC.scale.z);
        });

        
    }
};

#endif
