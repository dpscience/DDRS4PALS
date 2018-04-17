/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2018 Danny Petschke
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see http://www.gnu.org/licenses/.
**
*****************************************************************************
**
**  @author: Danny Petschke
**  @contact: danny.petschke@uni-wuerzburg.de
**
*****************************************************************************/

#ifndef DRS4SCRIPTDLG_H
#define DRS4SCRIPTDLG_H

#include "dversion.h"

#include <QMainWindow>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QLabel>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include "GUI/drs4scopedlg.h"

#include "Script/drs4scriptingengineaccessmanager.h"
#include "Script/drs4scriptmanager.h"

#include "drs4boardmanager.h"

#include "DLib.h"

class DRS4ScopeDlg;
class DRS4ScriptExecuter;
class DRS4ScriptTextEdit;

class Highlighter;

class DRS4ScriptString;

namespace Ui {
class DRS4ScriptDlg;
}

class DRS4ScriptDlg : public QMainWindow
{
    Q_OBJECT
public:
    explicit DRS4ScriptDlg(DRS4ScopeDlg *dlg, QWidget *parent = 0);
    virtual ~DRS4ScriptDlg();

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void showEvent(QShowEvent *event);

public slots:
    void loadScriptFile();
    void saveScriptFile();
    void saveAsScriptFile();

    void loadExample_DataAcqu_1();
    void loadExample_PulseStream_1();
    void loadExample_WarmingUp_2();
    void loadExample_Simulation_1();

    void saveLogCompleteFile();
    void saveLogSucceedFile();
    void saveLogFailedFile();
    void saveLogPrintOutFile();

    void doScriptAction();
    void finishScript();

    void updateScript();
    void updateLog();

signals:
    void scriptStarted();
    void scriptFinished();

private:
    void runScript();
    void abortScript();

    void setupCompleter();

    void updateCurrentFileLabel();

private:
    Ui::DRS4ScriptDlg *ui;

    DRS4ScopeDlg *m_dlgParent;
    bool m_scriptIsRunning;

    QThread *m_scriptWorkerThread;
    DRS4ScriptExecuter *m_scriptExecuter;

    QCompleter *m_completer;
    Highlighter *m_highlighter;

    DRS4ScriptString *m_logFile, *m_logFileSucceed, *m_logFileFailed, *m_logFilePrintOut;

    QTimer *m_logUpdateTimer;

    QLabel *m_currentScriptFileLabel;
    QString m_currentScriptPath;

    mutable QMutex m_mutex;
};


class DRS4ScriptTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    DRS4ScriptTextEdit(QWidget *parent = 0);
    virtual ~DRS4ScriptTextEdit();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    void resizeEvent(QResizeEvent *e);


private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter *m_completer;
    QWidget *lineNumberArea;
};


class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(DRS4ScriptTextEdit *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    DRS4ScriptTextEdit *codeEditor;
};

#endif // DRS4SCRIPTDLG_H
