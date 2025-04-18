/**
 * Copyright (c) 2020 ChungZH. ALl Rights Reserved.
 * Licensed under the MIT license.
 * See file LICENSE for detail or copy at <https://opensource.org/licenses/MIT>
 *
 * This file is a part of Notepanda.
 *
 * @file main.cpp
 * @brief This file is main of the project.
 *
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QStyleFactory>
#include <memory>

#include "./core/configmanager.h"
#include "./ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication App(argc, argv);

    App.setOrganizationName("ChungZH");
    App.setApplicationName("Notepanda");
    App.setApplicationVersion("0.1.4");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", "The source file to open.");
    QCommandLineOption configFileOption("c", "specify configuration file.", "config.json");
    parser.addOption(configFileOption);
    parser.process(App);

    QString configFile = parser.value(configFileOption);
    if (configFile.isEmpty()) {
#ifdef Q_OS_WIN

        /*
        if (QLocale::system().country() == QLocale::China ||
            QLocale::system().language() == QLocale::Chinese)
            App.setFont(QFont("Microsoft Yahei", 9, QFont::Normal, false));
        else */
        App.setFont(QFont("Segoe UI", 9, QFont::Normal, false));

        if (QDir(App.applicationDirPath() + "/config").exists()) {
            configFile = App.applicationDirPath() + "/config/notepanda.json";
        }
        else {
            QDir configDir = QStandardPaths::writableLocation(
                QStandardPaths::StandardLocation::AppConfigLocation);
            configFile = configDir.absolutePath() + "/config.json";
        }
#else
        QDir configDir = QDir::homePath() + "/.config/notepanda";
        configFile     = configDir.absolutePath() + "/config.json";
        if (!configDir.exists()) {
            configDir.mkpath(configDir.absolutePath());
        }
#endif
    }
    qDebug() << configFile;

    auto configManager = std::make_unique<ConfigManager>(configFile);
    App.setStyle(QStyleFactory::create(configManager.get()->getStyleTheme()));

    MainWindow notepanda(configManager.get());
    notepanda.show();
    if (parser.positionalArguments().size() == 1)
        notepanda.plainTextEdit->openFile(parser.positionalArguments().at(0));

    qInfo() << QStringLiteral("Welcome to Notepanda!") << "";

    return App.exec();
}
