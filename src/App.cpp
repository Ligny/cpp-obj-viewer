#include "App.hpp"
#include <iostream>

App::App(const char *objPath, const char *mtlPath, bool &ok)
    : m_window(sf::VideoMode(sf::Vector2u(800u, 600u)),
               "Low-Poly Tree Viewer"),
      m_renderer(),
      m_model(),
      m_running(true),
      m_angleY(0.5f),
      m_angleX(0.3f),
      m_zoom(1.2f),
      m_autoRotate(false),
      m_showEdges(false),
      m_font(),
      m_text(),
      m_hasFont(false),
      m_objName(""),
      m_mtlName("none"),
      m_btnLines(),
      m_btnAuto(),
      m_btnLinesLabel(),
      m_btnAutoLabel()
{
    bool loadedObj;
    bool loadedMtl;

    loadedObj = false;
    loadedMtl = false;
    if (!objPath) {
        std::cerr << "Error: no OBJ file provided." << std::endl;
        m_running = false;
        ok = false;
        return;
    }
    if (mtlPath) {
        loadedMtl = m_model.loadFromMtl(mtlPath);
        if (loadedMtl)
            m_mtlName = mtlPath;
        else
            std::cerr << "Warning: failed to load MTL, "
                      << "rendering in white." << std::endl;
    } else {
        std::cerr << "Info: no MTL argument, "
                  << "rendering in white." << std::endl;
    }
    loadedObj = m_model.loadFromObj(objPath);
    if (loadedObj)
        m_objName = objPath;
    if (!loadedObj) {
        std::cerr << "Error: failed to load OBJ file." << std::endl;
        m_running = false;
        ok = false;
        return;
    }
    ok = true;
    m_window.setFramerateLimit(60);
    m_renderer.setModel(&m_model);
    m_renderer.setAngles(m_angleY, m_angleX);
    m_renderer.setZoom(m_zoom);
    if (m_font.openFromFile("assets/DejaVuSans.ttf")) {
        m_hasFont = true;
        m_text.emplace(m_font, "", 14);
        m_text->setFillColor(sf::Color::White);
        m_text->setPosition(sf::Vector2f(8.0f, 8.0f));
        setupHud();
        updateHudText();
    }
}

void App::setupHud()
{
    if (!m_hasFont)
        return;
    m_btnLines.setSize(sf::Vector2f(110.0f, 26.0f));
    m_btnLines.setPosition(sf::Vector2f(8.0f, 520.0f));
    m_btnLines.setOutlineThickness(1.0f);
    m_btnLines.setOutlineColor(sf::Color(200, 200, 200));

    m_btnAuto.setSize(sf::Vector2f(110.0f, 26.0f));
    m_btnAuto.setPosition(sf::Vector2f(8.0f, 552.0f));
    m_btnAuto.setOutlineThickness(1.0f);
    m_btnAuto.setOutlineColor(sf::Color(200, 200, 200));

    m_btnLinesLabel.emplace(m_font, "Edges", 14);
    m_btnLinesLabel->setFillColor(sf::Color::White);
    m_btnLinesLabel->setPosition(sf::Vector2f(16.0f, 524.0f));

    m_btnAutoLabel.emplace(m_font, "Auto", 14);
    m_btnAutoLabel->setFillColor(sf::Color::White);
    m_btnAutoLabel->setPosition(sf::Vector2f(16.0f, 556.0f));
    updateButtonsStyle();
}

void App::updateButtonsStyle()
{
    if (!m_hasFont)
        return;
    if (m_showEdges)
        m_btnLines.setFillColor(sf::Color(60, 120, 200));
    else
        m_btnLines.setFillColor(sf::Color(40, 40, 40));
    if (m_autoRotate)
        m_btnAuto.setFillColor(sf::Color(60, 160, 80));
    else
        m_btnAuto.setFillColor(sf::Color(40, 40, 40));
}

void App::handleEvents()
{
    std::optional<sf::Event> opt;
    const sf::Event *ev;

    while ((opt = m_window.pollEvent())) {
        ev = &(*opt);
        if (ev->is<sf::Event::Closed>()) {
            m_running = false;
        } else if (const auto *key =
                       ev->getIf<sf::Event::KeyPressed>()) {
            sf::Keyboard::Key code;
            float rotStep;
            float zoomStep;

            code = key->code;
            rotStep = 0.1f;
            zoomStep = 0.1f;
            if (code == sf::Keyboard::Key::Escape)
                m_running = false;
            else if (code == sf::Keyboard::Key::Left)
                m_angleY -= rotStep;
            else if (code == sf::Keyboard::Key::Right)
                m_angleY += rotStep;
            else if (code == sf::Keyboard::Key::Up)
                m_angleX -= rotStep;
            else if (code == sf::Keyboard::Key::Down)
                m_angleX += rotStep;
            else if (code == sf::Keyboard::Key::W)
                m_zoom += zoomStep;
            else if (code == sf::Keyboard::Key::S)
                m_zoom -= zoomStep;
        } else if (const auto *mouse =
                       ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouse->button == sf::Mouse::Button::Left) {
                sf::Vector2i pos;
                sf::Vector2f fpos;

                pos = mouse->position;
                fpos = sf::Vector2f((float)pos.x, (float)pos.y);
                if (m_btnLines.getGlobalBounds().contains(fpos)) {
                    m_showEdges = !m_showEdges;
                    m_renderer.setShowEdges(m_showEdges);
                    updateButtonsStyle();
                } else if (m_btnAuto.getGlobalBounds()
                               .contains(fpos)) {
                    m_autoRotate = !m_autoRotate;
                    updateButtonsStyle();
                }
            }
        }
    }
}

void App::updateHudText()
{
    if (!m_hasFont || !m_text)
        return;
    std::string text;
    std::string obj;
    std::string mtl;

    obj = m_objName.empty()
        ? std::string("unknown.obj")
        : m_objName;
    mtl = m_mtlName;
    text =
        "OBJ: " + obj + "\n" +
        "MTL: " + mtl;
    m_text->setString(text);
}

void App::update()
{
    if (m_autoRotate)
        m_angleY += 0.01f;
    if (m_zoom < 0.3f)
        m_zoom = 0.3f;
    if (m_zoom > 3.0f)
        m_zoom = 3.0f;
    m_renderer.setAngles(m_angleY, m_angleX);
    m_renderer.setZoom(m_zoom);
}

void App::render()
{
    m_window.clear(sf::Color::Black);
    m_renderer.render(m_window);
    if (m_hasFont && m_text) {
        m_window.draw(*m_text);
        m_window.draw(m_btnLines);
        m_window.draw(m_btnAuto);
        if (m_btnLinesLabel)
            m_window.draw(*m_btnLinesLabel);
        if (m_btnAutoLabel)
            m_window.draw(*m_btnAutoLabel);
    }
    m_window.display();
}

void App::run()
{
    while (m_running) {
        handleEvents();
        update();
        render();
    }
}
