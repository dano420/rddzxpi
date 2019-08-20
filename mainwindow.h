#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include "ui_about.h"
#include "xidel.h"

#define DEFAULT_STYLE   0
#define WARNING_STYLE   1
#define ERROR_STYLE     2

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void on_analyzeXpathButton_clicked();
    void on_getSourceButton_clicked();

    void on_actionQt_triggered();
    void on_actionAbout_triggered();
    void on_actionEN_triggered();
    void on_actionFR_triggered();
    void on_actionAuto_triggered();
    void on_actionUTF_8_triggered();
    void on_actionLatin1_triggered();

    void disableButtonsSlot();
    void enableButtonsSlot();

    void displaySourceCodeSlot(QString sourceCode);
    void printOutputSlot(QString msg, int style=DEFAULT_STYLE);


private:
    Ui::MainWindow *ui;
    QTranslator         appTranslator;
    xidel *xidelClass;
    void setCheckedLang(QString lang);
    void setCheckedEncoding(QString encoding);
    void interpretXpath(QString sourceCode, QString xpathXpression);

    QVariant readConfig(QString group, QString key, QVariant deafultValue);
    void writeConfig(QString group, QString key, QVariant value);
};

#endif // MAINWINDOW_H
