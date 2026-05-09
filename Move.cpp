#include "../include/Move.h"

using namespace std;

string Move::toAlgebraic() const {
    // Simple coordinate notation: e2e4, e1g1 (castling), etc.
    auto colChar = [](int c) -> char { return static_cast<char>('a' + c); };
    auto rowChar = [](int r) -> char { return static_cast<char>('8' - r); };

    string s;
    s += colChar(from.col);
    s += rowChar(from.row);
    s += colChar(to.col);
    s += rowChar(to.row);

    if (type == MoveType::PROMOTION) {
        switch (promoteTo) {
            case PieceType::QUEEN:  s += 'q'; break;
            case PieceType::ROOK:   s += 'r'; break;
            case PieceType::BISHOP: s += 'b'; break;
            case PieceType::KNIGHT: s += 'n'; break;
            default: break;
        }
    }
    return s;
}
