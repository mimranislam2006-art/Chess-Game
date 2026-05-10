#include "../include/Bishop.h"
#include "../include/Board.h"

using namespace std;

Bishop::Bishop(PieceColor color, Position pos)
    : Piece(color, PieceType::BISHOP, pos)
{}

string Bishop::getSymbol() const {
    return (m_color == PieceColor::WHITE) ? "B" : "b";
}

vector<Position> Bishop::getValidMoves(const Board& board) const {
    vector<Position> moves;
    const int dirs[4][2] = { {-1,-1},{-1,1},{1,-1},{1,1} };

    for (auto& d : dirs) {
        Position cur = { m_position.col + d[0], m_position.row + d[1] };
        while (cur.isValid()) {
            Piece* target = board.getPiece(cur);
            if (!target) {
                moves.push_back(cur);
            } else {
                if (target->getColor() != m_color) moves.push_back(cur);
                break;
            }
            cur.col += d[0];
            cur.row += d[1];
        }
    }
    return moves;
}

void Bishop::draw(sf::RenderWindow& window, float tileSize) const {
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
