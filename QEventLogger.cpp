#include "QEventLogger.h"

QEventLogger::QEventLogger(const QString & logFileBaseName, QObject * parent) : QObject(parent) {
    // Build log file name.
    QDateTime now = QDateTime::currentDateTime();
    QString fullLogFileName = logFileBaseName + ' ' + now.toString(Qt::ISODate).replace(":", "-") + ".csv";

    // Open log file.
    this->logFile = new QFile(fullLogFileName);
    if (!this->logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        exit(1);
    this->log = new QTextStream(this->logFile);

    // Write header to log file.
    *log << "; Date and time are: " << now.toString(Qt::ISODate) << '\n';
    *log << "time,input type,event type,target widget class,details\n";
    log->flush();

    // Start timer.
    this->time = new QTime();
    this->time->start();
}

bool QEventLogger::eventFilter(QObject * obj, QEvent * event) {
    static QMouseEvent * mouseEvent;
    static QKeyEvent * keyEvent;
    static QHoverEvent * hoverEvent;
    static QFocusEvent * focusEvent;
    static QString eventType, details;
    static int inputType, mouseButton, modifierKey;
    inputType = NONE;

    switch (event->type()) {
    case QEvent::MouseMove:
        inputType = MOUSE;
        eventType = "MouseMove";
        break;
    case QEvent::MouseTrackingChange:
        inputType = MOUSE;
        eventType = "MouseTrackingChange";
        break;
    case QEvent::MouseButtonPress:
        inputType = MOUSE;
        eventType = "MouseButtonPress";
        break;
    case QEvent::MouseButtonRelease:
        inputType = MOUSE;
        eventType = "MouseButtonRelease";
        break;
    case QEvent::MouseButtonDblClick:
        inputType = MOUSE;
        eventType = "MouseButtonDblClick";
        break;
    case QEvent::KeyPress:
        inputType = KEYBOARD;
        eventType = "KeyPress";
    case QEvent::KeyRelease:
        inputType = KEYBOARD;
        eventType = "KeyRelease";
        break;
    case QEvent::HoverEnter:
        inputType = HOVER;
        eventType = "HoverEnter";
        break;
    case QEvent::GraphicsSceneHoverEnter:
        inputType = HOVER;
        eventType = "GraphicsSceneHoverEnter";
        break;
    case QEvent::HoverLeave:
        inputType = HOVER;
        eventType = "HoverLeave";
        break;
    case QEvent::GraphicsSceneHoverLeave:
        inputType = HOVER;
        eventType = "GraphicsSceneHoverLeave";
        break;
    case QEvent::FocusIn:
        inputType = FOCUS;
        eventType = "FocusIn";
        break;
    case QEvent::FocusOut:
        inputType = FOCUS;
        eventType = "FocusOut";
        break;
    default:
        break;
    }

    if (inputType == MOUSE) {
        mouseEvent = static_cast<QMouseEvent *>(event);

        // Collect the mouse buttons that are pressed.
        mouseButton = mouseEvent->buttons();
        QString buttonsPressed;
        if (mouseButton & Qt::LeftButton)
            buttonsPressed += 'L';
        if (mouseButton & Qt::MidButton)
            buttonsPressed += 'M';
        if (mouseButton & Qt::RightButton)
            buttonsPressed += 'R';

        // Build the details string.
        details = '"';
        details += QString::number(mouseEvent->x()) + ',' + QString::number(mouseEvent->y());
        details += ',' + buttonsPressed;
        details += '"';

        this->appendToLog("Mouse", eventType, obj->metaObject()->className(), details);
    }
    else if (inputType == KEYBOARD) {
        keyEvent = static_cast<QKeyEvent *>(event);

        // Collect the modifier keys that are pressed.
        modifierKey = keyEvent->modifiers();
        QString modifierKeysPressed;
        if (modifierKey & Qt::ShiftModifier)
            modifierKeysPressed += ":shift";
        if (modifierKey & Qt::ControlModifier)
            modifierKeysPressed += ":ctrl";
        if (modifierKey & Qt::AltModifier)
            modifierKeysPressed += ":alt";
        if (modifierKey & Qt::MetaModifier)
            modifierKeysPressed += ":meta";

        // TODO: support special keys, such as ESC and arrow keys, when
        // keyEvent->text() == "".

        // Build the details string.
        details = '"';
        details += QString::number(keyEvent->key());
        details += ',' + keyEvent->text();
        details += ',' + modifierKeysPressed;
        details += '"';

        this->appendToLog("Keyboard", eventType, obj->metaObject()->className(),details);
    }
    else if (inputType == HOVER) {
        hoverEvent = static_cast<QHoverEvent *>(event);

        qDebug() << hoverEvent << hoverEvent->pos() << obj->metaObject()->className();
    }
    else if (inputType == FOCUS) {
        focusEvent = static_cast<QFocusEvent *>(event);

        qDebug() << focusEvent << obj->metaObject()->className();
    }

    // Always propagate the event further.
    return false;
}

void QEventLogger::appendToLog(const QString & inputType, const QString & eventType, const QString & targetWidgetClass, const QString & details) {
    *(this->log) << this->time->elapsed() << ',' << inputType<< ',' << eventType << ',' << targetWidgetClass << ',' << details << '\n';
//    qDebug() << this->time->elapsed() << inputType << eventType << details;
    this->log->flush();
}
