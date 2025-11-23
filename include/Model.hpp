#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>
#include "Math.hpp"

struct Face {
    int a = 0;
    int b = 0;
    int c = 0;
    int mat = -1;
};

struct Material {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    std::string name;
};

class Model {
public:
    Model();

    const std::vector<Vec3> &getVertices() const;
    const std::vector<Face> &getFaces() const;

    bool loadFromObj(const std::string &path);
    bool loadFromMtl(const std::string &path);

    bool hasMaterial() const;
    void getFaceColor(int faceIndex,
                      float &r, float &g, float &b) const;

private:
    void normalize();

    std::vector<Vec3> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Material> m_materials;
    bool m_hasMaterial;
};

#endif
