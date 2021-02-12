#pragma once

#include <QWindow>
#include "ui_InputDialog.h"

class InputDialog : public QWindow
{
    Q_OBJECT

public:
    InputDialog(QWindow*parent = Q_NULLPTR);
    ~InputDialog();

    QString showAndWaitForResult(HWND window, INPUT_DIALOG_DISPLAY_MODE mode);

private:
    Ui::InputDialog ui;
};


enum INPUT_DIALOG_DISPLAY_MODE {
    DIALOG_FOR_FULLSCREEN,
    OVERLAY_FOR_BORDERLESS,
    OVERLAY_FOR_WINDOWED
};