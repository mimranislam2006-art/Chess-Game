#pragma once
#include "Piece.h"
#include <memory>

/**
 * @brief Encapsulates a single chess move with all metadata needed
 *        for undo, move-history display, and special-move handling.
 */
enum class MoveType {
    NORMAL,
    CAPTURE,
    EN_PASSANT,
    CASTLING_KINGSIDE,
    CASTLING_QUEENSIDE,
    PROMOTION
};

struct Move {
    Position  from;
    Position  to;
    MoveType  type        = MoveType::NORMAL;
    PieceType promoteTo   = PieceType::QUEEN; // used when type == PROMOTION
    bool      wasFirstMove = false;           // for undo: restore hasMoved flag

    // Snapshot of captured piece (owned by move for undo purposes)
    std::unique_ptr<Piece> capturedPiece;

    // Convenience constructor
    Move(Position f, Position t, MoveType mt = MoveType::NORMAL)
        : from(f), to(t), type(mt) {}

    // Moves are non-copyable (unique_ptr member)
    Move(const Move&)            = delete;
    Move& operator=(const Move&) = delete;
    Move(Move&&)                 = default;
    Move& operator=(Move&&)      = default;

    std::string toAlgebraic() const;
};
