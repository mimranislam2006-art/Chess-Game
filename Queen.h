#pragma once
#include "Piece.h"

/**
 * @brief Queen piece — combines Rook and Bishop movement.
 */
class Queen : public Piece {
public:
    Queen(PieceColor color, Position pos);

    std::vector<Position> getValidMoves(const Board& board) const override;
    void draw(sf::RenderWindow& window, float tileSize) const override;
    std::string getSymbol() const override;
};
