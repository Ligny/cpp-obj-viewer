#include "Model.hpp"
#include <fstream>
#include <sstream>
#include <limits>
#include <iostream>

Model::Model()
{
    m_vertices.clear();
    m_faces.clear();
    m_materials.clear();
    m_hasMaterial = false;
}

const std::vector<Vec3> &Model::getVertices() const
{
    return m_vertices;
}

const std::vector<Face> &Model::getFaces() const
{
    return m_faces;
}

void Model::normalize()
{
    if (m_vertices.empty())
        return;
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    float cx;
    float cy;
    float cz;
    float maxRange;
    float invScale;

    minX = std::numeric_limits<float>::infinity();
    maxX = -minX;
    minY = minX;
    maxY = maxX;
    minZ = minX;
    maxZ = maxX;
    for (const Vec3 &v : m_vertices) {
        if (v.x < minX) minX = v.x;
        if (v.x > maxX) maxX = v.x;
        if (v.y < minY) minY = v.y;
        if (v.y > maxY) maxY = v.y;
        if (v.z < minZ) minZ = v.z;
        if (v.z > maxZ) maxZ = v.z;
    }
    cx = (minX + maxX) * 0.5f;
    cy = (minY + maxY) * 0.5f;
    cz = (minZ + maxZ) * 0.5f;
    maxRange = maxX - minX;
    if (maxRange < maxY - minY)
        maxRange = maxY - minY;
    if (maxRange < maxZ - minZ)
        maxRange = maxZ - minZ;
    if (maxRange <= 0.0f)
        return;
    invScale = 2.0f / maxRange;
    for (Vec3 &v : m_vertices) {
        v.x = (v.x - cx) * invScale;
        v.y = (v.y - cy) * invScale;
        v.z = (v.z - cz) * invScale;
    }
}

bool Model::loadFromMtl(const std::string &path)
{
    std::ifstream file;
    std::string line;
    Material current;
    bool inMat;
    bool any;

    file.open(path.c_str());
    if (!file.is_open())
        return false;
    m_materials.clear();
    inMat = false;
    any = false;
    while (std::getline(file, line)) {
        if (line.rfind("newmtl", 0) == 0) {
            if (inMat)
                m_materials.push_back(current);
            current = Material();
            std::string name;
            std::string token;
            std::istringstream iss(line);

            iss >> token >> name;
            current.name = name;
            inMat = true;
        } else if (line.size() > 2 && line[0] == 'K'
                   && line[1] == 'd' && inMat) {
            std::istringstream iss(line.substr(2));
            float r;
            float g;
            float b;

            iss >> r >> g >> b;
            if (iss) {
                current.r = r;
                current.g = g;
                current.b = b;
                any = true;
            }
        }
    }
    if (inMat)
        m_materials.push_back(current);
    file.close();
    m_hasMaterial = any && !m_materials.empty();
    return m_hasMaterial;
}

bool Model::loadFromObj(const std::string &path)
{
    std::ifstream file;
    std::string line;
    std::vector<Vec3> verts;
    std::vector<Face> faces;
    int currentMat;

    currentMat = -1;
    file.open(path.c_str());
    if (!file.is_open())
        return false;
    while (std::getline(file, line)) {
        if (line.size() < 2)
            continue;

        if (line[0] == 'v' && line[1] == ' ') {
            std::istringstream iss(line.substr(2));
            float x;
            float y;
            float z;

            iss >> x >> y >> z;
            verts.push_back(make_vec3(x, y, z));
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            std::istringstream iss(line.substr(2));
            std::vector<int> indices;
            std::string token;

            while (iss >> token) {
                std::size_t slashPos;
                std::string num;
                int idx;

                slashPos = token.find('/');
                if (slashPos == std::string::npos)
                    num = token;
                else
                    num = token.substr(0, slashPos);
                if (num.empty())
                    continue;
                idx = std::stoi(num);
                indices.push_back(idx - 1);
            }

            if (indices.size() >= 3) {
                std::size_t i;
                Face f;

                i = 1;
                while (i + 1 < indices.size()) {
                    f.a = indices[0];
                    f.b = indices[i];
                    f.c = indices[i + 1];
                    f.mat = currentMat;
                    faces.push_back(f);
                    i++;
                }
            }
        }
        else if (line.rfind("usemtl", 0) == 0) {
            std::string name;
            std::string token;
            std::istringstream iss(line);
            int matIndex;

            iss >> token >> name;
            matIndex = -1;
            for (std::size_t i = 0; i < m_materials.size(); ++i) {
                if (m_materials[i].name == name) {
                    matIndex = (int)i;
                    break;
                }
            }
            currentMat = matIndex;
        }
    }
    file.close();

    if (verts.empty() || faces.empty())
        return false;
    m_vertices = verts;
    m_faces = faces;
    normalize();
    return true;
}

bool Model::hasMaterial() const
{
    return m_hasMaterial;
}

void Model::getFaceColor(int faceIndex,
                         float &r, float &g, float &b) const
{
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;

    if (!m_hasMaterial)
        return;
    if (faceIndex < 0
        || static_cast<std::size_t>(faceIndex) >= m_faces.size())
        return;

    int midx = m_faces[faceIndex].mat;
    if (midx < 0
        || static_cast<std::size_t>(midx) >= m_materials.size())
        return;

    r = m_materials[midx].r;
    g = m_materials[midx].g;
    b = m_materials[midx].b;
}