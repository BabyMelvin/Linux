#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// define serveral possible options for camera movement,
// used as abstraction to stay away form window-system specific input methods

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

// default camera value
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.f;

// An abstract camera class that processed input and caculates the corresponding Euler Angles,
// Vectors and Metrices for use in OpenGL

class Camera
{
public:
    // camera Attributes
    glm::vec3 Postion;
    glm::vec3 Front;
    glm::vec3 UP;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 positon=glm::vec3(0.0f, 0.0f,0.0f),glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH){

        }
};
#endif /* CAMERA_H */