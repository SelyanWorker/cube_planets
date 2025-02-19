#pragma once

#include <string>
#include <map>

#include <GLES3/gl3.h>
#include <glm/glm.hpp>

namespace cp {

    class Shader {
    public:
        using Ptr = std::shared_ptr<Shader>;
        using WeakPtr = std::weak_ptr<Shader>;

    public:
        static Ptr Create(
                const std::string &vertexSource,
                const std::string &fragmentSource,
                const std::string &positionAttributeName,
                const std::string &uvAttributeName,
                const std::string &normalAttributeName);

        ~Shader();

        void Activate() const;
        void Deactivate() const;

        void SetUniform(const std::string& name, const glm::mat4& value) const;
        void SetUniform(const std::string& name, const glm::vec3& value) const;
        void SetUniform(const std::string& name, float value) const;

        GLuint GetPositionAttributeIndex() const { return positionIndex; }
        GLuint GetUVAttributeIndex() const { return uvIndex; }
        GLuint GetNormalAttributeIndex() const { return normalIndex; }

    private:
        Shader(GLuint program, GLint position, GLint uv, GLint normal)
        : program(program)
        , positionIndex(position)
        , uvIndex(uv)
        , normalIndex(normal) {}

    private:
        GLuint program{0};
        GLint positionIndex{0};
        GLint uvIndex{0};
        GLint normalIndex{0};
    };

    class ShaderLib {
    public:
        void Add(const std::string& name, Shader::Ptr shader);
        [[nodiscard]] Shader::WeakPtr Get(const std::string& name) const;

        void Clear();

    private:
        std::map<std::string, Shader::Ptr> shaders;
    };

    extern ShaderLib gShaderLib;

}