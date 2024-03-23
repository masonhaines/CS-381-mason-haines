#ifndef PHYSICSCOMPONENT_HPP
#define PHYSICSCOMPONENT_HPP

#include "component.hpp"
#include "entity.hpp" 
// #include "src/bufferedInputComponent.hpp"
// #include "src/bufferedInputComponent.hpp"
#include "transformComponent.hpp" 
#include <iostream>


struct PhysicsComponent : public Component {
    raylib::Vector3 velocity = {0, 0, 0};
    float speed;
    float targetSpeed;
    raylib::Degree targetHeading;
    raylib::Degree heading = 0; 

    int minSpeed = 0;
    int maxSpeed = 25;

    float acceleration = 10;
    float angularAcceleration = 15;
    
    raylib::Vector3 offsetROT = {0, 0, 0};


    PhysicsComponent(Entity& entity,Vector3 OFFSET, const raylib::Vector3& initVELO = {0, 0, 0}, float initSPEED = 0, float ACC = 10, float angAcc = 15, float targSpeed = 0, float HEADING = 0, float targHEADING = 0)
        : Component(entity), offsetROT(OFFSET), velocity(initVELO), speed(initSPEED), targetSpeed(targSpeed), acceleration(ACC), angularAcceleration(angAcc), heading(HEADING), targetHeading(targHEADING){}
    
    void tick(float dt) override {

        

        static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
            int intPart = angle;
            float floatPart = float(angle) - intPart;
            intPart %= 360;
            intPart += (intPart < 0) * 360;
            return intPart + floatPart;
        };

        if (speed < targetSpeed) {
            speed += acceleration * dt;
        } else if (speed > targetSpeed) {
            speed -= acceleration * dt;
        }
        // Ensure speed stays within bounds
        speed = Clamp(speed, minSpeed, maxSpeed);
        
       float target = AngleClamp(targetHeading);
        int difference = abs(target - heading);
        if(target > heading) {
            if(difference < 180) heading += angularAcceleration * dt;
            else if(difference > 180) heading -= angularAcceleration * dt;
        } else if(target < heading) {
            if(difference < 180) heading -= angularAcceleration * dt;
            else if(difference > 180) heading += angularAcceleration * dt;
        } 
        if(difference < .5) heading = target; // If the heading is really close to correct 
        heading = AngleClamp(heading);
        float angle = raylib::Degree(heading);



        velocity.x = speed * cos(angle * DEG2RAD * 2);
        velocity.y = 0;
        velocity.z = -speed * sin(angle * DEG2RAD * 2);

         auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
        if (!ref) return; // does it exist 
        auto& transform = ref->get(); // get values stored in reference if it exists

        // Update position based on velocity
        transform.position.x += velocity.x * dt;
        transform.position.y += velocity.y * dt;
        transform.position.z += velocity.z * dt;

        transform.rotation.y = (angle * DEG2RAD *2) + offsetROT.y;
        // transform.rotation.x = -.1* angle * DEG2RAD;

        // Debug output
        std::cout << "PhysicsComponent tick function called." << std::endl;
        // std::cout << "Position: (" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << ")" << std::endl;
        std::cout << "axis: (" << transform.rotation.x << ", " << transform.rotation.y << ", " << transform.rotation.z << ")" << std::endl;
        // std::cout << "Velocity mag: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
    }
};

#endif 


//  auto ref = object->GetComponent<TransformComponent>(); // get optional reference to transform component 
//         if (!ref) return; // does it exist 
//         auto& transform = ref->get(); // get values stored in reference if it exists

//         // auto [axis, angle] = transform.rotation // gets quaternion
//         raylib::Vector3 velocity3D = {cos(transform.rotation.y) * speed, velocity.y, -sin(transform.rotation.y) * speed}; 
//         // Update position
//         transform.position.x += velocity3D.x * dt; 
//         transform.position.y += velocity3D.y * dt;
//         transform.position.z += velocity3D.z * dt;