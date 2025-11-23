#include "Renderer.hpp"
#include "Math.hpp"
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>

struct TriData {
    Vec3 w1;
    Vec3 w2;
    Vec3 w3;
    Vec2 p1;
    Vec2 p2;
    Vec2 p3;
    float intensity;
    unsigned char baseR;
    unsigned char baseG;
    unsigned char baseB;
};

Renderer::Renderer()
{
    m_model = nullptr;
    m_angleY = 0.0f;
    m_angleX = 0.0f;
    m_zoom = 1.0f;
    m_showEdges = false;
}

void Renderer::setModel(const Model *model)
{
    m_model = model;
}

void Renderer::setAngles(float angleY, float angleX)
{
    m_angleY = angleY;
    m_angleX = angleX;
}

void Renderer::setZoom(float zoom)
{
    m_zoom = zoom;
}

void Renderer::setShowEdges(bool showEdges)
{
    m_showEdges = showEdges;
}

static float compute_intensity(const Vec3 &w1,
                               const Vec3 &w2,
                               const Vec3 &w3)
{
    Vec3 e1;
    Vec3 e2;
    Vec3 n;
    Vec3 lightDir;
    float dot;
    float intensity;

    e1 = sub_vec3(w2, w1);
    e2 = sub_vec3(w3, w1);
    n = cross_vec3(e1, e2);
    n = normalize_vec3(n);
    lightDir = make_vec3(0.4f, 0.7f, -0.6f);
    lightDir = normalize_vec3(lightDir);
    dot = dot_vec3(n, lightDir);
    if (dot < 0.0f)
        dot = 0.0f;
    intensity = 0.3f + 0.7f * dot;
    return intensity;
}

static void build_triangles(const Model *model,
                            float angleY, float angleX,
                            float zoom,
                            float width, float height,
                            std::vector<TriData> &out)
{
    const std::vector<Vec3> *verts;
    const std::vector<Face> *faces;
    Vec3 camOffset;
    std::size_t i;

    verts = &model->getVertices();
    faces = &model->getFaces();
    out.clear();
    out.reserve(faces->size());
    camOffset = make_vec3(0.0f, 0.0f, 4.0f);
    i = 0;
    while (i < faces->size()) {
        const Face &f = (*faces)[i];
        TriData t;
        Vec3 wCenter;
        float cr;
        float cg;
        float cb;

        t.w1 = rotate_xy((*verts)[f.a], angleY, angleX);
        t.w2 = rotate_xy((*verts)[f.b], angleY, angleX);
        t.w3 = rotate_xy((*verts)[f.c], angleY, angleX);
        t.w1 = translate(t.w1, camOffset);
        t.w2 = translate(t.w2, camOffset);
        t.w3 = translate(t.w3, camOffset);
        t.p1 = project_perspective(t.w1, zoom, width, height);
        t.p2 = project_perspective(t.w2, zoom, width, height);
        t.p3 = project_perspective(t.w3, zoom, width, height);
        t.intensity = compute_intensity(t.w1, t.w2, t.w3);
        wCenter.x = (t.w1.x + t.w2.x + t.w3.x) / 3.0f;
        wCenter.y = (t.w1.y + t.w2.y + t.w3.y) / 3.0f;
        wCenter.z = (t.w1.z + t.w2.z + t.w3.z) / 3.0f;
        model->getFaceColor((int)i, cr, cg, cb);
        if (!model->hasMaterial()) {
            cr = 1.0f;
            cg = 1.0f;
            cb = 1.0f;
        }
        t.baseR = (unsigned char)(cr * 255.0f);
        t.baseG = (unsigned char)(cg * 255.0f);
        t.baseB = (unsigned char)(cb * 255.0f);
        out.push_back(t);
        i++;
    }
    std::sort(out.begin(), out.end(),
              [](const TriData &a, const TriData &b) {
                  float za;
                  float zb;

                  za = (a.w1.z + a.w2.z + a.w3.z) / 3.0f;
                  zb = (b.w1.z + b.w2.z + b.w3.z) / 3.0f;
                  return za > zb;
              });
}

static void raster_triangle(const TriData &t,
                            std::vector<float> &zbuf,
                            sf::Image &image,
                            unsigned int w, unsigned int h)
{
    float minX;
    float maxX;
    float minY;
    float maxY;
    int x;
    int y;

    minX = std::floor(std::fmin(t.p1.x, std::fmin(t.p2.x, t.p3.x)));
    maxX = std::ceil(std::fmax(t.p1.x, std::fmax(t.p2.x, t.p3.x)));
    minY = std::floor(std::fmin(t.p1.y, std::fmin(t.p2.y, t.p3.y)));
    maxY = std::ceil(std::fmax(t.p1.y, std::fmax(t.p2.y, t.p3.y)));
    if (minX < 0.0f)
        minX = 0.0f;
    if (minY < 0.0f)
        minY = 0.0f;
    if (maxX > (float)(w - 1))
        maxX = (float)(w - 1);
    if (maxY > (float)(h - 1))
        maxY = (float)(h - 1);
    y = (int)minY;
    while (y <= (int)maxY) {
        x = (int)minX;
        while (x <= (int)maxX) {
            float px;
            float py;
            float denom;
            float w1;
            float w2;
            float w3;
            float z;
            unsigned int idx;
            float k;
            unsigned char r;
            unsigned char g;
            unsigned char b;
            sf::Color col;

            px = (float)x + 0.5f;
            py = (float)y + 0.5f;
            denom = (t.p2.y - t.p3.y) * (t.p1.x - t.p3.x)
                + (t.p3.x - t.p2.x) * (t.p1.y - t.p3.y);
            if (denom == 0.0f) {
                x++;
                continue;
            }
            w1 = ((t.p2.y - t.p3.y) * (px - t.p3.x)
                + (t.p3.x - t.p2.x) * (py - t.p3.y))
                / denom;
            w2 = ((t.p3.y - t.p1.y) * (px - t.p3.x)
                + (t.p1.x - t.p3.x) * (py - t.p3.y))
                / denom;
            w3 = 1.0f - w1 - w2;
            if (w1 < 0.0f || w2 < 0.0f || w3 < 0.0f) {
                x++;
                continue;
            }
            z = w1 * t.w1.z + w2 * t.w2.z + w3 * t.w3.z;
            idx = (unsigned int)y * w + (unsigned int)x;
            if (z < zbuf[idx]) {
                zbuf[idx] = z;
                k = t.intensity;
                if (k < 0.0f)
                    k = 0.0f;
                if (k > 1.0f)
                    k = 1.0f;
                r = (unsigned char)(t.baseR * k);
                g = (unsigned char)(t.baseG * k);
                b = (unsigned char)(t.baseB * k);
                col = sf::Color(r, g, b);
                image.setPixel(sf::Vector2u((unsigned int)x,
                                            (unsigned int)y),
                               col);
            }
            x++;
        }
        y++;
    }
}

static void draw_wireframe(sf::RenderWindow &window,
                           const std::vector<TriData> &tris)
{
    sf::Vertex line[2];
    sf::Color col;
    std::size_t i;

    col = sf::Color::White;
    i = 0;
    while (i < tris.size()) {
        const TriData &t = tris[i];

        line[0].position = sf::Vector2f(t.p1.x, t.p1.y);
        line[1].position = sf::Vector2f(t.p2.x, t.p2.y);
        line[0].color = col;
        line[1].color = col;
        window.draw(line, 2, sf::PrimitiveType::Lines);
        line[0].position = sf::Vector2f(t.p2.x, t.p2.y);
        line[1].position = sf::Vector2f(t.p3.x, t.p3.y);
        window.draw(line, 2, sf::PrimitiveType::Lines);
        line[0].position = sf::Vector2f(t.p3.x, t.p3.y);
        line[1].position = sf::Vector2f(t.p1.x, t.p1.y);
        window.draw(line, 2, sf::PrimitiveType::Lines);
        i++;
    }
}

void Renderer::render(sf::RenderWindow &window) const
{
    const Model *model;
    sf::Vector2u size;
    unsigned int w;
    unsigned int h;
    std::vector<TriData> tris;
    std::vector<float> zbuf;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite(texture);
    unsigned int count;
    unsigned int i;
    bool ok;

    model = m_model;
    if (!model)
        return;
    size = window.getSize();
    w = size.x;
    h = size.y;
    build_triangles(model, m_angleY, m_angleX, m_zoom,
                    (float)w, (float)h, tris);
    count = w * h;
    zbuf.assign(count, std::numeric_limits<float>::infinity());
    image = sf::Image(sf::Vector2u(w, h), sf::Color::Black);
    i = 0;
    while (i < tris.size()) {
        raster_triangle(tris[i], zbuf, image, w, h);
        i++;
    }
    ok = texture.loadFromImage(image);
    (void)ok;
    sprite = sf::Sprite(texture);
    window.draw(sprite);
    if (m_showEdges) {
        draw_wireframe(window, tris);
    }
}
