#include "../include/ChessGame.h"
#include <iostream>

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
ChessGame::ChessGame()
    : m_playerWhite(make_unique<Player>(PieceColor::WHITE, "White"))
    , m_playerBlack(make_unique<Player>(PieceColor::BLACK, "Black"))
{}

// ─────────────────────────────────────────────────────────────────────────────
bool ChessGame::init(const string& assetDir) {
    // SFML 3: sf::VideoMode constructor takes sf::Vector2u
    sf::VideoMode vm(sf::Vector2u(
        static_cast<unsigned>(GUIManager::WINDOW_W),
        static_cast<unsigned>(GUIManager::WINDOW_H)
    ));
    // SFML 3: window style uses sf::State for fullscreen; regular window is default
    m_window.create(vm, "Chess  C++ / SFML 3");
    m_window.setFramerateLimit(60);

    if (!m_gui.init(assetDir)) {
        cerr << "[ChessGame] GUI init failed.\n";
        return false;
    }

    m_board.loadTextures(assetDir);
    m_statusMsg = "";
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
void ChessGame::run() {
    while (m_window.isOpen()) {
        handleEvents();

        m_gui.render(
            m_window,
            m_board,
            m_legalMoves,
            m_selectedPos,
            m_currentTurn,
            m_board.isInCheck(m_currentTurn),
            m_state == GameState::GAME_OVER,
            m_statusMsg,
            m_board.getMoveHistory(),
            m_board.getCaptured(PieceColor::WHITE),
            m_board.getCaptured(PieceColor::BLACK),
            m_lastFrom,
            m_lastTo
        );
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Event handling — SFML 3 variant-based event system
// ─────────────────────────────────────────────────────────────────────────────

void ChessGame::handleEvents() {
    // SFML 3: pollEvent() returns optional<sf::Event>
    while (const auto event = m_window.pollEvent()) {

        // Window closed
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }

        // Mouse click
        if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
            // SFML 3: scoped enum sf::Mouse::Button::Left
            if (mb->button == sf::Mouse::Button::Left) {
                handleMouseClick(mb->position);
            }
        }

        // Keyboard shortcuts
        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            // SFML 3: scoped enum sf::Keyboard::Key::R / Escape
            if (kp->code == sf::Keyboard::Key::R)      restartGame();
            if (kp->code == sf::Keyboard::Key::Escape) m_window.close();
        }
    }
}

void ChessGame::handleMouseClick(sf::Vector2i mousePos) {
    if (m_gui.isRestartClicked(m_window, mousePos)) { restartGame();    return; }
    if (m_gui.isExitClicked   (m_window, mousePos)) { m_window.close(); return; }

    if (m_state == GameState::GAME_OVER) return;

    Position boardPos = m_gui.screenToBoard(m_window, mousePos);
    if (!boardPos.isValid()) {
        deselectPiece();
        return;
    }

    handleBoardClick(boardPos);
}

void ChessGame::handleBoardClick(Position boardPos) {
    if (!m_pieceSelected) {
        Piece* piece = m_board.getPiece(boardPos);
        if (piece && piece->getColor() == m_currentTurn) {
            selectPiece(boardPos);
        }
    } else {
        Piece* clickedPiece = m_board.getPiece(boardPos);

        if (clickedPiece && clickedPiece->getColor() == m_currentTurn) {
            if (boardPos == m_selectedPos)
                deselectPiece();
            else
                selectPiece(boardPos);
            return;
        }

        attemptMove(boardPos);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Selection
// ─────────────────────────────────────────────────────────────────────────────

void ChessGame::selectPiece(Position pos) {
    m_pieceSelected = true;
    m_selectedPos   = pos;
    m_legalMoves    = m_board.getLegalMoves(pos);
}

void ChessGame::deselectPiece() {
    m_pieceSelected = false;
    m_selectedPos   = {-1, -1};
    m_legalMoves.clear();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Move attempt
// ─────────────────────────────────────────────────────────────────────────────

void ChessGame::attemptMove(Position to) {
    bool isLegal = false;
    for (auto& m : m_legalMoves) {
        if (m == to) { isLegal = true; break; }
    }

    if (!isLegal) {
        deselectPiece();
        return;
    }

    Piece* piece = m_board.getPiece(m_selectedPos);
    bool isPromotion = false;
    if (piece && piece->getType() == PieceType::PAWN) {
        int promRow = (m_currentTurn == PieceColor::WHITE) ? 0 : 7;
        if (to.row == promRow) isPromotion = true;
    }

    PieceType promoteTo = PieceType::QUEEN;
    if (isPromotion) {
        promoteTo = m_gui.showPromotionDialog(m_window, m_currentTurn);
    }

    Position from = m_selectedPos;
    deselectPiece();

    if (m_board.makeMove(from, to, promoteTo)) {
        m_lastFrom = from;
        m_lastTo   = to;
        switchTurn();
        updateStatus();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Turn / Status
// ─────────────────────────────────────────────────────────────────────────────

void ChessGame::switchTurn() {
    m_currentTurn = (m_currentTurn == PieceColor::WHITE)
                    ? PieceColor::BLACK : PieceColor::WHITE;
}

void ChessGame::updateStatus() {
    m_statusMsg = "";

    if (m_board.isCheckmate(m_currentTurn)) {
        PieceColor winner = (m_currentTurn == PieceColor::WHITE)
                            ? PieceColor::BLACK : PieceColor::WHITE;
        m_statusMsg  = (winner == PieceColor::WHITE ? "White" : "Black");
        m_statusMsg += " wins by Checkmate!";
        m_state = GameState::GAME_OVER;
    } else if (m_board.isStalemate(m_currentTurn)) {
        m_statusMsg = "Stalemate - Draw!";
        m_state = GameState::GAME_OVER;
    } else if (m_board.isInCheck(m_currentTurn)) {
        m_statusMsg  = (m_currentTurn == PieceColor::WHITE ? "White" : "Black");
        m_statusMsg += " is in Check!";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Restart
// ─────────────────────────────────────────────────────────────────────────────

void ChessGame::restartGame() {
    m_board.reset();
    m_board.loadTextures("assets");
    m_currentTurn = PieceColor::WHITE;
    m_state       = GameState::PLAYING;
    m_statusMsg   = "";
    m_lastFrom.reset();
    m_lastTo.reset();
    deselectPiece();
}

// ─────────────────────────────────────────────────────────────────────────────
Player* ChessGame::currentPlayer() const {
    return (m_currentTurn == PieceColor::WHITE)
           ? m_playerWhite.get() : m_playerBlack.get();
}
