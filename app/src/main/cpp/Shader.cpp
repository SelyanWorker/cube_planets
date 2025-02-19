#include <vector>

#include "Shader.h"
#include "LogBuffer.h"

namespace {

    GLint GetUniformLocation(GLuint program, const std::string& name) {
        const GLint location = glGetUniformLocation(program, name.c_str());
        if (location == -1) {
            cp::out << name << " uniform not found" << std::endl;
        }
        return location;
    }


    GLuint CreateShader(GLenum shaderType, const std::string &shaderSource) {
        const GLuint shader = glCreateShader(shaderType);
        if (!shader) {
            cp::out << "glCreateShader failed" << std::endl;
            return 0;
        }

        const GLchar* shaderRawString = static_cast<const GLchar*>(shaderSource.c_str());
        GLint shaderLength = static_cast<GLint>(shaderSource.length());
        glShaderSource(shader, 1, &shaderRawString, &shaderLength);
        glCompileShader(shader);

        GLint shaderCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
        if (!shaderCompiled) {
            GLint infoLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
            if (infoLength) {
                std::vector<GLchar> log(static_cast<size_t>(infoLength));
                glGetShaderInfoLog(shader, infoLength, nullptr, log.data());
                cp::out << "Failed to compile with:\n" << log.data() << std::endl;
            }

            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }


    GLuint CreateShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
        const GLuint program = glCreateProgram();
        if (program == 0) {
            return 0;
        }

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength != 0) {
                std::vector<GLchar> log(static_cast<size_t>(logLength));
                glGetProgramInfoLog(program, logLength, nullptr, log.data());
                cp::out << "Failed to link program with:\n" << log.data() << std::endl;
            }

            glDeleteProgram(program);
            return 0;
        }

        return program;
    }

}


namespace cp {

    ShaderLib gShaderLib;

    std::shared_ptr<Shader> Shader::Create(
            const std::string &vertexSource,
            const std::string &fragmentSource,
            const std::string &positionAttributeName,
            const std::string &uvAttributeName,
            const std::string &normalAttributeName) {
        const GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, vertexSource);
        if (!vertexShader) {
            return nullptr;
        }

        const GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentSource);
        if (!fragmentShader) {
            glDeleteShader(vertexShader);
            return nullptr;
        }

        const GLuint program = CreateShaderProgram(vertexShader, fragmentShader);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        if (!program) {
            return nullptr;
        }

        GLint positionAttribute = glGetAttribLocation(program, positionAttributeName.c_str());
        GLint uvAttribute = glGetAttribLocation(program, uvAttributeName.c_str());
        GLint normalAttribute = glGetAttribLocation(program, normalAttributeName.c_str());

        /*if (positionAttribute == -1 || uvAttribute == -1 || normalAttribute == -1) {
            glDeleteProgram(program);
            return nullptr;
        }*/

        return std::shared_ptr<Shader>(
                new Shader(program,
                positionAttribute,
                uvAttribute,
                normalAttribute));
    }


    Shader::~Shader() {
        if (program) {
            glDeleteProgram(program);
            program = 0;
        }
    }


    void Shader::Activate() const {
        glUseProgram(program);
    }


    void Shader::Deactivate() const {
        glUseProgram(0);
    }


    void Shader::SetUniform(const std::string &name, const glm::mat4 &value) const {
        if (GLint location = GetUniformLocation(program, name); location != -1) {
            glUniformMatrix4fv(location, 1, false, &value[0][0]);
        }
    }


    void Shader::SetUniform(const std::string &name, const glm::vec3 &value) const {
        if (GLint location = GetUniformLocation(program, name); location != -1) {
            glUniform3f(location, value.x, value.y, value.z);
        }
    }


    void Shader::SetUniform(const std::string &name, float value) const {
        if (GLint location = GetUniformLocation(program, name); location != -1) {
            glUniform1f(location, value);
        }
    }


    void ShaderLib::Add(const std::string &name, Shader::Ptr shader) {
        if (shaders.find(name) != shaders.cend()) {
            out << "ShaderLib::Add failed, given " << name << "already exist" << std::endl;
            return;
        }

        shaders[name] = std::move(shader);
    }


    Shader::WeakPtr ShaderLib::Get(const std::string &name) const {
        auto found = shaders.find(name);
        if (found == shaders.cend()) {
            return std::weak_ptr<Shader>{};
        }

        return found->second;
    }

    void ShaderLib::Clear() {
        shaders.clear();
    }
}