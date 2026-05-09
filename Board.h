#pragma once
#include "Piece.h"
#include "Move.h"
#include <array>
#include <vector>
#include <memory>
#include <optional>

/**
 * @brief Owns the 8×8 grid of pieces and enforces all chess rules.
 *
 *  Responsibilities:
 *   - Piece placement / removal
 *   - Legal-move generation (filters pseudo-legal moves that leave king in check)
 *   - Check / checkmate / stalemate detection
 *   - Special moves: castling, en-passant, pawn promotion
 *   - Move history (for undo and algebraic notation display)
 */
class Board {
public:
    // ── Grid type ────────────────────────────────────────────────────────────
    using Grid = std::array<std::array<std::unique_ptr<Piece>, 8>, 8>;

    Board();
    ~Board() = default;

    // Non-copyable (owns unique_ptrs)
    Board(const Board&)            = delete;
    Board& operator=(const Board&) = delete;

    // ── Setup ────────────────────────────────────────────────────────────────
    void reset();                          ///< Place pieces in starting position
    bool loadTextures(const std::string& assetDir);

    // ── Accessors ────────────────────────────────────────────────────────────
    Piece*       getPiece(Position pos) const;
    Piece*       getPiece(int col, int row) const;
    bool         isEmpty(Position pos) const;
    bool         isInBounds(Position pos) const;

    // ── Move generation ──────────────────────────────────────────────────────
    /**
     * @brief Returns fully legal moves for the piece at @p pos
     *        (pseudo-legal moves filtered for self-check).
     */
    std::vector<Position> getLegalMoves(Position pos) const;

    /**
     * @brief Returns all legal moves for @p color (used for checkmate/stalemate).
     */
    std::vector<Move> getAllLegalMoves(PieceColor color) const;

    // ── Move execution ───────────────────────────────────────────────────────
    /**
     * @brief Executes a move. Returns false if the move is illegal.
     *        Handles castling, en-passant, and promotion automatically.
     */
    bool makeMove(Position from, Position to,
                  PieceType promoteTo = PieceType::QUEEN);

    // ── State queries ────────────────────────────────────────────────────────
    bool isInCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color) const;
    bool isStalemate(PieceColor color) const;

    /**
     * @brief Returns the en-passant target square (if any) after the last move.
     */
    std::optional<Position> getEnPassantTarget() const { return m_enPassantTarget; }

    // ── Captured pieces ──────────────────────────────────────────────────────
    const std::vector<PieceType>& getCaptured(PieceColor color) const;

    // ── Move history ─────────────────────────────────────────────────────────
    const std::vector<std::string>& getMoveHistory() const { return m_moveHistory; }

    // ── Rendering ────────────────────────────────────────────────────────────
    void drawPieces(sf::RenderWindow& window, float tileSize) const;

    // ── Internal helpers (public so Piece subclasses can query the board) ────
    bool isSquareAttackedBy(Position pos, PieceColor attacker) const;
    bool isComputingAttacks() const { return m_computingAttacks; }

    // Used internally to break recursion: when true, King skips castling moves
    mutable bool m_computingAttacks = false;

private:
    Grid m_grid;

    std::optional<Position> m_enPassantTarget; ///< Square a pawn can capture en-passant

    std::vector<PieceType> m_capturedByWhite;  ///< Pieces captured by White
    std::vector<PieceType> m_capturedByBlack;  ///< Pieces captured by Black

    std::vector<std::string> m_moveHistory;    ///< Algebraic notation log

    // ── Internal helpers ─────────────────────────────────────────────────────
    void placePiece(std::unique_ptr<Piece> piece, Position pos);
    void removePiece(Position pos);

    /**
     * @brief Simulates a move on a temporary copy and checks whether
     *        @p color's king is in check afterwards.
     */
    bool moveLeavesKingInCheck(Position from, Position to, PieceColor color) const;

    Position findKing(PieceColor color) const;

    void handleCastling(Position from, Position to);
    void handleEnPassant(Position from, Position to);
    void handlePromotion(Position pos, PieceType promoteTo);

    std::string m_assetDir; ///< Stored so promoted pieces can load their texture

    std::string buildAlgebraic(Position from, Position to, MoveType mt,
                               PieceType promoteTo) const;
};
