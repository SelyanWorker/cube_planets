
#include "Geometry.h"

namespace cp {

    Geometry::Ptr CreateCube() {
        static Geometry::Ptr geometry = nullptr;
        if (geometry) {
            return geometry;
        }

        std::vector<Vertex> vertices =  {
                Vertex({-1.0f,  1.0f, -1.0f}, {0, 0}, {0, 0, -1}), /* Back. */
                Vertex({1.0f,  1.0f, -1.0f}, {1, 0}, {0, 0, -1}),
                Vertex({-1.0f, -1.0f, -1.0f}, {1, 1}, {0, 0, -1}),
                Vertex({1.0f, -1.0f, -1.0f}, {0, 1}, {0, 0, -1}),

                Vertex({-1.0f,  1.0f,  1.0f}, {0, 0}, {0, 0, 1}), /* Front. */
                Vertex({1.0f,  1.0f,  1.0f}, {1, 0}, {0, 0, 1}),
                Vertex({-1.0f, -1.0f,  1.0f}, {1, 1}, {0, 0, 1}),
                Vertex({1.0f, -1.0f,  1.0f}, {0, 1}, {0, 0, 1}),

                Vertex({-1.0f,  1.0f, -1.0f}, {0, 0}, {-1, 0, 0}), /* Left. */
                Vertex({-1.0f, -1.0f, -1.0f}, {1, 0}, {-1, 0, 0}),
                Vertex({-1.0f, -1.0f,  1.0f}, {1, 1}, {-1, 0, 0}),
                Vertex({-1.0f,  1.0f,  1.0f}, {0, 1}, {-1, 0, 0}),

                Vertex({1.0f,  1.0f, -1.0f}, {0, 0}, {1, 0, 0}), /* Right. */
                Vertex({1.0f, -1.0f, -1.0f}, {1, 0}, {1, 0, 0}),
                Vertex({1.0f, -1.0f,  1.0f}, {1, 1}, {1, 0, 0}),
                Vertex({1.0f,  1.0f,  1.0f}, {0, 1}, {1, 0, 0}),

                Vertex({-1.0f, -1.0f, -1.0f}, {0, 0}, {0, 1, 0}), /* Top. */
                Vertex({-1.0f, -1.0f,  1.0f}, {1, 0}, {0, 1, 0}),
                Vertex({1.0f, -1.0f,  1.0f}, {1, 1}, {0, 1, 0}),
                Vertex({1.0f, -1.0f, -1.0f}, {0, 1}, {0, 1, 0}),

                Vertex({-1.0f,  1.0f, -1.0f}, {0, 0}, {0, -1, 0}), /* Bottom. */
                Vertex({-1.0f,  1.0f,  1.0f}, {1, 0}, {0, -1, 0}),
                Vertex({1.0f,  1.0f,  1.0f}, {1, 1}, {0, -1, 0}),
                Vertex({1.0f,  1.0f, -1.0f}, {0, 1}, {0, -1, 0})
        };
        std::vector<Index> indices = { 0, 2, 3, 0, 1, 3, 4, 6, 7, 4, 5, 7, 8, 9, 10, 11, 8, 10, 12, 13, 14, 15, 12, 14, 16, 17, 18, 16, 19, 18, 20, 21, 22, 20, 23, 22 };

        geometry = Geometry::Create(std::move(vertices), std::move(indices));
        return geometry;
    }

} // cp