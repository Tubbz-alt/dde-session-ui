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

#include "recognizedialog.h"

#include <com_deepin_daemon_display_monitor.h>

#include <QTimer>
#include <QPainter>
#include <QScreen>
#include <QApplication>

using namespace com::deepin::daemon::display;

const int AUTOHIDE_DELAY = 1000 * 5;
const int HORIZENTAL_MARGIN = 30;
const int VERTICAL_MARGIN = 20;
const int FONT_SIZE = 20;
const int RADIUS = 5;
const char SERVICE_PATH[] = "com.deepin.daemon.Display";

RecognizeDialog::RecognizeDialog(Display *display, QWidget *parent)
    : QDialog(parent)
    , m_displayInter(display)
    , m_backgroundPen(QColor(255, 255, 255, 40))
    , m_textPen(QColor(255, 255, 255))
    , m_backgroundBrush(QColor(0, 0, 0, 200), Qt::SolidPattern)
    , m_textBrush(QColor(255, 255, 255), Qt::SolidPattern)
{
    connect(m_displayInter, &Display::ScreenHeightChanged, this, &RecognizeDialog::onScreenRectChanged);
    connect(m_displayInter, &Display::ScreenWidthChanged, this, &RecognizeDialog::onScreenRectChanged);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::NonModal);

    m_backgroundPen.setWidth(1);
    m_textPen.setWidth(1);

    onScreenRectChanged();

    // set auto hide timer
    QTimer *autoHideTimer = new QTimer(this);
    autoHideTimer->setInterval(AUTOHIDE_DELAY);
    connect(autoHideTimer, &QTimer::timeout, this, &RecognizeDialog::accept);
    autoHideTimer->start();
}

bool RecognizeDialog::monitorsIsIntersect() const
{
    auto monitorPathList = m_displayInter->monitors();

    if (monitorPathList.size() < 2) return false;

    // only support 2 screens
    Q_ASSERT(monitorPathList.size() == 2);

    auto first = monitorPathList.first();
    Monitor firstMonitor(SERVICE_PATH, first.path(), QDBusConnection::sessionBus());
    QRect firstRect(firstMonitor.x(), firstMonitor.y(), firstMonitor.width(), firstMonitor.height());

    auto last = monitorPathList.last();
    Monitor lastMonitor(SERVICE_PATH, last.path(), QDBusConnection::sessionBus());
    QRect lastRect(lastMonitor.x(), lastMonitor.y(), lastMonitor.width(), lastMonitor.height());

    return firstRect.intersects(lastRect);
}

void RecognizeDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto touchMap = m_displayInter->touchMap();
    auto monitorPathList = m_displayInter->monitors();
    if (!monitorsIsIntersect()) {
        for (auto m : monitorPathList) {
            Monitor monitor(SERVICE_PATH, m.path(), QDBusConnection::sessionBus());
            QString monitorName = monitor.name();

            QStringList touchscreens;
            for (auto i = touchMap.cbegin(); i != touchMap.cend(); ++i) {
                if (i.value() == monitorName) {
                    touchscreens << i.key();
                }
            }

            paintMonitorMark(painter,
                             QRect(monitor.x(), monitor.y(), monitor.width(), monitor.height()),
                             monitorName,
                             touchscreens);
        }
    }
}

void RecognizeDialog::onScreenRectChanged()
{
    const auto ratio = devicePixelRatioF();

    QRect r(0, 0, m_displayInter->screenWidth(), m_displayInter->screenHeight());

    const QScreen *screen = screenForGeometry(r);

    if (screen) {
        const QRect screenGeo = screen->geometry();
        r.moveTopLeft(screenGeo.topLeft() + (r.topLeft() - screenGeo.topLeft()) / ratio);
    }

    setGeometry(r);

    update();
}

void RecognizeDialog::paintMonitorMark(QPainter &painter,
                                       const QRect &rect,
                                       const QString &name,
                                       const QStringList &touchscreens)
{
    int line = touchscreens.size() + 1;

    const qreal ratio = devicePixelRatioF();
    const QRect r(rect.topLeft() / ratio, rect.size() / ratio);

    QFont font;
    font.setPixelSize(FONT_SIZE);
    const QFontMetrics fm(font);

    int containerWidth = fm.width(name);
    int y = r.center().y() - fm.height() * (line / 2) + fm.height() / 4;

    QPainterPath path;
    for (auto touchName : touchscreens) {
        int width = fm.width(touchName);
        if (width > containerWidth) containerWidth = width;

        int x = r.center().x() - width / 2;
        path.addText(x, y, font, touchName);
        y += fm.height();
    }

    int x = r.center().x() - fm.width(name) / 2;
    path.addText(x, y, font, name);

    painter.setPen(m_backgroundPen);
    painter.setBrush(m_backgroundBrush);

    const int rectX = r.center().x() - containerWidth / 2 - HORIZENTAL_MARGIN;
    const int rectY = r.center().y() - fm.height() * line / 2 - VERTICAL_MARGIN;
    QRect rec(rectX, rectY, containerWidth + HORIZENTAL_MARGIN * 2, fm.height() * line + VERTICAL_MARGIN * 2);
    painter.drawRoundedRect(rec, RADIUS, RADIUS);

    painter.setPen(m_textPen);
    painter.setBrush(m_textBrush);

    painter.drawPath(path);
}

const QScreen *RecognizeDialog::screenForGeometry(const QRect &rect) const
{
    const qreal ratio = qApp->devicePixelRatio();

    for (const auto *s : qApp->screens()) {
        const QRect &g(s->geometry());
        const QRect realRect(g.topLeft() / ratio, g.size());

        if (realRect.contains(rect.center())) return s;
    }

    return nullptr;
}
