/**
 * Copyright (c) 2020 ChungZH. ALl Rights Reserved.
 * Licensed under the MIT license.
 * See file LICENSE for detail or copy at <https://opensource.org/licenses/MIT>
 *
 * This file is a part of Notepanda.
 *
 * @file mainwindow.cpp
 * @brief This file implements the MainWindow class.
 *        It is the main window of notepanda.
 *
 */

#include "mainwindow.h"

#include <QDebug>
#include <QPainter>
#include <QSize>
#include <QSizePolicy>
#include <QStyle>
#include <QStyleFactory>
#include <QTabBar>
#include <QTextStream>
#include <QToolBar>

#include "aboutwindow.h"
#include "preferenceswindow.h"
#include "ui_preferenceswindow.h"

MainWindow::MainWindow(ConfigManager* cfManager, QWidget* parent)
    : QMainWindow(parent)
    , configManager(cfManager)
{
    resize(800, 600);
    setBaseSize(size());

    // Menu Bar
    setupMenuBar();
    // Tool Bars
    setupStickyNoteToolBar();
    setupNormalToolBar();
    setupTabToolBar();
    // Central Widget
    setupCentralWidget();
    // Dock Widgets
    setupDockWidget();
    // Status Bar
    setupStatusBar();

    changeWindowTitle();

    disableFeatures();
}

MainWindow::~MainWindow()
{
    delete plainTextEdit;
    delete pfWindow;
}

void MainWindow::setupMenuBar()
{
    actionNew = new QAction(QIcon(":/icons/add.svg"), tr("&New"), this);
    actionNew->setShortcut(QKeySequence::New);

    actionOpen = new QAction(QIcon(":/icons/open.svg"), tr("&Open"), this);
    actionOpen->setShortcut(QKeySequence::Open);

    actionSave = new QAction(QIcon(":/icons/save.svg"), tr("&Save"), this);
    actionSave->setShortcut(QKeySequence::Save);

    actionSaveAs = new QAction(QIcon(":/icons/save_as.svg"), tr("&Save as"), this);
    actionSaveAs->setShortcut(QKeySequence::SaveAs);

    actionUndo = new QAction(QIcon(":/icons/undo.svg"), tr("&Undo"), this);
    actionUndo->setShortcut(QKeySequence::Undo);

    actionRedo = new QAction(QIcon(":/icons/redo.svg"), tr("&Redo"), this);
    actionRedo->setShortcut(QKeySequence::Redo);

    actionCopy = new QAction(QIcon(":/icons/copy.svg"), tr("&Copy"), this);
    actionCopy->setShortcut(QKeySequence::Copy);

    actionPaste = new QAction(QIcon(":/icons/clipboard.svg"), tr("&Paste"), this);
    actionPaste->setShortcut(QKeySequence::Paste);

    actionQuit = new QAction(QIcon(":/icons/delete.svg"), tr("&Quit"), this);
    actionQuit->setShortcut(QKeySequence::Quit);
    connect(actionQuit, &QAction::triggered, this, &QCoreApplication::quit);

    actionCut = new QAction(QIcon(":/icons/cut.svg"), tr("Cut"), this);
    actionCut->setShortcut(QKeySequence::Cut);

    actionPrint = new QAction(QIcon(":/icons/print.svg"), tr("Print"), this);
    actionPrint->setShortcut(QKeySequence::Print);

    actionPreferences = new QAction(QIcon(":/icons/settings.svg"), tr("Preferences"), this);
    actionPreferences->setShortcut(QKeySequence::Preferences);

    actionStickyNoteMode = new QAction(QIcon(":/icons/sticker.svg"), tr("Sticky note mode"), this);
    actionStickyNoteMode->setToolTip(tr("Switch to sticky note mode"));
    connect(actionStickyNoteMode, &QAction::triggered, [&]() {
        stickyNoteMode();
        actionStickyNoteMode->setDisabled(true);
        actionNormalMode->setEnabled(true);
    });

    actionNormalMode = new QAction(QIcon(":/icons/document_edit.svg"), tr("Normal mode"), this);
    actionNormalMode->setToolTip(tr("Switch to normal mode"));
    connect(actionNormalMode, &QAction::triggered, [&]() {
        normalMode(false);
        actionNormalMode->setDisabled(true);
        actionStickyNoteMode->setEnabled(true);
    });

    actionPinToTop = new QAction(QIcon(":/icons/pin.svg"), tr("Pin to top"), this);
    connect(actionPinToTop, &QAction::triggered, [&]() {
        Qt::WindowFlags flags = this->windowFlags();
        if (!isPintotop) {
            this->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
            this->show();
        }
        else {
            this->setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
            this->show();
        }
        isPintotop = !isPintotop;
    });

    actionPreviewPanel = new QAction(QIcon(":/icons/preview.svg"), tr("Preview panel"), this);
    actionPreviewPanel->setCheckable(true);
    connect(actionPreviewPanel, &QAction::triggered, [&]() {
        previewPanel->setEnabled(true);
        dockWidget->setVisible(!dockWidget->isVisible());
    });

    actionAbout = new QAction(QIcon(":/icons/info.svg"), tr("&About"), this);
    connect(actionAbout, &QAction::triggered, [&]() { AboutWindow(this).exec(); });

    actionAboutQt = new QAction(tr("&About Qt"), this);
    connect(actionAboutQt, &QAction::triggered, this, &QApplication::aboutQt);

    actionReadOnlyMode = new QAction(QIcon(":/icons/read_only.svg"), tr("Read-Only mode"), this);
    actionReadOnlyMode->setCheckable(true);
    actionReadOnlyMode->setChecked(false);

    menuFile = menuBar()->addMenu(tr("&File"));
    menuFile->addAction(actionNew);
    menuFile->addAction(actionOpen);
    menuFile->addAction(actionSave);
    menuFile->addAction(actionSaveAs);
    menuFile->addSeparator();
    menuFile->addAction(actionPreferences);
    menuFile->addSeparator();
    menuFile->addAction(actionPrint);
    menuFile->addSeparator();
    menuFile->addAction(actionQuit);

    menuEdit = menuBar()->addMenu(tr("&Edit"));
    menuEdit->addAction(actionUndo);
    menuEdit->addAction(actionRedo);
    menuEdit->addSeparator();
    menuEdit->addAction(actionCopy);
    menuEdit->addAction(actionPaste);
    menuEdit->addAction(actionCut);
    menuEdit->addSeparator();
    menuEdit->addAction(actionReadOnlyMode);

    menuView = menuBar()->addMenu(tr("&View"));
    menuView->addAction(actionNormalMode);
    menuView->addAction(actionStickyNoteMode);
    menuView->addSeparator();
    menuView->addAction(actionPinToTop);
    menuView->addSeparator();
    menuView->addAction(actionPreviewPanel);

    menuHelp = menuBar()->addMenu(tr("&Help"));
    menuHelp->addAction(actionAbout);
    menuHelp->addAction(actionAboutQt);
}

void MainWindow::setupStickyNoteToolBar()
{
    // Sticky note mode
    changeBgColor =
        new QAction(QIcon(":/icons/color_background.svg"), tr("Change background color"), this);
    changeBgColor->setToolTip(tr("Change background color"));

    sToolBar->setIconSize(QSize(26, 26));
    sToolBar->setVisible(false);

    sToolBar->addAction(changeBgColor);
    sToolBar->addAction(actionNormalMode);
    sToolBar->addAction(actionPinToTop);

    addToolBar(Qt::ToolBarArea::BottomToolBarArea, sToolBar);

    connect(changeBgColor, &QAction::triggered, [&]() { colorDialog->open(); });
    connect(colorDialog, &QColorDialog::currentColorChanged, [&](const QColor& color) {
        currentColor = color;
        plainTextEdit->setStyleSheet("background-color: " + currentColor.name());
    });
}

void MainWindow::setupNormalToolBar()
{
    nToolBar->setIconSize(QSize(26, 26));
    addToolBar(Qt::ToolBarArea::LeftToolBarArea, nToolBar);
    normalMode(true);
}

void MainWindow::setupTabToolBar()
{
    tabToolBar = new QToolBar;
    tabToolBar->addWidget(tabBar);
    tabToolBar->setMovable(false);
    tabToolBar->setFloatable(false);
    addToolBar(Qt::ToolBarArea::TopToolBarArea, tabToolBar);

    tabBar->setTabsClosable(true);
    tabBar->addTab("Untitled.txt");
    tabData.push_back(TabData{"", "", false});

    connect(tabBar, &QTabBar::currentChanged, [&](const int index) {
        prevTabIndex = curTabIndex;
        curTabIndex  = index;

        if (curTabIndex == -1) curTabIndex = 0;
        if (prevTabIndex == -1) prevTabIndex = 0;

        if (tabBar->count() == 0) {
            prevTabIndex = 0;
            tabData.push_back(TabData{"", "", false});
            tabBar->addTab("Untitled.txt");
        }

        if (lastTabRemoveFlag == true) {
            prevTabIndex = curTabIndex = index;
            lastTabRemoveFlag          = false;
        }
        else if (doNotSaveDataFlag == true) {
            doNotSaveDataFlag = false;
        }
        else {
            saveTabData(prevTabIndex);
        }

        plainTextEdit->clear();
        if (index != -1) {
            plainTextEdit->setCurrentFile(tabData[index].fileName);
            plainTextEdit->setPlainText(tabData[index].plainText);
        }

        plainTextEdit->updateSyntaxHighlight();
        plainTextEdit->document()->setModified(tabData[index].isModified);
        changeWindowTitle();
    });

    connect(tabBar, &QTabBar::tabCloseRequested, [&](int index) {
        tabData.remove(index);
        lastTabRemoveFlag = true;
        tabBar->removeTab(index);

        if (tabBar->count() != 0) {
            TabData tempTD = tabData[tabBar->currentIndex()];
            plainTextEdit->setPlainText(tempTD.plainText);
            plainTextEdit->setCurrentFile(tempTD.fileName);
            plainTextEdit->document()->setModified(tempTD.isModified);
        }
        changeWindowTitle();
    });

    connect(actionNew, &QAction::triggered, [&]() {
        saveTabData(tabBar->currentIndex());

        if (plainTextEdit->newDocument()) {
            doNotSaveDataFlag  = true;
            const int newIndex = tabBar->addTab("Untitled.txt");
            tabData.push_back(TabData{"", "", false});
            tabBar->setCurrentIndex(newIndex);
        }
    });
    connect(actionOpen, &QAction::triggered, [&]() {
        saveTabData(tabBar->currentIndex());
        if (plainTextEdit->open()) {
            doNotSaveDataFlag  = true;
            const int newIndex = tabBar->addTab(plainTextEdit->currentFileName);
            tabData.push_back(
                TabData{plainTextEdit->toPlainText(), plainTextEdit->currentFileName, 0});
            tabBar->setCurrentIndex(newIndex);
        }
    });
}

void MainWindow::setupCentralWidget()
{
    plainTextEdit = new TextEditor(configManager);
    setCentralWidget(plainTextEdit);
    connect(actionSave, &QAction::triggered, plainTextEdit, &TextEditor::save);
    connect(actionSaveAs, &QAction::triggered, plainTextEdit, &TextEditor::saveAs);
    connect(actionRedo, &QAction::triggered, plainTextEdit, &TextEditor::redo);
    connect(actionUndo, &QAction::triggered, plainTextEdit, &TextEditor::undo);
    connect(actionReadOnlyMode, &QAction::triggered, [&]() {
        plainTextEdit->setReadOnly(!plainTextEdit->isReadOnly());
        setupStatusBar();
    });
    /**
    if user change Read-Only mode in right-click menu of TextEditor
    than setChecked in MainWindow::actionReadOnlyMode
    */
    connect(plainTextEdit, &TextEditor::readOnlyChanged, [&]() {
        actionReadOnlyMode->setChecked(plainTextEdit->isReadOnly());
        setupStatusBar();
    });
    connect(plainTextEdit, &TextEditor::changeTitle, this, &MainWindow::changeWindowTitle);
    connect(plainTextEdit->document(),
            &QTextDocument::contentsChanged,
            this,
            &MainWindow::documentWasModified);
    connect(plainTextEdit, &TextEditor::modifiedFalse, [=, this]() { setWindowModified(false); });
    connect(plainTextEdit, &TextEditor::undoAvailable, [=, this](bool undoIsAvailable) {
        actionUndo->setDisabled(!undoIsAvailable);
    });
    connect(plainTextEdit, &TextEditor::redoAvailable, [=, this](bool redoIsAvailable) {
        actionRedo->setDisabled(!redoIsAvailable);
    });

    connect(plainTextEdit, &TextEditor::textChanged, this, &MainWindow::setupStatusBar);
    connect(plainTextEdit, &TextEditor::openFileInNewTab, [&](const QString& file) {
        saveTabData(tabBar->currentIndex());

        const int newIndex = tabBar->addTab(plainTextEdit->currentFileName);
        plainTextEdit->openFile(file);
        tabData.push_back(
            TabData{plainTextEdit->toPlainText(), plainTextEdit->currentFileName, false});
        doNotSaveDataFlag = true;
        tabBar->setCurrentIndex(newIndex);
        changeWindowTitle();
    });
    connect(actionPrint, &QAction::triggered, plainTextEdit, &TextEditor::print);
}

void MainWindow::setupDockWidget()
{
    dockWidget = new QDockWidget(tr("Preview panel"), this);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setWidget(previewPanel);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);
    dockWidget->setVisible(0);

    previewPanel->setEnabled(0);

    connect(dockWidget, &QDockWidget::visibilityChanged, [&](bool visible) {
        actionPreviewPanel->setChecked(visible);
        previewPanel->setEnabled(visible);
    });

    // PreferencesWindow START

    pfWindow = new PreferencesWindow(configManager, this);

    connect(actionPreferences, &QAction::triggered, [&]() { pfWindow->exec(); });

    connect(pfWindow->ui->themeCombo, &QComboBox::currentTextChanged, [&](const QString& curTheme) {
        QApplication::setStyle(QStyleFactory::create(curTheme));
        configManager->setStyleTheme(curTheme);
    });
    connect(pfWindow->ui->fontComboBox, &QFontComboBox::currentFontChanged, [&](const QFont font) {
        plainTextEdit->setEditorFont(font);
    });
    connect(pfWindow->ui->spinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            [=, this](const int& value) { plainTextEdit->setEditorFontSize(value); });
    connect(pfWindow->ui->highlightThemeCombo,
            &QComboBox::currentTextChanged,
            [&](const QString& ctname) {
                plainTextEdit->setEditorColorTheme(ctname);
                configManager->setEditorColorTheme(ctname);
            });

    // User accepted, so change the `settings`.
    connect(pfWindow->ui->buttonBox, &QDialogButtonBox::accepted, [&]() { configManager->save(); });

    // User rejected, so read general settings, do not change the `settings` !!!
    // Otherwise it will not be restored
    connect(pfWindow->ui->buttonBox, &QDialogButtonBox::rejected, [&]() {
        // Restore the variables
        configManager->readGeneralSettings();

        // Restore TextEditor
        plainTextEdit->setEditorFont(configManager->getEditorFontFamily());
        plainTextEdit->setEditorFontSize(configManager->getEditorFontSize());
        plainTextEdit->setEditorColorTheme(configManager->getEditorColorTheme());

        // Restore MainWindow
        QApplication::setStyle(QStyleFactory::create(configManager->getStyleTheme()));

        // Restore PreferencesWindow
        pfWindow->resetAllValues(false);
    });
}

void MainWindow::setupStatusBar()
{
    if (currentMode != NoteMode::Sticky) {
        QString flags;
        if (plainTextEdit->isReadOnly() == 1) {
            flags += "[Read-Only]";
        }

        statusBar()->showMessage(
            tr("Characters:") + QString::number(plainTextEdit->document()->characterCount() - 1) +
            tr(" Lines:") + QString::number(plainTextEdit->document()->lineCount()) + flags);
    }
}

void MainWindow::changeWindowTitle()
{
    QString showName;
    if (!plainTextEdit->currentFile.isEmpty()) {
        showName = plainTextEdit->currentFileName;
    }
    else {
        showName = "Untitled";
    }

    for (int i = 0; i < tabData.size(); i++) {
        if (tabData[i].fileName.isEmpty()) {
            tabBar->setTabText(i, "Untitled");
        }
        else {
            tabBar->setTabText(i, tabData[i].fileName);
        }
    }
    setWindowTitle(showName + "[*] - Notepanda");
}

void MainWindow::disableFeatures()
{
    // Disable menu actions for unavailable features
    actionUndo->setDisabled(true);
    actionRedo->setDisabled(true);

#if !defined(QT_PRINTSUPPORT_LIB) || !QT_CONFIG(printer)
    actionPrint->setEnabled(false);
#endif
#if !QT_CONFIG(clipboard)
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    ui->actionPaste->setEnabled(false);
#else
    connect(actionCopy, &QAction::triggered, plainTextEdit, &TextEditor::copy);
    connect(actionPaste, &QAction::triggered, plainTextEdit, &TextEditor::paste);
    connect(actionCut, &QAction::triggered, plainTextEdit, &TextEditor::cut);
    connect(plainTextEdit, &TextEditor::copyAvailable, [&](bool isCopyAvailable) {
        actionCopy->setDisabled(!isCopyAvailable);
    });
    connect(plainTextEdit, &TextEditor::copyAvailable, [&](bool isCutAvailable) {
        actionCut->setDisabled(!isCutAvailable);
    });
    actionCopy->setDisabled(true);
    actionCut->setDisabled(true);
#endif
}

void MainWindow::normalMode(bool isFirst)
{
    if (!isFirst) {
        resize(baseSize());
        plainTextEdit->switchMode(false);
        nToolBar->setVisible(true);
        sToolBar->setVisible(false);
    }
    else {
        nToolBar->addAction(actionNew);
        nToolBar->addAction(actionOpen);
        nToolBar->addAction(actionSave);
        nToolBar->addAction(actionSaveAs);
        nToolBar->addAction(actionPreferences);
        nToolBar->addAction(actionUndo);
        nToolBar->addAction(actionRedo);
        nToolBar->addAction(actionQuit);
        nToolBar->addAction(actionAbout);
        nToolBar->addAction(actionStickyNoteMode);
        nToolBar->addAction(actionPreviewPanel);
        actionNormalMode->setDisabled(true);
    }
    actionPreferences->setEnabled(true);
    currentMode = NoteMode::Normal;
}

/**
 * @brief Sticky Note Mode, like Microsoft Sticky Notes. For more details, see
 * <https://www.microsoft.com/en-us/p/microsoft-sticky-notes/9nblggh4qghw?activetab=pivot:overviewtab#>
 */
void MainWindow::stickyNoteMode()
{
    resize(baseSize() * 0.7);
    plainTextEdit->switchMode(1);
    plainTextEdit->setStyleSheet("background: " + currentColor.name());
    statusBar()->clearMessage();
    actionPreferences->setDisabled(1);

    nToolBar->setVisible(0);
    sToolBar->setVisible(1);

    currentMode = NoteMode::Sticky;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (plainTextEdit->maybeSave()) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void MainWindow::documentWasModified()
{
    setWindowModified(plainTextEdit->document()->isModified());

    if (previewPanel->isEnabled()) {
        previewPanel->reload();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QString fileSuffix = plainTextEdit->currentFileName.split('.').last();

        if (fileSuffix == "md" || fileSuffix == "markdown" || fileSuffix == "mdown")
            previewPanel->setMarkdown(plainTextEdit->toPlainText());
        else
#endif
            previewPanel->setText(plainTextEdit->toPlainText());
    }
}

void MainWindow::saveTabData(const int index)
{
    tabData[index].fileName   = plainTextEdit->currentFileName;
    tabData[index].isModified = plainTextEdit->document()->isModified();
    tabData[index].plainText  = plainTextEdit->toPlainText();
}
