#pragma once

#include <Windows.h>
#include <QWidget>
#include <QMutex>
#include <QWaitCondition>
#include "ui_InputDialog.h"

enum class InputDisplayMode {
    DIALOG_FOR_FULLSCREEN,
    OVERLAY_FOR_BORDERLESS,
    OVERLAY_FOR_WINDOWED
};

class InputDialog : public QWidget
{
    Q_OBJECT

public:
    InputDialog(QWidget *parent = Q_NULLPTR);
    ~InputDialog();

    QString showAndWaitForResult(HWND window, InputDisplayMode mode);
public slots:
    void escPressed();
    void lostFocus();
    void enterPressed();

private:
    Ui::InputDialog ui;

    void keyPressEvent(QKeyEvent* e);

    QMutex mutex;
    QWaitCondition waitCondition;
    bool cancelled = false;
};
