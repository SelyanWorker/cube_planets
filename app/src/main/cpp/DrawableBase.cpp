#include "DrawableBase.h"

namespace cp {

    void DrawableBase::Draw(const Shader &shader) const {
        if (!geometry) {
            return;
        }

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
        glVertexAttribPointer(
                positionIndex,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                pVertexData
        );
        glEnableVertexAttribArray(positionIndex);

        glVertexAttribPointer(
                uvIndex,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                ((uint8_t *) pVertexData) + sizeof(Vertex::position)
        );
        glEnableVertexAttribArray(uvIndex);

        glVertexAttribPointer(
                normalIndex,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                ((uint8_t *) pVertexData) + sizeof(Vertex::position) + sizeof(Vertex::uv)
        );
        glEnableVertexAttribArray(normalIndex);
    }

    void DrawableBase::UnbindGeometry(const Shader& shader) const {
        glDisableVertexAttribArray(shader.GetNormalAttributeIndex());
        glDisableVertexAttribArray(shader.GetUVAttributeIndex());
        glDisableVertexAttribArray(shader.GetPositionAttributeIndex());
    }

} // cp