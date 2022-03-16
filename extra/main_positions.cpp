// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"

// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

// Timer
#include <chrono>
#include <iostream>

using namespace std;



int main(void)
{
    glm::vec3 a = glm::vec3(1.f,1.f,1.f);
    glm::vec3 b = glm::vec3(1.f,1.f,1.5f);

    if (a == b)
    {
        cout << "here" << endl;
    }

    return 0;
}