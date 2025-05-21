#include "PlayerCell.h"

void PlayerCell::resetContent() {
    content = CellContent::Empty;
}

char PlayerCell::getDisplayChar() const {
    switch (content) {
        case CellContent::Wall: return '#';
        case CellContent::Mine: return '@';
        case CellContent::EnemyTank: return 'E';
        case CellContent::MyTank: return 'M';
        case CellContent::Shell: return '*';
        default: return ' ';
    }
}
