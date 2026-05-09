#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <optional>
#include "Piece.h"

class Board;

/**
 * @brief Manages all SFML 3 rendering and UI elements.
 *
 * Uses a fixed logical coordinate space (LOGICAL_W x LOGICAL_H) rendered
 * via an sf::View that scales to any window/fullscreen size.
 * All mouse coordinates are mapped back through the view before hit-testing.
 */
class GUIManager {
public:
    // ── Fixed logical canvas size (never changes) ─────────────────────────────
    static constexpr float TILE_SIZE      = 80.f;
    static constexpr float BOARD_OFFSET_X = 40.f;
    static constexpr float BOARD_OFFSET_Y = 40.f;
    static constexpr float SIDEBAR_X      = BOARD_OFFSET_X + 8.f * TILE_SIZE + 20.f;
    static constexpr float LOGICAL_W      = SIDEBAR_X + 260.f;
    static constexpr float LOGICAL_H      = BOARD_OFFSET_Y * 2.f + 8.f * TILE_SIZE;

    // Keep WINDOW_W/H as aliases so existing code compiles
    static constexpr float WINDOW_W = LOGICAL_W;
    static constexpr float WINDOW_H = LOGICAL_H;

    // ── Colors ───────────────────────────────────────────────────────────────
    static const sf::Color LIGHT_SQUARE;
    static const sf::Color DARK_SQUARE;
    static const sf::Color HIGHLIGHT_SELECTED;
    static const sf::Color HIGHLIGHT_MOVE;
    static const sf::Color HIGHLIGHT_CHECK;
    static const sf::Color HIGHLIGHT_LAST_MOVE;

    GUIManager();

    bool init(const std::string& assetDir);

    // ── Main draw entry point ────────────────────────────────────────────────
    void render(sf::RenderWindow& window,
                const Board& board,
                const std::vector<Position>& legalMoves,
                Position selectedPos,
                PieceColor currentTurn,
                bool inCheck,
                bool gameOver,
                const std::string& statusMsg,
                const std::vector<std::string>& moveHistory,
                const std::vector<PieceType>& capturedWhite,
                const std::vector<PieceType>& capturedBlack,
                std::optional<Position> lastFrom,
                std::optional<Position> lastTo);

    // ── Promotion dialog ─────────────────────────────────────────────────────
    PieceType showPromotionDialog(sf::RenderWindow& window, PieceColor color);

    // ── Coordinate helpers ────────────────────────────────────────────────────
    // Convert a raw screen pixel position to logical canvas coordinates,
    // then to a board square. Works correctly in any window size / fullscreen.
    Position     screenToBoard(sf::RenderWindow& window, sf::Vector2i screenPos) const;
    // Legacy overload (no scaling) — kept for windowed mode compatibility
    Position     screenToBoard(sf::Vector2i screenPos) const;
    sf::Vector2f boardToScreen(Position pos) const;

    // ── Button hit-testing (logical coords) ──────────────────────────────────
    bool isRestartClicked(sf::RenderWindow& window, sf::Vector2i mousePos) const;
    bool isExitClicked   (sf::RenderWindow& window, sf::Vector2i mousePos) const;

private:
    sf::Font m_font;
    bool     m_fontLoaded = false;

    sf::FloatRect m_restartBtn;
    sf::FloatRect m_exitBtn;

    // ── View that maps logical canvas to actual window ────────────────────────
    sf::View m_view;

    // ── Convert raw pixel → logical canvas coordinate ────────────────────────
    sf::Vector2f pixelToLogical(sf::RenderWindow& window, sf::Vector2i pixel) const;

    // ── Draw helpers ─────────────────────────────────────────────────────────
    void drawBoard      (sf::RenderWindow& window,
                         const std::vector<Position>& legalMoves,
                         Position selectedPos,
                         bool inCheck,
                         Position kingPos,
                         std::optional<Position> lastFrom,
                         std::optional<Position> lastTo) const;

    void drawCoordinates(sf::RenderWindow& window) const;

    void drawSidebar    (sf::RenderWindow& window,
                         PieceColor currentTurn,
                         bool gameOver,
                         const std::string& statusMsg,
                         const std::vector<std::string>& moveHistory,
                         const std::vector<PieceType>& capturedWhite,
                         const std::vector<PieceType>& capturedBlack) const;

    void drawButton     (sf::RenderWindow& window,
                         const sf::FloatRect& rect,
                         const std::string& label,
                         sf::Color bg) const;

    std::string pieceSymbol(PieceType t, PieceColor c) const;
};
