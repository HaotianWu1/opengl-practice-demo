// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"


// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr


#include <iostream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

// VertexBufferObject wrapper
VertexBufferObject VBO_v;

VertexBufferObject VBO_c;

// Contains the vertex positions
std::vector<glm::vec3> V;

std::vector<glm::vec3> C;


glm::mat4 view;

glm::mat4 projection;

bool pers = true;

bool ver_first = true;

float phi = 0;
float theta = 0;

float cam_x = 0.f;
float cam_y = 3.f;
float cam_z = 5.f;

glm::vec3 cam_pos = glm::vec3(cam_x, cam_y, cam_z);

float x_ang = 0;
float y_ang = 0;
float zoom = 30;

float screen_height = 480;
float screen_width = 640;

int selected_mesh = -1;

int shadow_color = 1;

float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

glm::vec3 cal_nor(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    glm::vec3 v1 = b - a;
    glm::vec3 v2 = c - a;
    glm::vec3 n = glm::cross(v1, v2);
    return glm::normalize(n);
}

glm::mat4 x_r_m(float ang)
{
    return glm::mat4 (1.f, 0.f, 0.f, 0.f,
                      0.f ,cos(ang), -sin(ang), 0,
                      0, sin(ang), cos(ang), 0,
                      0.f, 0.f, 0.f, 1.f
                      );
}

glm::mat4 y_r_m(float ang)
{
    return glm::mat4 (cos(ang), 0.f, sin(ang), 0.f,
                      0.f ,1.f, 0.f, 0.f,
                      -sin(ang), 0.f, cos(ang), 0.f,
                      0.f, 0.f, 0.f, 1.f
    );
}

glm::mat4 z_r_m(float ang)
{
    return glm::mat4 (cos(ang), -sin(ang), 0.f, 0.f,
                      sin(ang),cos(ang), 0.f, 0.f,
                      0.f, 0.f, 1.f, 0,
                      0.f, 0.f, 0.f, 1.f
    );
}

class mesh
{
public:
    int n_ver;
    int n_face;
    int n_edge;
    int shadermode;
    bool selected;

    glm::vec3 trans;
    glm::vec3 scal;
    float x_ang;
    float y_ang;
    float z_ang;

    glm::mat4 model;
    vector<glm::vec3> vers;
    vector<glm::vec3> norms;
    vector<glm::vec3> shad_vers;
    vector<vector<int>> faces;

    void update_mode()
    {
        model = glm::mat4(1.f);

        model = glm::scale(model, scal);

        glm::mat4 temp = y_r_m(y_ang) * z_r_m(z_ang) * x_r_m(x_ang);

        model = temp * model;

        glm::mat4 tr = glm::translate(glm::mat4(1.f), trans);

        model = tr * model;
    }
};

vector<mesh> meshes;

glm::vec3 get_avg_nor(glm::vec3 a, vector<glm::vec3> t)
{
    vector<glm::vec3> temp = t;
    vector<vector< glm::vec3 > > res;

    for(int i = 0; i < temp.size() - 3; i = i + 3)
    {
        vector<glm::vec3> sub;
        if (temp[i] == a || temp[i+1] == a || temp[i+2] == a )
        {
            sub.push_back(temp[i]);
            sub.push_back(temp[i+1]);
            sub.push_back(temp[i+2]);
            res.push_back(sub);
        }
    }

    glm::vec3 nn = glm::vec3(0.f,0.f,0.f);
    for (int i = 0; i < res.size(); i++)
    {
        nn += cal_nor(res[i][0], res[i][1], res[i][2]);
    }
     return nn / (float)res.size();
}


float sig (float x11, float y11, float x22, float y22, float x33, float y33)
{
    return (x11 - x33) * (y22 - y33) - (x22 - x33) * (y11 - y33);
}

bool check_in_side(glm::vec4 a, glm::vec4 b, glm::vec4 c, float posx, float posy)
{
    float x1 = a[0];
    float y1 = a[1];
    float x2 = b[0];
    float y2 = b[1];
    float x3 = c[0];
    float y3 = c[1];

    float d1 = sig(posx, posy, x1, y1, x2, y2);
    float d2 = sig(posx, posy, x2, y2, x3, y3);
    float d3 = sig(posx, posy, x3, y3, x1, y1);

    float has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    float has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    if(!(has_neg && has_pos))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void change_cam_pos(float x_ang, float y_ang)
{
    glm::vec3 origin = glm::vec3(cam_x, cam_y, cam_z);

    glm::mat3 x_rotat = glm::mat3(1, 0, 0,
                                  0, cos(x_ang), -sin(x_ang),
                                  0, sin(x_ang), cos(x_ang));

    glm::mat3 y_rotat = glm::mat3(cos(y_ang), 0, sin(y_ang),
                                  0, 1, 0,
                                  -sin(y_ang), 0, cos(y_ang));



    cam_pos = origin * x_rotat * y_rotat;


}

glm::mat4 cam_m ()
{
    glm::mat4 c = glm::mat4(1.f);

    c = glm::lookAt(cam_pos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));

    return c;
}

glm::mat4 pro_m()
{
    glm::mat4 pro;

    if (pers == true)
    {
        pro = projection = glm::perspective(glm::radians(zoom), (float)screen_width / (float)screen_height, 1.f, 100.0f);
    }
    else
    {
        pro = glm::ortho(
                (float)-screen_width / screen_height,
                (float)screen_width / screen_height,
                -1.f,
                1.0f,
                1.0f,
                100.0f
        );

    }
    return pro;
}

vector<glm::vec3> recenter_resize(vector<glm::vec3> a)
{
    vector<glm::vec3> vertexs = a;

    float x_max = -9999999;
    float y_max = -9999999;
    float z_max = -9999999;
    float x_min = 9999999;
    float y_min = 9999999;
    float z_min = 9999999;


    for(int i = 0; i < vertexs.size(); i++)
    {

        if (vertexs[i][0] >= x_max)
        {
            x_max = vertexs[i][0];
        }
        if (vertexs[i][0] <= x_min)
        {
            x_min = vertexs[i][0];
        }

        if (vertexs[i][1] >= y_max)
        {
            y_max = vertexs[i][1];
        }
        if (vertexs[i][1] <= y_min)
        {
            y_min = vertexs[i][1];
        }

        if (vertexs[i][2] >= z_max)
        {
            z_max = vertexs[i][2];
        }
        if (vertexs[i][2] <= z_min)
        {
            z_min = vertexs[i][2];
        }
    }


    float c_x = (x_max + x_min) / 2.f;
    float c_y = (y_max + y_min) / 2.f;
    float c_z = (z_max + z_min) / 2.f;

    for (int i = 0; i < vertexs.size(); i++)
    {
        vertexs[i][0] = vertexs[i][0] - c_x;
        vertexs[i][1] = vertexs[i][1] - c_y;
        vertexs[i][2] = vertexs[i][2] - c_z;
    }



    float x_len = x_max - x_min;
    float y_len = y_max - y_min;
    float z_len = z_max - z_min;




    float x_ratio = 1.f / x_len;
    float y_ratio = 1.f / y_len;
    float z_ratio = 1.f / z_len;

    for (int i = 0; i < vertexs.size(); i++)
    {
        vertexs[i][0] = vertexs[i][0] * x_ratio;
        vertexs[i][1] = vertexs[i][1] * y_ratio;
        vertexs[i][2] = vertexs[i][2] * z_ratio;
    }

    return vertexs;
}

vector<string> div_str(string str)
{
    vector<string> ans;
    std::string delim = " ";

    auto start = 0;
    auto end = str.find(delim);
    while (end != std::string::npos)
    {
        ans.push_back(str.substr(start, end - start));
        start = end + delim.length();
        end = str.find(delim, start);
    }
    if (str.substr(start, end).length() != 0)
    {
        ans.push_back(str.substr(start, end - start));
    }
    return ans;
}

void read_off(string path, bool sc)
{
    mesh temp;
    temp.trans = glm::vec3(0.f, 0.f,0.f);
    temp.scal = glm::vec3(1.f, 1.f,1.f);
    temp.x_ang = 0;
    temp.y_ang = 0;
    temp.z_ang = 0;
    temp.model = glm::mat4(1.0f);
    temp.shadermode = 2;
    temp.selected = false;
    bool para = false;
    ifstream infile(path);
    string line;
    vector<glm::vec3> raw_data;
    vector<int> ver_ind;
    while (std::getline(infile, line))
    {
        if(line[0] != '#')
        {
            vector<string> substrs = div_str(line);
            if (substrs.size() != 1)
            {
                if (para == false && substrs.size() == 3)
                {
                    temp.n_face = stoi(substrs[1]);
                    temp.n_edge = stoi(substrs[2]);
                    para = true;
                }
                else
                {
                    if (substrs.size() == 3)
                    {
                        raw_data.push_back(glm::vec3(stof(substrs[0]),stof(substrs[1]),stof(substrs[2])));
                    }
                    else if (substrs.size() == 4)
                    {
                        ver_ind.push_back(stoi(substrs[1]));
                        ver_ind.push_back(stoi(substrs[2]));
                        ver_ind.push_back(stoi(substrs[3]));
                    }
                    else
                    {
                        cerr << "unkown line" << endl;
                    }
                }
            }
        }
    }

    for(int i = 0; i < ver_ind.size(); i++)
    {
        temp.vers.push_back(raw_data[ver_ind[i]]);
        glm::vec4 shd = projection * view * temp.model * glm::vec4(raw_data[ver_ind[i]], 1.0);
        shd = shd / shd[3];
        temp.shad_vers.push_back(shd);
    }

    if (sc == true)
    {
        temp.vers = recenter_resize(temp.vers);
    }

    for(int i = 0; i < temp.vers.size(); i++)
    {
        glm::vec4 shd = projection * view * temp.model * glm::vec4(temp.vers[i], 1.0);
        shd = shd / shd[3];
        temp.shad_vers.push_back(shd);
    }



    temp.n_ver = temp.vers.size();

    for (int i = 0; i < temp.vers.size(); i++)
    {
        glm::vec3 ss = get_avg_nor(temp.vers[i],temp.vers);
        if (sc == true)
        {
            temp.norms.push_back(ss);
        }
        else
        {
            temp.norms.push_back(glm::vec3(0.f,1.f, 0.f));
        }
    }


    for(int i = 0; i < temp.n_ver; i++)
    {
        if (V.size() == 1 && ver_first == true)
        {
            V[0] = temp.vers[i];
            C[0] = temp.norms[i];
            ver_first = false;
        }
        else
        {
            V.push_back(temp.vers[i]);
            C.push_back(temp.norms[i]);
        }
    }

    meshes.push_back(temp);
    VBO_v.update(V);
    VBO_c.update(C);
}

void renderscene(Program program)
{
    int start = 0;
    for (int i = 0; i < meshes.size(); i++)
    {
        glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, glm::value_ptr(meshes[i].model));

        if (meshes[i].shadermode == 1)
        {
            glUniform1i(program.uniform("shademode"), 1);
            if(meshes[i].selected == true)
            {
                glUniform1i(program.uniform("selected"), 1);
            }
            else
            {
                glUniform1i(program.uniform("selected"), 0);
            }

            for (int j = 0; j < meshes[i].n_ver; j++)
            {
                if (j % 3 == 0)
                {
                    glDrawArrays(GL_TRIANGLES, start + j, 3);
                }
            }
            start = start + meshes[i].n_ver;
        }
        else if (meshes[i].shadermode == 2)
        {
            glUniform1i(program.uniform("shademode"), 2);


            if(meshes[i].selected == true)
            {
                glUniform1i(program.uniform("selected"), 1);
            }
            else
            {
                glUniform1i(program.uniform("selected"), 0);
            }

            for (int j = 0; j < meshes[i].n_ver; j++)
            {
                if (j % 3 == 0)
                {
                    glDrawArrays(GL_TRIANGLES, start + j, 3);
                }
            }
            start = start + meshes[i].n_ver;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    screen_height = height;
    screen_width = width;
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
    glm::vec4 p_screen(xpos,height-1-ypos,0,1);
    glm::vec4 p_canonical((p_screen.x/width)*2-1,(p_screen.y/height)*2-1,0,1);

//    glm::vec4 p_world = glm::inverse(view) * p_canonical;
//    p_world = glm::inverse(projection) * p_world;



    // Update the position of the first vertex if the left button is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        float min_dis = 99999999;
        int ind_i = -1;

        for(int i = 1; i < meshes.size(); i ++)
        {
            for (int j = 0; j < meshes[i].vers.size() - 3; j = j + 3)
            {
                glm::vec4 v1 = projection * view * meshes[i].model * glm::vec4(meshes[i].vers[j], 1.0);
                v1 = v1 / v1[3];

                glm::vec4 v2 = projection * view * meshes[i].model * glm::vec4(meshes[i].vers[j+1], 1.0);
                v2 = v2 / v2[3];

                glm::vec4 v3 = projection * view * meshes[i].model * glm::vec4(meshes[i].vers[j+2], 1.0);
                v3 = v3 / v3[3];

                if (check_in_side(v1,v2,v3,p_canonical[0],p_canonical[1]) == true)
                {
                    float dis = (v1[2] + v2[2] + v3[2]) / 3;
                    if(dis < min_dis)
                    {
                        min_dis = dis;
                        ind_i = i;
                    }
                }
            }
        }


        for(int i = 1; i < meshes.size(); i++)
        {
            if (ind_i == -1)
            {
                meshes[i].selected = false;
                selected_mesh = -1;
            }
            else if (i == ind_i && ind_i != 0)
            {
                if (meshes[i].selected)
                {
                    meshes[i].selected = false;
                    selected_mesh = -1;
                }
                else
                {
                    meshes[i].selected = true;
                    selected_mesh = ind_i;
                }
            }
            else
            {
                meshes[i].selected = false;
            }
        }
    }

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case  GLFW_KEY_7:
                if (shadow_color == 1)
                {
                    shadow_color = 2;
                }
                else
                {
                    shadow_color = 1;
                }
                break;

            case  GLFW_KEY_1:
                read_off("../data/cube.off", true);
                break;
            case GLFW_KEY_3:
                read_off("../data/bunny.off", true);
                break;
            case  GLFW_KEY_2:
                read_off("../data/bumpy_cube.off", true);
                break;

            case  GLFW_KEY_S:
                zoom = zoom + 1;
                break;

            case  GLFW_KEY_W:
                zoom = zoom - 1;
                break;

            case  GLFW_KEY_A:
                x_ang = x_ang + 0.1;
                change_cam_pos(y_ang, x_ang);
                break;

            case  GLFW_KEY_D:
                x_ang = x_ang - 0.1;
                change_cam_pos(y_ang, x_ang);
                break;


            case  GLFW_KEY_Q:
                y_ang = y_ang + 0.1;
                change_cam_pos(y_ang, x_ang);
                break;

            case  GLFW_KEY_Z:
                y_ang = y_ang - 0.1;
                change_cam_pos(y_ang, x_ang);
                break;

            case  GLFW_KEY_O:
                pers = false;
                break;

            case  GLFW_KEY_P:
                pers = true;
                break;


            case  GLFW_KEY_F:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].trans[0] = meshes[selected_mesh].trans[0] - 0.1f;
                    meshes[selected_mesh].update_mode();
                }
                break;
            case  GLFW_KEY_H:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].trans[0] = meshes[selected_mesh].trans[0] + 0.1f;
                    meshes[selected_mesh].update_mode();
                }
                break;

            case  GLFW_KEY_T:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].trans[2] = meshes[selected_mesh].trans[2] - 0.1f;
                    meshes[selected_mesh].update_mode();
                }
                break;
            case  GLFW_KEY_G:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].trans[2] = meshes[selected_mesh].trans[2] + 0.1f;
                    meshes[selected_mesh].update_mode();
                }
                break;

            case  GLFW_KEY_R:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].trans[1] = meshes[selected_mesh].trans[1] + 0.1f;
                    meshes[selected_mesh].update_mode();
                }
                break;
            case  GLFW_KEY_V:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].trans[1] = meshes[selected_mesh].trans[1] - 0.1f;
                    meshes[selected_mesh].update_mode();
                }
                break;

            case  GLFW_KEY_Y:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].scal *= 1.1f;
                    meshes[selected_mesh].update_mode();
                }
                break;
            case  GLFW_KEY_N:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].scal *= 0.9f;
                    meshes[selected_mesh].update_mode();
                }
                break;

            case  GLFW_KEY_J:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].y_ang += 0.1;
                    meshes[selected_mesh].update_mode();
                }
                break;
            case  GLFW_KEY_L:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].y_ang -= 0.1;
                    meshes[selected_mesh].update_mode();
                }
                break;

            case  GLFW_KEY_I:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].x_ang += 0.1;
                    meshes[selected_mesh].update_mode();
                }
                break;
            case  GLFW_KEY_K:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].x_ang -= 0.1;
                    meshes[selected_mesh].update_mode();
                }
                break;

            case  GLFW_KEY_U:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].z_ang += 0.1;
                    meshes[selected_mesh].update_mode();
                }
                break;
            case  GLFW_KEY_M:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].z_ang -= 0.1;
                    meshes[selected_mesh].update_mode();
                }
                break;


            case  GLFW_KEY_X:
                if (selected_mesh != -1)
                {
                    vector<glm::vec3> temp;
                    vector<glm::vec3> temp2;
                    meshes.erase(meshes.begin() + selected_mesh);
                    for(int i = 0; i < meshes.size(); i++)
                    {
                        for(int j = 0; j < meshes[i].vers.size(); j++)
                        {
                            temp.push_back(meshes[i].vers[j]);
                            temp2.push_back(meshes[i].norms[j]);
                        }
                    }

                    V = temp;
                    C = temp2;
                    if(V.size() == 0)
                    {
                        V.push_back(glm::vec3(0.f, 0.f, 0.f));
                        C.push_back(glm::vec3(0.f, 0.f, 0.f));
                        ver_first = true;
                    }
                    selected_mesh = -1;
                }
                break;

            case  GLFW_KEY_4:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].shadermode = 1;
                }
                break;

            case  GLFW_KEY_5:
                if (selected_mesh != -1)
                {
                    meshes[selected_mesh].shadermode = 2;
                }
                break;


            default:
                break;
        }
    }


    VBO_v.update(V);
    VBO_c.update(C);
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



    glEnable(GL_DEPTH_TEST);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    vector<std::string> faces
            {
                    "../data/night_posx.png",
                    "../data/night_negx.png",
                    "../data/night_posy.png",
                    "../data/night_negy.png",
                    "../data/night_negz.png",
                    "../data/night_posz.png"
            };

    unsigned int cubemapTexture = loadCubemap(faces);

    VertexArrayObject VAO_s;
    VAO_s.init();
    VAO_s.bind();


    const GLchar* sky_vertex_shader =
            "#version 330 core\n"
            "    in vec3 position;"
            "    uniform mat4 view;"
            "    uniform mat4 projection;"
            "    out vec3 TexCoords;"
            "    void main()"
            "    {"
            "        TexCoords = position;"
            "        vec4 pos = projection * view * vec4(position, 1.0);"
            "        gl_Position = pos.xyww;"
            "    }";


    const GLchar* sky_fragment_shader =
            "#version 330 core\n"
            "    out vec4 FragColor;"
            "    in vec3 TexCoords;"
            "    uniform samplerCube skybox;"
            "    void main()"
            "    {"
            "        FragColor = texture(skybox, TexCoords);"
            "    }";

    Program program3;
    program3.init(sky_vertex_shader,sky_fragment_shader,"FragColor");
    program3.bind();

    VertexBufferObject VBO_s;
    std::vector<glm::vec3> S;


    for(int i = 0; i < 36 * 3; i = i + 3)
    {
        glm::vec3 temp = glm::vec3(skyboxVertices[i]*100, skyboxVertices[i+1]*100, skyboxVertices[i+2]*100);
        S.push_back(temp);
    }

    VBO_s.init();
    VBO_s.update(S);
    program3.bindVertexAttribArray("position",VBO_s);

    //glUniform1i(program3.uniform("skybox"), 0);

    glBindVertexArray(0);



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
    VBO_v.init();
    V.push_back(glm::vec3(0.0f,  0.5f,0.0f));
    VBO_v.update(V);

    VBO_c.init();
    C.push_back(glm::vec3(0,  0,0));
    VBO_c.update(C);





    const GLchar* depth_vertex_shader =
            "#version 330 core\n"
            "    in vec3 position;"
            "    uniform mat4 lightSpaceMatrix;"
            "    uniform mat4 model;"
            "    void main()"
            "    {"
            "        gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);"
            "    }";

    const GLchar* depth_fragment_shader =
            "#version 330 core\n"
            "    void main()"
            "    {"
            "    }";





    Program program2;
    program2.init(depth_vertex_shader,depth_fragment_shader,"FragColor");
    program2.bind();

    program2.bindVertexAttribArray("position",VBO_v);





    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
    const GLchar* vertex_shader =
            "#version 330 core\n"
            "    in vec3 position;"
            "    in vec3 anormal;"
            "    uniform vec3 color;"
            "    uniform int shademode;"
            "    uniform mat4 view;"
            "    uniform mat4 projection;"
            "    uniform mat4 model;"
            "    uniform mat4 lightSpaceMatrix;"
            "    out VS_OUT {"
            "            vec3 FragPos;"
            "            vec3 Normal;"
            "            vec4 FragPosLightSpace;"
            "            flat int shader_mode;"
            "    } vs_out;"
            "    void main()"
            "    {"
            "        vs_out.shader_mode = shademode;"
            "        vs_out.FragPos = vec3(model * vec4(position, 1.0));"
            "        vs_out.Normal = transpose(inverse(mat3(model))) * anormal;"
            "        vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);"
            "        gl_Position = projection * view  * model * vec4(position, 1.0);"
            "    }";

    const GLchar* fragment_shader =
            "   #version 330 core\n"
            "    out vec4 FragColor;"
            "    in  VS_OUT {"
            "            vec3 FragPos;"
            "            vec3 Normal;"
            "            vec4 FragPosLightSpace;"
            "            flat int shader_mode;"
            "    } fs_in;"
            "    uniform sampler2D shadowMap;"
            "    uniform vec3 lightPos;"
            "    uniform vec3 viewPos;"
            "    uniform int shadow_color;"
            "    uniform samplerCube skybox;"
            "    uniform int selected;"
            "    float ShadowCalculation(vec4 fragPosLightSpace)"
            "    {"
            "        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;"
            "        projCoords = projCoords * 0.5 + 0.5;"
            "        float closestDepth = texture(shadowMap, projCoords.xy).r;"
            "        float currentDepth = projCoords.z;"
            "        vec3 normal = normalize(fs_in.Normal);"
            "        vec3 lightDir = normalize(lightPos - fs_in.FragPos);"
            "        float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);"
            "        float shadow = 0.0;"
            "        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);"
            "        for(int x = -1; x <= 1; ++x)"
            "        {"
            "            for(int y = -1; y <= 1; ++y)"
            "            {"
            "                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;"
            "                shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;"
            "            }"
            "         }"
            "         shadow /= 9.0;"
            "         if(projCoords.z > 1.0)"
            "             shadow = 0.0;"
            "         return shadow;"
            "    }"
            "    void main()"
            "    {"
            "        vec3 color = vec3(1.f,1.f,1.f);"
            "        vec3 normal = normalize(fs_in.Normal);"
            "        vec3 lightColor = vec3(1.f);"
            "        vec3 ambient = 0.3 * lightColor;"
            "        vec3 lightDir = normalize(lightPos - fs_in.FragPos);"
            "        float diff = max(dot(lightDir, normal), 0.0);"
            "        vec3 diffuse = diff * lightColor;"
            "        vec3 viewDir = normalize(viewPos - fs_in.FragPos);"
            "        float spec = 0.0;"
            "        vec3 halfwayDir = normalize(lightDir + viewDir);"
            "        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);"
            "        vec3 specular = spec * lightColor;"
            "        float shadow = ShadowCalculation(fs_in.FragPosLightSpace);"
            "        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;"
            "        FragColor = vec4(lighting, 1.0);"
            "        if (fs_in.shader_mode == 1)"
            "        {"

            "            vec3 I = normalize(fs_in.FragPos - viewPos);"
            "            vec3 R = reflect(I, normalize(fs_in.Normal));"
            "            FragColor = vec4(texture(skybox, R).rgb, 1.0);"

            "        }"
            "        else"
            "       {"

            "        if (FragColor.x < 0.4 && FragColor.y < 0.4 && FragColor.z < 0.4 && shadow_color == 2) "
            "        {"
            "            FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
            "        }"

            "       }"
            "       if (selected == 1)"
            "       {"
            "           FragColor = vec4(0.0, 0.0, 1.0, 1.0);"
            "       }"

            "}";

    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader,fragment_shader,"FragColor");
    program.bind();

    // The vertex shader wants the position of the vertices as an input.
    // The following line connects the VBO we defined above with the position "slot"
    // in the vertex shader
    program.bindVertexAttribArray("position",VBO_v);
    program.bindVertexAttribArray("anormal",VBO_c);

    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);


    glBindVertexArray(0);

    view = cam_m();
    projection = pro_m();

    glUniform1i(program.uniform("shadowMap"), 0);

    glUniform1i(program.uniform("skybox"), 1);

    glUniform1i(program3.uniform("skybox"), 1);

    read_off("../data/plane.off", false);




    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Bind your VAO (not necessary if you have only one)
        VAO.bind();

        // Bind your program

        const float radius = 2.f;
        float light_x = sin(glfwGetTime()) * radius;
        float light_z = cos(glfwGetTime()) * radius;
        float light_y = 4.f;


        glm::vec3 lightPos = glm::vec3(light_x, light_y, light_z);

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        lightProjection = glm::perspective(glm::radians(zoom), (float)screen_width / (float)screen_height, 1.f, 7.5f);
        //lightProjection = glm::ortho((float)-screen_width / screen_height,(float)screen_width / screen_height,-1.f,1.0f,1.0f,100.0f);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        view  = cam_m();
        projection = pro_m();


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program2.bind();

        glUniformMatrix4fv(program2.uniform("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));


        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        int start = 0;
        for (int i = 0; i < meshes.size(); i++)
        {
            glUniformMatrix4fv(program2.uniform("model"), 1, GL_FALSE, glm::value_ptr(meshes[i].model));

            for (int j = 0; j < meshes[i].n_ver; j++)
            {
                if (j % 3 == 0)
                {
                    glDrawArrays(GL_TRIANGLES, start + j, 3);
                }
            }

            start = start + meshes[i].n_ver;
        }


        glBindFramebuffer(GL_FRAMEBUFFER, 0);




        program.bind();



        glViewport(0, 0, screen_width, screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




        view  = cam_m();
        projection = pro_m();

        glUniform1i(program.uniform("shadow_color"), shadow_color);

        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(program.uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUniformMatrix4fv(program.uniform("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        glUniform3f(program.uniform("viewPos"), cam_pos.x,cam_pos.y,cam_pos.z);
        glUniform3f(program.uniform("lightPos"), light_x, light_y, light_z);



        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);


        renderscene(program);



        glBindVertexArray(0);


        view  = cam_m();
        projection = pro_m();
        VAO_s.bind();

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        program3.bind();
        glm::mat4 sky_view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        glUniformMatrix4fv(program3.uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(program3.uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);






        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    VBO_v.free();
    VBO_c.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}