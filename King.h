#pragma once
#include "Piece.h"

/**
 * @brief King piece — one square in any direction, plus castling.
 */
class King : public Piece {
public:
    King(PieceColor color, Position pos);

    std::vector<Position> getValidMoves(const Board& board) const override;
    void draw(sf::RenderWindow& window, float tileSize) const override;
    std::string getSymbol() const override;
};
