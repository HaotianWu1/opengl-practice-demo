// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif

// OpenGL Mathematics Library
#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

// Timer
#include <chrono>
#include <iostream>
#include <math.h>

using namespace std;


bool i_mode = false;
bool o_mode = false;
bool p_mode = false;
bool c_mode = false;
bool f_mode = false;

int i_n = 0;
bool dum_ver = false;
bool fir_ver =true;
float o_x = -99;
float o_y = -99;
glm::vec3 o_1 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 o_2 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 o_3 = glm::vec3(0.0f, 0.0f, 0.0f);


float dis_x = 0;
float dis_y = 0;

int o_t = -99;
int p_t = -99;
int r_t = -99;
bool hold = false;
int c_i = -99;

float zoom =1.0f;
float m_x = 0.0f;
float m_y = 0.0f;
glm::mat4 mov = glm::mat4(1.f);
auto t_start = std::chrono::high_resolution_clock::now();



// VertexBufferObject wrapper
VertexBufferObject VBO;
VertexBufferObject VBO_C;

// Contains the vertex positions
//Eigen::MatrixXf V(2,3);
std::vector<glm::vec2> V;
std::vector<glm::vec3> C;
std::vector<glm::vec3> C_T;

glm::mat4 view;

int find_ver(float x, float y, vector<glm::vec2> a)
{
    float min_dis = pow(2, 30);
    int res = -99;
    for (int i = 0; i < a.size(); i++)
    {
        float xx = a[i][0];
        float yy = a[i][1];
        float temp_dis = pow((pow(xx - x, 2)+pow(yy - y, 2)), 0.5);
        if (temp_dis <= min_dis)
        {
            min_dis = temp_dis;
            res = i;
        }
    }
    return res;
}

vector<glm::vec3> black(vector<glm::vec3> a)
{
    vector<glm::vec3> t;
    for (int i = 0; i < a.size(); i++)
    {
        t.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    return t;
}

vector<float> scale(vector<glm::vec2> a, float ratio, int id)
{
    vector<float> ans;
    float x1 = a[id][0];
    float y1 = a[id][1];
    float x2 = a[id+1][0];
    float y2 = a[id+1][1];
    float x3 = a[id+2][0];
    float y3 = a[id+2][1];

    float cx = (x1 + x2 + x3) / 3;
    float cy = (y1 + y2 + y3) / 3;

    float nx1 = cx + (x1 - cx) * (1 + ratio);
    float ny1 = cy + (y1 - cy) * (1 + ratio);

    ans.push_back(nx1);
    ans.push_back(ny1);

    float nx2 = cx + (x2 - cx) * (1 + ratio);
    float ny2 = cy + (y2 - cy) * (1 + ratio);

    ans.push_back(nx2);
    ans.push_back(ny2);

    float nx3 = cx + (x3 - cx) * (1 + ratio);
    float ny3 = cy + (y3 - cy) * (1 + ratio);

    ans.push_back(nx3);
    ans.push_back(ny3);

    return ans;


}

vector<float> rotate(vector<glm::vec2> a, float ang, int id)
{
    vector<float> ans;
    float angle = ang * 3.14159265 / 180.0;
    float x1 = a[id][0];
    float y1 = a[id][1];
    float x2 = a[id+1][0];
    float y2 = a[id+1][1];
    float x3 = a[id+2][0];
    float y3 = a[id+2][1];

    float cx = (x1 + x2 + x3) / 3;
    float cy = (y1 + y2 + y3) / 3;

    float nx1 = cx + cos(angle) * (x1 - cx) - sin(angle) * (y1 - cy);
    float ny1 = cy + sin(angle) * (x1 - cx) + cos(angle) * (y1 - cy);

    ans.push_back(nx1);
    ans.push_back(ny1);

    float nx2 = cx + cos(angle) * (x2 - cx) - sin(angle) * (y2 - cy);
    float ny2 = cy + sin(angle) * (x2 - cx) + cos(angle) * (y2 - cy);

    ans.push_back(nx2);
    ans.push_back(ny2);

    float nx3 = cx + cos(angle) * (x3 - cx) - sin(angle) * (y3 - cy);
    float ny3 = cy + sin(angle) * (x3 - cx) + cos(angle) * (y3 - cy);

    ans.push_back(nx3);
    ans.push_back(ny3);

    return ans;
}

float sig (float x11, float y11, float x22, float y22, float x33, float y33)
{
    return (x11 - x33) * (y22 - y33) - (x22 - x33) * (y11 - y33);
}

int check_in_side(std::vector<glm::vec2> a, float posx, float posy)
{
    int res = a.size() / 3;
    vector<glm::vec2> temp;
    temp = vector<glm::vec2>(a.begin(), a.begin() + res * 3);


    for (int i = 0; i < temp.size(); i = i + 3)
    {
        float x1 = temp[i][0];
        float y1 = temp[i][1];
        float x2 = temp[i+1][0];
        float y2 = temp[i+1][1];
        float x3 = temp[i+2][0];
        float y3 = temp[i+2][1];

        float d1 = sig(posx, posy, x1, y1, x2, y2);
        float d2 = sig(posx, posy, x2, y2, x3, y3);
        float d3 = sig(posx, posy, x3, y3, x1, y1);

        float has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        float has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        if(!(has_neg && has_pos))
        {
            return i;
        }
    }
    return -99;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    glm::vec4 p_screen(xpos,height-1-ypos,0,1);
    glm::vec4 p_canonical((p_screen.x/width)*2-1,(p_screen.y/height)*2-1,0,1);
    glm::vec4 p_world = glm::inverse(view) * p_canonical;

    double xworld = p_world.x;
    double yworld = p_world.y;

    if (i_mode == true)
    {
        if (i_n > 1)
        {
            V[V.size() -1] = glm::vec2(xworld,yworld);
        }

        VBO.update(V);
    }

    if (o_mode == true && hold == true && o_t != -99)
    {

        dis_x = (xworld - o_x);
        dis_y = (yworld - o_y);
        o_x = xworld;
        o_y = yworld;
        V[o_t] = glm::vec2( V[o_t][0] + dis_x, V[o_t][1] + dis_y);
        V[o_t+1] = glm::vec2(V[o_t+1][0] + dis_x, V[o_t+1][1] + dis_y);
        V[o_t+2] = glm::vec2(V[o_t+2][0] + dis_x, V[o_t+2][1] + dis_y);

        C[o_t] = glm::vec3(0.0f, 0.0f, 1.0f);
        C[o_t+1] = glm::vec3(0.0f, 0.0f, 1.0f);
        C[o_t+2] = glm::vec3(0.0f, 0.0f, 1.0f);
        VBO.update(V);
        VBO_C.update(C);
    }



}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the position of the mouse in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates

    // Convert screen position to world coordinates
    glm::vec4 p_screen(xpos,height-1-ypos,0,1);
    glm::vec4 p_canonical((p_screen.x/width)*2-1,(p_screen.y/height)*2-1,0,1);
    glm::vec4 p_world = glm::inverse(view) * p_canonical;

    double xworld = p_world.x;
    double yworld = p_world.y;



//    double xworld = ((xpos/double(width))*2)-1;
//    double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw


    if (i_mode == true)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            if (fir_ver == true)
            {
                V[V.size() - 1] = glm::vec2(xworld, yworld);
                V.push_back(glm::vec2(xworld, yworld));
                C.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
                fir_ver = false;
                i_n = 2;
            }
            else
            {
                if (i_n == 2)
                {
                    i_n = i_n + 1;
                    V[V.size() - 1] = glm::vec2(xworld, yworld);
                    V.push_back(glm::vec2(xworld, yworld));
                    C.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
                }
                else if (i_n == 3)
                {
                    i_n = 0;
                    V[V.size() - 1] = glm::vec2(xworld, yworld);
                }
                else
                {
                    i_n = 2;
                    V.push_back(glm::vec2(xworld, yworld));
                    V.push_back(glm::vec2(xworld, yworld));
                    C.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
                    C.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
                }
            }
        }
    }


    if (o_mode == true)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            if (o_x == -99)
            {
                hold = true;
                o_x = xworld;
                o_y = yworld;
                o_t = check_in_side(V, xworld, yworld);
                r_t = o_t;
                o_1 = C[o_t];
                o_2 = C[o_t+1];
                o_3 = C[o_t+2];
                C[o_t] = glm::vec3(0.0f, 0.0f, 1.0f);
                C[o_t+1] = glm::vec3(0.0f, 0.0f, 1.0f);
                C[o_t+2] = glm::vec3(0.0f, 0.0f, 1.0f);
            }
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            C[o_t] = o_1;
            C[o_t+1] = o_2;
            C[o_t+2] = o_3;
            hold = false;
            o_x = -99;
            o_y -= -99;
            o_t = -99;
            dis_x = 0;
            dis_y = 0;
        }
    }


    if(p_mode == true)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            p_t = check_in_side(V, xworld, yworld);
            if (p_t != -99)
            {
                V.erase (V.begin()+ p_t, V.begin() + p_t + 3);
                p_t = -99;
            }
        }
    }



    if (c_mode == true)
    {
        c_i = find_ver(xworld, yworld, V);
    }

    // Upload the change to the GPU
    VBO.update(V);
    VBO_C.update(C);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
    switch (key)
    {
        case  GLFW_KEY_A:
            if (action == GLFW_PRESS)
            {
                m_x = m_x - 0.4;
            }
            break;

        case  GLFW_KEY_D:
            if (action == GLFW_PRESS)
            {
                m_x = m_x + 0.4;
            }
            break;

        case  GLFW_KEY_W:
            if (action == GLFW_PRESS)
            {
                m_y = m_y + 0.4;
            }
            break;
        case  GLFW_KEY_S:
            if (action == GLFW_PRESS)
            {
                m_y = m_y - 0.4;
            }
            break;


        case  GLFW_KEY_EQUAL:
            if (action == GLFW_PRESS)
            {
                zoom = zoom + 0.2;
            }
            break;

        case  GLFW_KEY_MINUS:
            if (action == GLFW_PRESS)
            {
                zoom = zoom - 0.2;
            }
            break;

        case  GLFW_KEY_I:
            if (i_mode == true && action == GLFW_PRESS)
            {
                i_mode = false;
                cout << "i mode is off" << endl;
            }
            else if (action == GLFW_PRESS)
            {
                i_mode = true;
                cout << "i mode is on" << endl;
            }
            break;

        case  GLFW_KEY_O:
            if (o_mode == true && action == GLFW_PRESS)
            {
                o_mode = false;
                r_t = -99;
                cout << "o mode is off" << endl;
            }
            else if (action == GLFW_PRESS)
            {
                o_mode = true;
                cout << "o mode is on" << endl;
            }
            break;
        case  GLFW_KEY_P:
            if (p_mode == true && action == GLFW_PRESS)
            {
                p_mode = false;
                cout << "p mode is off" << endl;
            }
            else if (action == GLFW_PRESS)
            {
                p_mode = true;
                cout << "p mode is on" << endl;
            }
            break;

        case  GLFW_KEY_F:
            if (f_mode == true && action == GLFW_PRESS)
            {
                f_mode = false;
                cout << "f mode is off" << endl;
            }
            else if (action == GLFW_PRESS)
            {
                f_mode = true;
                t_start = std::chrono::high_resolution_clock::now();
                cout << "f mode is on" << endl;
            }
            break;

        case  GLFW_KEY_H:
            if (o_mode == true && action == GLFW_PRESS && r_t != -99)
            {
                vector<float> np = rotate(V, -10, r_t);

                V[r_t] = glm::vec2( np[0], np[1]);

                V[r_t+1] = glm::vec2( np[2], np[3]);

                V[r_t+2] = glm::vec2( np[4], np[5]);
                VBO.update(V);
            }
            break;

        case  GLFW_KEY_J:
            if (o_mode == true && action == GLFW_PRESS && r_t != -99)
            {
                vector<float> np = rotate(V, 10, r_t);

                V[r_t] = glm::vec2( np[0], np[1]);

                V[r_t+1] = glm::vec2( np[2], np[3]);

                V[r_t+2] = glm::vec2( np[4], np[5]);
                VBO.update(V);
            }
            break;

        case  GLFW_KEY_K:
            if (o_mode == true && action == GLFW_PRESS && r_t != -99)
            {
                vector<float> np = scale(V, 0.25, r_t);

                V[r_t] = glm::vec2( np[0], np[1]);

                V[r_t+1] = glm::vec2( np[2], np[3]);

                V[r_t+2] = glm::vec2( np[4], np[5]);

                VBO.update(V);
            }
            break;

        case  GLFW_KEY_L:
            if (o_mode == true && action == GLFW_PRESS && r_t != -99)
            {
                vector<float> np = scale(V, -0.25, r_t);

                V[r_t] = glm::vec2( np[0], np[1]);

                V[r_t+1] = glm::vec2( np[2], np[3]);

                V[r_t+2] = glm::vec2( np[4], np[5]);

                VBO.update(V);
            }
            break;


        case  GLFW_KEY_C:
            if (c_mode == true && action == GLFW_PRESS)
            {
                c_mode = false;
                cout << "c mode is off" << endl;
            }
            else if (action == GLFW_PRESS)
            {
                c_mode = true;
                cout << "c mode is on" << endl;
            }
            break;

        case  GLFW_KEY_1:
            if (c_mode == true && action == GLFW_PRESS && c_i != -99)
            {
                C[c_i] = glm::vec3( 1.0f, 0.0f, 0.1f);
                VBO_C.update(C);
            }
            break;

        case  GLFW_KEY_2:
            if (c_mode == true && action == GLFW_PRESS && c_i != -99)
            {
                C[c_i] = glm::vec3( 1.0f, 0.0f, 0.5f);
                VBO_C.update(C);
            }
            break;

        case  GLFW_KEY_3:
            if (c_mode == true && action == GLFW_PRESS && c_i != -99)
            {
                C[c_i] = glm::vec3( 1.0f, 0.0f, 0.9f);
                VBO_C.update(C);
            }
            break;

        case  GLFW_KEY_4:
            if (c_mode == true && action == GLFW_PRESS && c_i != -99)
            {
                C[c_i] = glm::vec3( 0.1f, 1.0f, 0.0f);
                VBO_C.update(C);
            }
            break;

        case  GLFW_KEY_5:
            if (c_mode == true && action == GLFW_PRESS && c_i != -99)
            {
                C[c_i] = glm::vec3( 0.5f, 1.0f, 0.0f);
                VBO_C.update(C);
            }
            break;

        case  GLFW_KEY_6:
            if (c_mode == true && action == GLFW_PRESS && c_i != -99)
            {
                C[c_i] = glm::vec3( 0.9f, 1.0f, 0.0f);
                VBO_C.update(C);
            }
            break;

        case  GLFW_KEY_7:
            if (c_mode == true && action == GLFW_PRESS && c_i != -99)
            {
                C[c_i] = glm::vec3( 0.0f, 0.1f, 1.0f);
                VBO_C.update(C);
            }
            break;

        case  GLFW_KEY_8:
            if (c_mode == true && action == GLFW_PRESS && c_i != -99)
            {
                C[c_i] = glm::vec3( 0.0f, 0.5f, 1.0f);
                VBO_C.update(C);
            }
            break;

        case  GLFW_KEY_9:
            if (c_mode == true && action == GLFW_PRESS && c_i != -99)
            {
                C[c_i] = glm::vec3( 0.0f, 0.9f, 1.0f);
                VBO_C.update(C);
            }
            break;











        default:
            break;
    }

    // Upload the change to the GPU
    VBO.update(V);
}

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

#ifndef __APPLE__
    glewExperimental = true;
    GLenum err = glewInit();
    if(GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialize the VAO
    // A Vertex Array Object (or VAO) is an object that describes how the vertex
    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
    // the VAO is not the actual object storing the vertex data,
    // but the descriptor of the vertex data.
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    // Initialize the VBO with the vertices data
    // A VBO is a data container that lives in the GPU memory
    VBO.init();
    VBO_C.init();


    V.push_back(glm::vec2(0, 0));
    VBO.update(V);

    C.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    VBO_C.update(C);

    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
    const GLchar* vertex_shader =
            "#version 150 core\n"
            "in vec2 position;"
            "in vec3 color;"
            "out vec3 f_color;"
            "uniform mat4 view;"
            "void main()"
            "{"
            "    gl_Position = view * vec4(position, 0.0, 1.0);"
            "    f_color = color;"
            "}";
    const GLchar* fragment_shader =
            "#version 150 core\n"
            "in vec3 f_color;"
            "out vec4 outColor;"
            "uniform vec3 triangleColor;"
            "void main()"
            "{"
            "    outColor = vec4(f_color, 1.0);"
            "}";

    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader,fragment_shader,"outColor");
    program.bind();

    // The vertex shader wants the position of the vertices as an input.
    // The following line connects the VBO we defined above with the position "slot"
    // in the vertex shader
    program.bindVertexAttribArray("position",VBO);
    program.bindVertexAttribArray("color",VBO_C);

    // Save the current time --- it will be used to dynamically change the triangle color
    auto t_start = std::chrono::high_resolution_clock::now();

    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Bind your VAO (not necessary if you have only one)
        VAO.bind();

        // Bind your program
        program.bind();

        // Get size of the window
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect_ratio = float(height)/float(width); // corresponds to the necessary width scaling

        view = glm::scale(glm::mat4(1.f), glm::vec3(zoom, zoom, zoom));
        view = glm::translate(view, glm::vec3(m_x, m_y, 0.0f));
        //view = glm::scale(glm::mat4(1.f), glm::vec3(aspect_ratio, 1.f, 1.f));

        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, glm::value_ptr(view));


        // Clear the framebuffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        int tri = V.size() / 3;

        if (V.size() % 3 == 0 && i_n == 3)
        {
            tri = tri - 1;
        }


        for (int i = 0; i < tri * 3; i = i + 3)
        {
            glDrawArrays(GL_TRIANGLES, i, 3);

            C_T = C;
            C = black(C);
            VBO_C.update(C);
            glDrawArrays(GL_LINE_LOOP, i, 3);
            C = C_T;
            VBO_C.update(C);
        }



        if (i_mode && i_n >= 2)
        {
            C_T = C;
            C = black(C);
            VBO_C.update(C);
            glDrawArrays(GL_LINE_LOOP, tri * 3, i_n);
            C = C_T;
            VBO_C.update(C);
        }

        if (f_mode)
        {
            auto t_now = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
            if (V.size() >= 4 && time > 0.05)
            {
                t_start = std::chrono::high_resolution_clock::now();
                vector<float> np = rotate(V, -1, 0);

                V[0] = glm::vec2( np[0], np[1]);

                V[1] = glm::vec2( np[2], np[3]);

                V[2] = glm::vec2( np[4], np[5]);

                VBO.update(V);
            }
        }





        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    VBO.free();
    VBO_C.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}