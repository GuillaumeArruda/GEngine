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
            default:
                std::cerr << "OpenGLError: Unknow error: " << error << "\n";
                break;
            }
        }
    }
}