#include "player.h"

namespace game {

Player::Player(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture, SceneNode **wheels, int num_wheels, SceneNode **antennas, int num_antennas, float floor_length, float floor_width) : SceneNode(name, geometry, material, texture) {
    num_wheels_ = num_wheels;
    num_antennas_ = num_antennas;

    wheels_ = new SceneNode*[num_wheels_];
    antennas_ = new SceneNode*[num_antennas_];
    offsets_ = new glm::vec3[(num_wheels_ + num_antennas_)];

    floor_length_ = floor_length;
    floor_width_ = floor_width;

    for (int i = 0; i < num_wheels_; i++){
        wheels_[i] = wheels[i];
    }
    for (int i = 0; i < num_antennas_; i++){
        antennas_[i] = antennas[i];
    }

    offsets_[0] = glm::vec3(-0.6, -0.2, -1.0);
    offsets_[1] = glm::vec3(-0.6, -0.2, 0.0);
    offsets_[2] = glm::vec3(-0.6, -0.2, 1.0);
    offsets_[3] = glm::vec3(0.6, -0.2, -1.0);
    offsets_[4] = glm::vec3(0.6, -0.2, 0.0);
    offsets_[5] = glm::vec3(0.6, -0.2, 1.0);
    offsets_[6] = glm::vec3(0.35, 0.6, 1.0);
    offsets_[7] = glm::vec3(0.335, 1.2, 1.0);
}


Player::~Player(){
}


glm::quat Player::GetAngM(void) const {

    return angm_;
}


void Player::SetAngM(glm::quat angm){

    angm_ = angm;
}

void Player::Translate(glm::vec3 trans){
    for (int i = 0; i < num_wheels_; i++){
        wheels_[i]->Rotate(glm::angleAxis(0.05f, glm::vec3(GetOrientation() * glm::vec4(offsets_[i], 0.0f))));
    }

    glm::vec3 temp_pos = SceneNode::GetPosition() + trans;

    int x = floor(((temp_pos.x - floor_pos_.x) / (floor_length_ * floor_scale_.x) * impassable_map_.size()));
    int z = floor((-(temp_pos.z - floor_pos_.z) / (floor_width_ * floor_scale_.z) * impassable_map_[0].size()));

    if (impassable_map_[x][z]) {
    SceneNode::Translate(trans);
    }

}

void Player::Update(void){
    Rotate(angm_);

    antennas_[1]->Rotate(glm::angleAxis(1.0f, glm::vec3(0,0.1,0))); 
}

void Player::Draw(Camera *camera){
    SceneNode::Draw(camera);
    for (int i = 0; i < num_wheels_; i++){
        glm::vec3 offsetInWorldSpace = glm::vec3(GetOrientation() * glm::vec4(offsets_[i], 0.0f));
        wheels_[i]->SetPosition(GetPosition() + offsetInWorldSpace);
        wheels_[i]->Draw(camera);
    }
    for (int i = 0; i < num_antennas_; i++){
        glm::vec3 offsetInWorldSpace = glm::vec3(GetOrientation() * glm::vec4(offsets_[num_wheels_ + i], 0.0f));
        antennas_[i]->SetPosition(GetPosition() + offsetInWorldSpace);
        antennas_[i]->Draw(camera);
    }
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

void Player::Update(std::vector<std::vector<float>> height_values, float length, float width){

    int length_count = height_values.size();
    int width_count = height_values[0].size();
    glm::vec3 position = GetPosition();
    float x = ((position.x - floor_pos_.x) / (length * floor_scale_.x) * length_count);
    float z = (-(position.z - floor_pos_.z) / (width * floor_scale_.z) * width_count);

    if ((length_count > floor(x)) && (floor(x) >= 0) && (width_count > floor(z)) && (floor(z) >= 0)) {

        float a = height_values[floor(x)][ceil(z)];
        float b = height_values[ceil(x)][ceil(z)];
        float c = height_values[floor(x)][floor(z)];
        float d = height_values[ceil(x)][floor(z)];

        float s = x - floor(x);
        float t = z - floor(z);

        float height = (1 - t) * ((1 - s) * a + s * b) + (t * ((1 - s) * c + s * d));

        height = 7.0 + floor_pos_.y + (height/5.0f) * floor_scale_.y;

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