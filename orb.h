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
            Orb(const std::string name, const Resource *geometry, const Resource *material, const Resource* texture);

            // Destructor
            ~Orb();
            
            // Get/set attributes specific to asteroids
            glm::quat GetAngM(void) const;
            void SetAngM(glm::quat angm);

            // Update geometry configuration
            void Update(void);

            void Update(std::vector<std::vector<float>> height_values, float length, float width);

            void SetFloorScale(glm::vec3 floor_scale);
            void SetFloorPos(glm::vec3 floor_pos);
            void SetImpassableMap(std::vector<std::vector<bool>> impassable_map);
            
        private:
            // Angular momentum of asteroid
            glm::quat angm_;

            glm::vec3 floor_scale_;
            glm::vec3 floor_pos_;
            std::vector<std::vector<bool>> impassable_map_;
    }; // class Orb

} // namespace game

#endif // ORB_H_