#include <stdafx.h>

#include "grender/utils.h"

namespace grender
{
    void check_gl_error()
    {
        GLenum const error = glGetError();
        if (error != GL_NO_ERROR)
        {
            switch (error)
            {
            case GL_INVALID_ENUM:
                std::cerr << "OpenGLError: Invalid Enum.\n";
                break;
            case GL_INVALID_VALUE:
                std::cerr << "OpenGLError: Invalid Value.\n";
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "OpenGLError: Invalid Operation.\n";
                break;
            case GL_STACK_OVERFLOW:
                std::cerr << "OpenGLError: Stack overflow.\n";
                break;
            case GL_STACK_UNDERFLOW:
                std::cerr << "OpenGLError: Stack underflow.\n";
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "OpenGLError: Out of memory.\n";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr << "OpenGLError: Invalid framebuffer operation.\n";
                break;
            default:
                std::cerr << "OpenGLError: Unknow error: " << error << "\n";
                break;
            }
        }
    }

    glm::vec3 generate_valid_up(glm::vec3 forward_vector)
    {
        glm::vec3 const normalized_forward = glm::normalize(forward_vector);
        glm::vec3 const world_up = glm::vec3(0.f, 1.f, 0.f);
        glm::vec3 const fallback_up = glm::vec3(0.f, 0.f, 1.f);
        if (glm::abs(glm::dot(normalized_forward, world_up)) > 0.99f)
        {
            glm::vec3 const right_vector = glm::cross(normalized_forward, glm::vec3(0.f, 0.f, 1.f));
            return glm::normalize(glm::cross(right_vector, normalized_forward));
        }
        glm::vec3 const right_vector = glm::normalize(glm::cross(world_up, normalized_forward));
        return glm::normalize(glm::cross(normalized_forward, right_vector));
    }
}