#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Shader.hpp"
#include "DebugWindow.hpp"
#include "Texture.hpp"
#include "World.hpp"

void mouseCallback(GLFWwindow* window, double xPosIn, double yPosIn);
void processInput(GLFWwindow* window);

constexpr bool vSyncEnabled = false;

constexpr uint32_t SCR_WIDTH = 1280;
constexpr uint32_t SCR_HEIGHT = 720;

constexpr float CAMERA_SPEED = 16.f;

glm::vec3 cameraPos = World::getCenter();
glm::vec3 cameraFront = glm::vec3(0.f, 0.f, -1.f);
const glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);

bool firstMouse = true;
float yaw = -90.f;
float pitch = 0.f;
float lastX = SCR_WIDTH / 2.f;
float lastY = SCR_HEIGHT / 2.f;

float deltaTime = 0.f;
float lastTime = 0.f;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MineCppraft", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);

    glfwSwapInterval(vSyncEnabled);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (not gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    {
        World world;
        Shader shader{ "shaders/chunk.vert", "shaders/chunk.frag" };
        Texture texture{ "assets/frame.png" };

        DebugWindow::init();
        DebugWindow debugWindow{ window };

        while (not glfwWindowShouldClose(window)) {
            auto currentTime = static_cast<float>(glfwGetTime());
            deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            processInput(window);

            glClearColor(0.f, 0.7f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 projection = glm::perspective(glm::radians(60.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.f);
            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            shader.use();
            shader.setMat4("u_projection", projection);
            shader.setMat4("u_view", view);
            texture.use();

            world.render(shader);

            debugWindow.display(cameraPos, cameraFront);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    auto cameraSpeed = static_cast<float>(CAMERA_SPEED * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 front{ cameraFront.x, 0.f, cameraFront.z };
        cameraPos += cameraSpeed * glm::normalize(front);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 front{ cameraFront.x, 0.f, cameraFront.z };
        cameraPos -= cameraSpeed * glm::normalize(front);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}

void mouseCallback(GLFWwindow* window, double xPosIn, double yPosIn) {
    (void)window;

    auto xPos = static_cast<float>(xPosIn);
    auto yPos = static_cast<float>(yPosIn);

    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    const float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    front.y = std::sin(glm::radians(pitch));
    front.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
