/**
 * Copyright (c) 2020 ChungZH. ALl Rights Reserved.
 * Licensed under the MIT license.
 * See file LICENSE for detail or copy at <https://opensource.org/licenses/MIT>
 *
 * This file is a part of Notepanda.
 *
 * @file mainwindow.h
 * @brief This file declares the MainWindow class.
 *        It is the main window of notepanda.
 *
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QColorDialog>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QTextBrowser>
#include <QToolBar>
#include <QVector>

#include "../core/configmanager.h"
#include "../core/texteditor.h"
#include "preferenceswindow.h"

QT_BEGIN_NAMESPACE
class TextEditor;
QT_END_NAMESPACE

struct TabData
{
    QString plainText;
    QString fileName;
    bool    isModified;
};

enum NoteMode
{
    Normal = 0,
    Sticky = 1
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(ConfigManager* confManager, QWidget* parent = nullptr);
    ~MainWindow();
    TextEditor* plainTextEdit = nullptr;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void documentWasModified();

private:
    void setupMenuBar();
    void setupStickyNoteToolBar();
    void setupNormalToolBar();
    void setupTabToolBar();
    void setupCentralWidget();
    void setupDockWidget();
    void setupStatusBar();

    void normalMode(bool isFirst);
    void stickyNoteMode();

    void changeWindowTitle();
    void disableFeatures();
    void saveTabData(const int index);

private:
    // TODO: use QTabBar::tabData
    QVector<TabData> tabData;

    ConfigManager* configManager = nullptr;

    QToolBar* tabToolBar = nullptr;
    QTabBar*  tabBar     = new QTabBar;

    QToolBar*          nToolBar     = new QToolBar;
    QTextBrowser*      previewPanel = new QTextBrowser;
    PreferencesWindow* pfWindow     = nullptr;
    QDockWidget*       dockWidget   = nullptr;

    bool lastTabRemoveFlag = false;
    bool doNotSaveDataFlag = false;
    int  curTabIndex       = 0;
    int  prevTabIndex      = 0;

    int  currentMode = NoteMode::Normal;
    bool isPintotop  = false;

    // Sticky note mode
    QToolBar*     sToolBar      = new QToolBar;
    QColorDialog* colorDialog   = new QColorDialog;
    QAction*      changeBgColor = nullptr;
    QColor        currentColor  = "#AAFFFF";

    // UI
    QAction* actionNew            = nullptr;
    QAction* actionOpen           = nullptr;
    QAction* actionSave           = nullptr;
    QAction* actionSaveAs         = nullptr;
    QAction* actionUndo           = nullptr;
    QAction* actionRedo           = nullptr;
    QAction* actionCopy           = nullptr;
    QAction* actionPaste          = nullptr;
    QAction* actionQuit           = nullptr;
    QAction* actionCut            = nullptr;
    QAction* actionPrint          = nullptr;
    QAction* actionPreferences    = nullptr;
    QAction* actionStickyNoteMode = nullptr;
    QAction* actionNormalMode     = nullptr;
    QAction* actionPinToTop       = nullptr;
    QAction* actionPreviewPanel   = nullptr;
    QAction* actionReadOnlyMode   = nullptr;
    QAction* actionAbout          = nullptr;
    QAction* actionAboutQt        = nullptr;
    QMenu*   menuFile             = nullptr;
    QMenu*   menuEdit             = nullptr;
    QMenu*   menuView             = nullptr;
    QMenu*   menuHelp             = nullptr;
};
#endif   // MAINWINDOW_H
