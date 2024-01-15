#pragma once

#include <Windows.h>
#include <QWidget>
#include <QMutex>
#include <QWaitCondition>
#include "ui_InputDialog.h"

enum class InputDisplayMode {
    D3D_FULLSCREEN,
    BORDERLESS,
    WINDOWED
};

class InputDialog : public QWidget
{
    Q_OBJECT

public:
    InputDialog(QWidget *parent = Q_NULLPTR);
    ~InputDialog();

    bool eventFilter(QObject* obj, QEvent* event) override;

    QString showAndWaitForResult(HWND window, InputDisplayMode mode);

signals:
    void callInitializeWindow(HWND gameWindow, Qt::WindowFlags windowType, QSize size, QSize editSize, QPoint pos, InputDisplayMode mode);
    void enterProcessed();
    void fastSwitchedMode(int mode);

public slots:
    void escPressed();
    void lostFocus();
    void enterPressed();
    void switchClicked();
    void textTyped(const QString& text);

    void handleInitializeWindow(HWND gameWindow, Qt::WindowFlags windowType, QSize size, QSize editSize, QPoint pos, InputDisplayMode mode);

private:
    Ui::InputDialog ui;

    void keyPressEvent(QKeyEvent* e) override;

    QMutex mutex;
    QWaitCondition waitCondition;
    bool cancelled = false;
    bool showing = false; 
};
