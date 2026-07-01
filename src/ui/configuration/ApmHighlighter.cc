/*=====================================================================

APM_PLANNER Open Source Ground Control Station

(c) 2013, Bill Bonney <billbonney@communistech.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief APM Highligther for ArduPilot Console.
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 *
 */

#include "ApmHighlighter.h"

#include <QRegularExpression>

APMHighlighter::APMHighlighter(QObject *parent) :
    QSyntaxHighlighter(parent)
{
}

void APMHighlighter::highlightBlock(const QString &text)
 {
     QTextCharFormat myClassFormat;
     myClassFormat.setFontWeight(QFont::Bold);
     myClassFormat.setForeground(Qt::darkMagenta);
     QString pattern = "^\\Ardu[A-Za-z]+\\b";

     QRegularExpression expression(pattern);
     QRegularExpressionMatch match;
     int index = text.indexOf(expression, 0, &match);
     while (index >= 0) {
         int length = match.capturedLength();
         setFormat(index, length, myClassFormat);
         index = text.indexOf(expression, index + length, &match);
     }
 }
