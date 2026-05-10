#pragma once
#include "Piece.h"

/**
 * @brief Rook piece — slides along ranks and files.
 *        Also participates in castling (tracked via hasMoved()).
 */
class Rook : public Piece {
public:
    Rook(PieceColor color, Position pos);

    std::vector<Position> getValidMoves(const Board& board) const override;
    void draw(sf::RenderWindow& window, float tileSize) const override;
    std::string getSymbol() const override;
};
