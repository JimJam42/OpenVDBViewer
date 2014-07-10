#include "AboutWindow.h"
#include "ui_AboutWindow.h"

#include "MainWindow.h"

AboutWindow::AboutWindow(QWidget *parent) :  QDialog(parent), m_ui(new Ui::AboutWindow)
{
    m_ui->setupUi(this);
    connectAndInit();
}

AboutWindow::~AboutWindow()
{
    delete m_ui;
}

void AboutWindow::connectAndInit()
{
    connect(m_ui->btn_close,SIGNAL(clicked(bool)),this,SLOT(close()));
}

void AboutWindow::setParentWindow(MainWindow *_parent)
{
    if (_parent)
    {
        m_parentWindow = _parent;
        m_parentWindowConnected = true;
    }
}
