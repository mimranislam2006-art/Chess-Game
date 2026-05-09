#include "../include/GUIManager.h"
#include "../include/Board.h"
#include <iostream>
#include <sstream>

using namespace std;

// ── Static color constants ────────────────────────────────────────────────────
const sf::Color GUIManager::LIGHT_SQUARE       = sf::Color(240, 217, 181);
const sf::Color GUIManager::DARK_SQUARE        = sf::Color(181, 136,  99);
const sf::Color GUIManager::HIGHLIGHT_SELECTED = sf::Color(106, 168,  79, 200);
const sf::Color GUIManager::HIGHLIGHT_MOVE     = sf::Color( 85, 170, 255, 160);
const sf::Color GUIManager::HIGHLIGHT_CHECK    = sf::Color(220,  50,  50, 200);
const sf::Color GUIManager::HIGHLIGHT_LAST_MOVE= sf::Color(205, 210,  60, 140);

// ─────────────────────────────────────────────────────────────────────────────
GUIManager::GUIManager() {
    m_restartBtn = sf::FloatRect({ SIDEBAR_X,        LOGICAL_H - 110.f }, { 220.f, 40.f });
    m_exitBtn    = sf::FloatRect({ SIDEBAR_X,        LOGICAL_H -  60.f }, { 220.f, 40.f });

    // Fixed logical view — always renders into LOGICAL_W x LOGICAL_H space
    m_view = sf::View(sf::FloatRect({ 0.f, 0.f }, { LOGICAL_W, LOGICAL_H }));
}

bool GUIManager::init(const string& assetDir) {
    if (m_font.openFromFile(assetDir + "/arial.ttf")          ||
        m_font.openFromFile(assetDir + "/DejaVuSans.ttf")     ||
        m_font.openFromFile("C:/Windows/Fonts/arial.ttf")     ||
        m_font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
        m_font.openFromFile("/System/Library/Fonts/Helvetica.ttc"))
    {
        m_fontLoaded = true;
    } else {
        cerr << "[GUIManager] Warning: Could not load font.\n";
    }
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Pixel → logical canvas coordinate
//  This is the key function that makes fullscreen clicks work correctly.
//  SFML's mapPixelToCoords() applies the inverse of the view transform.
// ─────────────────────────────────────────────────────────────────────────────

sf::Vector2f GUIManager::pixelToLogical(sf::RenderWindow& window,
                                         sf::Vector2i pixel) const {
    return window.mapPixelToCoords(pixel, m_view);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Coordinate helpers
// ─────────────────────────────────────────────────────────────────────────────

Position GUIManager::screenToBoard(sf::RenderWindow& window,
                                    sf::Vector2i screenPos) const {
    sf::Vector2f logical = pixelToLogical(window, screenPos);
    int col = static_cast<int>((logical.x - BOARD_OFFSET_X) / TILE_SIZE);
    int row = static_cast<int>((logical.y - BOARD_OFFSET_Y) / TILE_SIZE);
    return {col, row};
}

// Legacy overload — no scaling, for when window is exact logical size
Position GUIManager::screenToBoard(sf::Vector2i screenPos) const {
    int col = static_cast<int>((screenPos.x - BOARD_OFFSET_X) / TILE_SIZE);
    int row = static_cast<int>((screenPos.y - BOARD_OFFSET_Y) / TILE_SIZE);
    return {col, row};
}

sf::Vector2f GUIManager::boardToScreen(Position pos) const {
    return {
        BOARD_OFFSET_X + pos.col * TILE_SIZE,
        BOARD_OFFSET_Y + pos.row * TILE_SIZE
    };
}

// ─────────────────────────────────────────────────────────────────────────────
//  Button hit-testing — map pixel to logical first
// ─────────────────────────────────────────────────────────────────────────────

bool GUIManager::isRestartClicked(sf::RenderWindow& window,
                                   sf::Vector2i mp) const {
    sf::Vector2f lp = pixelToLogical(window, mp);
    return m_restartBtn.contains(lp);
}

bool GUIManager::isExitClicked(sf::RenderWindow& window,
                                sf::Vector2i mp) const {
    sf::Vector2f lp = pixelToLogical(window, mp);
    return m_exitBtn.contains(lp);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Main render entry point
// ─────────────────────────────────────────────────────────────────────────────

void GUIManager::render(sf::RenderWindow& window,
                        const Board& board,
                        const vector<Position>& legalMoves,
                        Position selectedPos,
                        PieceColor currentTurn,
                        bool inCheck,
                        bool gameOver,
                        const string& statusMsg,
                        const vector<string>& moveHistory,
                        const vector<PieceType>& capturedWhite,
                        const vector<PieceType>& capturedBlack,
                        optional<Position> lastFrom,
                        optional<Position> lastTo)
{
    // Apply the logical view — everything drawn after this is in logical coords
    window.setView(m_view);
    window.clear(sf::Color(30, 30, 30));

    // Find king for check highlight
    Position kingPos = {-1, -1};
    if (inCheck) {
        for (int r = 0; r < 8; ++r)
            for (int c = 0; c < 8; ++c) {
                Piece* p = board.getPiece({c, r});
                if (p && p->getType() == PieceType::KING && p->getColor() == currentTurn)
                    kingPos = {c, r};
            }
    }

    drawBoard(window, legalMoves, selectedPos, inCheck, kingPos, lastFrom, lastTo);
    drawCoordinates(window);
    board.drawPieces(window, TILE_SIZE);
    drawSidebar(window, currentTurn, gameOver, statusMsg,
                moveHistory, capturedWhite, capturedBlack);

    window.display();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Board drawing
// ─────────────────────────────────────────────────────────────────────────────

void GUIManager::drawBoard(sf::RenderWindow& window,
                            const vector<Position>& legalMoves,
                            Position selectedPos,
                            bool inCheck,
                            Position kingPos,
                            optional<Position> lastFrom,
                            optional<Position> lastTo) const
{
    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos = {c, r};

            tile.setFillColor(((r + c) % 2 == 0) ? LIGHT_SQUARE : DARK_SQUARE);
            tile.setPosition({ BOARD_OFFSET_X + c * TILE_SIZE,
                               BOARD_OFFSET_Y + r * TILE_SIZE });
            window.draw(tile);

            if ((lastFrom.has_value() && pos == lastFrom.value()) ||
                (lastTo.has_value()   && pos == lastTo.value()))
            {
                tile.setFillColor(HIGHLIGHT_LAST_MOVE);
                window.draw(tile);
            }

            if (inCheck && pos == kingPos) {
                tile.setFillColor(HIGHLIGHT_CHECK);
                window.draw(tile);
            }

            if (pos == selectedPos) {
                tile.setFillColor(HIGHLIGHT_SELECTED);
                window.draw(tile);
            }
        }
    }

    // Legal move dots
    for (auto& dest : legalMoves) {
        sf::CircleShape dot(TILE_SIZE * 0.18f);
        dot.setFillColor(HIGHLIGHT_MOVE);
        dot.setOrigin({ dot.getRadius(), dot.getRadius() });
        dot.setPosition({
            BOARD_OFFSET_X + dest.col * TILE_SIZE + TILE_SIZE / 2.f,
            BOARD_OFFSET_Y + dest.row * TILE_SIZE + TILE_SIZE / 2.f
        });
        window.draw(dot);
    }

    // Board border
    sf::RectangleShape border(sf::Vector2f(8.f * TILE_SIZE + 4.f, 8.f * TILE_SIZE + 4.f));
    border.setPosition({ BOARD_OFFSET_X - 2.f, BOARD_OFFSET_Y - 2.f });
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(60, 60, 60));
    border.setOutlineThickness(3.f);
    window.draw(border);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Coordinate labels
// ─────────────────────────────────────────────────────────────────────────────

void GUIManager::drawCoordinates(sf::RenderWindow& window) const {
    if (!m_fontLoaded) return;

    sf::Text label(m_font, "", 13);
    label.setFillColor(sf::Color(200, 200, 200));

    for (int c = 0; c < 8; ++c) {
        label.setString(string(1, static_cast<char>('a' + c)));
        label.setPosition({
            BOARD_OFFSET_X + c * TILE_SIZE + TILE_SIZE / 2.f - 5.f,
            BOARD_OFFSET_Y + 8.f * TILE_SIZE + 4.f
        });
        window.draw(label);
    }

    for (int r = 0; r < 8; ++r) {
        label.setString(to_string(8 - r));
        label.setPosition({
            BOARD_OFFSET_X - 18.f,
            BOARD_OFFSET_Y + r * TILE_SIZE + TILE_SIZE / 2.f - 8.f
        });
        window.draw(label);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Sidebar
// ─────────────────────────────────────────────────────────────────────────────

string GUIManager::pieceSymbol(PieceType t, PieceColor c) const {
    if (c == PieceColor::WHITE) {
        switch (t) {
            case PieceType::PAWN:   return "P";
            case PieceType::ROOK:   return "R";
            case PieceType::KNIGHT: return "N";
            case PieceType::BISHOP: return "B";
            case PieceType::QUEEN:  return "Q";
            case PieceType::KING:   return "K";
            default: return "?";
        }
    } else {
        switch (t) {
            case PieceType::PAWN:   return "p";
            case PieceType::ROOK:   return "r";
            case PieceType::KNIGHT: return "n";
            case PieceType::BISHOP: return "b";
            case PieceType::QUEEN:  return "q";
            case PieceType::KING:   return "k";
            default: return "?";
        }
    }
}

void GUIManager::drawSidebar(sf::RenderWindow& window,
                              PieceColor currentTurn,
                              bool gameOver,
                              const string& statusMsg,
                              const vector<string>& moveHistory,
                              const vector<PieceType>& capturedWhite,
                              const vector<PieceType>& capturedBlack) const
{
    float x = SIDEBAR_X;
    float y = BOARD_OFFSET_Y;

    if (m_fontLoaded) {
        sf::Text title(m_font, "CHESS", 28);
        title.setFillColor(sf::Color(220, 220, 220));
        title.setStyle(sf::Text::Bold);
        title.setPosition({ x, y });
        window.draw(title);
        y += 40.f;

        // Turn indicator
        sf::RectangleShape turnBox(sf::Vector2f(220.f, 36.f));
        turnBox.setPosition({ x, y });
        turnBox.setFillColor(currentTurn == PieceColor::WHITE
                             ? sf::Color(240, 240, 240) : sf::Color(40, 40, 40));
        turnBox.setOutlineColor(sf::Color(100, 100, 100));
        turnBox.setOutlineThickness(1.5f);
        window.draw(turnBox);

        sf::Text turnText(m_font,
            (currentTurn == PieceColor::WHITE ? "White" : "Black") + string("'s Turn"), 16);
        turnText.setFillColor(currentTurn == PieceColor::WHITE
                              ? sf::Color::Black : sf::Color::White);
        turnText.setPosition({ x + 8.f, y + 8.f });
        window.draw(turnText);
        y += 50.f;

        // Status
        if (!statusMsg.empty()) {
            sf::Text status(m_font, statusMsg, 15);
            status.setFillColor(sf::Color(255, 100, 100));
            status.setStyle(sf::Text::Bold);
            status.setPosition({ x, y });
            window.draw(status);
            y += 30.f;
        }

        // Captured pieces
        y += 6.f;
        sf::Text capLabel(m_font, "Captured by White:", 13);
        capLabel.setFillColor(sf::Color(180, 180, 180));
        capLabel.setPosition({ x, y });
        window.draw(capLabel);
        y += 20.f;

        string capW;
        for (auto& t : capturedWhite) capW += pieceSymbol(t, PieceColor::BLACK) + " ";
        if (capW.empty()) capW = "-";
        sf::Text capWText(m_font, capW, 14);
        capWText.setFillColor(sf::Color(220, 220, 220));
        capWText.setPosition({ x, y });
        window.draw(capWText);
        y += 26.f;

        sf::Text capLabel2(m_font, "Captured by Black:", 13);
        capLabel2.setFillColor(sf::Color(180, 180, 180));
        capLabel2.setPosition({ x, y });
        window.draw(capLabel2);
        y += 20.f;

        string capB;
        for (auto& t : capturedBlack) capB += pieceSymbol(t, PieceColor::WHITE) + " ";
        if (capB.empty()) capB = "-";
        sf::Text capBText(m_font, capB, 14);
        capBText.setFillColor(sf::Color(220, 220, 220));
        capBText.setPosition({ x, y });
        window.draw(capBText);
        y += 36.f;

        // Move history
        sf::Text histLabel(m_font, "Move History:", 13);
        histLabel.setFillColor(sf::Color(180, 180, 180));
        histLabel.setPosition({ x, y });
        window.draw(histLabel);
        y += 20.f;

        int start = max(0, static_cast<int>(moveHistory.size()) - 16);
        for (int i = start; i < static_cast<int>(moveHistory.size()); i += 2) {
            int moveNum = i / 2 + 1;
            string line = to_string(moveNum) + ". " + moveHistory[i];
            if (i + 1 < static_cast<int>(moveHistory.size()))
                line += "  " + moveHistory[i + 1];
            sf::Text histEntry(m_font, line, 12);
            histEntry.setFillColor(sf::Color(200, 200, 200));
            histEntry.setPosition({ x, y });
            window.draw(histEntry);
            y += 16.f;
        }
    }

    drawButton(window, m_restartBtn, "New Game", sf::Color(60, 120, 60));
    drawButton(window, m_exitBtn,    "Exit",     sf::Color(120, 50, 50));
}

void GUIManager::drawButton(sf::RenderWindow& window,
                             const sf::FloatRect& rect,
                             const string& label,
                             sf::Color bg) const
{
    sf::RectangleShape btn(rect.size);
    btn.setPosition(rect.position);
    btn.setFillColor(bg);
    btn.setOutlineColor(sf::Color(200, 200, 200));
    btn.setOutlineThickness(1.5f);
    window.draw(btn);

    if (m_fontLoaded) {
        sf::Text text(m_font, label, 16);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        sf::FloatRect tb = text.getLocalBounds();
        text.setPosition({
            rect.position.x + (rect.size.x - tb.size.x) / 2.f - tb.position.x,
            rect.position.y + (rect.size.y - tb.size.y) / 2.f - tb.position.y
        });
        window.draw(text);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Promotion dialog
// ─────────────────────────────────────────────────────────────────────────────

PieceType GUIManager::showPromotionDialog(sf::RenderWindow& window, PieceColor color) {
    const vector<pair<PieceType, string>> choices = {
        {PieceType::QUEEN,  "Queen"},
        {PieceType::ROOK,   "Rook"},
        {PieceType::BISHOP, "Bishop"},
        {PieceType::KNIGHT, "Knight"}
    };

    const float btnW = 120.f, btnH = 50.f, gap = 10.f;
    const float totalW = choices.size() * btnW + (choices.size() - 1) * gap;
    const float startX = (LOGICAL_W - totalW) / 2.f;
    const float startY = LOGICAL_H / 2.f - btnH / 2.f;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return PieceType::QUEEN;
            }
            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    // Map pixel to logical coords
                    sf::Vector2f lp = pixelToLogical(window, mb->position);
                    for (int i = 0; i < static_cast<int>(choices.size()); ++i) {
                        float bx = startX + i * (btnW + gap);
                        if (lp.x >= bx && lp.x <= bx + btnW &&
                            lp.y >= startY && lp.y <= startY + btnH)
                        {
                            return choices[i].first;
                        }
                    }
                }
            }
        }

        window.setView(m_view);
        window.clear(sf::Color(20, 20, 20));

        sf::RectangleShape overlay(sf::Vector2f(LOGICAL_W, LOGICAL_H));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);

        if (m_fontLoaded) {
            sf::Text prompt(m_font, "Choose promotion piece:", 22);
            prompt.setFillColor(sf::Color::White);
            sf::FloatRect pb = prompt.getLocalBounds();
            prompt.setPosition({
                (LOGICAL_W - pb.size.x) / 2.f - pb.position.x,
                startY - 50.f
            });
            window.draw(prompt);
        }

        for (int i = 0; i < static_cast<int>(choices.size()); ++i) {
            float bx = startX + i * (btnW + gap);
            sf::RectangleShape btn(sf::Vector2f(btnW, btnH));
            btn.setPosition({ bx, startY });
            btn.setFillColor(sf::Color(80, 80, 160));
            btn.setOutlineColor(sf::Color::White);
            btn.setOutlineThickness(2.f);
            window.draw(btn);

            if (m_fontLoaded) {
                sf::Text lbl(m_font, choices[i].second, 15);
                lbl.setFillColor(sf::Color::White);
                sf::FloatRect lb = lbl.getLocalBounds();
                lbl.setPosition({
                    bx + (btnW - lb.size.x) / 2.f - lb.position.x,
                    startY + (btnH - lb.size.y) / 2.f - lb.position.y
                });
                window.draw(lbl);
            }
        }

        window.display();
    }
    return PieceType::QUEEN;
}
