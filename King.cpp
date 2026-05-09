#include "../include/King.h"
#include "../include/Board.h"

using namespace std;

King::King(PieceColor color, Position pos)
    : Piece(color, PieceType::KING, pos)
{}

string King::getSymbol() const {
    return (m_color == PieceColor::WHITE) ? "K" : "k";
}

vector<Position> King::getValidMoves(const Board& board) const {
    vector<Position> moves;

    // ── Normal one-square moves ───────────────────────────────────────────────
    const int dirs[8][2] = {
        {-1,-1},{0,-1},{1,-1},
        {-1, 0},       {1, 0},
        {-1, 1},{0, 1},{1, 1}
    };
    for (auto& d : dirs) {
        Position dest = { m_position.col + d[0], m_position.row + d[1] };
        if (!dest.isValid()) continue;
        Piece* target = board.getPiece(dest);
        if (!target || target->getColor() != m_color) {
            moves.push_back(dest);
        }
    }

    // ── Castling ──────────────────────────────────────────────────────────────
    // Skip castling entirely when called from isSquareAttackedBy to prevent
    // infinite recursion: isSquareAttackedBy->getValidMoves->isSquareAttackedBy
    if (!m_hasMoved && !board.isComputingAttacks()) {
        const int row = m_position.row;
        PieceColor enemy = (m_color == PieceColor::WHITE)
                           ? PieceColor::BLACK : PieceColor::WHITE;

        // Kingside (O-O): rook on h-file, f and g empty, f and g not attacked
        Piece* kRook = board.getPiece({7, row});
        if (kRook && kRook->getType() == PieceType::ROOK &&
            kRook->getColor() == m_color && !kRook->hasMoved() &&
            board.isEmpty({5, row}) && board.isEmpty({6, row}) &&
            !board.isSquareAttackedBy({5, row}, enemy) &&
            !board.isSquareAttackedBy({6, row}, enemy))
        {
            moves.push_back({6, row});
        }

        // Queenside (O-O-O): rook on a-file, b/c/d empty, c and d not attacked
        Piece* qRook = board.getPiece({0, row});
        if (qRook && qRook->getType() == PieceType::ROOK &&
            qRook->getColor() == m_color && !qRook->hasMoved() &&
            board.isEmpty({1, row}) && board.isEmpty({2, row}) &&
            board.isEmpty({3, row}) &&
            !board.isSquareAttackedBy({2, row}, enemy) &&
            !board.isSquareAttackedBy({3, row}, enemy))
        {
            moves.push_back({2, row});
        }
    }

    return moves;
}

void King::draw(sf::RenderWindow& window, float tileSize) const {
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
