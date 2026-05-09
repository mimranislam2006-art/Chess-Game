#include "../include/Board.h"
#include "../include/Pawn.h"
#include "../include/Rook.h"
#include "../include/Knight.h"
#include "../include/Bishop.h"
#include "../include/Queen.h"
#include "../include/King.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor / Reset
// ─────────────────────────────────────────────────────────────────────────────

Board::Board() {
    reset();
}

void Board::reset() {
    // Clear grid
    for (auto& row : m_grid)
        for (auto& cell : row)
            cell.reset();

    m_enPassantTarget.reset();
    m_capturedByWhite.clear();
    m_capturedByBlack.clear();
    m_moveHistory.clear();

    // ── Place pieces (row 0 = rank 8 = Black back rank) ──────────────────────
    auto placeRow = [&](int row, PieceColor color) {
        placePiece(make_unique<Rook>  (color, Position{0, row}), {0, row});
        placePiece(make_unique<Knight>(color, Position{1, row}), {1, row});
        placePiece(make_unique<Bishop>(color, Position{2, row}), {2, row});
        placePiece(make_unique<Queen> (color, Position{3, row}), {3, row});
        placePiece(make_unique<King>  (color, Position{4, row}), {4, row});
        placePiece(make_unique<Bishop>(color, Position{5, row}), {5, row});
        placePiece(make_unique<Knight>(color, Position{6, row}), {6, row});
        placePiece(make_unique<Rook>  (color, Position{7, row}), {7, row});
    };

    placeRow(0, PieceColor::BLACK);
    placeRow(7, PieceColor::WHITE);

    for (int c = 0; c < 8; ++c) {
        placePiece(make_unique<Pawn>(PieceColor::BLACK, Position{c, 1}), {c, 1});
        placePiece(make_unique<Pawn>(PieceColor::WHITE, Position{c, 6}), {c, 6});
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Texture loading
// ─────────────────────────────────────────────────────────────────────────────

bool Board::loadTextures(const string& assetDir) {
    m_assetDir = assetDir; // Store for promoted pieces

    // Map: (color, type) -> filename
    struct TexInfo { PieceColor color; PieceType type; string file; };
    const vector<TexInfo> infos = {
        {PieceColor::WHITE, PieceType::PAWN,   "white_pawn.png"},
        {PieceColor::WHITE, PieceType::ROOK,   "white_rook.png"},
        {PieceColor::WHITE, PieceType::KNIGHT, "white_knight.png"},
        {PieceColor::WHITE, PieceType::BISHOP, "white_bishop.png"},
        {PieceColor::WHITE, PieceType::QUEEN,  "white_queen.png"},
        {PieceColor::WHITE, PieceType::KING,   "white_king.png"},
        {PieceColor::BLACK, PieceType::PAWN,   "black_pawn.png"},
        {PieceColor::BLACK, PieceType::ROOK,   "black_rook.png"},
        {PieceColor::BLACK, PieceType::KNIGHT, "black_knight.png"},
        {PieceColor::BLACK, PieceType::BISHOP, "black_bishop.png"},
        {PieceColor::BLACK, PieceType::QUEEN,  "black_queen.png"},
        {PieceColor::BLACK, PieceType::KING,   "black_king.png"},
    };

    bool allOk = true;
    for (auto& row : m_grid) {
        for (auto& cell : row) {
            if (!cell) continue;
            for (auto& info : infos) {
                if (cell->getColor() == info.color && cell->getType() == info.type) {
                    if (!cell->loadTexture(assetDir + "/" + info.file)) {
                        allOk = false;
                    }
                    break;
                }
            }
        }
    }
    return allOk;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Grid helpers
// ─────────────────────────────────────────────────────────────────────────────

Piece* Board::getPiece(Position pos) const {
    if (!isInBounds(pos)) return nullptr;
    return m_grid[pos.row][pos.col].get();
}

Piece* Board::getPiece(int col, int row) const {
    return getPiece({col, row});
}

bool Board::isEmpty(Position pos) const {
    return getPiece(pos) == nullptr;
}

bool Board::isInBounds(Position pos) const {
    return pos.col >= 0 && pos.col < 8 && pos.row >= 0 && pos.row < 8;
}

void Board::placePiece(unique_ptr<Piece> piece, Position pos) {
    m_grid[pos.row][pos.col] = move(piece);
}

void Board::removePiece(Position pos) {
    m_grid[pos.row][pos.col].reset();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Attack detection
// ─────────────────────────────────────────────────────────────────────────────

bool Board::isSquareAttackedBy(Position pos, PieceColor attacker) const {
    // Set flag so King::getValidMoves skips castling (prevents infinite recursion)
    m_computingAttacks = true;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = m_grid[r][c].get();
            if (!p || p->getColor() != attacker) continue;
            auto moves = p->getValidMoves(*this);
            for (auto& m : moves) {
                if (m == pos) {
                    m_computingAttacks = false;
                    return true;
                }
            }
        }
    }
    m_computingAttacks = false;
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
//  King finder
// ─────────────────────────────────────────────────────────────────────────────

Position Board::findKing(PieceColor color) const {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Piece* p = m_grid[r][c].get();
            if (p && p->getType() == PieceType::KING && p->getColor() == color)
                return {c, r};
        }
    return {-1, -1}; // Should never happen in a valid game
}

// ─────────────────────────────────────────────────────────────────────────────
//  Check detection
// ─────────────────────────────────────────────────────────────────────────────

bool Board::isInCheck(PieceColor color) const {
    Position kingPos = findKing(color);
    if (!kingPos.isValid()) return false;
    PieceColor enemy = (color == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    return isSquareAttackedBy(kingPos, enemy);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Move simulation (for legality check)
// ─────────────────────────────────────────────────────────────────────────────

bool Board::moveLeavesKingInCheck(Position from, Position to, PieceColor color) const {
    // We temporarily mutate the board, check for check, then restore.
    // Using const_cast because this is a logical-const operation (simulation).
    Board* self = const_cast<Board*>(this);

    Piece* movingPiece = getPiece(from);
    if (!movingPiece) return false;

    // ── Save the destination cell (may be null) ───────────────────────────────
    unique_ptr<Piece> savedDest = move(self->m_grid[to.row][to.col]);

    // ── Apply the move ────────────────────────────────────────────────────────
    self->m_grid[to.row][to.col] = move(self->m_grid[from.row][from.col]);
    // from cell is now null

    Position savedPos = movingPiece->getPosition();
    movingPiece->setPosition(to);

    // ── Handle en-passant in simulation ──────────────────────────────────────
    Position epCapturePos = {-1, -1};
    unique_ptr<Piece> savedEpPawn;
    if (movingPiece->getType() == PieceType::PAWN &&
        m_enPassantTarget.has_value() && to == m_enPassantTarget.value())
    {
        int dir = (color == PieceColor::WHITE) ? 1 : -1;
        epCapturePos = { to.col, to.row + dir };
        savedEpPawn = move(self->m_grid[epCapturePos.row][epCapturePos.col]);
    }

    // ── Check detection ───────────────────────────────────────────────────────
    bool inCheck = isInCheck(color);

    // ── Restore state ─────────────────────────────────────────────────────────
    movingPiece->setPosition(savedPos);
    self->m_grid[from.row][from.col] = move(self->m_grid[to.row][to.col]);
    self->m_grid[to.row][to.col]     = move(savedDest);

    if (epCapturePos.isValid()) {
        self->m_grid[epCapturePos.row][epCapturePos.col] = move(savedEpPawn);
    }

    return inCheck;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Legal move generation
// ─────────────────────────────────────────────────────────────────────────────

vector<Position> Board::getLegalMoves(Position pos) const {
    Piece* piece = getPiece(pos);
    if (!piece) return {};

    auto pseudoMoves = piece->getValidMoves(*this);
    vector<Position> legal;
    legal.reserve(pseudoMoves.size());

    for (auto& dest : pseudoMoves) {
        if (!moveLeavesKingInCheck(pos, dest, piece->getColor())) {
            legal.push_back(dest);
        }
    }
    return legal;
}

vector<Move> Board::getAllLegalMoves(PieceColor color) const {
    vector<Move> allMoves;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = m_grid[r][c].get();
            if (!p || p->getColor() != color) continue;
            Position from = {c, r};
            auto legal = getLegalMoves(from);
            for (auto& to : legal) {
                allMoves.emplace_back(from, to);
            }
        }
    }
    return allMoves;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Checkmate / Stalemate
// ─────────────────────────────────────────────────────────────────────────────

bool Board::isCheckmate(PieceColor color) const {
    return isInCheck(color) && getAllLegalMoves(color).empty();
}

bool Board::isStalemate(PieceColor color) const {
    return !isInCheck(color) && getAllLegalMoves(color).empty();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Captured pieces accessors
// ─────────────────────────────────────────────────────────────────────────────

const vector<PieceType>& Board::getCaptured(PieceColor color) const {
    return (color == PieceColor::WHITE) ? m_capturedByWhite : m_capturedByBlack;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Special move handlers
// ─────────────────────────────────────────────────────────────────────────────

void Board::handleCastling(Position from, Position to) {
    // King has already moved; now move the rook
    int row = from.row;
    if (to.col == 6) {
        // Kingside: rook from h-file (col 7) to f-file (col 5)
        m_grid[row][5] = move(m_grid[row][7]);
        m_grid[row][5]->setPosition({5, row});
        m_grid[row][5]->setHasMoved(true);
    } else if (to.col == 2) {
        // Queenside: rook from a-file (col 0) to d-file (col 3)
        m_grid[row][3] = move(m_grid[row][0]);
        m_grid[row][3]->setPosition({3, row});
        m_grid[row][3]->setHasMoved(true);
    }
}

void Board::handleEnPassant(Position from, Position to) {
    // Remove the captured pawn (same rank as 'from', same file as 'to')
    int capturedRow = from.row;
    m_grid[capturedRow][to.col].reset();
}

void Board::handlePromotion(Position pos, PieceType promoteTo) {
    PieceColor color = m_grid[pos.row][pos.col]->getColor();
    unique_ptr<Piece> newPiece;
    switch (promoteTo) {
        case PieceType::QUEEN:  newPiece = make_unique<Queen> (color, pos); break;
        case PieceType::ROOK:   newPiece = make_unique<Rook>  (color, pos); break;
        case PieceType::BISHOP: newPiece = make_unique<Bishop>(color, pos); break;
        case PieceType::KNIGHT: newPiece = make_unique<Knight>(color, pos); break;
        default:                newPiece = make_unique<Queen> (color, pos); break;
    }
    newPiece->setHasMoved(true);

    // Load texture for the promoted piece
    if (!m_assetDir.empty()) {
        string colorStr = (color == PieceColor::WHITE) ? "white" : "black";
        string typeStr;
        switch (promoteTo) {
            case PieceType::QUEEN:  typeStr = "queen";  break;
            case PieceType::ROOK:   typeStr = "rook";   break;
            case PieceType::BISHOP: typeStr = "bishop"; break;
            case PieceType::KNIGHT: typeStr = "knight"; break;
            default:                typeStr = "queen";  break;
        }
        newPiece->loadTexture(m_assetDir + "/" + colorStr + "_" + typeStr + ".png");
    }

    m_grid[pos.row][pos.col] = move(newPiece);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Algebraic notation builder
// ─────────────────────────────────────────────────────────────────────────────

string Board::buildAlgebraic(Position from, Position to,
                              MoveType mt, PieceType promoteTo) const {
    auto colCh = [](int c) -> char { return static_cast<char>('a' + c); };
    auto rowCh = [](int r) -> char { return static_cast<char>('8' - r); };

    if (mt == MoveType::CASTLING_KINGSIDE)  return "O-O";
    if (mt == MoveType::CASTLING_QUEENSIDE) return "O-O-O";

    string s;
    Piece* p = getPiece(from);
    if (p && p->getType() != PieceType::PAWN) {
        s += p->getSymbol()[0]; // piece letter
        if (s[0] >= 'a') s[0] = static_cast<char>(s[0] - 32); // ensure uppercase
    }
    if (mt == MoveType::CAPTURE || mt == MoveType::EN_PASSANT) {
        if (p && p->getType() == PieceType::PAWN) s += colCh(from.col);
        s += 'x';
    }
    s += colCh(to.col);
    s += rowCh(to.row);
    if (mt == MoveType::PROMOTION) {
        s += '=';
        switch (promoteTo) {
            case PieceType::QUEEN:  s += 'Q'; break;
            case PieceType::ROOK:   s += 'R'; break;
            case PieceType::BISHOP: s += 'B'; break;
            case PieceType::KNIGHT: s += 'N'; break;
            default: s += 'Q'; break;
        }
    }
    return s;
}

// ─────────────────────────────────────────────────────────────────────────────
//  makeMove — the main move executor
// ─────────────────────────────────────────────────────────────────────────────

bool Board::makeMove(Position from, Position to, PieceType promoteTo) {
    Piece* piece = getPiece(from);
    if (!piece) return false;

    // Verify legality
    auto legal = getLegalMoves(from);
    bool isLegal = find(legal.begin(), legal.end(), to) != legal.end();
    if (!isLegal) return false;

    PieceColor color = piece->getColor();

    // ── Determine move type ───────────────────────────────────────────────────
    MoveType mt = MoveType::NORMAL;
    Piece* target = getPiece(to);

    if (target) mt = MoveType::CAPTURE;

    // En-passant
    bool isEnPassant = false;
    if (piece->getType() == PieceType::PAWN &&
        m_enPassantTarget.has_value() && to == m_enPassantTarget.value())
    {
        mt = MoveType::EN_PASSANT;
        isEnPassant = true;
    }

    // Castling
    bool isCastling = false;
    if (piece->getType() == PieceType::KING && !piece->hasMoved()) {
        if (to.col == 6 && from.col == 4) { mt = MoveType::CASTLING_KINGSIDE;  isCastling = true; }
        if (to.col == 2 && from.col == 4) { mt = MoveType::CASTLING_QUEENSIDE; isCastling = true; }
    }

    // Promotion
    bool isPromotion = false;
    if (piece->getType() == PieceType::PAWN) {
        int promRow = (color == PieceColor::WHITE) ? 0 : 7;
        if (to.row == promRow) { mt = MoveType::PROMOTION; isPromotion = true; }
    }

    // ── Record captured piece ─────────────────────────────────────────────────
    if (mt == MoveType::CAPTURE) {
        if (color == PieceColor::WHITE) m_capturedByWhite.push_back(target->getType());
        else                            m_capturedByBlack.push_back(target->getType());
    }
    if (mt == MoveType::EN_PASSANT) {
        int capturedRow = from.row;
        Piece* epPawn = getPiece({to.col, capturedRow});
        if (epPawn) {
            if (color == PieceColor::WHITE) m_capturedByWhite.push_back(epPawn->getType());
            else                            m_capturedByBlack.push_back(epPawn->getType());
        }
    }

    // ── Build algebraic notation BEFORE moving ────────────────────────────────
    string notation = buildAlgebraic(from, to, mt, promoteTo);

    // ── Reset en-passant target ───────────────────────────────────────────────
    m_enPassantTarget.reset();

    // ── Execute the move ──────────────────────────────────────────────────────
    m_grid[to.row][to.col] = move(m_grid[from.row][from.col]);
    m_grid[from.row][from.col].reset();
    piece->setPosition(to);
    piece->setHasMoved(true);

    // Save pawn status BEFORE promotion replaces the piece pointer
    bool wasPawn = (piece->getType() == PieceType::PAWN);

    if (isCastling)  handleCastling(from, to);
    if (isEnPassant) handleEnPassant(from, to);
    if (isPromotion) handlePromotion(to, promoteTo);

    // ── Set new en-passant target (double pawn push) ──────────────────────────
    if (wasPawn) {
        int startRow = (color == PieceColor::WHITE) ? 6 : 1;
        if (from.row == startRow && abs(to.row - from.row) == 2) {
            int epRow = (from.row + to.row) / 2;
            m_enPassantTarget = Position{from.col, epRow};
        }
    }

    // ── Append check/checkmate annotation ────────────────────────────────────
    PieceColor enemy = (color == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    if (isCheckmate(enemy))    notation += '#';
    else if (isInCheck(enemy)) notation += '+';

    m_moveHistory.push_back(notation);
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Rendering
// ─────────────────────────────────────────────────────────────────────────────

void Board::drawPieces(sf::RenderWindow& window, float tileSize) const {
    for (auto& row : m_grid)
        for (auto& cell : row)
            if (cell) cell->draw(window, tileSize);
}
