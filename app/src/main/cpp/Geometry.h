#pragma once

#include <memory>
#include <vector>

#include <GLES3/gl3.h>
#include <glm/glm.hpp>

namespace cp {

    struct Vertex {
        Vertex(const glm::vec3& position, const glm::vec2& uv, const glm::vec3& normal)
        : position(position)
        , uv(uv)
        , normal(normal) {}

        glm::vec3 position;
        glm::vec2 uv;
        glm::vec3 normal;
    };

    using Index = uint16_t;

    class Geometry {
    public:
        using Ptr = std::shared_ptr<Geometry>;

    public:
        static Ptr Create(std::vector<Vertex>&& vertices, std::vector<Index>&& indices) {
            return Ptr(new Geometry(std::move(vertices), std::move(indices)));
        }

        const std::vector<Vertex>& GetVertices() const { return vertices; }
        const std::vector<Index>& GetIndices() const { return indices; }

    private:
        Geometry(std::vector<Vertex>&& vertices, std::vector<Index>&& indices)
        : vertices(std::move(vertices))
        , indices(std::move(indices)) {}

    private:
        std::vector<Vertex> vertices;
        std::vector<Index> indices;
    };

    Geometry::Ptr CreateCube();

} // cp
