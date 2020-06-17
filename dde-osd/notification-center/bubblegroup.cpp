/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     chenwei <chenwei_cm@deepin.com>
 *
 * Maintainer: chenwei <chenwei_cm@deepin.com>
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
#include "bubblegroup.h"
#include "bubbleitem.h"
#include "notifymodel.h"
#include "bubbledelegate.h"
#include "shortcutmanage.h"
#include "appgroupmodel.h"
#include "notification/notificationentity.h"
#include "notification/constants.h"
#include "notifylistview.h"

#include <QBoxLayout>
#include <QKeyEvent>
#include <QDebug>

#include <DFontSizeManager>
#include <DGuiApplicationHelper>
#include <DWindowManagerHelper>
#include <DIconButton>

DWIDGET_USE_NAMESPACE

BubbleGroup::BubbleGroup(QWidget *parent, std::shared_ptr<NotifyModel> model)
    : QWidget(parent)
    , m_notifyModel(model)
{
    m_titleWidget = new QWidget();
    m_titleWidget->setFixedSize(OSD::BubbleWidth(OSD::ShowStyle::BUBBLEWIDGET), Notify::GroupTitleHeight);

    group_title = new DLabel;
    group_title->setForegroundRole(QPalette::BrightText);
    group_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QFont font;
    font.setBold(true);
    group_title->setFont(DFontSizeManager::instance()->t4(font));

    title_close = new DIconButton(DStyle::SP_CloseButton);
    title_close->setFlat(true);
    title_close->setIconSize(QSize(Notify::GroupButtonSize, Notify::GroupButtonSize));
    title_close->setFixedSize(Notify::GroupButtonSize, Notify::GroupButtonSize);
    title_close->setVisible(false);

    QHBoxLayout *head_Layout = new QHBoxLayout;
    head_Layout->setContentsMargins(10, 0, 0, 0);
    head_Layout->addWidget(group_title, Qt::AlignLeft);
    head_Layout->addStretch();
    head_Layout->addWidget(title_close, Qt::AlignRight);
    m_titleWidget->setLayout(head_Layout);

    m_groupList = new NotifyListView(this);
    m_notifyDelegate = new BubbleDelegate(this);
    m_notifyModel->setView(m_groupList);
    m_groupList->setModel(m_notifyModel.get());
    m_groupList->setItemDelegate(m_notifyDelegate);

    m_groupList->setAutoFillBackground(false);
    m_groupList->viewport()->setAutoFillBackground(false);
    m_groupList->setFrameStyle(QFrame::NoFrame);
    m_groupList->setMouseTracking(true);
    m_groupList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_groupList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_groupList->setVerticalScrollMode(QListView::ScrollPerPixel);
    m_groupList->setContentsMargins(0, 0, 0, 0);
    m_groupList->setUpdatesEnabled(true);
    m_groupList->setSelectionMode(QListView::NoSelection);
    m_groupList->setFocusPolicy(Qt::NoFocus);
    m_groupList->installEventFilter(this);

    QPalette pa = m_groupList->palette();
    pa.setColor(QPalette::Highlight, Qt::transparent);
    m_groupList->setPalette(pa);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(10);
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_titleWidget);
    mainLayout->addWidget(m_groupList);

    setLayout(mainLayout);

    connect(title_close, &DIconButton::clicked, this, [ = ] {
        m_appModel->removeGroup(m_notifyModel);
    });
    connect(m_notifyModel.get(), &NotifyModel::expandNotify, this, &BubbleGroup::expandAnimation);
    connect(m_notifyModel.get(), &NotifyModel::deleteNotify, this, &BubbleGroup::removeAnimation);
    connect(m_notifyModel.get(), &NotifyModel::appendNotify, this, &BubbleGroup::appendAnimation);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &BubbleGroup::refreshTheme);
    connect(DWindowManagerHelper::instance(), &DWindowManagerHelper::hasCompositeChanged, this, &BubbleGroup::CompositeChanged);

    refreshTheme();
    CompositeChanged();
}

BubbleGroup::~BubbleGroup()
{

}

void BubbleGroup::setParentModel(AppGroupModel *model)
{
    if (model != nullptr) {
        m_appModel = model;
    }
}

void BubbleGroup::enterEvent(QEvent *event)
{
    title_close->setVisible(true);
    QWidget::enterEvent(event);
}

void BubbleGroup::leaveEvent(QEvent *event)
{
    title_close->setVisible(false);
    QWidget::leaveEvent(event);
}

void BubbleGroup::focusInEvent(QFocusEvent *event)
{
    title_close->setVisible(true);
    QWidget::focusInEvent(event);
}

void BubbleGroup::hideEvent(QHideEvent *event)
{
    if (m_notifyModel->isExpand()) {
        m_notifyModel->collapseData();
    }
    QWidget::hideEvent(event);
}

bool BubbleGroup::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_groupList) {
        if (QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event)) {
            if (keyEvent->key() == Qt::Key_Up
                    || keyEvent->key() == Qt::Key_Down)
                return true;
        }
    }
    return false;
}

void BubbleGroup::appendAnimation()
{
    if(!m_hasComposite) {
        return;
    }
    if (!isVisible()) return;

    if (m_expandAnimation.isNull()) {
        m_expandAnimation = new ExpandAnimation(m_groupList);

        connect(m_expandAnimation, &ExpandAnimation::finished, [ = ]() {
            m_expandAnimation->hide();
            m_expandAnimation->deleteLater();
        });
    }

    // calculated animation widget height
    int need_bubble = animationNeedBubble();
    int expand_height = need_bubble * OSD::BubbleHeight(OSD::ShowStyle::BUBBLEWIDGET) + BubbleOverLapHeight;
    m_expandAnimation->setFixedSize(OSD::BubbleWidth(OSD::ShowStyle::BUBBLEWIDGET), expand_height);

    // calculated position
    m_expandAnimation->move(0, 0);
    m_expandAnimation->raise();
    m_expandAnimation->show();

    auto notifications = m_notifyModel->allNotifys();
    m_expandAnimation->appendData(notifications.mid(0, need_bubble));
    m_expandAnimation->start();
}

void BubbleGroup::removeAnimation(int index)
{
    if(!m_hasComposite) {
        return;
    }
    if (m_expandAnimation.isNull()) {
        m_expandAnimation = new ExpandAnimation(m_groupList);

        connect(m_expandAnimation, &ExpandAnimation::finished, [ = ]() {
            m_expandAnimation->hide();
            m_expandAnimation->deleteLater();
        });
    }

    // calculated animation widget height
    int need_bubble = animationNeedBubble(index);
    int expand_height = need_bubble * OSD::BubbleHeight(OSD::ShowStyle::BUBBLEWIDGET) + BubbleOverLapHeight;
    m_expandAnimation->setFixedSize(OSD::BubbleWidth(OSD::ShowStyle::BUBBLEWIDGET), expand_height);

    // calculated position
    int y = index * OSD::BubbleHeight(OSD::ShowStyle::BUBBLEWIDGET) + Notify::CenterMargin * index;
    m_expandAnimation->move(0, y);
    m_expandAnimation->raise();
    m_expandAnimation->show();

    auto notifications = m_notifyModel->allNotifys();
    m_expandAnimation->removeData(notifications.mid(index, need_bubble));
    m_expandAnimation->start();
}

void BubbleGroup::expandAnimation(int index)
{
    if(!m_hasComposite) {
        m_notifyModel->refreshContent();
        return;
    }
    if (m_expandAnimation.isNull()) {
        m_expandAnimation = new ExpandAnimation(m_groupList);

        connect(m_expandAnimation, &ExpandAnimation::finished, [ &, index]() {
            m_notifyModel->expandOver(index);
            m_expandAnimation->hide();
            m_expandAnimation->deleteLater();
            m_notifyModel->refreshContent();
        });
    }

    int need_bubble = animationNeedBubble();
    int expand_height = need_bubble * OSD::BubbleHeight(OSD::ShowStyle::BUBBLEWIDGET) + BubbleOverLapHeight;
    m_expandAnimation->setFixedSize(OSD::BubbleWidth(OSD::ShowStyle::BUBBLEWIDGET), expand_height);

    int limit_up = BubbleEntities - 1;
    int y = limit_up * OSD::BubbleHeight(OSD::ShowStyle::BUBBLEWIDGET) + Notify::CenterMargin * limit_up;
    m_expandAnimation->move(0, y);
    m_expandAnimation->raise();
    m_expandAnimation->show();

    auto notifications = m_notifyModel->allNotifys();

    need_bubble -= BubbleEntities;
    m_expandAnimation->expandData(notifications.mid(limit_up, need_bubble));
    m_expandAnimation->start();
}

int BubbleGroup::animationNeedBubble(int index)
{
    if (m_notifyModel->isExpand()) {
        int bubble_count = m_notifyModel->rowCount();
        int need_bubble =  parentWidget()->height() / OSD::BubbleHeight(OSD::ShowStyle::BUBBLEWIDGET);
        need_bubble = bubble_count < need_bubble ? bubble_count : need_bubble;

        return need_bubble;
    } else {
        return BubbleEntities - index;
    }
}

void BubbleGroup::refreshTheme()
{
    QFont font;
    font.setBold(true);
    group_title->setFont(DFontSizeManager::instance()->t4(font));

    QPalette pa = group_title->palette();
    pa.setBrush(QPalette::WindowText, pa.brightText());
    group_title->setPalette(pa);
}

void BubbleGroup::CompositeChanged()
{
    m_hasComposite = DWindowManagerHelper::instance()->hasComposite();
}
