#pragma once

class Timer {
public:
    Timer() = default;
    void start();
    [[nodiscard]] float getElapsedTime() const;
private:
    float m_startTime;
};
