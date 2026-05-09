#include "../include/Player.h"

using namespace std;

Player::Player(PieceColor color, const string& name)
    : m_color(color), m_name(name), m_isHuman(true)
{}
