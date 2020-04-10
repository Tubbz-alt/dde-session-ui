#include "mainwindow.h"
#include "content.h"

#include <QLabel>
#include <QVBoxLayout>

#include <DFontSizeManager>
DTK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : DAbstractDialog(false, parent)
    , m_title(new QLabel)
    , m_content(new Content)
{
    auto envType = qEnvironmentVariable("XDG_SESSION_TYPE");
    bool bWayland = envType.contains("wayland");
    if (bWayland) {
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    }

    m_title->setObjectName("TitleLabel");

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(20);
    layout->addWidget(m_title, 0, Qt::AlignHCenter);
    layout->addWidget(m_content);

    setLayout(layout);

    DFontSizeManager::instance()->bind(m_title, DFontSizeManager::SizeType::T5, 70);
}

MainWindow::~MainWindow()
{

}

void MainWindow::setTitle(const QString &title)
{
    m_title->setText(title);
}

void MainWindow::setCnSource(const QString &source)
{
    m_content->setCnSource(source);
}

void MainWindow::setEnSource(const QString &source)
{
    m_content->setEnSource(source);
}

void MainWindow::updateLocaleSource()
{
    m_content->updateLocaleSource();
}

void MainWindow::setAllowCheckBoxText(const QString &text)
{
    m_content->setAllowCheckBoxText(text);
}
