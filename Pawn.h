#pragma once
#include "Piece.h"

/**
 * @brief Pawn piece — supports single/double push, diagonal capture,
 *        en-passant capture, and promotion (handled by Board).
 */
class Pawn : public Piece {
public:
    Pawn(PieceColor color, Position pos);

    std::vector<Position> getValidMoves(const Board& board) const override;
    void draw(sf::RenderWindow& window, float tileSize) const override;
    std::string getSymbol() const override;
};
