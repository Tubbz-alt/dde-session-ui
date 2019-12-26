/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     zorowk <near.kingzero@gmail.com>
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

#ifndef SHORTCUTMANAGE_H
#define SHORTCUTMANAGE_H

#include <QObject>
#include <QModelIndex>
#include <memory>
#include <QWidget>
#include <QPointer>

class QKeyEvent;
class QMouseEvent;
class QFocusEvent;
class QEnterEvent;
class AppGroupModel;
class ApplicationGroup;

class ShortcutManage : public QObject
{
    Q_OBJECT
public:
    static ShortcutManage *instance(QObject *parent = nullptr);
    void setAppModel(AppGroupModel *model);
    void initIndex();

public Q_SLOTS:
    void onGroupIndexChanged(const QModelIndex &groupIndex);
    void onGroupIndexChanged_(const QModelIndex &groupIndex, const QModelIndex &index);
    void onViewIndexChanged(const QModelIndex &index);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    explicit ShortcutManage(QObject *parent = nullptr);
    bool handKeyEvent(QObject *object, QKeyEvent *event);
    bool handPressEvent(QObject *object);
    bool handBubbleTab(QWidget *item);
    bool calcNextBubbleIndex();
    bool calcNextGroupIndex();

private:
    static ShortcutManage *m_instance;
    AppGroupModel *m_appModel = nullptr;
    QModelIndex m_currentGroupIndex;
    QModelIndex m_currentIndex;
    QPointer<QWidget> m_currentElement = nullptr;
};

#endif // SHORTCUTMANAGE_H
