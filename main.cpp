#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include "DebugWindow.hpp"
#include "Texture.hpp"
#include "World.hpp"
#include "Logger.hpp"

void mouseCallback(GLFWwindow* window, double xPosIn, double yPosIn);
void processInput(GLFWwindow* window);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                            GLsizei length, const char *message, const void *userParam);

constexpr bool vSyncEnabled = false;

constexpr uint32_t SCR_WIDTH = 1280;
constexpr uint32_t SCR_HEIGHT = 720;

constexpr float CAMERA_SPEED = 16.f;

glm::vec3 cameraPos = glm::vec3{ 0.f, 80.f, 0.f };
glm::vec3 cameraFront = glm::vec3(0.f, 0.f, -1.f);
constexpr glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);

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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Engine", nullptr, nullptr);
    if (window == nullptr) {
        LOG_CRITICAL("Failed to create GLFW window!");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);

    glfwSwapInterval(vSyncEnabled);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (not gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_CRITICAL("Failed to initialize GLAD!");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        LOG_INFO("OpenGL Debug output enabled");
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

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                            GLsizei length, const char *message, const void* userParam) {
    (void)length;
    (void)userParam;
    // ignore non-significant error/warning codes
    if (id == 131169 or id == 131185 or id == 131218 or id == 131204) return;

    std::string logDetails = "OpenGL Debug source: ";
    switch (source) {
        case GL_DEBUG_SOURCE_API:             logDetails += "API";             break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   logDetails += "Window System";   break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: logDetails += "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     logDetails += "Third Party";     break;
        case GL_DEBUG_SOURCE_APPLICATION:     logDetails += "Application";     break;
        case GL_DEBUG_SOURCE_OTHER:           logDetails += "Other";           break;
        default:                              logDetails += "Unknown";         break;
    }

    logDetails += ", type: ";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               logDetails += "Error";               break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: logDetails += "Deprecated Behavior"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  logDetails += "Undefined Behavior";  break;
        case GL_DEBUG_TYPE_PORTABILITY:         logDetails += "Portability";         break;
        case GL_DEBUG_TYPE_PERFORMANCE:         logDetails += "Performance";         break;
        case GL_DEBUG_TYPE_MARKER:              logDetails += "Marker";              break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          logDetails += "Push Group";          break;
        case GL_DEBUG_TYPE_POP_GROUP:           logDetails += "Pop Group";           break;
        case GL_DEBUG_TYPE_OTHER:               logDetails += "Other";               break;
        default:                                logDetails += "Unknown";             break;
    }

    logDetails += ", severity: ";
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         logDetails += "High";         break;
        case GL_DEBUG_SEVERITY_MEDIUM:       logDetails += "Medium";       break;
        case GL_DEBUG_SEVERITY_LOW:          logDetails += "Low";          break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: logDetails += "Notification"; break;
        default:                             logDetails += "Unknown";      break;
    }

    LOG_ERROR(message);
    LOG_INFO(logDetails);

    std::exit(1);
}
