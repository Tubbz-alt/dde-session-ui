/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "notifycenterwidget.h"
#include "notification/persistence.h"
#include "notification/constants.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QBoxLayout>
#include <QDBusInterface>
#include <QPropertyAnimation>
#include <diconbutton.h>
#include <QPalette>
#include <DTipLabel>
#include <DGuiApplicationHelper>

DWIDGET_USE_NAMESPACE

NotifyCenterWidget::NotifyCenterWidget(Persistence *database)
    : m_notifyWidget(new NotifyWidget(this, database))
{
    initUI();
    initAnimations();
}

void NotifyCenterWidget::initUI()
{
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    m_headWidget = new QWidget;
    m_headWidget->setFixedSize(Notify::CenterWidth - 2 * Notify::CenterMargin, 32);

    DIconButton *bell_notify = new DIconButton(m_headWidget);
    bell_notify->setFlat(true);
    bell_notify->setIconSize(QSize(Notify::CenterTitleHeight, Notify::CenterTitleHeight));
    bell_notify->setFixedSize(Notify::CenterTitleHeight, Notify::CenterTitleHeight);
    const auto ratio = devicePixelRatioF();
    QIcon icon_pix = QIcon::fromTheme("://icons/notifications.svg").pixmap(bell_notify->iconSize() * ratio);
    bell_notify->setIcon(icon_pix);

    title_label = new DTipLabel("");
    title_label->setText(tr("Notification Center"));
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setForegroundRole(DPalette::TextTitle);

    DIconButton *close_btn = new DIconButton(DStyle::SP_CloseButton);
    close_btn->setFlat(true);
    close_btn->setIconSize(QSize(Notify::CenterTitleHeight, Notify::CenterTitleHeight));
    close_btn->setFixedSize(Notify::CenterTitleHeight, Notify::CenterTitleHeight);

    QHBoxLayout *head_Layout = new QHBoxLayout;
    head_Layout->addWidget(bell_notify, Qt::AlignLeft);
    head_Layout->setMargin(0);
    head_Layout->addStretch();
    head_Layout->addWidget(title_label, Qt::AlignCenter);
    head_Layout->addStretch();
    head_Layout->addWidget(close_btn, Qt::AlignRight);
    m_headWidget->setLayout(head_Layout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(m_headWidget);
    mainLayout->addWidget(m_notifyWidget);

    setLayout(mainLayout);

    connect(close_btn, &DIconButton::clicked, this, [ = ]() {
        m_outAnimation->start();
    });
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &NotifyCenterWidget::refreshTheme);
    refreshTheme();
}

void NotifyCenterWidget::initAnimations()
{
    m_inAnimation = new QPropertyAnimation(this, "pos", this);
    m_inAnimation->setDuration(500);
    m_inAnimation->setEasingCurve(QEasingCurve::OutCirc);

    m_outAnimation = new QPropertyAnimation(this, "pos", this);
    m_outAnimation->setDuration(500);
    m_outAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_outAnimation, &QPropertyAnimation::finished, [this] {hide();});
}

void NotifyCenterWidget::updateGeometry(OSD::DockPosition pos, int dock_size)
{
    QDesktopWidget *desktop = QApplication::desktop();
    QRect rect = desktop->screenGeometry();
    m_screenGeometry = rect;

    int width = Notify::CenterWidth;
    int height = rect.height() - Notify::CenterMargin * 2;
    if (pos == OSD::DockPosition::Top || pos == OSD::DockPosition::Bottom)
        height = rect.height() - Notify::CenterMargin * 2 - dock_size;

    int x = rect.width() - (Notify::CenterWidth + Notify::CenterMargin);
    if (pos == OSD::DockPosition::Right)
        x = Notify::CenterMargin;

    int y = rect.y() + Notify::CenterMargin;
    if (pos == OSD::DockPosition::Top)
        y = rect.y() + Notify::CenterMargin + dock_size;

    if (m_inAnimation->state() != QPropertyAnimation::Running) {
        if (pos == OSD::DockPosition::Right) {
            m_inAnimation->setStartValue(QPoint(-Notify::CenterWidth, y));
            m_inAnimation->setEndValue(QPoint(x, y));
        } else {
            m_inAnimation->setStartValue(QPoint(rect.width(), y));
            m_inAnimation->setEndValue(QPoint(x, y));
        }
    }

    if (m_outAnimation->state() != QPropertyAnimation::Running) {
        if (pos == OSD::DockPosition::Right) {
            m_outAnimation->setStartValue(QPoint(x, y));
            m_outAnimation->setEndValue(QPoint(-Notify::CenterWidth, y));
        } else {
            m_outAnimation->setStartValue(QPoint(x, y));
            m_outAnimation->setEndValue(QPoint(rect.width(), y));
        }
    }

    setGeometry(x, y, width, height);
}

void NotifyCenterWidget::showEvent(QShowEvent *event)
{
    m_inAnimation->start();
    DBlurEffectWidget::showEvent(event);
}


void NotifyCenterWidget::refreshTheme()
{
    QPalette pa = title_label->palette();
    pa.setBrush(QPalette::WindowText, pa.brightText());
    title_label->setPalette(pa);

    QFont font;
    font.setBold(true);
    title_label->setFont(DFontSizeManager::instance()->t4(font));
}

void NotifyCenterWidget::showWidget()
{
    if (!isVisible()) {
        show();
    } else {
        if (m_screenGeometry.contains(pos()))
            m_outAnimation->start();
        else
            m_inAnimation->start();
    }
}
