#include "Shader.h"
#include "AndroidOut.h"
#include "Utility.h"

namespace {

    GLint GetUniformLocation(GLuint program, const std::string& name)
    {
        const GLint location = glGetUniformLocation(program, name.c_str());
        if (location == -1) {
            aout << name << " uniform not found" << std::endl;
        }
        return location;
    }


    GLuint LoadShader(GLenum shaderType, const std::string &shaderSource) {
        Utility::assertGlError();

        const GLuint shader = glCreateShader(shaderType);
        if (!shader) {
            return 0;
        }

        auto *shaderRawString = static_cast<const GLchar*>(shaderSource.c_str());
        GLint shaderLength = shaderSource.length();
        glShaderSource(shader, 1, &shaderRawString, &shaderLength);
        glCompileShader(shader);

        GLint shaderCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);

        if (!shaderCompiled) {
            GLint infoLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

            if (infoLength) {
                auto *infoLog = new GLchar[infoLength];
                glGetShaderInfoLog(shader, infoLength, nullptr, infoLog);
                aout << "Failed to compile with:\n" << infoLog << std::endl;
                delete[] infoLog;
            }

            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

}


Shader *Shader::Create(
        const std::string &vertexSource,
        const std::string &fragmentSource,
        const std::string &positionAttributeName,
        const std::string &uvAttributeName,
        const std::string &normalAttributeName) {
    Shader *shader = nullptr;

    const GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) {
        return nullptr;
    }

    const GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return nullptr;
    }

    const GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength) {
                GLchar *log = new GLchar[logLength];
                glGetProgramInfoLog(program, logLength, nullptr, log);
                aout << "Failed to link program with:\n" << log << std::endl;
                delete[] log;
            }

            glDeleteProgram(program);
        } else {
            GLint positionAttribute = glGetAttribLocation(program, positionAttributeName.c_str());
            GLint uvAttribute = glGetAttribLocation(program, uvAttributeName.c_str());
            GLint normalAttribute = glGetAttribLocation(program, normalAttributeName.c_str());

            shader = new Shader(
                    program,
                    positionAttribute,
                    uvAttribute,
                    normalAttribute);

            // Only create a new shader if all the attributes are found.
            /*if (positionAttribute != -1
                && uvAttribute != -1
                && normalAttribute != -1) {

                shader = new Shader(
                        program,
                        positionAttribute,
                        uvAttribute,
                        normalAttribute);
            } else {
                glDeleteProgram(program);
            }*/
        }
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}


void Shader::Activate() const {
    glUseProgram(program);
}


void Shader::Deactivate() const {
    glUseProgram(0);
}


void Shader::SetUniform(const std::string& name, const glm::mat4& value) const
{
    if (GLint location = GetUniformLocation(program, name); location != -1) {
        glUniformMatrix4fv(location, 1, false, &value[0][0]);
    }
}


void Shader::SetUniform(const std::string& name, const glm::vec3& value) const
{
    if (GLint location = GetUniformLocation(program, name); location != -1) {
        glUniform3f(location, value.x, value.y, value.z);
    }
}
