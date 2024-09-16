#include "DebugWindow.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

DebugWindow::DebugWindow(GLFWwindow* window) : m_io(ImGui::GetIO()) {
    m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");
}

DebugWindow::~DebugWindow() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugWindow::init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
}

void DebugWindow::display(glm::vec3 cameraPos, glm::vec3 cameraFront) const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug panel");
    ImGui::SetWindowPos({ 16, 16 });
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / m_io.Framerate, m_io.Framerate);
    ImGui::Text("Camera position: %.3f %.3f %.3f", cameraPos.x, cameraPos.y, cameraPos.z);
    ImGui::Text("Camera front: %.3f %.3f %.3f", cameraFront.x, cameraFront.y, cameraFront.z);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
