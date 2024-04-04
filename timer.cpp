#include <GLFW/glfw3.h>
#include <iostream>

#include "timer.h"

namespace game {

Timer::Timer(void)
{
    end_time_ = -1.0f;
}


Timer::~Timer(void)
{
}


void Timer::Start(float end_time)
{
    // Set the end time to the current time plus the time to wait
    float current_time = glfwGetTime();
    if (end_time_ == -1.0f) {
        end_time_ = current_time + end_time;
        // std::cout<<"Timer started with end time: "<<end_time_<<" | Current time is: "<<current_time<<std::endl;
	}
}



bool Timer::Finished(void)
{
    double current_time = glfwGetTime();

    //Check if the current time is greater than the end time
    if (current_time >= end_time_ && end_time_ != -1.0f) {
        end_time_ = -1.0f;
		return true;
    }
    return false;
}

float Timer::TimeLeft(void) {
    if (Running()) {
        float currentTime = glfwGetTime();
        float timeLeft = end_time_ - currentTime;
        return timeLeft > 0 ? timeLeft : 0.0f;
    }
    return 0.0f;
}

void Timer::Stop(void) {
    end_time_ = -1.0f; // Indicate that the timer is not running
}

} // namespace game
