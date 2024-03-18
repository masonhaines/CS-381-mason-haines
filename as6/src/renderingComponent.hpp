#ifndef RENDERINGCOMPONENT_HPP
#define RENDERINGCOMPONENT_HPP

#include "component.hpp" 
#include "entity.hpp" 
#include "transformComponent.hpp" 
#include <iostream>

template<typename T>
    concept Transformer = requires( T t, raylib::Transform m) {
        {t.operator()(m)}->std::convertible_to<raylib::Transform>;
    };


struct RenderingComponent : public Component{

    void DrawModel(raylib::Model& model, Transformer auto transformer) {
        raylib::Transform backupTransform = model.transform; // Save the model's transform
        model.transform = transformer(backupTransform); // Update the model's transform based on the transformation function
        
        model.Draw({});
        model.transform = backupTransform; // Restore the original transform
    }


    raylib::Model model;

    RenderingComponent(Entity& e, raylib::Model model = {}) : Component(e), model(std::move(model)) {}
    
    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transformC = ref->get(); // get values stored in reference if it exists

        // model.Draw(transformC.position, transformC.rotation ,transformC.heading);

        // model.Draw(
        //     transform.position, 
        // transform.rotation ,
        // transform.heading, 
        // transform.scale, 
        // transform.shade);

         DrawModel(model, [&transformC](raylib::Transform t) -> raylib::Transform {
            return t.Translate(transformC.position).
            RotateY(transformC.rotation.y).
            RotateX(transformC.rotation.x).
            Scale(transformC.scale.x, transformC.scale.y, transformC.scale.z);
        });

        
    }
};

#endif
