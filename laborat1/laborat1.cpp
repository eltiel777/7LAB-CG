#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "Model.h"

const unsigned int SCR_WIDTH = 512;
const unsigned int SCR_HEIGHT = 512;

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
float fov = 45.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float moveBase = 0.0f;
float rotateJoint1 = 0.0f;
float rotateJoint2 = 0.0f;
float rotateJoint3 = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

int main() {
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Robot Arm", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    glEnable(GL_DEPTH_TEST);

    Shader myShader("vertex.glsl", "fragment.glsl");
    Model ourModel("lab322.obj");

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glUniform3f(glGetUniformLocation(myShader.ID, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(glGetUniformLocation(myShader.ID, "material.ambient"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(myShader.ID, "material.diffuse"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(myShader.ID, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(myShader.ID, "material.shininess"), 32.0f);

        glUniform3f(glGetUniformLocation(myShader.ID, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(myShader.ID, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(myShader.ID, "light.diffuse"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(myShader.ID, "light.specular"), 1.0f, 1.0f, 1.0f);

     
        glm::mat4 modelMatrices[4];

        modelMatrices[0] = glm::mat4(1.0f);
        modelMatrices[0] = glm::translate(modelMatrices[0], glm::vec3(moveBase, 0.0f, 0.0f));

       
        modelMatrices[1] = modelMatrices[0];
        modelMatrices[1] = glm::translate(modelMatrices[1], glm::vec3(-0.20815f, 0.20252f, -0.57073f));
        modelMatrices[1] = glm::rotate(modelMatrices[1], glm::radians(rotateJoint1), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrices[1] = glm::translate(modelMatrices[1], glm::vec3(0.20815f, -0.20252f, 0.57073f));

        modelMatrices[2] = modelMatrices[1];
        modelMatrices[2] = glm::translate(modelMatrices[2], glm::vec3(0.023179f, 0.45585f, -1.6227f));
        modelMatrices[2] = glm::rotate(modelMatrices[2], glm::radians(rotateJoint2), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrices[2] = glm::translate(modelMatrices[2], glm::vec3(-0.023179f, -0.45585f, 1.6227f));

        
        modelMatrices[3] = modelMatrices[2];
        modelMatrices[3] = glm::translate(modelMatrices[3], glm::vec3(0.03052f, 0.22222f, -3.3283f));
        modelMatrices[3] = glm::rotate(modelMatrices[3], glm::radians(rotateJoint3), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrices[3] = glm::translate(modelMatrices[3], glm::vec3(-0.03052f, -0.22222f, 3.3283f));

        ourModel.Draw(myShader, modelMatrices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(myShader.ID);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) moveBase += deltaTime * 2.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) moveBase -= deltaTime * 2.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) rotateJoint1 += deltaTime * 50.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) rotateJoint1 -= deltaTime * 50.0f;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) rotateJoint2 += deltaTime * 50.0f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) rotateJoint2 -= deltaTime * 50.0f;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) rotateJoint3 += deltaTime * 50.0f;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) rotateJoint3 -= deltaTime * 50.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
