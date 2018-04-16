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

#include "drs4scriptdlg.h"
#include "ui_drs4scriptdlg.h"

DRS4ScriptDlg::DRS4ScriptDlg(DRS4ScopeDlg *dlg, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DRS4ScriptDlg),
    m_dlgParent(dlg),
    m_scriptIsRunning(false),
    m_completer(nullptr)
{
    ui->setupUi(this);

    ui->actionLoad_Script->setIcon(QIcon(":/images/images/007-folder-3.png"));
    ui->actionSave_As->setIcon(QIcon(":/images/images/008-folder-2.png"));
    ui->actionSave_Script->setIcon(QIcon(":/images/images/008-folder-2.png"));

    ui->actionSave_Log->setIcon(QIcon(":/images/images/log.png"));

    m_currentScriptPath = "";
    m_currentScriptFileLabel = new QLabel;
    m_currentScriptFileLabel->setFont(QFont("Arial", 10));
    ui->statusBar->addPermanentWidget(m_currentScriptFileLabel);

    m_logFile = new DRS4ScriptString;
    m_logFileSucceed = new DRS4ScriptString;
    m_logFileFailed = new DRS4ScriptString;
    m_logFilePrintOut = new DRS4ScriptString;

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled()  )
        setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE");
    else
        setWindowTitle(PROGRAM_NAME);

    m_scriptWorkerThread = new QThread();
    m_scriptExecuter = new DRS4ScriptExecuter(m_logFile, m_logFileSucceed, m_logFileFailed, m_logFilePrintOut);

    setupCompleter();

    m_logUpdateTimer = new QTimer;
    m_logUpdateTimer->setSingleShot(false);
    m_logUpdateTimer->setInterval(1000);

    m_scriptExecuter->moveToThread(m_scriptWorkerThread);

    connect(m_scriptWorkerThread, SIGNAL(started()), m_scriptExecuter, SLOT(execute()));
    connect(m_scriptWorkerThread, SIGNAL(finished()), this, SLOT(finishScript()));
    connect(m_scriptExecuter, SIGNAL(finished()), m_scriptWorkerThread, SLOT(terminate()));

    connect(ui->actionLoad_Script, SIGNAL(triggered()), this, SLOT(loadScriptFile()));
    connect(ui->actionSave_Script, SIGNAL(triggered()), this, SLOT(saveScriptFile()));
    connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(saveAsScriptFile()));

    connect(ui->pushButton_runScript, SIGNAL(clicked()), this, SLOT(doScriptAction()));

    connect(ui->textEdit_scriptInput, SIGNAL(textChanged()), this, SLOT(updateScript()));
    connect(ui->textEdit_scriptInput, SIGNAL(cursorPositionChanged()), this, SLOT(updateScript()));
    connect(ui->textEdit_scriptInput, SIGNAL(selectionChanged()), this, SLOT(updateScript()));

    connect(ui->actionSave_Log, SIGNAL(triggered()), this, SLOT(saveLogCompleteFile()));
    connect(ui->actionSave_Log_Succeed, SIGNAL(triggered()), this, SLOT(saveLogSucceedFile()));
    connect(ui->actionSave_Log_Failed, SIGNAL(triggered()), this, SLOT(saveLogFailedFile()));
    connect(ui->actionSave_Log_PrintOut, SIGNAL(triggered()), this, SLOT(saveLogPrintOutFile()));


    /* Add examples here: */
    connect(ui->actionLoad_Settings_File_and_Save_after_Counts, SIGNAL(triggered()), this, SLOT(loadExample_DataAcqu_1()));
    connect(ui->actionOptimize_FWHM_by_CFD_Levels, SIGNAL(triggered()), this, SLOT(loadExample_PulseStream_1()));
    connect(ui->actionWarming_Up_DRS4_Board_and_Start_Measurement, SIGNAL(triggered()), this, SLOT(loadExample_WarmingUp_2()));

    connect(this, SIGNAL(scriptStarted()), ui->textEdit_log, SLOT(clear()), Qt::DirectConnection);
    connect(this, SIGNAL(scriptStarted()), ui->textEdit_logSucceed, SLOT(clear()), Qt::DirectConnection);
    connect(this, SIGNAL(scriptStarted()), ui->textEdit_logFailed, SLOT(clear()), Qt::DirectConnection);
    connect(this, SIGNAL(scriptStarted()), ui->textEdit_logPrintOut, SLOT(clear()), Qt::DirectConnection);

    connect(m_logUpdateTimer, SIGNAL(timeout()), this, SLOT(updateLog()));

    ui->actionSave_Log->setEnabled(false);
    ui->actionSave_Log_Failed->setEnabled(false);
    ui->actionSave_Log_PrintOut->setEnabled(false);
    ui->actionSave_Log_Succeed->setEnabled(false);

    m_highlighter = new Highlighter(ui->textEdit_scriptInput->document());

    m_currentScriptPath = NO_SCRIPT_FILE_PLACEHOLDER;
    updateCurrentFileLabel();

    ui->menuExamples->setEnabled(false); //to be done!
}

DRS4ScriptDlg::~DRS4ScriptDlg()
{
    if (m_scriptWorkerThread) {
        if ( m_scriptWorkerThread->isRunning() )
            m_scriptWorkerThread->exit(0);

        while ( m_scriptWorkerThread->isRunning() ) {}
    }

    DDELETE_SAFETY(m_logUpdateTimer);
    DDELETE_SAFETY(m_logFile);
    DDELETE_SAFETY(m_logFileSucceed);
    DDELETE_SAFETY(m_logFileFailed);
    DDELETE_SAFETY(m_logFilePrintOut);
    DDELETE_SAFETY(m_completer);
    DDELETE_SAFETY(m_scriptWorkerThread);
    DDELETE_SAFETY(m_scriptExecuter);
    DDELETE_SAFETY(ui);
}

void DRS4ScriptDlg::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
}

void DRS4ScriptDlg::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
}

void DRS4ScriptDlg::loadScriptFile()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Load DRS4-Script File..."),
                               DRS4ProgramSettingsManager::sharedInstance()->saveScriptFilePath(),
                               QString(" (*" + EXT_SCRIPT_FILE + ")"));

    if ( fileName.isEmpty() )
        return;

    DRS4ProgramSettingsManager::sharedInstance()->setSaveScriptFilePath(fileName);


    QFile file(fileName);

    if ( file.open(QIODevice::ReadOnly) )
    {
        ui->textEdit_scriptInput->clear();
        ui->textEdit_scriptInput->appendPlainText(QString(file.readAll()));

        emit ui->textEdit_scriptInput->textChanged();

        file.close();
    }
    else
    {
        MSGBOX("Sorry, cannot load this Script-File!");
        return;
    }

    m_currentScriptPath = fileName;
    updateCurrentFileLabel();
}

void DRS4ScriptDlg::saveScriptFile()
{
    if ( m_currentScriptPath.isEmpty() || m_currentScriptPath.contains(NO_SCRIPT_FILE_PLACEHOLDER) )
    {
        saveAsScriptFile();
        return;
    }

    QFile file(m_currentScriptPath);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) )
    {
        stream << ui->textEdit_scriptInput->toPlainText();
        file.close();

        //MSGBOX("Script-File saved successfully!");
    }
    else
    {
        MSGBOX("Sorry, an Error occurred while saving the Script-File!");
    }

    updateCurrentFileLabel();
}

void DRS4ScriptDlg::saveAsScriptFile()
{
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save DRS4-Script File..."),
                               DRS4ProgramSettingsManager::sharedInstance()->saveScriptFilePath(),
                               QString(" (*" + EXT_SCRIPT_FILE + ")"));

    if ( fileName.isEmpty() )
        return;

    DRS4ProgramSettingsManager::sharedInstance()->setSaveScriptFilePath(fileName);


    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) )
    {
        stream << ui->textEdit_scriptInput->toPlainText();
        file.close();

        //MSGBOX("Script-File saved successfully!");
    }
    else
    {
        MSGBOX("Sorry, an Error occurred while saving the Script-File!");
    }

    m_currentScriptPath = fileName;
    updateCurrentFileLabel();
}

void DRS4ScriptDlg::loadExample_DataAcqu_1()
{
    if ( !ui->textEdit_scriptInput->document()->isEmpty() )
    {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Load Script-File?");
            msgBox.setText("Loading a Script will overwrite the existing Script. Do you wan`t to load a new Script?");
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.addButton(QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);

            const int retVal = msgBox.exec();

            if ( retVal == QMessageBox::No
                 ||  retVal == QMessageBox::Cancel  )
                return;
    }

    QFile file(":/settings/DataAcqu_1");

    if ( file.open(QIODevice::ReadOnly) )
    {
        ui->textEdit_scriptInput->clear();
        ui->textEdit_scriptInput->appendPlainText(QString(file.readAll()));

        emit ui->textEdit_scriptInput->textChanged();

        file.close();
    }
    else
    {
        MSGBOX("Sorry, cannot load this Script-File!")
    }
}

void DRS4ScriptDlg::loadExample_PulseStream_1()
{
    if ( !ui->textEdit_scriptInput->document()->isEmpty() )
    {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Load Script-File?");
            msgBox.setText("Loading a Script will overwrite the existing Script. Do you wan`t to load a new Script?");
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.addButton(QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);

            const int retVal = msgBox.exec();

            if ( retVal == QMessageBox::No
                 ||  retVal == QMessageBox::Cancel  )
                return;
    }

    QFile file(":/settings/PulseStream_1");

    if ( file.open(QIODevice::ReadOnly) )
    {
        ui->textEdit_scriptInput->clear();
        ui->textEdit_scriptInput->appendPlainText(QString(file.readAll()));

        emit ui->textEdit_scriptInput->textChanged();

        file.close();
    }
    else
    {
        MSGBOX("Sorry, cannot load this Script-File!")
    }
}

void DRS4ScriptDlg::loadExample_WarmingUp_2()
{
    if ( !ui->textEdit_scriptInput->document()->isEmpty() )
    {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Load Script-File?");
            msgBox.setText("Loading a Script will overwrite the existing Script. Do you wan`t to load a new Script?");
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.addButton(QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);

            const int retVal = msgBox.exec();

            if ( retVal == QMessageBox::No
                 ||  retVal == QMessageBox::Cancel  )
                return;
    }

    QFile file(":/settings/WarmingUp_1");

    if ( file.open(QIODevice::ReadOnly) )
    {
        ui->textEdit_scriptInput->clear();
        ui->textEdit_scriptInput->appendPlainText(QString(file.readAll()));

        emit ui->textEdit_scriptInput->textChanged();

        file.close();
    }
    else
    {
        MSGBOX("Sorry, cannot load this Script-File!")
    }
}

void DRS4ScriptDlg::saveLogCompleteFile()
{
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log-File..."),
                               DRS4ProgramSettingsManager::sharedInstance()->saveLogFilePath(),
                               tr(" (*.log)"));

    if ( fileName.isEmpty() )
        return;

    DRS4ProgramSettingsManager::sharedInstance()->setSaveLogFilePath(fileName);


    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) )
    {
        stream << ui->textEdit_log->toPlainText();
        file.close();
    }
    else
    {
        MSGBOX("Sorry, an Error occurred while saving the Log-File!");
    }
}

void DRS4ScriptDlg::saveLogSucceedFile()
{
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log-File..."),
                               DRS4ProgramSettingsManager::sharedInstance()->saveLogFilePath(),
                               tr(" (*.log)"));

    if ( fileName.isEmpty() )
        return;

    DRS4ProgramSettingsManager::sharedInstance()->setSaveLogFilePath(fileName);


    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) )
    {
        stream << ui->textEdit_logSucceed->toPlainText();
        file.close();
    }
    else
    {
        MSGBOX("Sorry, an Error occurred while saving the Log-File!");
    }
}

void DRS4ScriptDlg::saveLogFailedFile()
{
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log-File..."),
                               DRS4ProgramSettingsManager::sharedInstance()->saveLogFilePath(),
                               tr(" (*.log)"));

    if ( fileName.isEmpty() )
        return;

    DRS4ProgramSettingsManager::sharedInstance()->setSaveLogFilePath(fileName);


    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) )
    {
        stream << ui->textEdit_logFailed->toPlainText();
        file.close();
    }
    else
    {
        MSGBOX("Sorry, an Error occurred while saving the Log-File!");
    }
}

void DRS4ScriptDlg::saveLogPrintOutFile()
{
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log-File..."),
                               DRS4ProgramSettingsManager::sharedInstance()->saveLogFilePath(),
                               tr(" (*.log)"));

    if ( fileName.isEmpty() )
        return;

    DRS4ProgramSettingsManager::sharedInstance()->setSaveLogFilePath(fileName);


    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) )
    {
        stream << ui->textEdit_logPrintOut->toPlainText();
        file.close();
    }
    else
    {
        MSGBOX("Sorry, an Error occurred while saving the Log-File!");
    }
}

void DRS4ScriptDlg::doScriptAction()
{
    if ( !m_scriptIsRunning )
        runScript();
    else
        abortScript();
}

void DRS4ScriptDlg::runScript()
{
    if ( ui->textEdit_scriptInput->toPlainText().trimmed().isEmpty() )
    {
        MSGBOX("No Script available!");
        return;
    }

    DRS4ScriptManager::sharedInstance()->setArmed(true);
    emit scriptStarted();

    ui->menuFile->setEnabled(false);
    ui->menuLog->setEnabled(false);
    ui->menuExamples->setEnabled(false);

    ui->textEdit_scriptInput->setEnabled(false);

    m_scriptIsRunning = true;
    ui->pushButton_runScript->setText("Abort Running Script...");

    disconnect(ui->textEdit_scriptInput, SIGNAL(textChanged()), this, SLOT(updateScript()));
    disconnect(ui->textEdit_scriptInput, SIGNAL(cursorPositionChanged()), this, SLOT(updateScript()));
    disconnect(ui->textEdit_scriptInput, SIGNAL(selectionChanged()), this, SLOT(updateScript()));

    m_logUpdateTimer->start();
    m_scriptWorkerThread->start();

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled()  )
    {
        setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE - SCRIPT IS CURRENTLY RUNNING");
        m_dlgParent->setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE - SCRIPT IS CURRENTLY RUNNING");
    }
    else
    {
        setWindowTitle(PROGRAM_NAME + " - SCRIPT IS CURRENTLY RUNNING");
        m_dlgParent->setWindowTitle(PROGRAM_NAME + " - SCRIPT IS CURRENTLY RUNNING");
    }
}

void DRS4ScriptDlg::abortScript()
{
    if ( m_scriptWorkerThread->isRunning() )
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Abort Running Script?");
        msgBox.setText("Script is still Running. Do you really want to abort the Script?");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);

        const int retVal = msgBox.exec();

        if ( retVal == QMessageBox::No
             ||  retVal == QMessageBox::Cancel  )
            return;
    }

    m_scriptExecuter->abort();
    m_scriptWorkerThread->terminate();

    DRS4ScriptManager::sharedInstance()->setArmed(false);

    ui->menuFile->setEnabled(true);
    ui->menuLog->setEnabled(true);
    ui->menuExamples->setEnabled(true);

    ui->textEdit_scriptInput->setEnabled(true);

    m_scriptIsRunning = false;
    ui->pushButton_runScript->setText("Run Script...");

    connect(ui->textEdit_scriptInput, SIGNAL(textChanged()), this, SLOT(updateScript()));
    connect(ui->textEdit_scriptInput, SIGNAL(cursorPositionChanged()), this, SLOT(updateScript()));
    connect(ui->textEdit_scriptInput, SIGNAL(selectionChanged()), this, SLOT(updateScript()));

    emit scriptFinished();

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled()  )
    {
        setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE");
        m_dlgParent->setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE");
    }
    else
    {
        setWindowTitle(PROGRAM_NAME);
        m_dlgParent->setWindowTitle(PROGRAM_NAME);
    }
}

void DRS4ScriptDlg::setupCompleter()
{
    DDELETE_SAFETY(m_completer);
    m_completer = new QCompleter;

    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCompletionRole(Qt::DisplayRole);
    m_completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setWrapAround(false);

    m_completer->setModel(new QStringListModel(m_scriptExecuter->functionCollection(), m_completer));

    ui->textEdit_scriptInput->setCompleter(m_completer);
}

void DRS4ScriptDlg::updateCurrentFileLabel()
{
    m_currentScriptFileLabel->setText(m_currentScriptPath);

    DRS4ScriptManager::sharedInstance()->setFileName(m_currentScriptPath);

    update();
}

void DRS4ScriptDlg::finishScript()
{
    ui->menuFile->setEnabled(true);
    ui->menuLog->setEnabled(true);
    ui->menuExamples->setEnabled(true);

    ui->textEdit_scriptInput->setEnabled(true);

    m_scriptIsRunning = false;
    ui->pushButton_runScript->setText("Run Script...");

    connect(ui->textEdit_scriptInput, SIGNAL(textChanged()), this, SLOT(updateScript()));
    connect(ui->textEdit_scriptInput, SIGNAL(cursorPositionChanged()), this, SLOT(updateScript()));
    connect(ui->textEdit_scriptInput, SIGNAL(selectionChanged()), this, SLOT(updateScript()));

    DRS4ScriptManager::sharedInstance()->setArmed(false);

    emit scriptFinished();

    if ( DRS4ScriptDataInterChangeManager::sharedInstance()->getScriptState() == SCRIPT_FINISHED_OK )
    {
        //m_logFile->append("[" + QDateTime::currentDateTime().toString() + "] Script Execution finished successfully!");
    }
    if ( DRS4ScriptDataInterChangeManager::sharedInstance()->getScriptState() == SCRIPT_SYNTAX_ERROR )
    {
        //m_logFile->append("[" + QDateTime::currentDateTime().toString() + "] Script Execution aborted: Syntax-Error!");
    }
    else if ( DRS4ScriptDataInterChangeManager::sharedInstance()->getScriptState() == SCRIPT_ABORTED )
    {
        //m_logFile->append("[" + QDateTime::currentDateTime().toString() + "] Script aborted by User!");
    }

    if (DRS4BoardManager::sharedInstance()->isDemoModeEnabled()  )
    {
        setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE");
        m_dlgParent->setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE");
    }
    else
    {
        setWindowTitle(PROGRAM_NAME);
        m_dlgParent->setWindowTitle(PROGRAM_NAME);
    }

    updateLog();
    updateScript();
    m_logUpdateTimer->stop();
}

void DRS4ScriptDlg::updateScript()
{
    if ( ui->textEdit_log->toPlainText().isEmpty() ) {
        //ui->menuLog->setEnabled(false);
        ui->actionSave_Log->setEnabled(false);
        ui->actionSave_Log_Failed->setEnabled(false);
        ui->actionSave_Log_PrintOut->setEnabled(false);
        ui->actionSave_Log_Succeed->setEnabled(false);
    }
    else {
        //ui->menuLog->setEnabled(true);
        ui->actionSave_Log->setEnabled(true);
        ui->actionSave_Log_Failed->setEnabled(true);
        ui->actionSave_Log_PrintOut->setEnabled(true);
        ui->actionSave_Log_Succeed->setEnabled(true);
    }

    DRS4ScriptDataInterChangeManager::sharedInstance()->setScript(ui->textEdit_scriptInput->toPlainText());
}

void DRS4ScriptDlg::updateLog()
{
    const QString printLog = m_logFile->dataBundle();
    const QString printLog1 = m_logFilePrintOut->dataBundle();
    const QString printLog2 = m_logFileSucceed->dataBundle();
    const QString printLog3 = m_logFileFailed->dataBundle();

    if (!printLog.isEmpty() && !(printLog == "<br>"))
        ui->textEdit_log->append(printLog);

    if (!printLog1.isEmpty() && !(printLog1 == "<br>"))
        ui->textEdit_logPrintOut->append(printLog1);

    if (!printLog2.isEmpty() && !(printLog2 == "<br>"))
        ui->textEdit_logSucceed->append(printLog2);

    if (!printLog3.isEmpty() && !(printLog3 == "<br>"))
        ui->textEdit_logFailed->append(printLog3);
}


DRS4ScriptTextEdit::DRS4ScriptTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    m_completer = nullptr;

    setPlaceholderText("Type <Ctrl+E> to see all available functions.");

    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

DRS4ScriptTextEdit::~DRS4ScriptTextEdit()
{
    DDELETE_SAFETY(lineNumberArea);
}

void DRS4ScriptTextEdit::setCompleter(QCompleter *c)
{
    if (!c)
        return;

    disconnect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

    m_completer = c;

    connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

QCompleter *DRS4ScriptTextEdit::completer() const
{
    return m_completer;
}

void DRS4ScriptTextEdit::keyPressEvent(QKeyEvent *e)
{
    if (m_completer
            && m_completer->popup()->isVisible())
    {
        switch ( e->key() )
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                e->ignore();
                return;
            default:
                break;
        }
    }

    const bool isShortcut = ((e->modifiers() & Qt::ControlModifier)
                             && e->key() == Qt::Key_E); // CTRL+E

    if (!m_completer || !isShortcut) // do not process the shortcut when we have a completer
        QPlainTextEdit::keyPressEvent(e);


    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);

    if ( !m_completer
         || (ctrlOrShift && e->text().isEmpty()) )
        return;

    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;

    QString completionPrefix = textUnderCursor();

    if (!isShortcut
            && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 1))
    {
        m_completer->popup()->hide();
        return;
    }

    if (completionPrefix != m_completer->completionPrefix())
    {
        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }

    QRect cr = cursorRect();

    cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());

    m_completer->complete(cr); // popup it up!
}

void DRS4ScriptTextEdit::focusInEvent(QFocusEvent *e)
{
    if (m_completer)
        m_completer->setWidget(this);

     QPlainTextEdit::focusInEvent(e);
}

void DRS4ScriptTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void DRS4ScriptTextEdit::insertCompletion(const QString &completion)
{
    if ( m_completer->widget() != this)
        return;

    QTextCursor tc = textCursor();

    const int extra = completion.length() - m_completer->completionPrefix().length();

    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);

    const bool funct = (completion.contains("(__") && completion.contains("__)"))
            || (completion.contains("(\"__") && completion.contains("__\")"));

    const bool functWithRet = completion.contains("(") && completion.contains(")") && completion.contains(" << ");


   QString compl = completion;
   compl.remove("__int_value_of_maximum_1024__").remove("\"__name_of_file__\"").remove("__int_value_in_mV__");
   compl.remove("__int_cell__").remove("__int_number_of_points__").remove("__double_value_in_%__");
   compl.remove("__bool_using_linear?__").remove("__bool_using_cubic?__").remove("__int_delay_in_nanoseconds__").remove("__double_value_in_picoseconds__");
   compl.remove("__bool_positive?__").remove("__bool_burst?__").remove("__int_value__").remove("__bool_areaFilter?__").remove("__bool_burstMode?__");
   compl.remove("__bool_automatically?__").remove("__variant__");
   compl.remove(" << bool").remove(" << int").remove(" << double").remove(" << string");

   const int extraFunct = compl.length() - m_completer->completionPrefix().length();

    if ( funct || functWithRet )
    {
        tc.insertText(compl.right(extraFunct));
        tc.movePosition(QTextCursor::EndOfWord);

        if ( funct )
            tc.setPosition(tc.position()-1);
    }
    else
        tc.insertText(completion.right(extra));

    setTextCursor(tc);
}

QString DRS4ScriptTextEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();

    tc.select(QTextCursor::WordUnderCursor);

    return tc.selectedText();
}


int DRS4ScriptTextEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void DRS4ScriptTextEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void DRS4ScriptTextEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void DRS4ScriptTextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(qRgb(153, 211, 232));//.lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void DRS4ScriptTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(qRgb(206, 215, 219)));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::darkGray);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}


Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor(qRgb(156, 77, 183)));
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {

        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

