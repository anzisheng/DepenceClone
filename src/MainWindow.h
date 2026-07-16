// MainWindow.h
#pragma once

#include <QMainWindow>
#include <QDockWidget>
#include <QTextEdit>
#include <QListWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupMenuBar();
    void setupDockWindows();
    void setupCentralWidget();

    // 部件指针
    QTextEdit *m_centralTextEdit;
    QListWidget *m_projectDockList;
    QListWidget *m_libraryDockList;
};