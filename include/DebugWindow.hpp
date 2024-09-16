#pragma once
#include <glm/glm.hpp>

struct GLFWwindow;
struct ImGuiIO;

class DebugWindow {
public:
    explicit DebugWindow(GLFWwindow* p_window);
    ~DebugWindow();
    void display(glm::vec3 cameraPos, glm::vec3 cameraFront) const;
    static void init();
private:
    ImGuiIO& m_io;
};
