#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString locale = QLocale::system().name().section('_', 0, 0);
    QString lang;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "RDDZ_XPI", "RDDZ_XPI");
    bool writeConfig;
    writeConfig = (settings.childGroups().contains("Global")) ? false : true;
    settings.beginGroup("Global");
    if(!writeConfig)
        lang = settings.value("lang",locale).toString();
    else
    {
        settings.setValue("lang",locale);
        lang = locale;
    }
    settings.endGroup();
    settings.sync();

    this->xidelClass = new xidel();
    ui->setupUi(this);

    this->setCheckedLang(lang);
    this->setCheckedEncoding(this->readConfig("Global","encoding","Auto").toString());
    appTranslator.load(":/lang/resources/lang/rddzxpi_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(&appTranslator);
    ui->retranslateUi(this);

    QObject::connect(xidelClass,SIGNAL(enableButtons()), this, SLOT(enableButtonsSlot()));
    QObject::connect(xidelClass,SIGNAL(disableButtons()), this, SLOT(disableButtonsSlot()));
    QObject::connect(xidelClass, SIGNAL(displaySourceCode(QString)), this, SLOT(displaySourceCodeSlot(QString)));
    QObject::connect(xidelClass, SIGNAL(printOutput(QString,int)), this, SLOT(printOutputSlot(QString,int)));

    ui->xpathEdit->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQt_triggered()
{
    QMessageBox::aboutQt(this,tr("About QT"));
}

void MainWindow::on_actionAbout_triggered()
{
    QDialog *widget = new QDialog(this);
    Ui::About aboutUi;
    aboutUi.setupUi(widget);
    widget->show();
}

void MainWindow::on_actionEN_triggered()
{
    this->writeConfig("Global", "lang", "en");
    this->setCheckedLang("EN");

    appTranslator.load(":/lang/resources/lang/rddzxpi_en");
    ui->retranslateUi(this);
}

void MainWindow::on_actionFR_triggered()
{
    this->writeConfig("Global", "lang", "fr");
    this->setCheckedLang("FR");

    appTranslator.load(":/lang/resources/lang/rddzxpi_fr");
    ui->retranslateUi(this);
}

void MainWindow::setCheckedLang(QString lang)
{
    if(lang.toUpper() == "FR")
    {
        ui->actionFR->setChecked(true);
        ui->actionEN->setChecked(false);
    }
    else
    {
        ui->actionFR->setChecked(false);
        ui->actionEN->setChecked(true);
    }
}

void MainWindow::on_actionAuto_triggered()
{
    this->setCheckedEncoding("Auto");
}

void MainWindow::on_actionUTF_8_triggered()
{
    this->setCheckedEncoding("UTF-8");
}

void MainWindow::on_actionLatin1_triggered()
{
    this->setCheckedEncoding("Latin1");
}

void MainWindow::setCheckedEncoding(QString encoding)
{
    ui->actionAuto->setChecked(false);
    ui->actionLatin1->setChecked(false);
    ui->actionUTF_8->setChecked(false);

    xidelClass->encoding = encoding;

    if(encoding == "UTF-8")
    {
        this->writeConfig("Global","encoding","UTF-8");
        ui->actionUTF_8->setChecked(true);
    }
    else if(encoding == "Auto")
    {
        this->writeConfig("Global","encoding","Auto");
        ui->actionAuto->setChecked(true);
    }
    else if(encoding == "Latin1")
    {
        this->writeConfig("Global","encoding","Latin1");
        ui->actionLatin1->setChecked(true);
    }
}


void MainWindow::on_analyzeXpathButton_clicked()
{
    if(ui->sourceCodeEdit->toPlainText().trimmed().isEmpty())
    {
        this->printOutputSlot(tr("[Error] : Source code field is empty."),ERROR_STYLE);
        return;
    }
    else if(ui->xpathEdit->toPlainText().trimmed().isEmpty())
    {
       this-> printOutputSlot(tr("[Error] : XPath expression field is empty."),ERROR_STYLE);
        return;
    }
    else
        this->interpretXpath(ui->sourceCodeEdit->toPlainText(), ui->xpathEdit->toPlainText());
}

void MainWindow::on_getSourceButton_clicked()
{
    this->disableButtonsSlot();
    QtConcurrent::run(this->xidelClass, &xidel::getSourceCode, ui->urlEdit->text());
}

void MainWindow::printOutputSlot(QString msg, int style)
{
   QTextCharFormat tf;
   tf = ui->outputEdit->currentCharFormat();

    switch(style) {
    case DEFAULT_STYLE :
        tf.setForeground(QBrush(Qt::black));
        break;
    case WARNING_STYLE :
        tf.setForeground(QBrush(QColor(201,117,9)));
        break;
    case ERROR_STYLE :
        tf.setForeground(QBrush(Qt::red));
        break;
    default:
        break;
    }

    ui->outputEdit->setCurrentCharFormat(tf);
    ui->outputEdit->appendPlainText(msg);

}

void MainWindow::interpretXpath(QString sourceCode, QString xpathXpression)
{

    QString xidelCommand, xidelBinaryName, escapedQuery, tmpFileName;
    QStringList arguments;
    QProcess *xidelProcess = new QProcess(nullptr);
    QJsonDocument jsonDoc, errJsonDoc;
    int i,j;


    ui->outputEdit->clear();

    xpathXpression = xpathXpression.trimmed();
    escapedQuery = xpathXpression;

#ifdef Q_OS_WIN32
    // On win 32, args are auto enclose by ' if no spaces are found, otherwise they are enclosed by "
    if(!escapedQuery.contains(" "))
    escapedQuery.replace("'","\\\'");
#endif

#ifdef Q_OS_WIN
    xidelBinaryName = "xidel.exe";
#else
    xidelBinaryName = "xidel";
#endif

    tmpFileName = QDir::tempPath()+"/rddzStack"+QCryptographicHash::hash(sourceCode.toUtf8(),QCryptographicHash::Md5).toHex();
    QFile stackFile(tmpFileName);
    QTextStream stream( &stackFile );
    if(!stackFile.exists())
    {
        if ( stackFile.open(QIODevice::ReadWrite) )
        {
            stream.setCodec("UTF-8");
            stream << sourceCode << endl;
            stream.flush();
        }
        stackFile.close();
    }

    if (ui->radioXpathButton->isChecked())
        arguments << "--data" << tmpFileName << "-e" << escapedQuery << "--output-format" << "json-wrapped" << "--output-encoding" << "input";
    else
         arguments << "--data" << tmpFileName << "-e" << escapedQuery << "--output-format" << "json-wrapped" << "--output-encoding" << "input" << "--input-format" << "json" << "--printed-json-format" << "compact";
    xidelCommand = QCoreApplication::applicationDirPath()+"/dist/"+xidelBinaryName;
    xidelProcess->start(xidelCommand, arguments);


    if (!xidelProcess->waitForStarted())
    {
        this->printOutputSlot(tr("Unable to start xidel process"),ERROR_STYLE);
        stackFile.close();
        stackFile.remove();
        return;

    }

    if (!xidelProcess->waitForFinished())
    {
       this->printOutputSlot(tr("Xidel process has crashed"),ERROR_STYLE);
       xidelProcess->kill();
       stackFile.remove();
       return;
    }


    QByteArray results = xidelProcess->readAllStandardOutput();
    QString xpathError = xidelProcess->readAllStandardError();
    xidelProcess->close();

    xpathError.replace(QRegExp("([\\*]{4}[^\\*]+[\\*]{4}|^$)"),"");

    errJsonDoc = QJsonDocument::fromJson(xpathError.toUtf8());
    jsonDoc = QJsonDocument::fromJson(results);
    QJsonArray values = jsonDoc.array();

   if (errJsonDoc.isObject())
   {
       QJsonObject jsonObj;
       jsonObj = errJsonDoc.object();
       jsonObj = jsonObj.value("_error").toObject();

       this->printOutputSlot(tr("[XPath Error]  ") + tr("Unable to evaluate XPath query : ") + xpathXpression, ERROR_STYLE);
       this->printOutputSlot(tr("[XPath Error]  ") + jsonObj.value("_message").toString(), ERROR_STYLE);
       stackFile.remove();
       return;
   }

   QTextDocument txtDoc;
   bool objectFound;
   int  itemsFound;

   objectFound =false;
   itemsFound = 0;

    for (i=0;i<values.size();i++)
    {

        if(values.at(i).isArray())
        {
            QJsonArray arrayValues = values.at(i).toArray();
            for(j=0;j<arrayValues.size();j++)
            {
                 QString strValue;
                if(arrayValues.at(j).type() == QJsonValue::Object)
                    objectFound = true;
                if(arrayValues.at(j).type() == QJsonValue::Double)
                    strValue = QString::number(arrayValues.at(j).toDouble());
                else
                 strValue = arrayValues.at(j).toString();
                if(ui->decodeHtml->isChecked())
                {
                    txtDoc.setHtml(strValue);
                    strValue = txtDoc.toPlainText();
                }
                this->printOutputSlot(strValue);
                itemsFound++;
            }
        }
        else
        {
            if(values.at(i).isString() || values.at(i).isBool())
            {
                QString strValue = values.at(i).toString();;
                if(ui->decodeHtml->isChecked())
                {
                    txtDoc.setHtml(strValue);
                    strValue = txtDoc.toPlainText();
                }
                this->printOutputSlot(strValue);

            }
            else if (values.at(i).isDouble())
                this->printOutputSlot(QString::number(values.at(i).toDouble()));
            itemsFound++;
        }
    }


    if (ui->radioJsoniqButton->isChecked() && objectFound == true)
        this->printOutputSlot(QString(results));
    stackFile.remove();
    ui->nbItems->setText(tr("%1 item(s) found").arg(QString::number(itemsFound)));


}

QVariant MainWindow::readConfig(QString group, QString key, QVariant deafultValue)
{
    QVariant returnValue;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "RDDZ_XPI", "RDDZ_XPI");
    settings.beginGroup(group);
    returnValue = settings.value(key,deafultValue);
    settings.endGroup();
    settings.sync();

    return returnValue;
}

void MainWindow::writeConfig(QString group, QString key, QVariant value)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "RDDZ_XPI", "RDDZ_XPI");
    settings.beginGroup(group);
    settings.setValue(key,value);
    settings.endGroup();
    settings.sync();
}


void MainWindow::disableButtonsSlot()
{
    ui->getSourceButton->setEnabled(false);
    ui->analyzeXpathButton->setEnabled(false);
}

void MainWindow::enableButtonsSlot()
{
    ui->getSourceButton->setEnabled(true);
    ui->analyzeXpathButton->setEnabled(true);
}

void MainWindow::displaySourceCodeSlot(QString sourceCode)
{
    ui->sourceCodeEdit->clear();
    ui->sourceCodeEdit->appendPlainText(sourceCode);
}


