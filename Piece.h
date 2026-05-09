#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

// Forward declaration
class Board;

/**
 * @brief Represents a board position (column, row) — 0-indexed.
 */
struct Position {
    int col; // 0-7 (a-h)
    int row; // 0-7 (rank 8 to rank 1)

    bool operator==(const Position& o) const { return col == o.col && row == o.row; }
    bool operator!=(const Position& o) const { return !(*this == o); }
    bool isValid() const { return col >= 0 && col < 8 && row >= 0 && row < 8; }
};

/**
 * @brief Piece color enumeration.
 */
enum class PieceColor { WHITE, BLACK, NONE };

/**
 * @brief Piece type enumeration.
 */
enum class PieceType { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, NONE };

/**
 * @brief Abstract base class for all chess pieces.
 *        Demonstrates Abstraction, Inheritance, and Polymorphism.
 */
class Piece {
protected:
    PieceColor  m_color;
    PieceType   m_type;
    Position    m_position;
    bool        m_hasMoved;
    sf::Texture m_texture;
    // Sprite is constructed with texture in SFML 3 — stored as optional
    mutable bool m_textureLoaded = false;

public:
    Piece(PieceColor color, PieceType type, Position pos);
    virtual ~Piece() = default;

    // ── Pure virtual interface ──────────────────────────────────────────────
    virtual std::vector<Position> getValidMoves(const Board& board) const = 0;
    virtual void draw(sf::RenderWindow& window, float tileSize) const = 0;
    virtual std::string getSymbol() const = 0;

    // ── Texture loading ─────────────────────────────────────────────────────
    bool loadTexture(const std::string& path);
    bool hasTexture() const { return m_textureLoaded; }
    const sf::Texture& getTexture() const { return m_texture; }

    // ── Accessors ───────────────────────────────────────────────────────────
    PieceColor  getColor()    const { return m_color; }
    PieceType   getType()     const { return m_type; }
    Position    getPosition() const { return m_position; }
    bool        hasMoved()    const { return m_hasMoved; }

    void setPosition(const Position& pos) { m_position = pos; }
    void setHasMoved(bool moved)          { m_hasMoved = moved; }

    // ── Utility ─────────────────────────────────────────────────────────────
    bool isEnemy(const Piece* other) const;
    bool isAlly (const Piece* other) const;
};
