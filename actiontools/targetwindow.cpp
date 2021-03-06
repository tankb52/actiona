/*
    Actiona
    Copyright (C) 2005-2017 Jonathan Mercier-Ganady

    Actiona is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Actiona is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

    Contact : jmgr@jmgr.info
*/

#include "targetwindow.h"

#include <QMouseEvent>
#include <QCursor>
#include <QPainter>
#include <QMessageBox>
#include <QApplication>

#ifdef Q_OS_LINUX
#include <QX11Info>
#include <X11/Xlib.h>
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#ifdef Q_OS_LINUX
#include <X11/cursorfont.h>
#include <xcb/xcb.h>
#endif
#endif

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

namespace ActionTools
{
    TargetWindow::TargetWindow()
        : QWidget(0, Qt::Widget
                  | Qt::Window
                  | Qt::FramelessWindowHint
                  | Qt::WindowStaysOnTopHint
                  | Qt::X11BypassWindowManagerHint
                  | Qt::Tool
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                  | Qt::NoDropShadowWindowHint
                  | Qt::BypassWindowManagerHint
#endif
                  ),
          mMousePressed(false)
#ifdef Q_OS_LINUX
          , mGrabbingPointer(false)
          , mGrabbingKeyboard(false)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
          , mCrossCursor(XCreateFontCursor(QX11Info::display(), XC_crosshair))
#endif
#endif
    {
        setWindowModality(Qt::ApplicationModal);
        setAttribute(Qt::WA_TranslucentBackground);
        setMinimumSize(1, 1);
        setCursor(Qt::CrossCursor);

        connect(&mUpdateTimer, SIGNAL(timeout()), this, SLOT(update()));
    }

    TargetWindow::~TargetWindow()
    {
#ifdef Q_OS_LINUX
        if(mGrabbingPointer || mGrabbingKeyboard)
            ungrab();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        XFreeCursor(QX11Info::display(), mCrossCursor);
#endif
#endif
    }

#ifdef Q_OS_WIN
    void TargetWindow::keyPressEvent(QKeyEvent *event)
    {
        if(event->key() == Qt::Key_Escape)
            close();

        event->ignore();
    }

    void TargetWindow::mousePressEvent(QMouseEvent *event)
    {
        mMouseClickPosition = event->globalPos();
        mMousePressed = true;
    }

    void TargetWindow::mouseReleaseEvent(QMouseEvent *event)
    {
        Q_UNUSED(event)

        mMousePressed = false;

        mouseButtonReleased();
        close();
    }
#endif

    void TargetWindow::paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event)

        QPainter painter(this);

#ifdef Q_OS_LINUX
        if(mMousePressed)
            painter.fillRect(0, 0, width(), height(), QBrush(Qt::black));
#endif

#ifdef Q_OS_WIN
        if(!mMousePressed)
        {
            painter.fillRect(0, 0, width() - 1, height() - 1, QBrush(QColor(0, 0, 0, 1)));

            return;
        }

        painter.fillRect(0, 0, width() - 1, height() - 1, QBrush(QColor(0, 0, 255, 32)));

        painter.setPen(QColor(Qt::gray));
        painter.drawRect(0, 0, width() - 1, height() - 1);

        painter.setBrush(Qt::SolidPattern);
        painter.setPen(QColor(Qt::black));

        QFont font = painter.font();
        font.setPointSize(18);
        painter.setFont(font);

        painter.drawText(rect(), Qt::AlignCenter, QString::number(width()) + " x " + QString::number(height()));
#endif
    }

    void TargetWindow::showEvent(QShowEvent *event)
    {
        Q_UNUSED(event)

#ifdef Q_OS_WIN
        resize(100, 100);
#endif
#ifdef Q_OS_LINUX
        resize(1, 1);
#endif

        mUpdateTimer.start(1);

        mMousePressed = false;
        mResult = QRect();

#ifdef Q_OS_LINUX
        QCursor newCursor(Qt::CrossCursor);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QCoreApplication::instance()->installNativeEventFilter(this);
#else
        nativeEventFilteringApp->installNativeEventFilter(this);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if(XGrabPointer(QX11Info::display(), DefaultRootWindow(QX11Info::display()), True, ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync,
                        None, mCrossCursor, CurrentTime) != GrabSuccess)
        {
            QMessageBox::warning(this, tr("Choose a screen rectangle"), tr("Unable to grab the pointer."));
            event->ignore();
        }
#else
        if(XGrabPointer(QX11Info::display(), DefaultRootWindow(QX11Info::display()), True, ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync,
                        None, newCursor.handle(), CurrentTime) != GrabSuccess)
        {
            QMessageBox::warning(this, tr("Choose a screen rectangle"), tr("Unable to grab the pointer."));
            event->ignore();
        }
#endif

        mGrabbingPointer = true;

        if(XGrabKeyboard(QX11Info::display(), DefaultRootWindow(QX11Info::display()), True, GrabModeAsync, GrabModeAsync, CurrentTime) != GrabSuccess)
        {
            QMessageBox::warning(this, tr("Choose a screen rectangle"), tr("Unable to grab the pointer."));
            event->ignore();
        }

        mGrabbingKeyboard = true;
#endif
    }

    void TargetWindow::hideEvent(QHideEvent *event)
    {
        Q_UNUSED(event)

        mUpdateTimer.stop();

#ifdef Q_OS_LINUX
        ungrab();
#endif

        emit rectangleSelected(mResult);
    }

    void TargetWindow::update()
    {
#ifdef Q_OS_LINUX
        if(mMousePressed)
            setMask(QRegion(rect()).subtracted(QRegion(QRect(2, 2, width() - 4, height() - 4))));
#endif

#ifdef Q_OS_WIN
        if(GetKeyState(VK_ESCAPE) < 0)
            close();
#endif

        if(mMousePressed)
        {
            QPoint cursorPosition = QCursor::pos();
            QPoint topLeft(qMin(mMouseClickPosition.x(), cursorPosition.x()),
                           qMin(mMouseClickPosition.y(), cursorPosition.y()));
            QPoint bottomRight(qMax(mMouseClickPosition.x(), cursorPosition.x()),
                           qMax(mMouseClickPosition.y(), cursorPosition.y()));
            QSize size(bottomRight.x() - topLeft.x(),
                       bottomRight.y() - topLeft.y());

            setGeometry(QRect(topLeft, size));
        }
        else
            move(QCursor::pos() - QPoint(width() / 2, height() / 2));
    }

#ifdef Q_OS_LINUX
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    bool TargetWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *)
    {
        if(eventType == "xcb_generic_event_t")
        {
            xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);

            switch(event->response_type)
            {
            case XCB_BUTTON_PRESS:
                mMouseClickPosition = QCursor::pos();
                mMousePressed = true;

                return true;
            case XCB_BUTTON_RELEASE:
                mMousePressed = false;

                mouseButtonReleased();
                close();

                return true;
            case XCB_KEY_PRESS:
            {
                xcb_key_press_event_t *keyPressEvent = reinterpret_cast<xcb_key_press_event_t *>(event);
                if(keyPressEvent->detail == 0x09)//Escape
                {
                    close();

                    return false;
                }

                return true;
            }
            }

            return false;
        }

        return false;
    }
#else
    bool TargetWindow::x11EventFilter(XEvent *event)
    {
        switch(event->type)
        {
        case ButtonPress:
            mMouseClickPosition = QCursor::pos();
            mMousePressed = true;

            return true;
        case ButtonRelease:
            mMousePressed = false;

            mouseButtonReleased();
            close();

            return true;
        case KeyPress:
            if(event->xkey.keycode == 0x09)//Escape
            {
                close();

                return false;
            }

            return true;
        }

        return false;
    }
#endif
#endif

#ifdef Q_OS_LINUX
    void TargetWindow::ungrab()
    {
        if(mGrabbingKeyboard)
            XUngrabKeyboard(QX11Info::display(), CurrentTime);

        if(mGrabbingPointer)
            XUngrabPointer(QX11Info::display(), CurrentTime);

        if(mGrabbingKeyboard || mGrabbingPointer)
            XFlush(QX11Info::display());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QCoreApplication::instance()->removeNativeEventFilter(this);
#else
        nativeEventFilteringApp->removeNativeEventFilter(this);
#endif
        mGrabbingPointer = false;
        mGrabbingKeyboard = false;
    }
#endif

    void TargetWindow::mouseButtonReleased()
    {
        if(width() < 1 || height() < 1)
            return;

        mResult = QRect(pos(), size());
    }
}
