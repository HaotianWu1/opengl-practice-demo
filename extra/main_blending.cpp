//// This example is heavily based on the tutorial at https://open.gl
//
//// OpenGL Helpers to reduce the clutter
//#include "Helpers.h"
//
//
//// GLFW is necessary to handle the OpenGL context
//#include <GLFW/glfw3.h>
//
//#include <glm/glm.hpp>
//#include <glm/vec4.hpp>
//#include <glm/mat4x4.hpp> // glm::mat4
//#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
//
//
//#include <iostream>
//#include <fstream>
//
//using namespace std;
//
//// VertexBufferObject wrapper
//VertexBufferObject VBO_v;
//
//VertexBufferObject VBO_c;
//
//// Contains the vertex positions
//std::vector<glm::vec3> V;
//
//std::vector<glm::vec3> C;
//
//
//glm::mat4 view;
//
//glm::mat4 projection;
//
//bool pers = true;
//
//bool ver_first = true;
//
//float phi = 0;
//float theta = 0;
//
//float cam_x = 0.f;
//float cam_y = 0.f;
//float cam_z = 5.f;
//
//glm::vec3 cam_pos = glm::vec3(cam_x, cam_y, cam_z);
//
//float x_ang = 0;
//float y_ang = 0;
//float zoom = 25;
//
//float screen_height = 480;
//float screen_width = 640;
//
//int selected_mesh = -1;
//
//glm::vec3 cal_nor(glm::vec3 a, glm::vec3 b, glm::vec3 c)
//{
//    glm::vec3 v1 = b - a;
//    glm::vec3 v2 = c - a;
//    glm::vec3 n = glm::cross(v1, v2);
//    return glm::normalize(n);
//}
//
//glm::mat4 x_r_m(float ang)
//{
//    return glm::mat4 (1.f, 0.f, 0.f, 0.f,
//                      0.f ,cos(ang), -sin(ang), 0,
//                      0, sin(ang), cos(ang), 0,
//                      0.f, 0.f, 0.f, 1.f
//    );
//}
//
//glm::mat4 y_r_m(float ang)
//{
//    return glm::mat4 (cos(ang), 0.f, sin(ang), 0.f,
//                      0.f ,1.f, 0.f, 0.f,
//                      -sin(ang), 0.f, cos(ang), 0.f,
//                      0.f, 0.f, 0.f, 1.f
//    );
//}
//
//glm::mat4 z_r_m(float ang)
//{
//    return glm::mat4 (cos(ang), -sin(ang), 0.f, 0.f,
//                      sin(ang),cos(ang), 0.f, 0.f,
//                      0.f, 0.f, 1.f, 0,
//                      0.f, 0.f, 0.f, 1.f
//    );
//}
//
//class mesh
//{
//public:
//    int n_ver;
//    int n_face;
//    int n_edge;
//    int shadermode;
//    bool selected;
//
//    glm::vec3 trans;
//    glm::vec3 scal;
//    float x_ang;
//    float y_ang;
//    float z_ang;
//
//    glm::mat4 model;
//    vector<glm::vec3> vers;
//    vector<glm::vec3> norms;
//    vector<glm::vec3> shad_vers;
//    vector<vector<int>> faces;
//
//    void update_mode()
//    {
//        model = glm::mat4(1.f);
//
//        model = glm::scale(model, scal);
//
//        glm::mat4 temp = y_r_m(y_ang) * z_r_m(z_ang) * x_r_m(x_ang);
//
//        model = temp * model;
//
//        glm::mat4 tr = glm::translate(glm::mat4(1.f), trans);
//
//        model = tr * model;
//    }
//};
//
//vector<mesh> meshes;
//
//glm::vec3 get_avg_nor(glm::vec3 a, vector<glm::vec3> t)
//{
//    vector<glm::vec3> temp = t;
//    vector<vector< glm::vec3 > > res;
//
//    for(int i = 0; i < temp.size() - 3; i = i + 3)
//    {
//        vector<glm::vec3> sub;
//        if (temp[i] == a || temp[i+1] == a || temp[i+2] == a )
//        {
//            sub.push_back(temp[i]);
//            sub.push_back(temp[i+1]);
//            sub.push_back(temp[i+2]);
//            res.push_back(sub);
//        }
//    }
//
//    glm::vec3 nn = glm::vec3(0.f,0.f,0.f);
//    for (int i = 0; i < res.size(); i++)
//    {
//        nn += cal_nor(res[i][0], res[i][1], res[i][2]);
//    }
//    return nn / (float)res.size();
//}
//
//
//float sig (float x11, float y11, float x22, float y22, float x33, float y33)
//{
//    return (x11 - x33) * (y22 - y33) - (x22 - x33) * (y11 - y33);
//}
//
//bool check_in_side(glm::vec4 a, glm::vec4 b, glm::vec4 c, float posx, float posy)
//{
//    float x1 = a[0];
//    float y1 = a[1];
//    float x2 = b[0];
//    float y2 = b[1];
//    float x3 = c[0];
//    float y3 = c[1];
//
//    float d1 = sig(posx, posy, x1, y1, x2, y2);
//    float d2 = sig(posx, posy, x2, y2, x3, y3);
//    float d3 = sig(posx, posy, x3, y3, x1, y1);
//
//    float has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
//    float has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
//
//    if(!(has_neg && has_pos))
//    {
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
//
//void change_cam_pos(float x_ang, float y_ang)
//{
//    glm::vec3 origin = glm::vec3(cam_x, cam_y, cam_z);
//
//    glm::mat3 x_rotat = glm::mat3(1, 0, 0,
//                                  0, cos(x_ang), -sin(x_ang),
//                                  0, sin(x_ang), cos(x_ang));
//
//    glm::mat3 y_rotat = glm::mat3(cos(y_ang), 0, sin(y_ang),
//                                  0, 1, 0,
//                                  -sin(y_ang), 0, cos(y_ang));
//
//
//
//    cam_pos = origin * x_rotat * y_rotat;
//
//
//}
//
//
//glm::mat4 cam_m ()
//{
//    glm::mat4 c = glm::mat4(1.f);
//
//    c = glm::lookAt(cam_pos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));
//
//    return c;
//}
//
//glm::mat4 pro_m()
//{
//    glm::mat4 pro;
//
//    if (pers == true)
//    {
//        pro = projection = glm::perspective(glm::radians(zoom), (float)screen_width / (float)screen_height, 1.f, 100.0f);
//    }
//    else
//    {
//        pro = glm::ortho(
//                (float)-screen_width / screen_height,
//                (float)screen_width / screen_height,
//                -1.f,
//                1.0f,
//                1.0f,
//                100.0f
//        );
//
//    }
//    return pro;
//}
//
//vector<glm::vec3> recenter_resize(vector<glm::vec3> a)
//{
//    vector<glm::vec3> vertexs = a;
//
//    float x_max = -9999999;
//    float y_max = -9999999;
//    float z_max = -9999999;
//    float x_min = 9999999;
//    float y_min = 9999999;
//    float z_min = 9999999;
//
//
//    for(int i = 0; i < vertexs.size(); i++)
//    {
//
//        if (vertexs[i][0] >= x_max)
//        {
//            x_max = vertexs[i][0];
//        }
//        if (vertexs[i][0] <= x_min)
//        {
//            x_min = vertexs[i][0];
//        }
//
//        if (vertexs[i][1] >= y_max)
//        {
//            y_max = vertexs[i][1];
//        }
//        if (vertexs[i][1] <= y_min)
//        {
//            y_min = vertexs[i][1];
//        }
//
//        if (vertexs[i][2] >= z_max)
//        {
//            z_max = vertexs[i][2];
//        }
//        if (vertexs[i][2] <= z_min)
//        {
//            z_min = vertexs[i][2];
//        }
//    }
//
//
//    float c_x = (x_max + x_min) / 2.f;
//    float c_y = (y_max + y_min) / 2.f;
//    float c_z = (z_max + z_min) / 2.f;
//
//    for (int i = 0; i < vertexs.size(); i++)
//    {
//        vertexs[i][0] = vertexs[i][0] - c_x;
//        vertexs[i][1] = vertexs[i][1] - c_y;
//        vertexs[i][2] = vertexs[i][2] - c_z;
//    }
//
//
//
//    float x_len = x_max - x_min;
//    float y_len = y_max - y_min;
//    float z_len = z_max - z_min;
//
//
//
//
//    float x_ratio = 1.f / x_len;
//    float y_ratio = 1.f / y_len;
//    float z_ratio = 1.f / z_len;
//
//    for (int i = 0; i < vertexs.size(); i++)
//    {
//        vertexs[i][0] = vertexs[i][0] * x_ratio;
//        vertexs[i][1] = vertexs[i][1] * y_ratio;
//        vertexs[i][2] = vertexs[i][2] * z_ratio;
//    }
//
//
//
//
//
//
//
//
//
//    return vertexs;
//}
//
//vector<string> div_str(string str)
//{
//    vector<string> ans;
//    std::string delim = " ";
//
//    auto start = 0;
//    auto end = str.find(delim);
//    while (end != std::string::npos)
//    {
//        ans.push_back(str.substr(start, end - start));
//        start = end + delim.length();
//        end = str.find(delim, start);
//    }
//    if (str.substr(start, end).length() != 0)
//    {
//        ans.push_back(str.substr(start, end - start));
//    }
//    return ans;
//}
//
//void read_off(string path)
//{
//    mesh temp;
//    temp.trans = glm::vec3(0.f, 0.f,0.f);
//    temp.scal = glm::vec3(1.f, 1.f,1.f);
//    temp.x_ang = 0;
//    temp.y_ang = 0;
//    temp.z_ang = 0;
//    temp.model = glm::mat4(1.0f);
//    temp.shadermode = 1;
//    temp.selected = false;
//    bool para = false;
//    ifstream infile(path);
//    string line;
//    vector<glm::vec3> raw_data;
//    vector<int> ver_ind;
//    while (std::getline(infile, line))
//    {
//        if(line[0] != '#')
//        {
//            vector<string> substrs = div_str(line);
//            if (substrs.size() != 1)
//            {
//                if (para == false && substrs.size() == 3)
//                {
//                    temp.n_face = stoi(substrs[1]);
//                    temp.n_edge = stoi(substrs[2]);
//                    para = true;
//                }
//                else
//                {
//                    if (substrs.size() == 3)
//                    {
//                        raw_data.push_back(glm::vec3(stof(substrs[0]),stof(substrs[1]),stof(substrs[2])));
//                    }
//                    else if (substrs.size() == 4)
//                    {
//                        ver_ind.push_back(stoi(substrs[1]));
//                        ver_ind.push_back(stoi(substrs[2]));
//                        ver_ind.push_back(stoi(substrs[3]));
//                    }
//                    else
//                    {
//                        cerr << "unkown line" << endl;
//                    }
//                }
//            }
//        }
//    }
//
//    for(int i = 0; i < ver_ind.size(); i++)
//    {
//        temp.vers.push_back(raw_data[ver_ind[i]]);
//        glm::vec4 shd = projection * view * temp.model * glm::vec4(raw_data[ver_ind[i]], 1.0);
//        shd = shd / shd[3];
//        temp.shad_vers.push_back(shd);
//    }
//    temp.vers = recenter_resize(temp.vers);
//
//    for(int i = 0; i < temp.vers.size(); i++)
//    {
//        glm::vec4 shd = projection * view * temp.model * glm::vec4(temp.vers[i], 1.0);
//        shd = shd / shd[3];
//        temp.shad_vers.push_back(shd);
//    }
//
//
//
//    temp.n_ver = temp.vers.size();
//
//    for (int i = 0; i < temp.vers.size(); i++)
//    {
//        glm::vec3 ss = get_avg_nor(temp.vers[i],temp.vers);
//        temp.norms.push_back(ss);
//        //cout << ss.x << " " << ss.y << " " << ss.z << endl;
//    }
//
//
//    for(int i = 0; i < temp.n_ver; i++)
//    {
//        if (V.size() == 1 && ver_first == true)
//        {
//            V[0] = temp.vers[i];
//            C[0] = temp.norms[i];
//            ver_first = false;
//        }
//        else
//        {
//            V.push_back(temp.vers[i]);
//            C.push_back(temp.norms[i]);
//        }
//    }
//
////    for (int i = 0; i < temp.vers.size(); i++)
////    {
////        glm::vec3 ss = get_avg_nor(temp.vers[i],temp.vers);
////        temp.norms.push_back(ss);
////        //cout << ss.x << " " << ss.y << " " << ss.z << endl;
////    }
//
//
//
//    meshes.push_back(temp);
//    VBO_v.update(V);
//    VBO_c.update(C);
//}
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    glViewport(0, 0, width, height);
//    screen_height = height;
//    screen_width = width;
//}
//
//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
//{
//    // Get the position of the mouse in the window
//    double xpos, ypos;
//    glfwGetCursorPos(window, &xpos, &ypos);
//
//    // Get the size of the window
//    int width, height;
//    glfwGetWindowSize(window, &width, &height);
//
//    // Convert screen position to world coordinates
//    glm::vec4 p_screen(xpos,height-1-ypos,0,1);
//    glm::vec4 p_canonical((p_screen.x/width)*2-1,(p_screen.y/height)*2-1,0,1);
//
////    glm::vec4 p_world = glm::inverse(view) * p_canonical;
////    p_world = glm::inverse(projection) * p_world;
//
//
//
//    // Update the position of the first vertex if the left button is pressed
//    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
//    {
//        float min_dis = 99999999;
//        int ind_i = -1;
//
//        for(int i = 0; i < meshes.size(); i ++)
//        {
//            for (int j = 0; j < meshes[i].vers.size() - 3; j = j + 3)
//            {
//                glm::vec4 v1 = projection * view * meshes[i].model * glm::vec4(meshes[i].vers[j], 1.0);
//                v1 = v1 / v1[3];
//
//                glm::vec4 v2 = projection * view * meshes[i].model * glm::vec4(meshes[i].vers[j+1], 1.0);
//                v2 = v2 / v2[3];
//
//                glm::vec4 v3 = projection * view * meshes[i].model * glm::vec4(meshes[i].vers[j+2], 1.0);
//                v3 = v3 / v3[3];
//
//                if (check_in_side(v1,v2,v3,p_canonical[0],p_canonical[1]) == true)
//                {
//                    float dis = (v1[2] + v2[2] + v3[2]) / 3;
//                    if(dis < min_dis)
//                    {
//                        min_dis = dis;
//                        ind_i = i;
//                    }
//                }
//            }
//        }
//
//
//        for(int i = 0; i < meshes.size(); i++)
//        {
//            if (ind_i == -1)
//            {
//                meshes[i].selected = false;
//                selected_mesh = -1;
//            }
//            else if (i == ind_i)
//            {
//                if (meshes[i].selected)
//                {
//                    meshes[i].selected = false;
//                    selected_mesh = -1;
//                }
//                else
//                {
//                    meshes[i].selected = true;
//                    selected_mesh = ind_i;
//                }
//            }
//            else
//            {
//                meshes[i].selected = false;
//            }
//        }
//    }
//
//}
//
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
//    if (action == GLFW_PRESS)
//    {
//        switch (key)
//        {
//
//            case  GLFW_KEY_1:
//                read_off("../data/cube.off");
//                break;
//            case GLFW_KEY_3:
//                read_off("../data/bunny.off");
//                break;
//            case  GLFW_KEY_2:
//                read_off("../data/bumpy_cube.off");
//                break;
//
//            case  GLFW_KEY_S:
//                zoom = zoom + 1;
//                break;
//
//            case  GLFW_KEY_W:
//                zoom = zoom - 1;
//                break;
//
//            case  GLFW_KEY_A:
//                x_ang = x_ang + 0.1;
//                change_cam_pos(y_ang, x_ang);
//                break;
//
//            case  GLFW_KEY_D:
//                x_ang = x_ang - 0.1;
//                change_cam_pos(y_ang, x_ang);
//                break;
//
//
//            case  GLFW_KEY_Q:
//                y_ang = y_ang + 0.1;
//                change_cam_pos(y_ang, x_ang);
//                break;
//
//            case  GLFW_KEY_Z:
//                y_ang = y_ang - 0.1;
//                change_cam_pos(y_ang, x_ang);
//                break;
//
//            case  GLFW_KEY_O:
//                pers = false;
//                break;
//
//            case  GLFW_KEY_P:
//                pers = true;
//                break;
//
//
//            case  GLFW_KEY_F:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].trans[0] = meshes[selected_mesh].trans[0] - 0.1f;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//            case  GLFW_KEY_H:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].trans[0] = meshes[selected_mesh].trans[0] + 0.1f;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//
//            case  GLFW_KEY_T:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].trans[2] = meshes[selected_mesh].trans[2] - 0.1f;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//            case  GLFW_KEY_G:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].trans[2] = meshes[selected_mesh].trans[2] + 0.1f;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//
//            case  GLFW_KEY_R:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].trans[1] = meshes[selected_mesh].trans[1] + 0.1f;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//            case  GLFW_KEY_V:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].trans[1] = meshes[selected_mesh].trans[1] - 0.1f;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//
//            case  GLFW_KEY_Y:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].scal *= 1.1f;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//            case  GLFW_KEY_N:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].scal *= 0.9f;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//
//            case  GLFW_KEY_J:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].y_ang += 0.1;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//            case  GLFW_KEY_L:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].y_ang -= 0.1;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//
//            case  GLFW_KEY_I:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].x_ang += 0.1;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//            case  GLFW_KEY_K:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].x_ang -= 0.1;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//
//            case  GLFW_KEY_U:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].z_ang += 0.1;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//            case  GLFW_KEY_M:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].z_ang -= 0.1;
//                    meshes[selected_mesh].update_mode();
//                }
//                break;
//
//
//            case  GLFW_KEY_X:
//                if (selected_mesh != -1)
//                {
//                    vector<glm::vec3> temp;
//                    meshes.erase(meshes.begin() + selected_mesh);
//                    for(int i = 0; i < meshes.size(); i++)
//                    {
//                        for(int j = 0; j < meshes[i].vers.size(); j++)
//                        {
//                            temp.push_back(meshes[i].vers[j]);
//                        }
//                    }
//
//                    V = temp;
//                    if(V.size() == 0)
//                    {
//                        V.push_back(glm::vec3(0.f, 0.f, 0.f));
//                        ver_first = true;
//                    }
//                    selected_mesh = -1;
//                }
//                break;
//
//            case  GLFW_KEY_4:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].shadermode = 0;
//                }
//                break;
//
//            case  GLFW_KEY_5:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].shadermode = 1;
//                }
//                break;
//
//            case  GLFW_KEY_6:
//                if (selected_mesh != -1)
//                {
//                    meshes[selected_mesh].shadermode = 2;
//                }
//                break;
//
//            default:
//                break;
//        }
//    }
//
//
//    VBO_v.update(V);
//    VBO_c.update(C);
//}
//
//int main(void)
//{
//    GLFWwindow* window;
//
//    // Initialize the library
//    if (!glfwInit())
//        return -1;
//
//    // Activate supersampling
//    glfwWindowHint(GLFW_SAMPLES, 8);
//
//    // Ensure that we get at least a 3.2 context
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
//
//    // On apple we have to load a core profile with forward compatibility
//#ifdef __APPLE__
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//    // Create a windowed mode window and its OpenGL context
//    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
//    if (!window)
//    {
//        glfwTerminate();
//        return -1;
//    }
//
//    // Make the window's context current
//    glfwMakeContextCurrent(window);
//
//#ifndef __APPLE__
//    glewExperimental = true;
//    GLenum err = glewInit();
//    if(GLEW_OK != err)
//    {
//        /* Problem: glewInit failed, something is seriously wrong. */
//        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
//    }
//    glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
//    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
//#endif
//
//    int major, minor, rev;
//    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
//    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
//    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
//    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
//    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
//    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
//
//    // Initialize the VAO
//    // A Vertex Array Object (or VAO) is an object that describes how the vertex
//    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
//    // the VAO is not the actual object storing the vertex data,
//    // but the descriptor of the vertex data.
//    VertexArrayObject VAO;
//    VAO.init();
//    VAO.bind();
//
//    // Initialize the VBO with the vertices data
//    // A VBO is a data container that lives in the GPU memory
//    VBO_v.init();
//    V.push_back(glm::vec3(0.0f,  0.5f,0.0f));
//    VBO_v.update(V);
//
//    VBO_c.init();
//    C.push_back(glm::vec3(0,  0,0));
//    VBO_c.update(C);
//
//    // Initialize the OpenGL Program
//    // A program controls the OpenGL pipeline and it must contains
//    // at least a vertex shader and a fragment shader to be valid
//    Program program;
//    const GLchar* vertex_shader =
//            "#version 150 core\n"
//            "in vec3 position;"
//            "in vec3 color;"
//            "uniform vec3 anormal;"
//            "uniform mat4 view;"
//            "uniform mat4 projection;"
//            "uniform mat4 model;"
//            "out vec3 normal;"
//            "out vec3 FragPos;"
//            "void main()"
//            "{"
//            "    gl_Position = projection * view * model * vec4(position, 1.0);"
//            "    FragPos = vec3(model * vec4(position, 1.0));"
//            "    normal = mat3(transpose(inverse(model))) * anormal;"
//            "}";
//    const GLchar* fragment_shader =
//            "#version 150 core\n"
//            "in vec3 FragPos;"
//            "in vec3 normal;"
//            "uniform vec3 viewPos;"
//            "uniform int line;"
//            "uniform vec3 lineColor;"
//            "uniform vec3 lightColor;"
//            "uniform vec3 lightPos;"
//            "out vec4 FragColor;"
//            "void main()"
//            "{"
//
//            "   float ambientStrength = 0.1;"
//            "   vec3 ambient = ambientStrength * lightColor;"
//
//            "   vec3 norm = normalize(normal);"
//            "   vec3 lightDir = normalize(lightPos - FragPos);"
//            "   float diff = max(dot(norm, lightDir), 0.0);"
//            "   vec3 diffuse = diff * lightColor;"
//
//            "    float specularStrength = 0.5;"
//            "    vec3 viewDir = normalize(viewPos - FragPos);"
//            "    vec3 reflectDir = reflect(-lightDir, norm);"
//            "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
//            "    vec3 specular = specularStrength * spec * lightColor;"
//
//
//            "   vec3 result = (ambient + diffuse) * vec3(1.f,1.f,1.f);"
//            "   FragColor = vec4(result, 1.0);"
//            "   if (line == 1)"
//            "{"
//            "    FragColor = vec4(lineColor, 1.0);"
//            "}"
//
//            "}";
//
//    // Compile the two shaders and upload the binary to the GPU
//    // Note that we have to explicitly specify that the output "slot" called outColor
//    // is the one that we want in the fragment buffer (and thus on screen)
//    program.init(vertex_shader,fragment_shader,"FragColor");
//    program.bind();
//
//    // The vertex shader wants the position of the vertices as an input.
//    // The following line connects the VBO we defined above with the position "slot"
//    // in the vertex shader
//    program.bindVertexAttribArray("position",VBO_v);
//    program.bindVertexAttribArray("color",VBO_c);
//
//    // Register the keyboard callback
//    glfwSetKeyCallback(window, key_callback);
//
//    // Register the mouse callback
//    glfwSetMouseButtonCallback(window, mouse_button_callback);
//
//    // Update viewport
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//    glEnable(GL_DEPTH_TEST);
//
//    glDepthFunc(GL_LESS);
//
//
//    view = cam_m();
//    projection = pro_m();
//
//    // Loop until the user closes the window
//    while (!glfwWindowShouldClose(window))
//    {
//        // Bind your VAO (not necessary if you have only one)
//        VAO.bind();
//
//        // Bind your program
//        program.bind();
//
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//        glm::vec3 light(glm::vec3(1.f,1.f,1.f));
//
//
//        glUniform3f(program.uniform("lightPos"), 0.f, 0.f, 5.f);
//        glUniform3f(program.uniform("viewPos"), cam_pos.x,cam_pos.y,cam_pos.z);
//
//
//        view  = cam_m();
//        projection = pro_m();
//        //glm::mat4 model = glm::mat4(1.f);
//        glUniformMatrix4fv(program.uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
//        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
//        //glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, glm::value_ptr(model));
//
//
//        int start = 0;
//        for (int i = 0; i < meshes.size(); i++)
//        {
//            glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, glm::value_ptr(meshes[i].model));
//
//            if (meshes[i].shadermode == 1)
//            {
//                glUniform1i(program.uniform("line"), 0);
//                if(meshes[i].selected == true)
//                {
//                    //glUniform3f(program.uniform("color"), 0.f, 0.f, 1.f);
//                    glUniform3f(program.uniform("lightColor"), 0.f,0.f, 1.f);
//
//                }
//                else
//                {
//                    glUniform3f(program.uniform("lightColor"), 1.f,1.f, 1.f);
//                }
//
//                for (int j = 0; j < meshes[i].n_ver; j++)
//                {
//                    glUniform3f(program.uniform("anormal"), meshes[i].norms[j][0], meshes[i].norms[j][1], meshes[i].norms[j][2]);
//                    if (j % 3 == 0)
//                    {
//                        glDrawArrays(GL_TRIANGLES, start + j, 3);
//                    }
//                }
//
//
//                glUniform1i(program.uniform("line"), 1);
//                if(meshes[i].selected == true)
//                {
//                    glUniform3f(program.uniform("lineColor"), 0.f,0.f, 1.f);
//                }
//                else
//                {
//                    glUniform3f(program.uniform("lineColor"), 0.5f,0.5f, 0.5f);
//                }
//                for (int j = 0; j < meshes[i].n_ver; j = j+3)
//                {
//                    glDrawArrays(GL_LINE_LOOP, start + j, 3);
//                }
//
//            }
//            else if (meshes[i].shadermode == 0)
//            {
//
//                glUniform1i(program.uniform("line"), 1);
//
//                if(meshes[i].selected == true)
//                {
//                    glUniform3f(program.uniform("lineColor"), 0.f, 0.f, 1.f);
//                }
//                else
//                {
//                    glUniform3f(program.uniform("lineColor"), 0.5f, 0.5f, 0.5f);
//                }
//
//                for (int j = 0; j < meshes[i].n_ver; j = j+3)
//                {
//                    glDrawArrays(GL_LINE_LOOP, start + j, 3);
//                }
//            }
//            else
//            {
//
//
//            }
//            start = start + meshes[i].n_ver;
//        }
//
//        // Swap front and back buffers
//        glfwSwapBuffers(window);
//
//        // Poll for and process events
//        glfwPollEvents();
//    }
//
//    // Deallocate opengl memory
//    program.free();
//    VAO.free();
//    VBO_v.free();
//    VBO_c.free();
//
//    // Deallocate glfw internals
//    glfwTerminate();
//    return 0;
//}
//
//
//
//
//
//
//
//
//
//#version 330 core
//in vec3 position;
//in vec3 anormal;
//uniform vec3 color;
//uniform int shademode;
//uniform mat4 view;
//uniform mat4 projection;
//uniform mat4 model;
//out VS_OUT {
//        vec3 FragPos;
//        vec3 Normal;
//        vec4 FragPosLightSpace;
//} vs_out;
//void main()
//{
//    vs_out.FragPos = vec3(model * vec4(position, 1.0));
//    if (shademode == 1)
//    {
//        vs_out.Normal = mat3(transpose(inverse(model))) * color;
//    }
//    else
//    {
//        vs_out.Normal = mat3(transpose(inverse(model))) * anormal;
//    }
//    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
//    gl_Position = projection * view * model * vec4(vs_out.FragPos, 1.0);
//    FragPos = vec3(model * vec4(position, 1.0));
//};
//
//
//
//
//
//
//#version 330 core\n
//uniform int line;
//out vec4 FragColor;
//in  VS_OUT {
//    vec3 FragPos;
//    vec3 Normal;
//    vec4 FragPosLightSpace;
//} fs_in;
//uniform sampler2D shadowMap;
//uniform vec3 lightPos;
//uniform viewPos;
//float ShadowCalculation(vec4 fragPosLightSpace)
//{
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    projCoords = projCoords * 0.5 + 0.5;
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//    float currentDepth = projCoords.z;
//    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
//    return shadow;
//}
//void main()
//{
//    vec3 color = vec3(1.f,1.f,1.f);
//    vec3 normal = normalize(fs_in.Normal);
//    vec3 lightColor = vec3(1.0);
//
//    vec3 ambient = 0.15 * lightColor;
//
//    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
//    float diff = max(dot(lightDir, normal), 0.0);
//    vec3 diffuse = diff * lightColor;
//
//    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
//    float spec = 0.0;
//    vec3 halfwayDir = normalize(lightDir + viewDir);
//    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
//    vec3 specular = spec * lightColor;
//
//    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
//    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
//
//    FragColor = vec4(lighting, 1.0);
//
////
////
////    float ambientStrength = 0.1;
////   vec3 ambient = ambientStrength * lightColor;
////
////   vec3 norm = normalize(normal);
////   vec3 lightDir = normalize(lightPos - FragPos);
////   float diff = max(dot(norm, lightDir), 0.0);
////   vec3 diffuse = diff * lightColor;
////
////    float specularStrength = 0.5;
////    vec3 viewDir = normalize(viewPos - FragPos);
////    vec3 reflectDir = reflect(-lightDir, norm);
////    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
////    vec3 specular = specularStrength * spec * lightColor;
////
////
////   vec3 result = (ambient + diffuse) * vec3(1.f,1.f,1.f);
////   FragColor = vec4(result, 1.0);
////   if (line == 1)
////{
////    FragColor = vec4(lineColor, 1.0);
////}
//
//};
//
//
//
//
//
//
//   #version 330 core\n
//    uniform int line;
//    out vec4 FragColor;
//    in  VS_OUT {
//            vec3 FragPos;
//            vec3 Normal;
//            vec4 FragPosLightSpace;
//    } fs_in;
//    uniform sampler2D shadowMap;
//    uniform vec3 lightPos;
//    uniform viewPos;
//    float ShadowCalculation(vec4 fragPosLightSpace)
//    {
//        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//        projCoords = projCoords * 0.5 + 0.5;
//        float closestDepth = texture(shadowMap, projCoords.xy).r;
//        float currentDepth = projCoords.z;
//        float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
//        return shadow;
//    }
//    void main()
//    {
//        vec3 color = vec3(1.f,1.f,1.f);
//        vec3 normal = normalize(fs_in.Normal);
//        vec3 lightColor = vec3(1.0);
//        vec3 ambient = 0.15 * lightColor;
//        vec3 lightDir = normalize(lightPos - fs_in.FragPos);
//        float diff = max(dot(lightDir, normal), 0.0);
//        vec3 diffuse = diff * lightColor;
//        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
//        float spec = 0.0;
//        vec3 halfwayDir = normalize(lightDir + viewDir);
//        spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
//        vec3 specular = spec * lightColor;
//        float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
//        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
//        FragColor = vec4(lighting, 1.0);
//}


