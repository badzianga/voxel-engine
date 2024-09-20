#include "Timer.hpp"
#include <GLFW/glfw3.h>

void Timer::start() {
    m_startTime = static_cast<float>(glfwGetTime());
}

float Timer::getElapsedTime() const {
    return static_cast<float>(glfwGetTime()) - m_startTime;
}
