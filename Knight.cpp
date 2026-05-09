#include "../include/Knight.h"
#include "../include/Board.h"

using namespace std;

Knight::Knight(PieceColor color, Position pos)
    : Piece(color, PieceType::KNIGHT, pos)
{}

string Knight::getSymbol() const {
    return (m_color == PieceColor::WHITE) ? "N" : "n";
}

vector<Position> Knight::getValidMoves(const Board& board) const {
    vector<Position> moves;
    const int offsets[8][2] = {
        {-2,-1},{-2,1},{-1,-2},{-1,2},
        { 1,-2},{ 1,2},{ 2,-1},{ 2,1}
    };

    for (auto& o : offsets) {
        Position dest = { m_position.col + o[0], m_position.row + o[1] };
        if (!dest.isValid()) continue;
        Piece* target = board.getPiece(dest);
        if (!target || target->getColor() != m_color) {
            moves.push_back(dest);
        }
    }
    return moves;
}

void Knight::draw(sf::RenderWindow& window, float tileSize) const {
    if (!m_textureLoaded) return;
    sf::Sprite spr(m_texture);
    sf::Vector2u sz = m_texture.getSize();
    const float TARGET = 72.f;
    if (sz.x > 0 && sz.y > 0)
        spr.setScale({ TARGET / sz.x, TARGET / sz.y });
    float offsetX = (tileSize - spr.getGlobalBounds().size.x) / 2.f;
    float offsetY = (tileSize - spr.getGlobalBounds().size.y) / 2.f;
    spr.setPosition({ 40.f + m_position.col * tileSize + offsetX,
                      40.f + m_position.row * tileSize + offsetY });
    window.draw(spr);
}
