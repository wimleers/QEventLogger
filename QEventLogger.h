#ifndef QEVENTLOGGER_H
#define QEVENTLOGGER_H

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDateTime>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#define NONE -1
#define MOUSE 0
#define KEYBOARD 1
#define HOVER 2
#define FOCUS 3

class QEventLogger : public QObject {
    Q_OBJECT

public:
    explicit QEventLogger(const QString & logFileBaseName, QObject * parent = 0);

protected:
    bool eventFilter(QObject * obj, QEvent * event);
    void appendToLog(const QString & inputType, const QString & eventType, const QString & targetWidgetClass, const QString & details);

private:
    QFile * logFile;
    QTextStream * log;
    QTime * time;
};

#endif // QEVENTLOGGER_H
