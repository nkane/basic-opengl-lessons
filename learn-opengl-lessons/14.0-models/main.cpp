/*
 *  Multiple Lights
 */


#include <cstddef>
#include <stdio.h>
#include <memory.h>
#include <math.h>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>

#define GLM_DEFINED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.cpp"
#include "camera.cpp"
#include "material.cpp"
#include "texture.cpp"

#include "vertex.cpp"
#include "mesh.cpp"
#include "model.cpp"

static unsigned int WireFrameEnabled = 0;
static unsigned int width = 800;
static unsigned int height = 600;

static Camera *camera = NULL;
static glm::vec3 camera_position   = glm::vec3(0.0f, 0.0f, 3.0f);
static glm::vec3 camera_up         = glm::vec3(0.0f, 1.0f, 0.0f);
static float delta_time = 0.0f;
static float last_frame = 0.0f;

static float yaw = -90.0f;
static float pitch = 0.0f;
static glm::vec3 direction;

void
framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void
process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) ==  GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        WireFrameEnabled = ~WireFrameEnabled;
    }

    const float camera_speed = 2.5f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        ProcessKeyboard(camera, FORWARD, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        ProcessKeyboard(camera, BACKWARD, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        ProcessKeyboard(camera, LEFT, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        ProcessKeyboard(camera, RIGHT, delta_time);
    }
}

void
mouse_callback(GLFWwindow *window, double x, double y)
{
    static bool first_mouse = true;
    static float last_x = 400.0f;
    static float last_y = 300.0f;
    if (first_mouse)
    {
        last_x = x;
        last_y = y;
        first_mouse = false;
        return;
    }
    float x_offset = x - last_x;
    float y_offset = last_y - y;
    last_x = x;
    last_y = y;
    ProcessMouseMovement(camera, x_offset, y_offset, true);
}

void
scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
{
    ProcessMouseScroll(camera, (float)y_offset);
}

int
main()
{
    GLFWwindow *window;
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (glewInit() != GLEW_OK)
    {
        printf("[GLEW ERROR]: failed to init glew\n");
    }
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Texture container_texture = {};
    stbi_set_flip_vertically_on_load(true);
    container_texture.data = stbi_load("assets/container2.png", &container_texture.width, &container_texture.height, &container_texture.channels, 0);
    if (!container_texture.data)
    {
        printf("[STB_Image ERROR]: failed to load image\n");
        return -1;
    }
    glGenTextures(1, &container_texture.id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, container_texture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, container_texture.width, container_texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, container_texture.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(container_texture.data);

    Texture container_metal_outline_texture = {};
    stbi_set_flip_vertically_on_load(true);
    container_metal_outline_texture.data = stbi_load("assets/container2_specular.png",
                                           &container_metal_outline_texture.width,
                                           &container_metal_outline_texture.height,
                                           &container_metal_outline_texture.channels, 0);
    if (!container_metal_outline_texture.data)
    {
        printf("[STB_Image ERROR]: failed to load image\n");
        return -1;
    }
    glGenTextures(1, &container_metal_outline_texture.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, container_metal_outline_texture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, container_metal_outline_texture.width, container_metal_outline_texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, container_metal_outline_texture.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(container_metal_outline_texture.data);

    // bind vertex array
    unsigned int vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // copy vertices array in a vertex buffer
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glm::vec3 cube_positions[] = 
    {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 point_light_positions[] =
    {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    unsigned vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set vertex arrtribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int vertex_array_light_id;
    glGenVertexArrays(1, &vertex_array_light_id);
    glBindVertexArray(vertex_array_light_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    ShaderProgram *shader_program = CreateShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");   
    if (shader_program == NULL)
    {
        printf("[SHADER ERROR]: failed to load base shader\n");
    }

    ShaderProgram *light_shader_program = CreateShaderProgram("shaders/vertex_light_source.glsl", "shaders/fragment_light_source.glsl");
    if (light_shader_program == NULL)
    {
        printf("[SHADER ERROR]: failed to load light shader\n");
    } 

    ShaderProgram *model_shader_program = CreateShaderProgram("shaders/model_vertex.glsl", "shaders/model_fragment.glsl");
    if (model_shader_program == NULL)
    {
        printf("[SHADER ERROR]: failed to model shader\n");
    }

    Model *model_obj = CreateModel("assets/rock.obj");
    if (model_obj == NULL)
    {
        printf("[MODEL ERROR]: failed to load model\n");
    }

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    camera = CreateCamera(camera_position, camera_up, yaw, pitch);

    glUseProgram(shader_program->id);
    SetIntUniform(shader_program, "u_material.diffuse", 0);
    SetIntUniform(shader_program, "u_material.specular", 1);

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);
        {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (WireFrameEnabled)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            glm::mat4 model = glm::mat4(1.0f);

            // render cubes
            glUseProgram(shader_program->id);
            glBindVertexArray(vertex_array_id);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, container_texture.id);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, container_metal_outline_texture.id);
            SetFloatVec3Uniform(shader_program, "u_view_position", camera->position);
            SetFloatUniform(shader_program, "u_material.shininess", 32.0f);
            /*
             *  here we set all the uniforms for the 5/6 types of light we have. we have to set them manually and index
             *  the proper point light struct array to set each uniform variable. this can be done more code-friendly
             *  by defining light types as classes and set their values in there, or by using a more efficient uniform approach
             *  by using 'uniform buffer ojects', but this is something we'll discuss in the 'Advanced GLSL' tutorial.
             */
            // directional light
            SetFloatVec3Uniform(shader_program, "u_directional_light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
            SetFloatVec3Uniform(shader_program, "u_directional_light.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
            SetFloatVec3Uniform(shader_program, "u_directional_light.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
            SetFloatVec3Uniform(shader_program, "u_directional_light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            // point light 1
            SetFloatVec3Uniform(shader_program, "u_point_lights[0].position", point_light_positions[0]);
            SetFloatVec3Uniform(shader_program, "u_point_lights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
            SetFloatVec3Uniform(shader_program, "u_point_lights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            SetFloatVec3Uniform(shader_program, "u_point_lights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
            SetFloatUniform(shader_program, "u_point_lights[0].constant", 1.0f);
            SetFloatUniform(shader_program, "u_point_lights[0].linear", 0.09f);
            SetFloatUniform(shader_program, "u_point_lights[0].quadratic", 0.032f);
            // point light 2
            SetFloatVec3Uniform(shader_program, "u_point_lights[1].position", point_light_positions[1]);
            SetFloatVec3Uniform(shader_program, "u_point_lights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
            SetFloatVec3Uniform(shader_program, "u_point_lights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            SetFloatVec3Uniform(shader_program, "u_point_lights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
            SetFloatUniform(shader_program, "u_point_lights[1].constant", 1.0f);
            SetFloatUniform(shader_program, "u_point_lights[1].linear", 0.09f);
            SetFloatUniform(shader_program, "u_point_lights[1].quadratic", 0.032f);
            // point light 3 
            SetFloatVec3Uniform(shader_program, "u_point_lights[2].position", point_light_positions[2]);
            SetFloatVec3Uniform(shader_program, "u_point_lights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
            SetFloatVec3Uniform(shader_program, "u_point_lights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            SetFloatVec3Uniform(shader_program, "u_point_lights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
            SetFloatUniform(shader_program, "u_point_lights[2].constant", 1.0f);
            SetFloatUniform(shader_program, "u_point_lights[2].linear", 0.09f);
            SetFloatUniform(shader_program, "u_point_lights[2].quadratic", 0.032f);
            // point light 4 
            SetFloatVec3Uniform(shader_program, "u_point_lights[3].position", point_light_positions[2]);
            SetFloatVec3Uniform(shader_program, "u_point_lights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
            SetFloatVec3Uniform(shader_program, "u_point_lights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            SetFloatVec3Uniform(shader_program, "u_point_lights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
            SetFloatUniform(shader_program, "u_point_lights[3].constant", 1.0f);
            SetFloatUniform(shader_program, "u_point_lights[3].linear", 0.09f);
            SetFloatUniform(shader_program, "u_point_lights[3].quadratic", 0.032f);
            // point light 4 
            SetFloatVec3Uniform(shader_program, "u_point_lights[4].position", point_light_positions[2]);
            SetFloatVec3Uniform(shader_program, "u_point_lights[4].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
            SetFloatVec3Uniform(shader_program, "u_point_lights[4].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            SetFloatVec3Uniform(shader_program, "u_point_lights[4].specular", glm::vec3(1.0f, 1.0f, 1.0f));
            SetFloatUniform(shader_program, "u_point_lights[4].constant", 1.0f);
            SetFloatUniform(shader_program, "u_point_lights[4].linear", 0.09f);
            SetFloatUniform(shader_program, "u_point_lights[4].quadratic", 0.032f);
            // spotlight
            SetFloatVec3Uniform(shader_program, "u_spot_light.position", camera->position);
            SetFloatVec3Uniform(shader_program, "u_spot_light.direction", camera->front);
            SetFloatVec3Uniform(shader_program, "u_spot_light.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            SetFloatVec3Uniform(shader_program, "u_spot_light.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
            SetFloatVec3Uniform(shader_program, "u_spot_light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            SetFloatUniform(shader_program, "u_spot_light.constant", 1.0f);
            SetFloatUniform(shader_program, "u_spot_light.linear", 0.09f);
            SetFloatUniform(shader_program, "u_spot_light.quadratic", 0.032f);
            SetFloatUniform(shader_program, "u_spot_light.cut_off", glm::cos(glm::radians(12.5f)));
            SetFloatUniform(shader_program, "u_spot_light.outer_cut_off", glm::cos(glm::radians(15.5f)));
            // view and projection tranformations
            glm::mat4 view = glm::mat4(1.0f);
            view = CustomGetViewMatrix(camera);
            glm::mat4 perspective_projection = glm::perspective(glm::radians(camera->zoom), (float)width / (float)height, 0.1f, 100.0f);
            SetFloatMat4Uniform(shader_program, "u_view", glm::value_ptr(view));
            SetFloatMat4Uniform(shader_program, "u_projection", glm::value_ptr(perspective_projection));
            for (unsigned int i = 0; i < 10; i++) 
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, cube_positions[i]);
                float angle = 20.0f * i;
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                SetFloatMat4Uniform(shader_program, "u_model", glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // render lamps
            glUseProgram(light_shader_program->id);
            glBindVertexArray(vertex_array_light_id);
            SetFloatMat4Uniform(light_shader_program, "u_view", glm::value_ptr(view));
            SetFloatMat4Uniform(light_shader_program, "u_projection", glm::value_ptr(perspective_projection));
            for (unsigned int i = 0; i < 4; i++)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, point_light_positions[i]);
                model = glm::scale(model, glm::vec3(0.2f));
                SetFloatMat4Uniform(light_shader_program, "u_model", glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            glUseProgram(model_shader_program->id);
            SetFloatMat4Uniform(model_shader_program, "view", glm::value_ptr(view));
            SetFloatMat4Uniform(model_shader_program, "projection", glm::value_ptr(perspective_projection));
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
            SetFloatMat4Uniform(model_shader_program, "model", glm::value_ptr(model));
            DrawModel(model_obj, model_shader_program);

            float current_frame = glfwGetTime();
            delta_time = current_frame - last_frame;
            last_frame = current_frame;
        }
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    {
        ReleaseShaderProgram(shader_program);
        ReleaseShaderProgram(light_shader_program);
        ReleaseShaderProgram(model_shader_program);
        glDeleteVertexArrays(1, &vertex_array_id);
        glDeleteBuffers(1, &vertex_buffer_id);
        glfwTerminate();
    }
    
    return 0;
}
