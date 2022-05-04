#include "Highlighter.h"
#include "RuntimeProvider.h"

#include <QSyntaxHighlighter>
#include <QFont>
#include <QDebug>

Highlighter::Highlighter(QTextDocument *parent, RuntimeProvider *rp) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // keywords
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    static auto keywordPatterns = Keywords::all();

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // types
    typeFormat.setForeground(Qt::blue);
    typeFormat.setFontWeight(QFont::Bold);
    static auto typePatterns = Types::all();

    for (const QString &pattern : typePatterns) {
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = typeFormat;
        highlightingRules.append(rule);
    }

    // the builtin functions
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkBlue);
    for (const auto &f : rp->get_builtin_functions() )
    {
        QString pattern = "\\b" + QString::fromStdString(f) + "\\b";
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    // the defined functions
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    for (const auto &f : rp->get_functions() )
    {
        QString pattern = "\\b" + QString::fromStdString(f) + "\\b";
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    // the defined variables
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkGreen);
    for (const auto &f : rp->get_variables() )
    {
        QString pattern = "\\b" + QString::fromStdString(f) + "\\b";
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    // the built in angle chooser (degrees/radians)
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkGray);
    for (const auto &f : {"degrees", "radians"})
    {
        QString pattern = "\\b" + QString::fromStdString(f) + "\\b";
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }


    // comments, start with a #
    singleLineCommentFormat.setForeground(Qt::red);
    singleLineCommentFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("#[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
}

void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);
}
