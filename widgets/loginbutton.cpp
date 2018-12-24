#include "loginbutton.h"

#include <QHBoxLayout>
#include <QLabel>
#include <DHiDPIHelper>
#include <QPainter>

DWIDGET_USE_NAMESPACE

LoginButton::LoginButton(QWidget *parent)
    : QWidget(parent)
    , m_text(new QLabel)
    , m_icon(new QLabel)
{
    setObjectName("LoginButton");

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addStretch();
    layout->addWidget(m_text);
    layout->addSpacing(20);
    layout->addWidget(m_icon);
    layout->addStretch();

    m_icon->setFixedSize(18, 12);

    setLayout(layout);
    setIcon(":/img/nopassword_login_normal.svg");
    m_text->setStyleSheet("font-size: 12px; color: white;");
}

void LoginButton::setText(const QString &text)
{
    m_text->setText(text);
    m_text->adjustSize();
}

void LoginButton::setIcon(const QString &icon)
{
    m_icon->setPixmap(DHiDPIHelper::loadNxPixmap(icon));
    emit iconChanged(icon);
}

void LoginButton::enterEvent(QEvent *event)
{
    setIcon(":/img/nopassword_login_hover.svg");
    return QWidget::enterEvent(event);
}

void LoginButton::leaveEvent(QEvent *event)
{
    setIcon(":/img/nopassword_login_normal.svg");
    return QWidget::leaveEvent(event);
}

void LoginButton::mousePressEvent(QMouseEvent *event)
{
    setIcon(":/img/nopassword_login_press.svg");
    return QWidget::mousePressEvent(event);
}

void LoginButton::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked();
    setIcon(":/img/nopassword_login_normal.svg");
    return QWidget::mouseReleaseEvent(event);
}

void LoginButton::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 0.1 * 255));
    painter.drawRoundedRect(rect(), 5, 5);
}