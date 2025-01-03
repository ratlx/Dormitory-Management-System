//
// Created by 小火锅 on 24-11-27.
//

#include "../inc/dark.h"

#include <QtGui/qpalette.h>
#include <QApplication>

bool isDarkMode() {
    QPalette palette = QApplication::palette();
    QColor bgColor = palette.color(QPalette::Window); // 获取窗口背景色
    return bgColor.lightness() < 128; // 如果亮度低于128，表示是暗色模式
}
