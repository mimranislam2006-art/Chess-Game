#include "../include/Pawn.h"
#include "../include/Board.h"

using namespace std;

Pawn::Pawn(PieceColor color, Position pos)
    : Piece(color, PieceType::PAWN, pos)
{}

string Pawn::getSymbol() const {
    return (m_color == PieceColor::WHITE) ? "P" : "p";
}

vector<Position> Pawn::getValidMoves(const Board& board) const {
    vector<Position> moves;
    const int dir      = (m_color == PieceColor::WHITE) ? -1 : 1;
    const int startRow = (m_color == PieceColor::WHITE) ?  6 : 1;

    Position oneStep = { m_position.col, m_position.row + dir };
    Position twoStep = { m_position.col, m_position.row + 2 * dir };

    // ── Forward push ──────────────────────────────────────────────────────────
    if (oneStep.isValid() && board.isEmpty(oneStep)) {
        moves.push_back(oneStep);
        if (m_position.row == startRow && twoStep.isValid() && board.isEmpty(twoStep)) {
            moves.push_back(twoStep);
        }
    }

    // ── Diagonal captures ─────────────────────────────────────────────────────
    for (int dc : {-1, 1}) {
        Position cap = { m_position.col + dc, m_position.row + dir };
        if (!cap.isValid()) continue;

        Piece* target = board.getPiece(cap);
        if (target && target->getColor() != m_color) {
            moves.push_back(cap);
        }

        // En-passant
        auto ep = board.getEnPassantTarget();
        if (ep.has_value() && ep.value() == cap) {
            moves.push_back(cap);
        }
    }

    return moves;
}

void Pawn::draw(sf::RenderWindow& window, float tileSize) const {
    if (!m_textureLoaded) return;
    // SFML 3: sf::Sprite requires texture in constructor
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
