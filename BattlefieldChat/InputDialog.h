﻿#pragma once

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

    bool eventFilter(QObject* obj, QEvent* event) override;

    QString showAndWaitForResult(HWND window, InputDisplayMode mode);

signals:
    void callInitializeWindow(Qt::WindowFlags windowType, QSize size, QSize editSize, QPoint pos);
    void enterProcessed();
    void fastSwitchedMode(int mode);

public slots:
    void escPressed();
    void lostFocus();
    void enterPressed();
    void switchClicked();
    void textTyped(const QString& text);

    void handleInitializeWindow(Qt::WindowFlags windowType, QSize size, QSize editSize, QPoint pos);

private:
    Ui::InputDialog ui;

    void keyPressEvent(QKeyEvent* e);

    QMutex mutex;
    QWaitCondition waitCondition;
    bool cancelled = false;
    bool showing = false; 
};
