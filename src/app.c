#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <camera.h>
#include <stdio.h>
#include <shader.h>
#include <voxel.h>
#include <snx/math/math.h>
#include <unistd.h>

void framebuffer_size_callback(GLFWwindow*, int, int);
void mouse_callback(GLFWwindow*, double, double);
void key_callback(GLFWwindow*, int, int, int, int);

int screen_width = 800, screen_height = 600;
GLFWwindow* window;

float delta_time;
stnx_camera camera;
stnx_voxel_object* object;
stnx_shader* shader;

int input_mode = 0;
void toggle_input_mode() {
    glfwSetInputMode(window, GLFW_CURSOR, (input_mode = !input_mode) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

int main(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    printf("Using GLFW Version: %s\n", glfwGetVersionString());
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    window = glfwCreateWindow(screen_width, screen_height, "OpenGL Test", 0, 0);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    toggle_input_mode();
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }


    printf("Using GLAD OpenGL Version: %d.%d\n", GLVersion.major, GLVersion.minor);

    stnx_shader_initialize(&shader, &object);
    stnx_voxel_initialize_object(&object, stnx_math_vec3i(5, 5, 5));
    stnx_camera_init(&camera, stnx_math_vec3(64.0f, 64.0f, 64.0f), 0.0f, 0.0f, screen_width, screen_height, 2.5f);

    double last_time = glfwGetTime(), current_time;
    while (!glfwWindowShouldClose(window)) {
        current_time = glfwGetTime();
        delta_time = (float)(current_time - last_time);
        usleep(10000);
        last_time = current_time;
        glfwPollEvents();
        if (input_mode) stnx_camera_update_movement(&camera,
                                    glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS,
                                    glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS,
                                    glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS,
                                    glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS,
                                    glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS,
                                    glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS,
                                    delta_time);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        stnx_voxel_render_object(object, &camera);
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    } else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        toggle_input_mode();
    }

}

void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    screen_width = width;
    screen_height = height;
    glViewport(0, 0, screen_width, screen_height);
    stnx_camera_update_projection(&camera, screen_width, screen_height, 2.5f);
}

void mouse_callback(GLFWwindow* w, double x_pos, double y_pos) {
    static double x_last, y_last;
    static int first = 1;
    if (first) {
        first = 0;
        x_last = x_pos;
        y_last = y_pos;
        return;
    }
    double x_off = x_last - x_pos;
    double y_off = y_last - y_pos;
    x_last = x_pos;
    y_last = y_pos;
    if (!input_mode) return;
    float sensitivity = 0.01f;
    camera.yaw -= (float)x_off * sensitivity;
    camera.pitch += (float)y_off * sensitivity;
    stnx_camera_update_yaw_pitch(&camera);
}