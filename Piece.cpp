#include "../include/Piece.h"
#include <iostream>

using namespace std;

Piece::Piece(PieceColor color, PieceType type, Position pos)
    : m_color(color), m_type(type), m_position(pos), m_hasMoved(false)
{}

bool Piece::loadTexture(const string& path) {
    if (!m_texture.loadFromFile(path)) {
        cerr << "[Piece] Failed to load texture: " << path << "\n";
        return false;
    }
    m_textureLoaded = true;
    return true;
}

bool Piece::isEnemy(const Piece* other) const {
    if (!other) return false;
    return other->m_color != m_color;
}

bool Piece::isAlly(const Piece* other) const {
    if (!other) return false;
    return other->m_color == m_color;
}
