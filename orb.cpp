#include "orb.h"

namespace game {

Orb::Orb(const std::string name, const Resource *geometry, const Resource *material, const Resource* texture, SceneNode* particles) : SceneNode(name, geometry, material, texture), particles_(particles) {
}


Orb::~Orb(){
}


glm::quat Orb::GetAngM(void) const {

    return angm_;
}


void Orb::SetAngM(glm::quat angm){

    angm_ = angm;
}

void Orb::SetRadius(float radius){
    radius_ = radius;
}

void Orb::Update(std::vector<std::vector<float>> height_values, float length, float width){

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

        height = 4.0 + floor_pos_.y + (height/5.0f) * floor_scale_.y;

        SetPosition(glm::vec3(position.x, height, position.z));
        particles_->SetPosition(GetPosition());
    }
}

bool Orb::Colliding(glm::vec3 position, float radius){
    if(glm::distance(position, GetPosition()) < radius + radius_){
        return true; 
    }
    return false;
}

void Orb::SetImpassableMap(std::vector<std::vector<bool>> impassable_map) {
    impassable_map_ = impassable_map;
}

void Orb::SetFloorScale(glm::vec3 floor_scale) {
    floor_scale_ = floor_scale;
}

void Orb::SetFloorPos(glm::vec3 floor_pos) {
    floor_pos_ = floor_pos;
}

void Orb::Update(void){
    particles_->SetPosition(GetPosition());
    particles_->Update();
    Rotate(angm_);
    // SceneNode::Update();
}

void Orb::Draw(Camera *camera){
    particles_->Draw(camera);
    SceneNode::Draw(camera);
}

} // namespace game
