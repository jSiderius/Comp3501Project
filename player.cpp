#include "player.h"

namespace game {

Player::Player(const std::string name, const Resource *geometry, const Resource *material) : SceneNode(name, geometry, material) {
}


Player::~Player(){
}


glm::quat Player::GetAngM(void) const {

    return angm_;
}


void Player::SetAngM(glm::quat angm){

    angm_ = angm;
}


void Player::Update(void){
    Rotate(angm_);
}

void Player::Pitch(float angle){
    glm::quat rotation = glm::angleAxis(angle, glm::vec3(1,0,0));
    SetOrientation(glm::normalize(GetOrientation() * rotation));
}


void Player::Yaw(float angle){

    glm::quat rotation = glm::angleAxis(angle, glm::vec3(0,1,0));
    SetOrientation(glm::normalize(GetOrientation() * rotation));
}


void Player::Roll(float angle){

    glm::quat rotation = glm::angleAxis(angle, orientation_ * forward_); // what axis is used for rolling?
    SetOrientation(glm::normalize(GetOrientation() * rotation));
}

glm::vec3 Player::GetForward(void) const {
    glm::vec3 current_forward = GetOrientation() * forward_;
    return -current_forward; // Return -forward since the camera coordinate system points in the opposite direction
}


glm::vec3 Player::GetSide(void) const {
    glm::vec3 current_side = GetOrientation() * glm::vec3(1, 0, 0); //likely wrong idk check back assignment feedback
    return current_side;
}


glm::vec3 Player::GetUp(void) const {
    glm::vec3 current_up = GetOrientation() * glm::vec3(0, 1, 0);
    return current_up;
}

void Player::SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up){
    // Store initial forward and side vectors
    forward_ = look_at - position;
    forward_ = -glm::normalize(forward_);
    side_ = glm::cross(up, forward_);
    side_ = glm::normalize(side_);

    // Reset orientation and position of camera
    position_ = position;
    orientation_ = glm::quat();
}

void Player::Update(std::vector<std::vector<float>> height_values){

    glm::vec3 position = GetPosition();
    float x = ((position.x - floor_pos_.x) / (200.0 * floor_scale_.x) * 360);
    float z = (-(position.z - floor_pos_.z) / (200.0 * floor_scale_.z) * 360);

    if ((360 > floor(x)) && (floor(x) >= 0) && (360 > floor(z)) && (floor(z) >= 0)) {

        float a = height_values[floor(x)][ceil(z)];
        float b = height_values[ceil(x)][ceil(z)];
        float c = height_values[floor(x)][floor(z)];
        float d = height_values[ceil(x)][floor(z)];

        float s = x - floor(x);
        float t = z - floor(z);

        float height = (1 - t) * ((1 - s) * a + s * b) + (t * ((1 - s) * c + s * d));

        height = 7.0 + floor_pos_.y + (height/15.0f) * floor_scale_.y;

        SetPosition(glm::vec3(position.x, height, position.z));
    }

    //  (1-t)*( (1-s)*a+s*b)) + t*( (1-s)*c+s*d)

    // float trans_factor = 0.5;
    // Translate(GetForward()*trans_factor*speed_);
}

void Player::SetImpassableMap(std::vector<std::vector<bool>> impassable_map) {
    impassable_map_ = impassable_map;
}

void Player::SetFloorScale(glm::vec3 floor_scale) {
    floor_scale_ = floor_scale;
}

void Player::SetFloorPos(glm::vec3 floor_pos) {
    floor_pos_ = floor_pos;
}
            
} // namespace game