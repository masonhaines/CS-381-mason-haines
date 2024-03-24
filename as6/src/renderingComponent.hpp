#ifndef RENDERINGCOMPONENT_HPP
#define RENDERINGCOMPONENT_HPP

#include "component.hpp" 
#include "entity.hpp" 
#include "src/bufferedInputComponent.hpp"
#include "transformComponent.hpp" 
// #include <iostream>

template<typename T>
concept Transformer = requires( T t, raylib::Transform m) {
    {t.operator()(m)}->std::convertible_to<raylib::Transform>;
};

//chat gpt provided logic for updating draw model on 3/24/2024, after conversation about model.Draw({}) that only had position initialized but no scale or tint.
//No specific prompt and no direct code used other than const raylib::Color& color for draw model parameter.
struct RenderingComponent : public Component {

    void DrawModel(raylib::Model& model, const raylib::Color& color, Transformer auto transformer) {
        auto ref = object->GetComponent<bufferedComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& buffer = ref->get(); // get values stored in reference if it exists

        raylib::Transform backupTransform = model.transform; // Save the model's transform
        model.transform = transformer(backupTransform); // Update the model's transform based on the transformation function

        // Draw the model
        model.Draw({}, 1.0f, color); //Default position, scale, color-tint

        if (buffer.selected) {
            model.GetTransformedBoundingBox().Draw(raylib::Color(BLACK));
        }

        model.transform = backupTransform; // Restore the original transform
    }

    raylib::Model model;
    raylib::Color modelColor;

    //Default Constructor
    RenderingComponent(Entity& e, raylib::Model model = {}, raylib::Color TINT = (WHITE)) : Component(e), model(std::move(model)), modelColor(TINT) {}

    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transformC = ref->get(); // get values stored in reference if it exists

        DrawModel(model, modelColor, [&transformC](raylib::Transform t) -> raylib::Transform {
            return t.Translate(transformC.position).
                RotateXYZ(transformC.rotation.x, transformC.rotation.y, transformC.rotation.z).
                Scale(transformC.scale.x, transformC.scale.y, transformC.scale.z);
        });
    }
};

#endif

