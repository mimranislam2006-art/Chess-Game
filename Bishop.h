#pragma once
#include "Piece.h"

/**
 * @brief Bishop piece — slides diagonally.
 */
class Bishop : public Piece {
public:
    Bishop(PieceColor color, Position pos);

    std::vector<Position> getValidMoves(const Board& board) const override;
    void draw(sf::RenderWindow& window, float tileSize) const override;
    std::string getSymbol() const override;
};
