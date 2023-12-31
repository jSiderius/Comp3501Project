#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "camera.h"

namespace game {

Camera::Camera(void){
}


Camera::~Camera(){
}

void Camera::Update(std::vector<std::vector<float>> height_values){

}


glm::vec3 Camera::GetPosition(void) const {
    return position_;
}


glm::quat Camera::GetOrientation(void) const {
    return orientation_;
}

float Camera::GetSpeed(void){
    return speed_;
}

float Camera::GetMaxSpeed(void){
    return max_speed_;
}

void Camera::SetSpeed(float speed){
    if(0 < speed && speed < max_speed_){ speed_ = speed; }
}

void Camera::SetMaxSpeed(float max_speed){
    max_speed_ = max_speed;
}

void Camera::SetImpassableMap(std::vector<std::vector<bool>> impassable_map) {
    impassable_map_ = impassable_map;
}

void Camera::SetFloorScale(glm::vec3 floor_scale) {
    floor_scale_ = floor_scale;
}

void Camera::SetFloorPos(glm::vec3 floor_pos) {
    floor_pos_ = floor_pos;
}

void Camera::SetPosition(glm::vec3 position){
    position_ = position;
}


void Camera::SetOrientation(glm::quat orientation){
    orientation_ = orientation;
}


void Camera::Translate(glm::vec3 trans){
    
}


void Camera::Rotate(glm::quat rot){
    // apply this rotation to the camera's current orientation
    orientation_ = rot * orientation_;
    orientation_ = glm::normalize(orientation_);
}


glm::vec3 Camera::GetForward(void) const {

    glm::vec3 current_forward = orientation_ * forward_;
    return -current_forward; // Return -forward since the camera coordinate system points in the opposite direction
}


glm::vec3 Camera::GetSide(void) const {
    // how do you get the side vector?
    glm::vec3 current_side = orientation_ * glm::vec3(1, 0, 0);
    return current_side;
}


glm::vec3 Camera::GetUp(void) const {
    // how do you get the up vector?
    glm::vec3 current_up = orientation_ * glm::vec3(0, 1, 0);
    return current_up;
}


void Camera::Pitch(float angle){
    // put the proper functionality in here
    //based on up / down arrow keys
    glm::quat rotation = glm::angleAxis(angle, glm::vec3(1,0,0));
    orientation_ = orientation_ * rotation;
    orientation_ = glm::normalize(orientation_);
}


void Camera::Yaw(float angle){

    glm::quat rotation = glm::angleAxis(angle, glm::vec3(0,1,0));
    orientation_ = orientation_ * rotation;
    orientation_ = glm::normalize(orientation_);
}


void Camera::Roll(float angle){

    glm::quat rotation = glm::angleAxis(angle, orientation_ * forward_); // what axis is used for rolling?
    orientation_ = rotation * orientation_; // update the orientation
    orientation_ = glm::normalize(orientation_);
}


void Camera::SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up){

    // Store initial forward and side vectors
    // See slide in "Camera control" for details
    // probably will not be much used (directly setting view a rare occurrence in games)
    forward_ = look_at - position;
    forward_ = -glm::normalize(forward_);
    side_ = glm::cross(up, forward_);
    side_ = glm::normalize(side_);

    // Reset orientation and position of camera
    position_ = position;
    orientation_ = glm::quat();
}


void Camera::SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h){

    // Set projection based on field-of-view
    float top = tan((fov/2.0)*(glm::pi<float>()/180.0))*near;
    float right = top * w/h;
    projection_matrix_ = glm::frustum(-right, right, -top, top, near, far);
}


void Camera::SetupShader(GLuint program){

    // Update view matrix
    SetupViewMatrix();

    // Set view matrix in shader
    GLint view_mat = glGetUniformLocation(program, "view_mat");
    glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view_matrix_));
    
    // Set projection matrix in shader
    GLint projection_mat = glGetUniformLocation(program, "projection_mat");
    glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(projection_matrix_));
}


void Camera::SetupViewMatrix(void){

    //view_matrix_ = glm::lookAt(position, look_at, up);

    // Get current vectors of coordinate system
    // [side, up, forward]
    // See slide in "Camera control" for details
    glm::vec3 current_forward = orientation_ * forward_;
    glm::vec3 current_side = orientation_ * side_;
    glm::vec3 current_up = glm::cross(current_forward, current_side);
    current_up = glm::normalize(current_up);

    // Initialize the view matrix as an identity matrix
    view_matrix_ = glm::mat4(1.0); 

    // Copy vectors to matrix
    // Add vectors to rows, not columns of the matrix, so that we get
    // the inverse transformation
    // Note that in glm, the reference for matrix entries is of the form
    // matrix[column][row]
    view_matrix_[0][0] = current_side[0]; // First row
    view_matrix_[1][0] = current_side[1];
    view_matrix_[2][0] = current_side[2];
    view_matrix_[0][1] = current_up[0]; // Second row
    view_matrix_[1][1] = current_up[1];
    view_matrix_[2][1] = current_up[2];
    view_matrix_[0][2] = current_forward[0]; // Third row
    view_matrix_[1][2] = current_forward[1];
    view_matrix_[2][2] = current_forward[2];

    // Create translation to camera position
    glm::mat4 trans = glm::translate(glm::mat4(1.0), -position_);

    // Combine translation and view matrix in proper order
    view_matrix_ *= trans;
}

} // namespace game
