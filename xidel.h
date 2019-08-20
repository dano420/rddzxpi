#ifndef XIDEL_H
#define XIDEL_H

#include <QUrl>
#include <QProcess>
#include <QWidget>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QTextCodec>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>


#define DEFAULT_STYLE   0
#define WARNING_STYLE   1
#define ERROR_STYLE     2

class MainWindow;

class xidel : public QWidget
{
    Q_OBJECT

public:
    xidel();
    void getSourceCode(QString url);
    QString encoding;

signals:
    void enableButtons();
    void disableButtons();
    void displaySourceCode(QString str);
    void printOutput(QString msg, int style=DEFAULT_STYLE);

};

#endif // XIDEL_H
