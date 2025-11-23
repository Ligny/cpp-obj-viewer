#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SFML/Graphics.hpp>
#include "Model.hpp"

class Renderer {
public:
    Renderer();

    void setModel(const Model *model);
    void setAngles(float angleY, float angleX);
    void setZoom(float zoom);
    void setShowEdges(bool showEdges);

    void render(sf::RenderWindow &window) const;

private:
    const Model *m_model;
    float m_angleY;
    float m_angleX;
    float m_zoom;
    bool m_showEdges;
};

#endif
