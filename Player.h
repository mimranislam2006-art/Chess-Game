#pragma once
#include "Piece.h"
#include <string>

/**
 * @brief Represents a chess player (human or future AI).
 *        Encapsulates color, name, and turn state.
 */
class Player {
public:
    Player(PieceColor color, const std::string& name);
    virtual ~Player() = default;

    PieceColor        getColor() const { return m_color; }
    const std::string& getName() const { return m_name; }

    bool isHuman() const { return m_isHuman; }

protected:
    PieceColor  m_color;
    std::string m_name;
    bool        m_isHuman = true;
};
