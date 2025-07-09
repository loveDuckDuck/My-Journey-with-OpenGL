// #ifndef CALLBACK_H
// #define CALLBACK_H

// #include <iostream>

// class Callback
// {

// public:
//     // glfw: whenever the mouse moves, this callback is called
//     // -------------------------------------------------------
//     void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
//     {
//         float xpos = static_cast<float>(xposIn);
//         float ypos = static_cast<float>(yposIn);

//         if (firstMouse)
//         {
//             lastX = xpos;
//             lastY = ypos;
//             firstMouse = false;
//         }

//         float xoffset = xpos - lastX;
//         float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

//         lastX = xpos;
//         lastY = ypos;

//         camera.ProcessMouseMovement(xoffset, yoffset);
//     }
// };

// #endif