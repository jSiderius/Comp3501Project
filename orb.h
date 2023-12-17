#ifndef ORB_H_
#define ORB_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "scene_node.h"

namespace game {

    // Abstraction of an asteroid
    class Orb : public SceneNode {

        public:
            // Create asteroid from given resources
            Orb(const std::string name, const Resource *geometry, const Resource *material, const Resource* texture, SceneNode* particles);

            // Destructor
            ~Orb();
            
            // Get/set attributes specific to asteroids
            glm::quat GetAngM(void) const;
            void SetAngM(glm::quat angm);

            void Update(std::vector<std::vector<float>> height_values, float length, float width);

            void SetFloorScale(glm::vec3 floor_scale);
            void SetFloorPos(glm::vec3 floor_pos);
            void SetImpassableMap(std::vector<std::vector<bool>> impassable_map);

            void SetRadius(float radius);

            bool Colliding(glm::vec3 position, float radius);

            void Update(void) override;
            void Draw(Camera *camera) override;
            
        private:
            // Angular momentum of asteroid
            glm::quat angm_;

            glm::vec3 floor_scale_;
            glm::vec3 floor_pos_;
            std::vector<std::vector<bool>> impassable_map_;

            SceneNode* particles_;

            float radius_;
    }; // class Orb

} // namespace game

#endif // ORB_H_