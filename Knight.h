#pragma once
#include "Piece.h"

/**
 * @brief Knight piece — L-shaped jumps, ignores blocking pieces.
 */
class Knight : public Piece {
public:
    Knight(PieceColor color, Position pos);

    std::vector<Position> getValidMoves(const Board& board) const override;
    void draw(sf::RenderWindow& window, float tileSize) const override;
    std::string getSymbol() const override;
};
