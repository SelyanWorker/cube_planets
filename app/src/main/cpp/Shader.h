#pragma once

#include <string>
#include <GLES3/gl3.h>

#include "Entity.h"

class Shader {
public:
    static Shader *Create(
            const std::string &vertexSource,
            const std::string &fragmentSource,
            const std::string &positionAttributeName,
            const std::string &uvAttributeName,
            const std::string &normalAttributeName);

    inline ~Shader() {
        if (program) {
            glDeleteProgram(program);
            program = 0;
        }
    }

    void Activate() const;
    void Deactivate() const;

    void SetUniform(const std::string& name, const glm::mat4& value) const;
    void SetUniform(const std::string& name, const glm::vec3& value) const;

    GLuint GetPositionAttributeIndex() const { return positionIndex; }
    GLuint GetUVAttributeIndex() const { return uvIndex; }
    GLuint GetNormalAttributeIndex() const { return normalIndex; }

private:
    constexpr Shader(GLuint program, GLint position, GLint uv, GLint normal)
    : program(program),
      positionIndex(position),
      uvIndex(uv),
      normalIndex(normal) {}

private:
    GLuint program;
    GLint positionIndex;
    GLint uvIndex;
    GLint normalIndex;
};
