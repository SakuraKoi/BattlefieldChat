#include "InputDialog.h"
#include <QKeyEvent>
#include "Utils.h"
#include "GlobalVariables.h"

InputDialog::InputDialog(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    QObject::connect(ui.editContent, SIGNAL(returnPressed()), this, SLOT(enterPressed()));
    QObject::connect(ui.editContent, SIGNAL(editingFinished()), this, SLOT(lostFocus()));
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
    if (mode == InputDisplayMode::DIALOG_FOR_FULLSCREEN) {
        setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);

        RECT r;
        GetWindowRect(GetDesktopWindow(), &r);
        pos.setX((r.right - size.height()) / 2);
        pos.setY((r.bottom - size.width()) / 2);
    } else {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

        RECT r;
        GetWindowRect(window, &r);
        size.setWidth(r.right - r.left - 24);
        editSize.setWidth(size.width() - (ui.lblStatus->size()).width());
        pos.setX(r.left + 12);

        if (mode == InputDisplayMode::OVERLAY_FOR_BORDERLESS) {
            pos.setY(r.top + getSystemTitleHeight() + 12);
        } else {
            pos.setY(r.top + 12);
        }
    }
    this->move(pos);
    this->resize(size);
    ui.editContent->resize(editSize);

    cancelled = false;
    ui.editContent->setText("");
    ui.editContent->setStyleSheet("color: rgb(0, 0, 0);");
    ui.lblStatus->setText("¾ÍÐ÷");

    this->show();

    mutex.lock();
    waitCondition.wait(&mutex);
    mutex.unlock();

    this->hide();
    if (cancelled)
        return Q_NULLPTR;
    return ui.editContent->text();
}

void InputDialog::escPressed() {
    QString result = preprocessor.escPressed(ui.editContent->text());
    if (result == Q_NULLPTR) {
        lostFocus(); // cancel
        return;
    }
    ui.editContent->setText(result);
}

void InputDialog::lostFocus() {
    cancelled = true;
    waitCondition.wakeAll();
}

void InputDialog::enterPressed() {
    QString result = preprocessor.enterPressed(ui.editContent->text());
    if (result == Q_NULLPTR) {
        cancelled = false;
        waitCondition.wakeAll(); // send
        return;
    }
    ui.editContent->setText(result);
}