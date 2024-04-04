#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include "Model3D.h"

#include "Shader.h"

#include "Light.h"
#include "PointLight.h"
#include "DirectionLight.h"

#include "PerspectiveCamera.h"
#include "OrthoCamera.h"

#include "Player.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


//general movement
// 
float x = 0.f, y = 0.f, z = 0.f;

float x_scale = 0.5f, y_scale = 0.5f, z_scale = 0.5f;

float thetaX = 1.f, thetaY = 1.f;

float x_axisX = 0.f, y_axisX = 1.f, z_axisX = 0.f;

float x_axisY = 1.f, y_axisY = 0.f, z_axisY = 0.f;

//for all time related additions
float deltaTime = 0.0f;	
float lastFrame = 0.0f; 
float coolDown = 0.0f;

//initialize camera vars
glm::vec3 cameraPos = glm::vec3(0, 4, -8.f);
glm::vec3 rdCamPos = glm::vec3(0.f, 0.f, 0.f);
glm::vec3 brCamPos = glm::vec3(0.f, 10.f, 0.f);
glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
glm::vec3 Front = glm::vec3(0, 0.0f, -1.0);
//initialize for mouse movement
bool firstMouse = true;
float pitch = 0.0f;
float yaw = -90.0f;

int stateCam = 0;
//for initial mouse movement
float lastX = 400, lastY = 400;

//Vector for holding the Models
std::vector<Model3D*> vecModels;

//Point light variables
float brightness = 0.5f;

//screen
float height = 1280.f;
float width = 1280.f;

Player* P1;
//      CAMERA
PerspectiveCamera perca(cameraPos, WorldUp, Front, 60.f, height, width, 100,false);//test view
PerspectiveCamera Therca({ 0,15,28 }, WorldUp, Front, 60.f, height, width, 50,true);//Third person view
PerspectiveCamera Unerca({ 0,0,0 }, WorldUp, Front, 60.f, height, width, 100,false);//First person view
OrthoCamera Berca({ 0,10.f,0 }, WorldUp, {0,-1,0}, 60.f, height, width, 1000, true); //bird view

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (stateCam == 1) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        if (firstMouse) // for first mouse move
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        // calculate offsets to add influence front
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; //reversed
        lastX = xpos;
        lastY = ypos;

        //set how STRONG the movement is
        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        //for front calculation
        yaw += xoffset;
        pitch += yoffset;

        //making sure that you can't 360 via neck breaking
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        //setting front and the change
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * 2;
        direction.y = sin(glm::radians(pitch)) * 2;
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * 2;
        rdCamPos = P1->getPosition() + direction;


        
    }
    
}

void Key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods) {
    
    float cameraSpeed = 400.0f * deltaTime;// set Strength of movement
    //simple keypresses trigger the corresponding calculation
    if (stateCam != 2) {
        if (glfwGetKey(window, GLFW_KEY_W)) {

            glm::vec3 pos = P1->getPosition();
            P1->setPosition(pos + Front);
            cameraPos += Front;
            rdCamPos += Front;

            glm::vec3 direction;

            if (stateCam == 0) {
                direction.x = cos(glm::radians(P1->getYrot() - 90)) * 0.8;//
                direction.y = 0.4;
                direction.z = -sin(glm::radians(P1->getYrot() - 90)) * 0.8;
                cameraPos = direction + P1->getPosition();
            }
        }

        if (glfwGetKey(window, GLFW_KEY_S)) {

            glm::vec3 pos = P1->getPosition();
            P1->setPosition(pos - Front);
            cameraPos -= Front;
            rdCamPos -= Front;

            glm::vec3 direction;

            if (stateCam == 0) {
                direction.x = cos(glm::radians(P1->getYrot() - 90)) * 0.8;//
                direction.y = 0.4;
                direction.z = -sin(glm::radians(P1->getYrot() - 90)) * 0.8;
                cameraPos = direction + P1->getPosition();
            }
        }
        if (glfwGetKey(window, GLFW_KEY_Q)) {

            glm::vec3 pos = P1->getPosition() + glm::vec3{ 0,1,0 };
            P1->setPosition(pos);
            cameraPos += glm::vec3{ 0.f, 1.f, 0.f };

            glm::vec3 direction;

            if (stateCam == 0) {
                direction.x = cos(glm::radians(P1->getYrot() - 90)) * 0.8;//
                direction.y = 0.4;
                direction.z = -sin(glm::radians(P1->getYrot() - 90)) * 0.8;
                cameraPos = direction + P1->getPosition();
            }

        }
        if (glfwGetKey(window, GLFW_KEY_E)) {
            glm::vec3 pos = P1->getPosition() - glm::vec3{ 0,1,0 };
            P1->setPosition(pos);
            cameraPos -= glm::vec3{ 0.f, 1.f, 0.f };

            glm::vec3 direction;

            if (stateCam == 0) {
                direction.x = cos(glm::radians(P1->getYrot() - 90)) * 0.8;//
                direction.y = 0.4;
                direction.z = -sin(glm::radians(P1->getYrot() - 90)) * 0.8;
                cameraPos = direction + P1->getPosition();
            }

        }
        if (glfwGetKey(window, GLFW_KEY_A)) {
            P1->rotate('y', '+');
            glm::vec3 direction;



            direction.x = cos(glm::radians(P1->getYrot() - 90));//
            direction.y = 0;
            direction.z = -sin(glm::radians(P1->getYrot() - 90));
            Front = glm::normalize(direction);

            if (stateCam == 0) {
                direction.x = cos(glm::radians(P1->getYrot() - 90)) * 0.8;//
                direction.y = 0.4;
                direction.z = -sin(glm::radians(P1->getYrot() - 90)) * 0.8;
                cameraPos = direction + P1->getPosition();
            }

        }

        if (glfwGetKey(window, GLFW_KEY_D)) {
            P1->rotate('y', '-');

            glm::vec3 direction;

            direction.x = cos(glm::radians(P1->getYrot() - 90));
            direction.y = 0;
            direction.z = -sin(glm::radians(P1->getYrot() - 90));
            Front = glm::normalize(direction);
            if (stateCam == 0) {
                direction.x = cos(glm::radians(P1->getYrot() - 90)) * 0.8;//
                direction.y = 0.4;
                direction.z = -sin(glm::radians(P1->getYrot() - 90)) * 0.8;
                cameraPos = direction + P1->getPosition();
               
            }

        }

        if (glfwGetKey(window, GLFW_KEY_F)) {
            brightness += 0.5f;
            if (brightness > 1.5f) brightness = 0.5f;
        }
    }
    if (stateCam == 2) {
        if (glfwGetKey(window, GLFW_KEY_LEFT)) {
            brCamPos -= glm::vec3{ 1,0,0 };
            std::cout << brCamPos.x << " " << brCamPos.y << " " << brCamPos.z << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
            brCamPos += glm::vec3{ 1,0,0 };
            std::cout << brCamPos.x << " " << brCamPos.y << " " << brCamPos.z << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_UP)) {
            brCamPos += glm::vec3{ 0,0,1 };
            std::cout << brCamPos.x << " " << brCamPos.y << " " << brCamPos.z << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN)) {
            brCamPos -= glm::vec3{ 0,0,1 };
            std::cout << brCamPos.x << " " << brCamPos.y << " " << brCamPos.z << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_1)) {
        if (stateCam == 0) {
            stateCam = 1;
            rdCamPos = cameraPos;
        }
        else if (stateCam == 1) {
            stateCam = 0;
        }
        if (stateCam == 2) {
            stateCam = 0;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_2)) {
        stateCam = 2;
        brCamPos = glm::vec3{ P1->getPosition().x, P1->getPosition().y + 2, P1->getPosition().z};
        Berca.setFront({ P1->getPosition().x, P1->getPosition().y - 1, P1->getPosition().z });
    }
       
 
    
    if (glfwGetKey(window, GLFW_KEY_I)) brightness += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_K)) brightness -= 1.0f;
};

GLuint createTexture(const char* fileName) {
    int img_width, img_height, colorChannels;

    unsigned char* tex_bytes = stbi_load(
        fileName,
        &img_width,
        &img_height,
        &colorChannels,
        0
    );

    GLuint texture;

    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA,
        img_width,
        img_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    return texture;
}
GLuint createTextureJPG(const char* fileName) {
    int img_width, img_height, colorChannels;

    unsigned char* tex_bytes = stbi_load(
        fileName,
        &img_width,
        &img_height,
        &colorChannels,
        0
    );

    GLuint texture;

    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width,
        img_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    return texture;
}

GLuint createNormalTextureJPG(const char* fileName) {
    int img_width, img_height, colorChannels;

    unsigned char* tex_bytes = stbi_load(
        fileName,
        &img_width,
        &img_height,
        &colorChannels,
        0
    );

    GLuint texture;

    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width,
        img_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    return texture;
}

void useTexture(GLuint shaderProg, GLuint texture) {
    glActiveTexture(GL_TEXTURE0);
    GLuint mainObjtex0Address = glGetUniformLocation(shaderProg, "tex0");
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(mainObjtex0Address, 0);
}

void useNormTexture(GLuint shaderProg, GLuint texture) {
    glActiveTexture(GL_TEXTURE1);
    GLuint mainObjNORMtex0Address = glGetUniformLocation(shaderProg, "norm_tex");
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(mainObjNORMtex0Address, 1);
}


GLuint createNormTexture(const char* fileName) {
    int img_width_map, img_height_map, colorChannels_map;

    unsigned char* tex_bytes_norm = stbi_load(
        fileName,
        &img_width_map,
        &img_height_map,
        &colorChannels_map,
        0
    );

    GLuint norm_tex;

    glGenTextures(1, &norm_tex);

    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, norm_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width_map,
        img_height_map,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes_norm);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes_norm);

    return norm_tex;
}

std::vector<GLfloat> getFullVertexData(std::string fileName) {
    std::string path = fileName;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );


    //vector to hold our tangents
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    //for loop that will go over all the verteces by 3
    for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3)
    {
        tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
        tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
        tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

        //position of vertex 1
        glm::vec3 v1 = glm::vec3(
            attributes.vertices[vData1.vertex_index * 3],
            attributes.vertices[(vData1.vertex_index * 3) + 1],
            attributes.vertices[(vData1.vertex_index * 3) + 2]
        );

        //position of vertex 2
        glm::vec3 v2 = glm::vec3(
            attributes.vertices[vData2.vertex_index * 3],
            attributes.vertices[(vData2.vertex_index * 3) + 1],
            attributes.vertices[(vData2.vertex_index * 3) + 2]
        );

        //position of vertex 3
        glm::vec3 v3 = glm::vec3(
            attributes.vertices[vData3.vertex_index * 3],
            attributes.vertices[(vData3.vertex_index * 3) + 1],
            attributes.vertices[(vData3.vertex_index * 3) + 2]
        );

        //UV of vertex 1
        glm::vec2 uv1 = glm::vec2(
            attributes.texcoords[(vData1.texcoord_index * 2)],
            attributes.texcoords[(vData1.texcoord_index * 2) + 1]
        );

        //UV of vertex 2
        glm::vec2 uv2 = glm::vec2(
            attributes.texcoords[(vData2.texcoord_index * 2)],
            attributes.texcoords[(vData2.texcoord_index * 2) + 1]
        );

        //UV of vertex 3
        glm::vec2 uv3 = glm::vec2(
            attributes.texcoords[(vData3.texcoord_index * 2)],
            attributes.texcoords[(vData3.texcoord_index * 2) + 1]
        );

        //edges of the triangle : position delta
        glm::vec3 deltaPos1 = v2 - v1;
        glm::vec3 deltaPos2 = v3 - v1;

        //UV delta
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

        //tangent (T)
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        //bitangent (B)
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        //push the tangent and bitangent
        //3x for the 3  vertices of the triangle
        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }

    //get the  indices array OF mesh that will be placed into vbo
    //std::vector<GLuint> mesh_indices;
    std::vector<GLfloat> fullVertexData;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        //vertex
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3)]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 2]
        );

        //normal
        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3)]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 2]
        );

        //texcoord
        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2)]
        );

        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

        //push the tangent and bitangent to the vertex data
        fullVertexData.push_back(tangents[i].x);
        fullVertexData.push_back(tangents[i].y);
        fullVertexData.push_back(tangents[i].z);
        fullVertexData.push_back(bitangents[i].x);
        fullVertexData.push_back(bitangents[i].y);
        fullVertexData.push_back(bitangents[i].z);
    }

    return fullVertexData;
}

std::vector<GLuint> createVAOandVBO(std::vector<GLfloat> fullVertexData) {
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        //
        sizeof(GLfloat) * fullVertexData.size(),
        fullVertexData.data(),
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,

        14 * sizeof(float),
        (void*)0

    );
    glEnableVertexAttribArray(0);

    GLintptr litPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,

        14 * sizeof(float),
        (void*)litPtr

    );
    glEnableVertexAttribArray(1);

    GLintptr uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,

        14 * sizeof(float),
        (void*)uvPtr
    );
    glEnableVertexAttribArray(2);

    //tangent point
    GLintptr tangentPtr = 8 * sizeof(float);
    glVertexAttribPointer(
        3, //3 == tangent
        3, //T (XYZ)
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(float),
        (void*)tangentPtr
    );
    glEnableVertexAttribArray(3);

    //bitangent 
    GLintptr bitangentPtr = 11 * sizeof(float);
    glVertexAttribPointer(
        4, //4 == bitangent
        3, //B(XYZ)
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(float),
        (void*)bitangentPtr
    );
    glEnableVertexAttribArray(4);

    return { VAO, VBO };
}

int main(void)
{
    GLFWwindow* window;

    


    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Jaropojop & Reblando", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    stbi_set_flip_vertically_on_load(true);  

    //      LOADING THE OBJECTS
    //textures
    std::vector<GLuint> vecTextures;
    std::vector<const char*> texturePaths;
    texturePaths.push_back("3D/CamoStellarJet.png");
    texturePaths.push_back("3D/MicroRecon.png");
    texturePaths.push_back("3D/DualStriker.png");
    texturePaths.push_back("3D/GalactixRacer.png");
    texturePaths.push_back("3D/InfraredFurtive.png");
    texturePaths.push_back("3D/RedFighter.png");
    texturePaths.push_back("3D/Transtellar.png");

    for (const char* texturePath : texturePaths) {
        vecTextures.push_back(createTexture(texturePath));
    }

    GLuint mainObjTex = createTextureJPG("3D/main_hull_Base_Color.png");
    GLuint normalMainObjTex = createNormalTextureJPG("3D/main_hull_Normal_OpenGL.png");

   
    //getting the paths of obj files
    std::vector<std::string> vecPaths;
    vecPaths.push_back("3D/CamoStellarJet.obj");
    vecPaths.push_back("3D/MicroRecon.obj");
    vecPaths.push_back("3D/DualStriker.obj");
    vecPaths.push_back("3D/GalactixRacer.obj");
    vecPaths.push_back("3D/InfraredFurtive.obj");
    vecPaths.push_back("3D/RedFighter.obj");
    vecPaths.push_back("3D/Transtellar.obj");
    int posIndex = 0;

    for (std::string path : vecPaths)
    {
        std::vector<GLfloat> fullVertexData = getFullVertexData(path);

        vecModels.push_back(new Model3D(
            //{posIndex, 0, 0},
            {posIndex,0,-10},
            fullVertexData,
            "Shaders/mainObj.vert", "Shaders/mainObj.frag", 0.0f
        ));

        posIndex -= 1.f;
    }

    //setting the positions of the models
    vecModels[0]->setPosition({ 0,0,-5 });
    vecModels[1]->setPosition({ -5,-5,-10 });
    vecModels[2]->setPosition({ -8,-5,-10 });
    vecModels[3]->setPosition({ 5,-10,-15 });
    vecModels[4]->setPosition({ -3,-15,-20 });
    vecModels[5]->setPosition({ 8,0,-20 });


    //      OBJ CREATIONS
    Shader voxelShader("Shaders/mainObj.vert", "Shaders/mainObj.frag");
    Player* main_object = new Player({ 0,0,0 },
        getFullVertexData("3D/miniSub02.obj"),
        "Shaders/mainObj.vert", "Shaders/mainObj.frag" , 180.f
    );
    
    P1 = main_object;

    //      OBJECTS VAO
    for (Model3D* model : vecModels)
    {
        model->initVAO();
    }
    main_object->initVAO();
    //      NORMAL MAP TEXTURE
    //Gluin norm_tex = createNormTexture("");

    //shader of skybox
    Shader skyboxShader("Shaders/skybox.vert", "Shaders/skybox.frag");

    //faces of the skybox

    std::string facesSkybox[]{
        "Skybox/Underwater Box_Front.jpg",//RIGHT front
        "Skybox/Underwater Box_Back.jpg", //left back
        "Skybox/Underwater Box_Top.jpg", //up
        "Skybox/Underwater Box_Bottom.jpg", //down
        "Skybox/Underwater Box_Right.jpg",//front right
        "Skybox/Underwater Box_Left.jpg",//back left
    };

    unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);

    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++)
    {
        int w, h, skyCChannel;

        stbi_set_flip_vertically_on_load(false);

        unsigned char* data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyCChannel, 0);

        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                w,
                h,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );

            stbi_image_free(data);
        }
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, Key_Callback);

    /////////////////////////////////////////////////////////////////////////////////////

    //set cursor to NONEXISTANT and make mouse events be called
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    
    //SKYBOX
    //Vertices for the cube
    float skyboxVertices[]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };

    //Skybox Indices
    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };

    //skybox
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    //SET BINDINGS TO NULL
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glm::mat4 identity_matrix = glm::mat4(1.0f);

    //      CAMERA VARIABLES
    /*glm::mat4 projection = glm::ortho(
        -2.f, //left
        2.f, //right
        - 2.f, //bot
        2.f, //top
        -1.f, //z near
        1.f);  //z far*/

    glm::mat4 projection = glm::perspective(
        glm::radians(60.f),//FOV
        height / width, //aspect ratio
        0.1f, //znear > 0
        1000.f //zfar
    );

    //      LIGHT VARIABLES
    glm::vec3 lightPos = glm::vec3(0, 0, 10);

    glm::vec3 lightColor = glm::vec3(1,1,1);

    float ambientStr = 0.5f;

    glm::vec3 ambientColor = lightColor;

    float specStr = 0.5f;

    float specPhong = 16;

    //      LIGHT
    Light mainLight(lightPos, lightColor, ambientStr, specStr, specPhong);
    PointLight pointLight(P1->getPosition() + Front, lightColor, ambientStr, ambientColor, specStr, specPhong, brightness);
    DirectionLight dirLight(lightPos, lightColor, ambientStr, ambientColor, specStr, specPhong, -WorldUp, 1.f);
    


    //enable blending
    glEnable(GL_BLEND);
    //choose the blending function
    glBlendFunc(GL_SRC_ALPHA, //source factor or foreground layer
        GL_ONE_MINUS_SRC_ALPHA); //destination factor or background layer
    //same as Cfinal = Cf(Af) + Cb(1 - Af)

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //set time so that movement is uniform for all devices if needed
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        coolDown -= 1 * deltaTime;
        

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 viewMatrix = glm::mat4{ 1.0f };
        
        //set camera to be MOVEABLE i.e. can be influenced
        switch (stateCam) {
        case 0:

            Unerca.setCameraPos(cameraPos);
            Unerca.setFront(Front);
            viewMatrix = Unerca.getViewMat();
            break;
        case 1:
            
            
            Therca.setFront(P1->getPosition());
            Therca.setCameraPos(rdCamPos);
            viewMatrix = Therca.getViewMat();
            break;
        case 2:
            
            Berca.setCameraPos(brCamPos);
            
            Berca.setFront({ brCamPos.x,  brCamPos.y -1, brCamPos.z + 0.01 });
            
            viewMatrix = Berca.getViewMat();
           
            break;
        }
        

        //      SKYBOX
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();

        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(
            //cast the same view matrix of the camera turn it into a mat3 to remove translations
            glm::mat3(viewMatrix)
            //then reconvert it to a mat4
        );

        skyboxShader.setMat4("view", sky_view);

        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        //updating the point light position
        pointLight.setPosition(P1->getPosition() + Front);
        pointLight.setBrightness(brightness);

        int textureIndex = 0;
        //      DRAWING THE OBJECTS
        for (Model3D* model : vecModels)
        {
            model->getShader()->use();
            //camera
            switch (stateCam) {
            case 0:
                Unerca.perfromSpecifics(model->getShader());
                break;
            case 1:
                Therca.perfromSpecifics(model->getShader());
                break;
            case 2:
                Berca.perfromSpecifics(model->getShader());
                break;
            }
           
            
            //light
            dirLight.attachSpecifics(model->getShader());
            pointLight.attachSpecifics(model->getShader());

            //texture
            useTexture(model->getShaderID(), vecTextures[textureIndex]);
            model->draw();

            textureIndex++;
        }

       
            main_object->getShader()->use();

            switch (stateCam) {
            case 0:
                Unerca.perfromSpecifics(main_object->getShader());
                break;
            case 1:
                Therca.perfromSpecifics(main_object->getShader());
                break;
            case 2:
                Berca.perfromSpecifics(main_object->getShader());
                break;
                
            }

            //light
            dirLight.attachSpecifics(main_object->getShader());
            pointLight.attachSpecifics(main_object->getShader());

            useTexture(main_object->getShaderID(), mainObjTex);
            useNormTexture(main_object->getShaderID(), normalMainObjTex);
            main_object->draw();
        
            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
     
        
        

    }
    

    /*dualStriker.deleteVAO();
    microRecon.deleteVAO();*/

    for (Model3D* model : vecModels)
    {
        model->deleteVAO();
    }

   /* glDeleteBuffers(1, &EBO);*/
    glfwTerminate();
    return 0;
}