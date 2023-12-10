#ifndef PLAYER_H_
#define PLAYER_H_

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
    class Player : public SceneNode {

        public:
            // Create asteroid from given resources
            Player(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture, SceneNode **wheels, int num_wheels, SceneNode **antennas, int num_antennas);

            // Destructor
            ~Player();
            
            // Get/set attributes specific to asteroids
            glm::quat GetAngM(void) const;
            void SetAngM(glm::quat angm);

            void SetFloorScale(glm::vec3 floor_scale);
            void SetFloorPos(glm::vec3 floor_pos);
            void SetImpassableMap(std::vector<std::vector<bool>> impassable_map);
            
            // Update geometry configuration
            void Update(void);

            void Pitch(float angle);
            void Yaw(float angle);
            void Roll(float angle);
            glm::vec3 GetForward(void) const;
            glm::vec3 GetSide(void) const;
            glm::vec3 GetUp(void) const;
            void SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up);

            void Update(std::vector<std::vector<float>> height_values, float length, float width);
            void Translate(glm::vec3 trans) override;

            void Draw(Camera *camera) override;

            
        private:
            // Angular momentum of asteroid
            glm::quat angm_;
            glm::quat orientation_; // Orientation of player
            glm::vec3 forward_; // Initial forward vector
            glm::vec3 side_; // Initial side vector
            glm::vec3 position_; // Position of camera

            glm::vec3 floor_scale_;
            glm::vec3 floor_pos_;
            std::vector<std::vector<bool>> impassable_map_;
            
            int num_wheels_;
            int num_antennas_;
            SceneNode **wheels_;
            SceneNode **antennas_;
            glm::vec3 *offsets_;
            
    }; // class Player

} // namespace game

#endif // PLAYER_H_