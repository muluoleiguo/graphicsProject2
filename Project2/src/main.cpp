#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <Light.h>

#include <iostream>

//GUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <vector>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scanCode, int action, int mods);

void processInput(GLFWwindow* window);
bool IsLightOn(int index);

unsigned int load_cubemap(std::vector<std::string> faces);


// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 960;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


unsigned int light_bits = 0;

//GUI
int isOrtho = 0;
bool isSkybox = false;
bool canMouseRotate = false;
bool isDirLight = true;
bool isSpotLight = false;



float xScale = 1.0f, yScale = 1.0f, zScale = 1.0f;

float xPosition = 10.0f, yPosition = 5.0f, zPosition = 0.0f;


// 点光源位置
glm::vec3 point_light_positions[] = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.5f, 0.0f),
    glm::vec3(0.0f, 0.0f, 2.0f),
    glm::vec3(0.0f, -1.5f, 0.0f),
    glm::vec3(0.0f, 0.0f, -2.0f),
    glm::vec3(-1.5f, 0.0f, 0.0f)
};



// 光源颜色
glm::vec3 light_colors[] = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.5f, 0.5f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.5f, 0.5f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.5f, 0.0f, 0.5f)
};

glm::vec3 lightColor =  glm::vec3(1.0f, 0.0f, 0.0f);


//天空盒顶点数组
const float skybox_vertices[] = {   
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



//光源和环境贴图的立方体都用这个立方体数组表示
float cube_vertices[] = {
    // positions          // normals
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};


int main()
{
    // glfw初始化，采用的GL版本为3.3核心版本
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //创建GL窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2018141411296", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    // 编译并链接 shaders 
    
    Shader ourShader("res/shader/model_loading.vs", "res/shader/model_loading.fs");
    Shader lampShader("res/shader/DrawLamp.vs", "res/shader/DrawLamp.fs");
    Shader skyboxShader("res/shader/skybox.vs", "res/shader/skybox.fs");
    Shader cubeShader("res/shader/cubemaps.vs", "res/shader/cubemaps.fs");


    // 设置1个定向光，6个点光源
    DirectLight dirLight = DirectLight(glm::vec3(0.3f, -0.5f, 1.0f), glm::vec3(0.05f), glm::vec3(0.4f), glm::vec3(1.0f));
    PointLight pointLights[6];

    // 点光源
    for (int i = 0; i < 6; i++)
    {
        pointLights[i] = PointLight(point_light_positions[i], 0.05f * light_colors[i], 0.8f * light_colors[i], light_colors[i], 1.0f, 0.09f, 0.032f);
    }

    // 聚光
    SpotLight spotLight = SpotLight(camera.Position, camera.Front, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f),
        1.0f, 0.09f, 0.032f, cos(glm::radians(12.5f)), cos(glm::radians(15.0f)));




    // 加载模型
    // -----------
    //想办法改成相对路径
    //Model ourModel("D:/workspace/OpenGlWorkspace/GUI/Project2/Project2/res/object/backpack/backpack.obj");
    Model ourModel("res/object/backpack/backpack.obj");
    // 线框模式
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // ---------------------绑定顶点数组对象----------------------
   
    
    // cube VAO VBO
    unsigned int cube_vao, cube_vbo;
    glGenVertexArrays(1, &cube_vao);
    glGenBuffers(1, &cube_vbo);
    glBindVertexArray(cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    //lights
    unsigned int lamp_vbo, lamp_vao;
    glGenVertexArrays(1, &lamp_vao);
    glGenBuffers(1, &lamp_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, lamp_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glBindVertexArray(lamp_vao);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //绑定天空盒VAO VBO
    unsigned int skybox_vao, skybox_vbo;
    glGenVertexArrays(1, &skybox_vao);
    glGenBuffers(1, &skybox_vbo);
    glBindVertexArray(skybox_vao);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //加载天空盒纹理
    std::vector<std::string> faces
    {
      ("res/skybox1/right.jpg"),
      ("res/skybox1/left.jpg"),
      ("res/skybox1/top.jpg"),
      ("res/skybox1/bottom.jpg"),
      ("res/skybox1/front.jpg"),
      ("res/skybox1/back.jpg"),
    };

    unsigned int cubemap_texture = load_cubemap(faces);

    cubeShader.use();
    cubeShader.setInt("skybox", 0);

    //使用SkyBoxShader着色器
    skyboxShader.use();
    //传入天空盒纹理
    skyboxShader.setInt("skybox", 0);


    //  GUI
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");


    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        

        ImGui::Begin("2018141411296");
        ImGui::Text("use WSAD to control,F to enable mouse");
        ImGui::Text("use mouse to zoom or rotate");
        //ImGui::Checkbox("Enable Mouse Rotate", &canMouseRotate);
        ImGui::Checkbox("dirlight", &isDirLight);
        ImGui::Checkbox("spotlight", &isSpotLight);
        ImGui::Text("Press 1 to 6 to turn on point lights");

        ImGui::ColorEdit3("color of light1", (float*)&light_colors[0]);

        ImGui::RadioButton("perspective camera", &isOrtho, 0);
        ImGui::SameLine();
        ImGui::RadioButton("ortho camera", &isOrtho, 1);
        ImGui::Checkbox("skybox and environment mapping", &isSkybox);

        ImGui::Text("scale object");
        ImGui::SliderFloat("xS", &xScale, 0.0f, 2.0f);
        ImGui::SliderFloat("yS", &yScale, 0.0f, 2.0f);
        ImGui::SliderFloat("zS", &zScale, 0.0f, 2.0f);

        ImGui::Text("translate object");
        ImGui::SliderFloat("xP", &xPosition, -10.0f, 10.0f);
        ImGui::SliderFloat("yP", &yPosition, -10.0f, 10.0f);
        ImGui::SliderFloat("zP", &zPosition, -10.0f, 10.0f);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();

       
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        ourShader.use();

        // view/projection transformations
        float ratio = (float)SCR_WIDTH / SCR_HEIGHT;
        glm::mat4 projection(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        if (!isOrtho)
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        else
            projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 100.0f);

        
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setVec3("viewPos", camera.Position);


        

        // 平行光光源
        dirLight.SetOn(isDirLight);
        dirLight.Draw(ourShader, "dirLight");

        // 点光源
        for (int i = 0; i < 6; i++)
        {
            std::stringstream name_stream;
            name_stream << "pointLights[" << i << "]";
            std::string name = name_stream.str();
            pointLights[i].SetOn(IsLightOn(i + 1));
            pointLights[i].Draw(ourShader, name);
        }

        // 手电筒光源
        spotLight.SetOn(isSpotLight);
        spotLight.SetPosition(camera.Position);
        spotLight.SetDirection(camera.Front);
        spotLight.Draw(ourShader, "spotLight");


        // 渲染已加载的模型
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(0.0f,0.0f,0.0f)); 
        model = glm::scale(model, glm::vec3(0.3f,0.3f,0.3f));
        model = glm::rotate(model, (float)glfwGetTime() * 0.3f, glm::vec3(0.0f, 1.0f, 0.0f));

        
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        //再来一个
        model = glm::translate(model, glm::vec3(xPosition, yPosition, zPosition));
        model = glm::scale(model, glm::vec3(xScale,yScale,zScale));
        model = glm::rotate(model, -(float)glfwGetTime() * 0.3f, glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);
  

        

        //lights
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        // 绘制灯
        glBindVertexArray(lamp_vao);
        for (int i = 0; i < 6; i++)
        {
            if (IsLightOn(i + 1))
            {
                glm::mat4 model(1.0f);
                model = glm::translate(model, point_light_positions[i]);
                model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
                lampShader.setMat4("model", model);
                lampShader.setVec3("lightColor", light_colors[i]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        if (isSkybox)
        {   
            //环境纹理立方体
            cubeShader.use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-4.0f, -1.0f, 0.0f));
            
            model = glm::rotate(model, -(float)glfwGetTime() * 0.3f, glm::vec3(0.5f, 1.0f, 0.0f));
            cubeShader.setMat4("projection", projection);
            cubeShader.setMat4("view", view);
            cubeShader.setVec3("cameraPos", camera.Position);
            cubeShader.setMat4("model", model);

            glBindVertexArray(cube_vao);
            //glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

            //skybox
            //深度测试  输入的深度值小于或等于参考值，则通过
            glDepthFunc(GL_LEQUAL);
            //禁止向深度缓冲区写入数据
            glDepthMask(GL_FALSE);
            skyboxShader.use();
            // remove translation from the view matrix
            view = glm::mat4(glm::mat3(camera.GetViewMatrix())); 
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

            skyboxShader.setMat4("view", view);
            skyboxShader.setMat4("projection", projection);

            glBindVertexArray(skybox_vao);
            //绑定天空盒纹理
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
            //绘制天空盒
            glDrawArrays(GL_TRIANGLES, 0, 36);
            //允许向深度缓冲区写入数据
            glDepthMask(GL_TRUE);
            glBindVertexArray(0);          
            glDepthFunc(GL_LESS);

        }
        

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


// 键盘控制回调
void key_callback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, true); break;
        //case GLFW_KEY_Z: light_bits ^= 1; break;
        case GLFW_KEY_1: light_bits ^= (1 << 1); break;
        case GLFW_KEY_2: light_bits ^= (1 << 2); break;
        case GLFW_KEY_3: light_bits ^= (1 << 3); break;
        case GLFW_KEY_4: light_bits ^= (1 << 4); break;
        case GLFW_KEY_5: light_bits ^= (1 << 5); break;
        case GLFW_KEY_6: light_bits ^= (1 << 6); break;
        //case GLFW_KEY_F: light_bits ^= (1 << 7); break;
        case GLFW_KEY_F: canMouseRotate = !canMouseRotate; break;
        default:
            break;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
       glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    // reversed since y-coordinates go from bottom to top
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;
    if(canMouseRotate)
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


// 控制开关灯
bool IsLightOn(int index)
{
    return (light_bits >> index) & 1;
}


unsigned int load_cubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrchannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrchannels, 0);
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