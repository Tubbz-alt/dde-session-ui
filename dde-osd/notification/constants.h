#ifndef DEFINE_H
#define DEFINE_H

#include <QSize>
#include <QStringList>
#include <QStandardPaths>

static const int ScreenPadding = 46;    //最上方通知距屏幕上方间隔
static const int BubbleMargin = 12;     //桌面消息通知间隔
static const int BubblePadding = 10;    //消息通知内部Padding
static const int BubbleSpacing = 10;    //消息通知内部Space
static const int BubbleTimeout = 5000;  //通知默认超时时间(毫秒)
static const int BubbleEntities = 3;
static const int BubbleOverLap = 2;     //层叠的气泡数量
static const int BubbleOverLapHeight = 12;  //通知中心层叠层高度
static const QStringList Directory = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
static const QString CachePath = Directory.first() + "/.cache/deepin/deepin-notifications/";

namespace Notify {
static const int CenterWidth = 400;
static const int CenterMargin = 10;
static const int CenterTitleHeight = 32;

static const int GroupTitleHeight = 32;
static const int GroupButtonSize = 24;
static const int GroupMargin = 30;
}

class OSD
{
public:
    typedef enum {
        BUBBLEWINDOW,
        BUBBLEWIDGET
    } ShowStyle;

    enum DockPosition {
        Top = 0,
        Right = 1,
        Bottom = 2,
        Left = 3
    };

    static QSize BubbleSize(ShowStyle style)
    {
        QSize size;
        if (style == BUBBLEWINDOW) {
            size = QSize(600, 60);
        } else if (style == BUBBLEWIDGET) {
            size = QSize(380, 90);
        }
        return size;
    }

    static int BubbleWidth(ShowStyle style)
    {
        return BubbleSize(style).width();
    }

    static int BubbleHeight(ShowStyle style)
    {
        return BubbleSize(style).height();
    }

    static QSize ButtonSize(ShowStyle style)
    {
        QSize size;
        if (style == BUBBLEWINDOW) {
            size = QSize(70, 40);
        } else if (style == BUBBLEWIDGET) {
            size = QSize(60, 36);
        }
        return size;
    }

    static QSize IconButtonSize(ShowStyle style)
    {
        QSize size;
        if (style == BUBBLEWINDOW) {
            size = QSize(36, 36);
        } else if (style == BUBBLEWIDGET) {
            size = QSize(36, 36);
        }
        return size;
    }

    static QSize IconSize(ShowStyle style)
    {
        QSize size;
        if (style == BUBBLEWINDOW) {
            size = QSize(40, 40);
        } else if (style == BUBBLEWIDGET) {
            size = QSize(20, 20);
        }
        return size;
    }

    static QSize CloseButtonSize(ShowStyle style)
    {
        QSize size;
        if (style == BUBBLEWINDOW) {
            size = QSize(30, 30);
        } else if (style == BUBBLEWIDGET) {
            size = QSize(20, 20);
        }
        return size;
    }
};
#endif // DEFINE_H
