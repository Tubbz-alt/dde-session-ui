/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author: quezhiyong@uniontech.com
 *         wangwei@uniontech.com
 *
 * Maintainer: quezhiyong@uniontech.com
 *             wangwei@uniontech.com
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

#ifndef TOUCHSCREENSETTING_H
#define TOUCHSCREENSETTING_H

#include <com_deepin_daemon_display.h>

#include <DDialog>
#include <DComboBox>

DWIDGET_USE_NAMESPACE

using namespace com::deepin::daemon;

class RecognizeDialog;

class TouchscreenSetting : public DDialog
{
    Q_OBJECT
public:
    TouchscreenSetting(const QString &touchscreen, QWidget *parent = nullptr);

private Q_SLOTS:
    void onMonitorChanged();
    void onButtonClicked(int index, const QString &text);

Q_SIGNALS:
    void requestAssociateTouch(const QString &touchscreenSerial, const QString &monitor);

private:
    Display *m_displayInter;
    RecognizeDialog *m_recognize;
    QString m_touchscreenSerial;
    DComboBox *m_listCombo;
};

#endif // TOUCHSCREENSETTING_H
