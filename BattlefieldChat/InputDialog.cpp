#include "InputDialog.h"
#include <QKeyEvent>
#include "Utils.h"
#include "GlobalVariables.h"

InputDialog::InputDialog(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->setFocusPolicy(Qt::StrongFocus);
    QObject::connect(ui.editContent, SIGNAL(returnPressed()), this, SLOT(enterPressed()));
    QObject::connect(ui.editContent, SIGNAL(editingFinished()), this, SLOT(lostFocus()));
    QObject::connect(ui.editContent, SIGNAL(textChanged(QString)), this, SLOT(textTyped(QString)));

    qRegisterMetaType<Qt::WindowFlags>("Qt::WindowFlags");
    QObject::connect(this, SIGNAL(callInitializeWindow(Qt::WindowFlags, QSize, QSize, QPoint)), 
        this, SLOT(handleInitializeWindow(Qt::WindowFlags, QSize, QSize, QPoint)));
}

InputDialog::~InputDialog()
{
}

void InputDialog::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Escape) {
        escPressed();
    }
}

QString InputDialog::showAndWaitForResult(HWND window, InputDisplayMode mode) {
    QSize size = this->size();
    QSize editSize = ui.editContent->size();
    QPoint pos;
    Qt::WindowFlags style;
    if (mode == InputDisplayMode::DIALOG_FOR_FULLSCREEN) {
        style = (Qt::Window | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);

        RECT r;
        GetWindowRect(GetDesktopWindow(), &r);
        pos.setX((r.right - size.height()) / 2);
        pos.setY((r.bottom - size.width()) / 2);
    } else {
        style = (Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

        RECT r;
        GetWindowRect(window, &r);
        size.setWidth(r.right - r.left - 25);
        editSize.setWidth(size.width() - (ui.lblStatus->size()).width());
        pos.setX(r.left + 12);

        if (mode == InputDisplayMode::OVERLAY_FOR_BORDERLESS) {
            pos.setY(r.top + 12);
        } else {
            pos.setY(r.top + getSystemTitleHeight() + 12);
        }
    }

    cancelled = true;

    emit callInitializeWindow(style, size, editSize, pos);

    mutex.lock();
    waitCondition.wait(&mutex);
    mutex.unlock();

    QMetaObject::invokeMethod(this, "hide");
    if (cancelled)
        return Q_NULLPTR;
    return ui.editContent->text();
}

void InputDialog::escPressed() {
    QString result = preprocessor->escPressed(ui.editContent->text());
    if (result == Q_NULLPTR) {
        lostFocus(); // cancel
        return;
    }
    ui.editContent->setText(result);
}

void InputDialog::lostFocus() {
    if (cancelled) {
        waitCondition.wakeAll();
    }
}

void InputDialog::enterPressed() {
    QString result = preprocessor->enterPressed(ui.editContent->text());
    if (result == Q_NULLPTR) {
        cancelled = false;
        waitCondition.wakeAll(); // send
        return;
    }
    ui.editContent->setText(result);
}

void InputDialog::handleInitializeWindow(Qt::WindowFlags style, QSize size, QSize editSize, QPoint pos) {
    setWindowFlags(style);
    move(pos);
    resize(size);
    ui.editContent->resize(editSize);

    ui.editContent->setText("");
    ui.editContent->setStyleSheet("color: rgb(0, 0, 0);font: 11pt;");
    ui.lblStatus->setText(QString::fromUtf8(u8"就绪"));

    this->show();
    this->raise();
    this->activateWindow();
    this->setFocus();
    ui.editContent->setFocus();
}

void InputDialog::textTyped(const QString& text) {
    if (preprocessor->shouldValidateLength()) {
        if (preprocessor->process(text).size() > 90) {
            if (allowExceedLimit) {
                ui.lblStatus->setText(QString::fromUtf8(u8"过长"));
                ui.editContent->setStyleSheet("color: rgb(255, 152, 0);font: 11pt;");
                ui.lblStatus->setStyleSheet("color: rgb(255, 152, 0);");
            } else {
                ui.lblStatus->setText(QString::fromUtf8(u8"超长"));
                ui.editContent->setStyleSheet("color: rgb(255, 0, 0);font: 11pt;");
                ui.lblStatus->setStyleSheet("color: rgb(255, 0, 0);");
            }
            return;
        }
    }
    ui.editContent->setStyleSheet("color: rgb(0, 0, 0);font: 11pt;");
    ui.lblStatus->setStyleSheet("");
}