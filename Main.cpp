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
glm::vec3 cameraPos = glm::vec3(0, 0, 2.f);
glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
glm::vec3 Front = glm::vec3(0, 0.0f, -1);
//initialize for mouse movement
bool firstMouse = true;
float pitch = 0.0f;
float yaw = -90.0f;

//for initial mouse movement
float lastX = 400, lastY = 400;

//Vector for holding the Models
std::vector<Model3D*> vecModels;

//Point light variables
float brightness = 5.0f;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{   
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
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); 
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    Front = glm::normalize(direction);
}

void Key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods) {

    float cameraSpeed = 400.0f * deltaTime;// set Strength of movement
    //simple keypresses trigger the corresponding calculation
    if (glfwGetKey(window, GLFW_KEY_W) )
        cameraPos += cameraSpeed * Front; 
    if (glfwGetKey(window, GLFW_KEY_S))
        cameraPos -= cameraSpeed * Front;
    if (glfwGetKey(window, GLFW_KEY_A))
        cameraPos -= glm::normalize(glm::cross(Front, WorldUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D))
        cameraPos += glm::normalize(glm::cross(Front, WorldUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        
        glm::vec3 offset = { 0.5,0.5,0.5 };//don't make it spawn on you code
        //cooldown check and reset
        if (coolDown <= 0) {
            vecModels.push_back(new Model3D(cameraPos + Front * offset));
            coolDown = 0;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_I)) brightness += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_K)) brightness -= 1.0f;
};


const char* shaderCompiler(std::string path) {
    //compile skybox fragment
    std::fstream src(path);
    std::stringstream buffer;

    buffer << src.rdbuf();

    std::string s = buffer.str();
    return s.c_str();
}

int main(void)
{
    
    vecModels.push_back(new Model3D({ 0,0,0 })); //create model object
    GLFWwindow* window;

    float height = 800.0f;
    float width = 800.0f;



    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Reblando", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    

    stbi_set_flip_vertically_on_load(true);

    //      MAIN OBJ TEXTURE
    int img_width, img_height, colorChannels;

    unsigned char* tex_bytes = stbi_load(
        "3D/brickwall.jpg",
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

    //      NORMAL MAP TEXTURE
    int img_width_map, img_height_map, colorChannels_map;

    unsigned char* tex_bytes_norm = stbi_load(
        "3D/brickwall_normal.jpg",
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

    //faces of the skybox
    //std::string facesSkybox[]{
    //    "Skybox/rainbow_rt.png", //RIGHT
    //    "Skybox/rainbow_lf.png", //left
    //    "Skybox/rainbow_up.png", //up
    //    "Skybox/rainbow_dn.png", //down
    //    "Skybox/rainbow_ft.png", //front
    //    "Skybox/rainbow_bk.png", //back
    //};

    //faces of the skybox
    std::string facesSkybox[]{
        "Skybox/image3x2.png", //RIGHT
        "Skybox/image1x2.png", //left
        "Skybox/image2x1.png", //up
        "Skybox/image2x3.png", //down
        "Skybox/image2x2.png", //front
        "Skybox/image4x2.png", //back
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
                GL_RGBA,
                w,
                h,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data
            );

            stbi_image_free(data);
        }
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, 800, 800);

    glfwSetKeyCallback(window, Key_Callback);

    //OBJECT SHADER ///////////////////////////////////////////////////////////////////////////
    //compile shader vertex
    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;

    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    //compile shader fragment
    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;

    fragBuff << fragSrc.rdbuf();

    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    //SKYBOX ////////////////////////////////////////////////////////////////////////////
    //compile skybox vertex
    std::fstream vertSkyboxSrc("Shaders/skybox.vert");
    std::stringstream vertSkyboxBuff;

    vertSkyboxBuff << vertSkyboxSrc.rdbuf();

    std::string vertSB = vertSkyboxBuff.str();
    const char* vsb = vertSB.c_str();

    //compile skybox fragment
    std::fstream fragSkyboxSrc("Shaders/skybox.frag");
    std::stringstream fragSkyboxBuff;

    fragSkyboxBuff << fragSkyboxSrc.rdbuf();

    std::string fragSB = fragSkyboxBuff.str();
    const char* fsb = fragSB.c_str();

    //OBJECT //////////////////////////////////////////////////////////////////////
    //create vertex shader(used for movements)
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &v, NULL);

    glCompileShader(vertexShader);

    //create frag shader (our objects are turned into pixels/fragments which we will use to color in an object)
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragShader, 1, &f, NULL);

    glCompileShader(fragShader);

    //create shader program that'll just run both frag and vert together as one.
    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    glLinkProgram(shaderProg);//compile to make sure computer remembers

    //using shader class
    Shader mainObjShader("Shaders/sample.vert", "Shaders/sample.frag");

    //SKYBOX //////////////////////////////////////////////////////////////////////////
    //create vertex shader
    GLuint vertexSkyboxShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexSkyboxShader, 1, &vsb, NULL);

    glCompileShader(vertexSkyboxShader);

    //create frag shader (our objects are turned into pixels/fragments which we will use to color in an object)
    GLuint fragSkyboxShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragSkyboxShader, 1, &fsb, NULL);

    glCompileShader(fragSkyboxShader);

    //create shader program that'll just run both frag and vert together as one.
    GLuint skyboxShaderProg = glCreateProgram();
    glAttachShader(skyboxShaderProg, vertexSkyboxShader);
    glAttachShader(skyboxShaderProg, fragSkyboxShader);

    glLinkProgram(skyboxShaderProg);//compile to make sure computer remembers

    /////////////////////////////////////////////////////////////////////////////////////

    //set cursor to NONEXISTANT and make mouse events be called
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    std::string path = "3D/MicroRecon.obj";
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

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    //vector to hold our tangents
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    //for loop that will go over all the verteces by 3
    for (int i = 0; i < shapes[0].mesh.indices.size(); i+=3)
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


    GLfloat vertices[]{//def of 3d mdel
        //xyz
        0.f, 0.5,0.f,
        -0.5f, -0.5f,0.f,
        0.5f,-0.5f,0.f
    };

    GLuint indices[] {
        0,1,2
    };
    
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

    //object
    GLuint VAO, VBO;// EBO;// VBO_UV;

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

        
        //set camera to be MOVEABLE i.e. can be influenced
        glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + Front, WorldUp);

        //      SKYBOX
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxShaderProg);

        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(
            //cast the same view matrix of the camera turn it into a mat3 to remove translations
            glm::mat3(viewMatrix)
            //then reconvert it to a mat4
        );

        unsigned int skyboxViewLoc = glGetUniformLocation(skyboxShaderProg, "view");
        glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));

        unsigned int skyboxProjLoc = glGetUniformLocation(skyboxShaderProg, "projection");
        glUniformMatrix4fv(skyboxProjLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projection));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        //      MAIN OBJECT
        mainObjShader.use();

        //camera
        mainObjShader.setMat4("projection", projection);
        mainObjShader.setMat4("view", viewMatrix);

        //light
        mainObjShader.setVec3("lightPos", lightPos);
        mainObjShader.setVec3("lightColor", lightColor);
        mainObjShader.setInt("ambientStr", ambientStr);
        mainObjShader.setVec3("ambientColor", ambientColor);
        mainObjShader.setVec3("cameraPos", cameraPos);
        mainObjShader.setFloat("specStr", specStr);
        mainObjShader.setFloat("specPhong", specPhong);
        mainObjShader.setFloat("brightness", brightness);

        //texture
        glActiveTexture(GL_TEXTURE0);
        GLuint mainObjtex0Address = glGetUniformLocation(mainObjShader.getID(), "tex0");
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(mainObjtex0Address, 0);

        glActiveTexture(GL_TEXTURE1);
        GLuint mainObjMaptex0Address = glGetUniformLocation(mainObjShader.getID(), "norm_tex");
        glBindTexture(GL_TEXTURE_2D, norm_tex);
        glUniform1i(mainObjMaptex0Address, 1);

        thetaY += 100.f * deltaTime;

        for (int i = 0; i < (int)vecModels.size(); i++)
        {
            //vecModels[i]->draw(&shaderProg, &VAO, &fullVertexData);
            vecModels[i]->rotate('x', thetaY);
            vecModels[i]->draw(&mainObjShader, &VAO, &fullVertexData);
        }
       

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        

    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
   /* glDeleteBuffers(1, &EBO);*/
    glfwTerminate();
    return 0;
}