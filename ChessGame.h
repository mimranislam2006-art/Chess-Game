#pragma once
#include "Board.h"
#include "Player.h"
#include "GUIManager.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>

/**
 * @brief Top-level game controller.
 *
 *  Owns the window, board, players, and GUI manager.
 *  Implements the main game loop and state machine.
 *
 *  States:
 *   PLAYING   – normal gameplay
 *   PROMOTION – waiting for promotion piece selection
 *   GAME_OVER – checkmate or stalemate reached
 */
class ChessGame {
public:
    ChessGame();
    ~ChessGame() = default;

    /**
     * @brief Initialises SFML window, loads assets, resets board.
     * @return false if initialisation failed.
     */
    bool init(const std::string& assetDir = "assets");

    /**
     * @brief Runs the main game loop until the window is closed.
     */
    void run();

private:
    // ── State machine ────────────────────────────────────────────────────────
    enum class GameState { PLAYING, PROMOTION, GAME_OVER };
    GameState m_state = GameState::PLAYING;

    // ── Core objects ─────────────────────────────────────────────────────────
    sf::RenderWindow          m_window;
    Board                     m_board;
    std::unique_ptr<Player>   m_playerWhite;
    std::unique_ptr<Player>   m_playerBlack;
    GUIManager                m_gui;

    // ── Turn tracking ────────────────────────────────────────────────────────
    PieceColor m_currentTurn = PieceColor::WHITE;

    // ── Selection state ──────────────────────────────────────────────────────
    bool                   m_pieceSelected = false;
    Position               m_selectedPos   = {-1, -1};
    std::vector<Position>  m_legalMoves;

    // ── Last move (for highlighting) ─────────────────────────────────────────
    std::optional<Position> m_lastFrom;
    std::optional<Position> m_lastTo;

    // ── Promotion pending ────────────────────────────────────────────────────
    Position m_promotionPos = {-1, -1};

    // ── Status message ───────────────────────────────────────────────────────
    std::string m_statusMsg;

    // ── Event handlers ───────────────────────────────────────────────────────
    void handleEvents();
    void handleMouseClick(sf::Vector2i mousePos);  // sf::Vector2i from mb->position
    void handleBoardClick(Position boardPos);

    // ── Game logic ───────────────────────────────────────────────────────────
    void selectPiece(Position pos);
    void deselectPiece();
    void attemptMove(Position to);
    void switchTurn();
    void updateStatus();
    void restartGame();

    // ── Helpers ──────────────────────────────────────────────────────────────
    Player* currentPlayer() const;
};
