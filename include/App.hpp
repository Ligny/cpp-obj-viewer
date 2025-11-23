#ifndef APP_HPP
#define APP_HPP

#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include "Renderer.hpp"
#include "Model.hpp"

class App {
public:
    App(const char *objPath, const char *mtlPath, bool &ok);

    void run();

private:
    void handleEvents();
    void update();
    void render();
    void updateHudText();
    void setupHud();
    void updateButtonsStyle();

    sf::RenderWindow m_window;
    Renderer m_renderer;
    Model m_model;
    bool m_running;
    float m_angleY;
    float m_angleX;
    float m_zoom;
    bool m_autoRotate;
    bool m_showEdges;

    sf::Font m_font;
    std::optional<sf::Text> m_text;
    bool m_hasFont;
    std::string m_objName;
    std::string m_mtlName;

    sf::RectangleShape m_btnLines;
    sf::RectangleShape m_btnAuto;
    std::optional<sf::Text> m_btnLinesLabel;
    std::optional<sf::Text> m_btnAutoLabel;
};

#endif
