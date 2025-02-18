#include "DrawableBase.h"

namespace cp {

    void DrawableBase::Draw(const Shader &shader) const {
        shader.SetUniform("uModel", CreateModelMatrix());

        BindGeometry(shader);
        DrawElements();
        UnbindGeometry(shader);
    }

    void DrawableBase::DrawElements() const {
        const auto& indices = geometry->GetIndices();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_SHORT, indices.data());
    }

    void DrawableBase::BindGeometry(const Shader& shader) const {
        const GLuint positionIndex = shader.GetPositionAttributeIndex();
        const GLuint uvIndex = shader.GetUVAttributeIndex();
        const GLuint normalIndex = shader.GetNormalAttributeIndex();

        auto pVertexData = geometry->GetVertices().data();
        // The position attribute is 3 floats
        glVertexAttribPointer(
                positionIndex, // attrib
                3, // elements
                GL_FLOAT, // of type float
                GL_FALSE, // don't normalize
                sizeof(Vertex), // stride is Vertex bytes
                pVertexData // pull from the start of the vertex data
        );
        glEnableVertexAttribArray(positionIndex);

        // The uv attribute is 2 floats
        glVertexAttribPointer(
                uvIndex, // attrib
                2, // elements
                GL_FLOAT, // of type float
                GL_FALSE, // don't normalize
                sizeof(Vertex), // stride is Vertex bytes
                ((uint8_t *) pVertexData) + sizeof(Vertex::position) // offset position data from the start
        );
        glEnableVertexAttribArray(uvIndex);

        // The normal attribute is 3 floats
        glVertexAttribPointer(
                normalIndex, // attrib
                3, // elements
                GL_FLOAT, // of type float
                GL_FALSE, // don't normalize
                sizeof(Vertex), // stride is Vertex bytes
                ((uint8_t *) pVertexData) + sizeof(Vertex::position) + sizeof(Vertex::uv) // offset position and uv data from the start
        );
        glEnableVertexAttribArray(normalIndex);
    }

    void DrawableBase::UnbindGeometry(const Shader& shader) const {
        glDisableVertexAttribArray(shader.GetNormalAttributeIndex());
        glDisableVertexAttribArray(shader.GetUVAttributeIndex());
        glDisableVertexAttribArray(shader.GetPositionAttributeIndex());
    }

} // cp