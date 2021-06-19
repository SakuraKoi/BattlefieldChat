﻿#include "InputDialog.h"
#include <QKeyEvent>
#include <QtConcurrent>
#include "Utils.h"
#include "GlobalVariables.h"

InputDialog::InputDialog(QWidget* parent)
    : QWidget(parent) {
    ui.setupUi(this);
    this->setFocusPolicy(Qt::StrongFocus);
    QObject::connect(ui.editContent, SIGNAL(returnPressed()), this, SLOT(enterPressed()));
    QObject::connect(ui.btnSwitch, SIGNAL(clicked()), this, SLOT(switchClicked()));
    QObject::connect(ui.editContent, SIGNAL(textChanged(QString)), this, SLOT(textTyped(QString)));

    qRegisterMetaType<Qt::WindowFlags>("Qt::WindowFlags");
    QObject::connect(this, SIGNAL(callInitializeWindow(Qt::WindowFlags, QSize, QSize, QPoint)),
        this, SLOT(handleInitializeWindow(Qt::WindowFlags, QSize, QSize, QPoint)));
    //ui.editContent->installEventFilter(this);
    //ui.btnSwitch->installEventFilter(this);

    this->installEventFilter(this);

    blurWindow((HWND)winId());
}

InputDialog::~InputDialog() {
}

void InputDialog::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Escape) {
        escPressed();
    }
}

QString InputDialog::showAndWaitForResult(HWND window, InputDisplayMode mode) {
    showing = true;
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
        size.setWidth(r.right - r.left - 64);
        editSize.setWidth(size.width() - (ui.lblStatus->size()).width() - 12 - ui.btnSwitch->width());
        pos.setX(r.left + 32);

        if (mode == InputDisplayMode::OVERLAY_FOR_BORDERLESS) {
            pos.setY(r.top + 12);
        } else {
            pos.setY(r.top + getSystemTitleHeight() + 12);
        }
    }

    cancelled = false;

    emit callInitializeWindow(style, size, editSize, pos);

    mutex.lock();
    waitCondition.wait(&mutex);
    mutex.unlock();

    QMetaObject::invokeMethod(this, "hide");
    if (cancelled)
        return Q_NULLPTR;
    return ui.editContent->text();
}

bool InputDialog::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::ActivationChange) {
        if (!this->isActiveWindow()) {
            lostFocus();
        }
    }

    return false;
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
    if (!cancelled && !showing) {
        cancelled = true;
        waitCondition.wakeAll();
    }
}

void InputDialog::enterPressed() {
    ui.lblStatus->setText(QString::fromUtf8(u8"处理中"));
    ui.editContent->setReadOnly(true);
    QtConcurrent::run([=]() {
        try {
            QString result = preprocessor->enterPressed(ui.editContent->text());
            ui.lblStatus->setText(QString::fromUtf8(u8"完成"));
            if (result == Q_NULLPTR) {
                waitCondition.wakeAll(); // send
            } else {
                ui.editContent->setText(result);
            }
        } catch (TranslateException error) {
            ui.lblStatus->setText(u8"错误");
            mainWindow->pushLog(u8" [x] 翻译服务错误: " + error.reason); mainWindow->logColor(Qt::red);
        }
        emit enterProcessed();
        });
}

void InputDialog::handleInitializeWindow(Qt::WindowFlags style, QSize size, QSize editSize, QPoint pos) {
    setWindowFlags(style);
    move(pos);
    resize(size);
    ui.editContent->resize(editSize);
    ui.btnSwitch->move(QPoint(this->size().width() - 36, 4));


    ui.btnSwitch->setEnabled(preprocessor == &SINGLETON_PREPROCESSOR_PINYIN || preprocessor == &SINGLETON_PREPROCESSOR_TRAD || preprocessor == &SINGLETON_PREPROCESSOR_ENGLISH);
    if (preprocessor == &SINGLETON_PREPROCESSOR_PINYIN) {
        ui.btnSwitch->setText(QString::fromUtf8(u8"拼"));
    } else if (preprocessor == &SINGLETON_PREPROCESSOR_TRAD) {
        ui.btnSwitch->setText(QString::fromUtf8(u8"繁"));
    } else if (preprocessor == &SINGLETON_PREPROCESSOR_ENGLISH) {
        ui.btnSwitch->setText(QString::fromUtf8(u8"英"));
    }

    ui.editContent->setReadOnly(false);
    ui.editContent->setText("");
    ui.lblStatus->setText(QString::fromUtf8(u8"就绪"));
    ui.lblStatus->setStyleSheet("color: rgb(83, 164, 60);\nbackground-color: rgba(255, 255, 255, 0);");

    HWND hForgroundWnd = GetForegroundWindow();
    DWORD dwForeID = GetWindowThreadProcessId(hForgroundWnd, NULL);
    DWORD dwCurID = GetCurrentThreadId();

    AttachThreadInput(dwCurID, dwForeID, TRUE);
    this->show();
    this->raise();
    this->activateWindow();
    this->setFocus();
    SetForegroundWindow((HWND)winId());
    AttachThreadInput(dwCurID, dwForeID, FALSE);

    ui.editContent->setFocus();

    preprocessor->initialize();
    showing = false;
}

void InputDialog::textTyped(const QString& text) {
    if (preprocessor->shouldValidateLength()) {
        if (preprocessor->process(text).size() > 90) {
            if (allowExceedLimit) {
                ui.lblStatus->setText(QString::fromUtf8(u8"过长"));
                ui.lblStatus->setStyleSheet("color: rgb(244, 67, 54);\nbackground-color: rgba(255, 255, 255, 0);");
            } else {
                ui.lblStatus->setText(QString::fromUtf8(u8"超长"));
                ui.lblStatus->setStyleSheet("color: rgb(255, 0, 0);\nbackground-color: rgba(255, 255, 255, 0);");
            }
            return;
        }
    }
    ui.lblStatus->setText(QString::fromUtf8(u8"就绪"));
    ui.lblStatus->setStyleSheet("color: rgb(83, 164, 60);\nbackground-color: rgba(255, 255, 255, 0);");
}

void InputDialog::switchClicked() {
    if (preprocessor == &SINGLETON_PREPROCESSOR_PINYIN) {
        ui.btnSwitch->setText(QString::fromUtf8(u8"繁"));
        emit fastSwitchedMode(1);
    } else if (preprocessor == &SINGLETON_PREPROCESSOR_TRAD) {
        ui.btnSwitch->setText(QString::fromUtf8(u8"英"));
        emit fastSwitchedMode(3);
    } else {
        ui.btnSwitch->setText(QString::fromUtf8(u8"拼"));
        emit fastSwitchedMode(2);
    }

    ui.editContent->setFocus();
}