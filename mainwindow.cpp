#include "mainwindow.h"
#include "glwidget.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    glWidget = new GLWidget(this);
    setCentralWidget(glWidget);
    setWindowTitle("Bezier Intersection");
    resize(800, 600);
}

MainWindow::~MainWindow() {}