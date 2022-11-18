#include "Highlighter.h"
#include "PaletteListForm.h"
#include "RuntimeProvider.h"

#include <QSyntaxHighlighter>
#include <QFont>
#include <QDebug>
#include <colors.h>

Highlighter::Highlighter(QTextDocument *parent, RuntimeProvider *rp) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // keywords and domains all are dark blue
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    static auto keywordPatterns = Keywords::all();

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    for (const QString &pattern : Domains::all()) {
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
    for (const auto &f : rp->getBuiltinFunctions() )
    {
        auto it = std::find_if(supported_functions.begin(), supported_functions.end(), [&f](const auto& sf) -> bool {
            return sf.name == f;
        });
        if(it != supported_functions.end())
        {
            if((*it).needs_highlight)
            {
                QString pattern = "\\b" + QString::fromStdString(f) + "\\b";
                rule.pattern = QRegularExpression(pattern);
                rule.format = functionFormat;
                highlightingRules.append(rule);
            }
        }
    }

    // the defined functions
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    for (const auto &f : rp->getFunctionNames() )
    {
        QString pattern = "\\b" + QString::fromStdString(f) + "\\b";
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    // the defined variables
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkGreen);
    for (const auto &f : rp->getVariables() )
    {
        QString pattern = "\\b" + QString::fromStdString(f) + "\\b";
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    // the declared variables
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkRed);
    for (const auto &f : rp->getDeclaredVariables() )
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

    // the colors
    functionFormat.setFontItalic(true);
    functionFormat.setFontWeight(2);
    functionFormat.setForeground(Qt::cyan);
    for (const auto &f : Colors::colormap)
    {
        QString pattern = "\\b" + QString::fromStdString(f.first) + "\\b";
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    // the palettes
    functionFormat.setFontItalic(true);
    functionFormat.setFontWeight(2);
    functionFormat.setForeground(Qt::darkCyan);
    for (const auto &f : PaletteListForm::retrievePalettes())
    {
        QString pattern = "\\b" + f + "\\b";
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    // what can be set with the set keyword
    functionFormat.setFontItalic(true);
    functionFormat.setFontWeight(2);
    functionFormat.setForeground(Qt::darkMagenta);
    for (const auto &st : SetTargets::all())
    {
        QString pattern = "\\b" + st + "\\b";
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    // scripting languages
    functionFormat.setFontItalic(false);
    functionFormat.setFontWeight(4);
    functionFormat.setFontUnderline(true);
    functionFormat.setForeground(Qt::darkYellow);
    for (const auto &st : ScriptingLangugages::all())
    {
        QString pattern = "\\b" + st + "\\b";
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

    QTextCharFormat multiLineCommentFormat;
    multiLineCommentFormat.setFontItalic(true);
    multiLineCommentFormat.setFontWeight(2);
    multiLineCommentFormat.setForeground(Qt::darkYellow);

    QRegularExpression startExpression("python(\\t| )*script(\\t| )*begin");
    QRegularExpression endExpression("end");

    if (previousBlockState() != 1)
    {
        startIndex = text.indexOf(startExpression);
    }

    while (startIndex >= 0)
    {
        QRegularExpressionMatch endMatch;
        int endIndex = text.indexOf(endExpression, startIndex, &endMatch);
        int scriptLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            scriptLength = text.length() - startIndex;
        }
        else
        {
            scriptLength = 0; // endIndex - startIndex + endMatch.capturedLength();
        }
        if(previousBlockState() != 0 || currentBlockState() == 0)
        {
            setFormat(startIndex, scriptLength, multiLineCommentFormat);
        }
        startIndex = text.indexOf(startExpression, startIndex + scriptLength );
    }
}
