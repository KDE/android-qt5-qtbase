/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwidgetwindow_qpa_p.h"

#include "private/qwidget_p.h"
#include "private/qapplication_p.h"
#ifndef QT_NO_ACCESSIBILITY
#include <QtGui/qaccessible.h>
#endif
#include <private/qwidgetbackingstore_p.h>

QT_BEGIN_NAMESPACE

QWidget *qt_button_down = 0; // widget got last button-down
static QWidget *qt_tablet_target = 0;

// popup control
QWidget *qt_popup_down = 0; // popup that contains the pressed widget
extern int openPopupCount;
bool qt_replay_popup_mouse_event = false;
extern bool qt_try_modal(QWidget *widget, QEvent::Type type);

QWidgetWindow::QWidgetWindow(QWidget *widget)
    : m_widget(widget)
{
    updateObjectName();
    connect(m_widget, &QObject::objectNameChanged, this, &QWidgetWindow::updateObjectName);
}

#ifndef QT_NO_ACCESSIBILITY
QAccessibleInterface *QWidgetWindow::accessibleRoot() const
{
    if (m_widget)
        return QAccessible::queryAccessibleInterface(m_widget);
    return 0;
}
#endif

QObject *QWidgetWindow::focusObject() const
{
    QWidget *widget = m_widget->focusWidget();

    if (!widget)
        widget = m_widget;

    return widget;
}

bool QWidgetWindow::event(QEvent *event)
{
    if (m_widget->testAttribute(Qt::WA_DontShowOnScreen)) {
        // \a event is uninteresting for QWidgetWindow, the event was probably
        // generated before WA_DontShowOnScreen was set
        return m_widget->event(event);
    }

    switch (event->type()) {
    case QEvent::Close:
        handleCloseEvent(static_cast<QCloseEvent *>(event));
        return true;

    case QEvent::Enter:
    case QEvent::Leave:
        handleEnterLeaveEvent(event);
        return true;

    // these should not be sent to QWidget, the corresponding events
    // are sent by QApplicationPrivate::notifyActiveWindowChange()
    case QEvent::FocusIn:
    case QEvent::FocusOut: {
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::State state;
        state.active = true;
        QAccessibleStateChangeEvent ev(widget(), state);
        QAccessible::updateAccessibility(&ev);
#endif
        return false; }

    case QEvent::FocusAboutToChange:
        if (QApplicationPrivate::focus_widget) {
            if (QApplicationPrivate::focus_widget->testAttribute(Qt::WA_InputMethodEnabled))
                qApp->inputMethod()->commit();

            QGuiApplication::sendSpontaneousEvent(QApplicationPrivate::focus_widget, event);
        }
        return true;

    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::ShortcutOverride:
        handleKeyEvent(static_cast<QKeyEvent *>(event));
        return true;

    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
        handleMouseEvent(static_cast<QMouseEvent *>(event));
        return true;

    case QEvent::NonClientAreaMouseMove:
    case QEvent::NonClientAreaMouseButtonPress:
    case QEvent::NonClientAreaMouseButtonRelease:
    case QEvent::NonClientAreaMouseButtonDblClick:
        handleNonClientAreaMouseEvent(static_cast<QMouseEvent *>(event));
        return true;

    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    case QEvent::TouchCancel:
        handleTouchEvent(static_cast<QTouchEvent *>(event));
        return true;

    case QEvent::Move:
        handleMoveEvent(static_cast<QMoveEvent *>(event));
        return true;

    case QEvent::Resize:
        handleResizeEvent(static_cast<QResizeEvent *>(event));
        return true;

#ifndef QT_NO_WHEELEVENT
    case QEvent::Wheel:
        handleWheelEvent(static_cast<QWheelEvent *>(event));
        return true;
#endif

#ifndef QT_NO_DRAGANDDROP
    case QEvent::DragEnter:
    case QEvent::DragMove:
        handleDragEnterMoveEvent(static_cast<QDragMoveEvent *>(event));
        return true;
    case QEvent::DragLeave:
        handleDragLeaveEvent(static_cast<QDragLeaveEvent *>(event));
        return true;
    case QEvent::Drop:
        handleDropEvent(static_cast<QDropEvent *>(event));
        return true;
#endif

    case QEvent::Expose:
        handleExposeEvent(static_cast<QExposeEvent *>(event));
        return true;

    case QEvent::WindowStateChange:
        handleWindowStateChangedEvent(static_cast<QWindowStateChangeEvent *>(event));
        return true;

    case QEvent::ThemeChange: {
        QEvent widgetEvent(QEvent::ThemeChange);
        QGuiApplication::sendSpontaneousEvent(m_widget, &widgetEvent);
    }
        return true;

#ifndef QT_NO_TABLETEVENT
    case QEvent::TabletPress:
    case QEvent::TabletMove:
    case QEvent::TabletRelease:
        handleTabletEvent(static_cast<QTabletEvent *>(event));
        return true;
#endif

    default:
        break;
    }

    return m_widget->event(event) || QWindow::event(event);
}

QPointer<QWidget> qt_last_mouse_receiver = 0;

void QWidgetWindow::handleEnterLeaveEvent(QEvent *event)
{
    if (event->type() == QEvent::Leave) {
        QWidget *leave = qt_last_mouse_receiver ? qt_last_mouse_receiver.data() : m_widget;
        QApplicationPrivate::dispatchEnterLeave(0, leave);
        qt_last_mouse_receiver = 0;
    } else {
        QApplicationPrivate::dispatchEnterLeave(m_widget, 0);
        qt_last_mouse_receiver = m_widget;
    }
}

void QWidgetWindow::handleNonClientAreaMouseEvent(QMouseEvent *e)
{
    QApplication::sendSpontaneousEvent(m_widget, e);
}

void QWidgetWindow::handleMouseEvent(QMouseEvent *event)
{
    if (qApp->d_func()->inPopupMode()) {
        QWidget *activePopupWidget = qApp->activePopupWidget();
        QWidget *popup = activePopupWidget;
        QPoint mapped = event->pos();
        if (popup != m_widget)
            mapped = popup->mapFromGlobal(event->globalPos());
        bool releaseAfter = false;
        QWidget *popupChild  = popup->childAt(mapped);

        if (popup != qt_popup_down) {
            qt_button_down = 0;
            qt_popup_down = 0;
        }

        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick:
            qt_button_down = popupChild;
            qt_popup_down = popup;
            break;
        case QEvent::MouseButtonRelease:
            releaseAfter = true;
            break;
        default:
            break; // nothing for mouse move
        }

        int oldOpenPopupCount = openPopupCount;

        if (popup->isEnabled()) {
            // deliver event
            qt_replay_popup_mouse_event = false;
            QWidget *receiver = popup;
            QPoint widgetPos = mapped;
            if (qt_button_down)
                receiver = qt_button_down;
            else if (popupChild)
                receiver = popupChild;
            if (receiver != popup)
                widgetPos = receiver->mapFromGlobal(event->globalPos());
            QWidget *alien = m_widget->childAt(m_widget->mapFromGlobal(event->globalPos()));
            QMouseEvent e(event->type(), widgetPos, event->windowPos(), event->screenPos(), event->button(), event->buttons(), event->modifiers());
            e.setTimestamp(event->timestamp());
            QApplicationPrivate::sendMouseEvent(receiver, &e, alien, m_widget, &qt_button_down, qt_last_mouse_receiver);
        } else {
            // close disabled popups when a mouse button is pressed or released
            switch (event->type()) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonDblClick:
            case QEvent::MouseButtonRelease:
                popup->close();
                break;
            default:
                break;
            }
        }

        if (qApp->activePopupWidget() != activePopupWidget
            && qt_replay_popup_mouse_event) {
            if (m_widget->windowType() != Qt::Popup)
                qt_button_down = 0;
            qt_replay_popup_mouse_event = false;
#ifndef QT_NO_CONTEXTMENU
        } else if (event->type() == QEvent::MouseButtonPress
                   && event->button() == Qt::RightButton
                   && (openPopupCount == oldOpenPopupCount)) {
            QWidget *popupEvent = popup;
            if (qt_button_down)
                popupEvent = qt_button_down;
            else if(popupChild)
                popupEvent = popupChild;
            QContextMenuEvent e(QContextMenuEvent::Mouse, mapped, event->globalPos(), event->modifiers());
            QApplication::sendSpontaneousEvent(popupEvent, &e);
#endif
        }

        if (releaseAfter) {
            qt_button_down = 0;
            qt_popup_down = 0;
        }
        return;
    }

    // modal event handling
    if (QApplicationPrivate::instance()->modalState() && !qt_try_modal(m_widget, event->type()))
        return;

    // which child should have it?
    QWidget *widget = m_widget->childAt(event->pos());
    QPoint mapped = event->pos();

    if (!widget)
        widget = m_widget;

    if (event->type() == QEvent::MouseButtonPress && !qt_button_down)
        qt_button_down = widget;

    QWidget *receiver = QApplicationPrivate::pickMouseReceiver(m_widget, event->windowPos().toPoint(), &mapped, event->type(), event->buttons(),
                                                               qt_button_down, widget);

    if (!receiver) {
        if (event->type() == QEvent::MouseButtonRelease)
            QApplicationPrivate::mouse_buttons &= ~event->button();
        return;
    }

    QMouseEvent translated(event->type(), mapped, event->windowPos(), event->screenPos(), event->button(), event->buttons(), event->modifiers());
    translated.setTimestamp(event->timestamp());
    QApplicationPrivate::sendMouseEvent(receiver, &translated, widget, m_widget, &qt_button_down,
                                        qt_last_mouse_receiver);

#ifndef QT_NO_CONTEXTMENU
    if (event->type() == QEvent::MouseButtonPress && event->button() == Qt::RightButton) {
        QContextMenuEvent e(QContextMenuEvent::Mouse, mapped, event->globalPos(), event->modifiers());
        QGuiApplication::sendSpontaneousEvent(receiver, &e);
    }
#endif
}

void QWidgetWindow::handleTouchEvent(QTouchEvent *event)
{
    if (event->type() == QEvent::TouchCancel)
        QApplicationPrivate::translateTouchCancel(event->device(), event->timestamp());
    else
        QApplicationPrivate::translateRawTouchEvent(m_widget, event->device(), event->touchPoints(), event->timestamp());
}

void QWidgetWindow::handleKeyEvent(QKeyEvent *event)
{
    if (QApplicationPrivate::instance()->modalState() && !qt_try_modal(m_widget, event->type()))
        return;

    QObject *receiver = 0;
    if (QApplicationPrivate::inPopupMode()) {
        QWidget *popup = QApplication::activePopupWidget();
        QWidget *popupFocusWidget = popup->focusWidget();
        receiver = popupFocusWidget ? popupFocusWidget : popup;
    }
    if (!receiver)
        receiver = focusObject();
    QGuiApplication::sendSpontaneousEvent(receiver, event);
}

void QWidgetWindow::updateGeometry()
{
    if (m_widget->testAttribute(Qt::WA_OutsideWSRange))
        return;

    const QMargins margins = frameMargins();

    m_widget->data->crect = geometry();
    QTLWExtra *te = m_widget->d_func()->topData();
    te->posIncludesFrame= false;
    te->frameStrut.setCoords(margins.left(), margins.top(), margins.right(), margins.bottom());
    m_widget->data->fstrut_dirty = false;
}

void QWidgetWindow::handleMoveEvent(QMoveEvent *event)
{
    updateGeometry();
    QGuiApplication::sendSpontaneousEvent(m_widget, event);
}

void QWidgetWindow::handleResizeEvent(QResizeEvent *event)
{
    QSize oldSize = m_widget->data->crect.size();

    updateGeometry();
    QGuiApplication::sendSpontaneousEvent(m_widget, event);

    if (m_widget->d_func()->paintOnScreen()) {
        QRegion updateRegion(geometry());
        if (m_widget->testAttribute(Qt::WA_StaticContents))
            updateRegion -= QRect(0, 0, oldSize.width(), oldSize.height());
        m_widget->d_func()->syncBackingStore(updateRegion);
    } else {
        m_widget->d_func()->syncBackingStore();
    }
}

void QWidgetWindow::handleCloseEvent(QCloseEvent *)
{
    m_widget->d_func()->close_helper(QWidgetPrivate::CloseWithSpontaneousEvent);
}

#ifndef QT_NO_WHEELEVENT

void QWidgetWindow::handleWheelEvent(QWheelEvent *event)
{
    if (QApplicationPrivate::instance()->modalState() && !qt_try_modal(m_widget, event->type()))
        return;

    // which child should have it?
    QWidget *widget = m_widget->childAt(event->pos());

    if (!widget)
        widget = m_widget;

    QPoint mapped = widget->mapFrom(m_widget, event->pos());

    QWheelEvent translated(mapped, event->globalPos(), event->pixelDelta(), event->angleDelta(), event->delta(), event->orientation(), event->buttons(), event->modifiers());
    QGuiApplication::sendSpontaneousEvent(widget, &translated);
}

#endif // QT_NO_WHEELEVENT

#ifndef QT_NO_DRAGANDDROP

void QWidgetWindow::handleDragEnterMoveEvent(QDragMoveEvent *event)
{
     Q_ASSERT(event->type() ==QEvent::DragMove || !m_dragTarget);
    // Find a target widget under mouse that accepts drops (QTBUG-22987).
    QWidget *widget = m_widget->childAt(event->pos());
    if (!widget)
        widget = m_widget;
    for ( ; widget && widget != m_widget && !widget->acceptDrops(); widget = widget->parentWidget()) ;
    if (widget && !widget->acceptDrops())
        widget = 0;
    // Target widget unchanged: DragMove
    if (widget && widget == m_dragTarget.data()) {
        Q_ASSERT(event->type() == QEvent::DragMove);
        const QPoint mapped = widget->mapFrom(m_widget, event->pos());
        QDragMoveEvent translated(mapped, event->possibleActions(), event->mimeData(), event->mouseButtons(), event->keyboardModifiers());
        translated.setDropAction(event->dropAction());
        QGuiApplication::sendSpontaneousEvent(widget, &translated);
        if (translated.isAccepted()) {
            event->accept();
        } else {
            event->ignore();
        }
        event->setDropAction(translated.dropAction());
        return;
    }
    // Target widget changed: Send DragLeave to previous, DragEnter to new if there is any
    if (m_dragTarget.data()) {
        QDragLeaveEvent le;
        QGuiApplication::sendSpontaneousEvent(m_dragTarget.data(), &le);
        m_dragTarget = 0;
    }
    if (!widget) {
         event->ignore();
         return;
    }
    m_dragTarget = widget;
    const QPoint mapped = widget->mapFrom(m_widget, event->pos());
    QDragEnterEvent translated(mapped, event->possibleActions(), event->mimeData(), event->mouseButtons(), event->keyboardModifiers());
    QGuiApplication::sendSpontaneousEvent(widget, &translated);
    if (translated.isAccepted()) {
        event->accept();
    } else {
        event->ignore();
    }
    event->setDropAction(translated.dropAction());
}

void QWidgetWindow::handleDragLeaveEvent(QDragLeaveEvent *event)
{
    if (m_dragTarget)
        QGuiApplication::sendSpontaneousEvent(m_dragTarget.data(), event);
    m_dragTarget = 0;
}

void QWidgetWindow::handleDropEvent(QDropEvent *event)
{
    const QPoint mapped = m_dragTarget.data()->mapFrom(m_widget, event->pos());
    QDropEvent translated(mapped, event->possibleActions(), event->mimeData(), event->mouseButtons(), event->keyboardModifiers());
    QGuiApplication::sendSpontaneousEvent(m_dragTarget.data(), &translated);
    if (translated.isAccepted())
        event->accept();
    event->setDropAction(translated.dropAction());
    m_dragTarget = 0;
}

#endif // QT_NO_DRAGANDDROP

void QWidgetWindow::handleExposeEvent(QExposeEvent *event)
{
    if (isExposed()) {
        m_widget->setAttribute(Qt::WA_Mapped);
        if (!event->region().isNull()) {
            // Exposed native widgets need to be marked dirty to get them repainted correctly.
            if (m_widget->internalWinId() && !m_widget->isWindow() && m_widget->isVisible() && m_widget->updatesEnabled()) {
                if (QWidgetBackingStore *bs = m_widget->d_func()->maybeBackingStore())
                    bs->markDirty(event->region(), m_widget);
            }
            m_widget->d_func()->syncBackingStore(event->region());
        }
    } else {
        m_widget->setAttribute(Qt::WA_Mapped, false);
    }
}

Qt::WindowState effectiveState(Qt::WindowStates state);

void QWidgetWindow::handleWindowStateChangedEvent(QWindowStateChangeEvent *event)
{
    // QWindow does currently not know 'active'.
    Qt::WindowStates eventState = event->oldState();
    Qt::WindowStates widgetState = m_widget->windowState();
    if (widgetState & Qt::WindowActive)
        eventState |= Qt::WindowActive;

    // Determine the new widget state, remember maximized/full screen
    // during minimized.
    switch (windowState()) {
    case Qt::WindowNoState:
        widgetState &= ~(Qt::WindowMinimized | Qt::WindowMaximized | Qt::WindowFullScreen);
        break;
    case Qt::WindowMinimized:
        widgetState |= Qt::WindowMinimized;
        break;
    case Qt::WindowMaximized:
        if (effectiveState(widgetState) == Qt::WindowNoState)
            if (QTLWExtra *tle = m_widget->d_func()->maybeTopData())
                tle->normalGeometry = m_widget->geometry();
        widgetState |= Qt::WindowMaximized;
        break;
    case Qt::WindowFullScreen:
        if (effectiveState(widgetState) == Qt::WindowNoState)
            if (QTLWExtra *tle = m_widget->d_func()->maybeTopData())
                tle->normalGeometry = m_widget->geometry();
        widgetState |= Qt::WindowFullScreen;
        break;
    case Qt::WindowActive: // Not handled by QWindow
        break;
    }

    // Sent event if the state changed (that is, it is not triggered by
    // QWidget::setWindowState(), which also sends an event to the widget).
    if (widgetState != int(m_widget->data->window_state)) {
        m_widget->data->window_state = widgetState;
        QWindowStateChangeEvent widgetEvent(eventState);
        QGuiApplication::sendSpontaneousEvent(m_widget, &widgetEvent);
    }
}

bool QWidgetWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    return m_widget->nativeEvent(eventType, message, result);
}

#ifndef QT_NO_TABLETEVENT
void QWidgetWindow::handleTabletEvent(QTabletEvent *event)
{
    if (event->type() == QEvent::TabletPress) {
        QWidget *widget = m_widget->childAt(event->pos());
        if (!widget)
            widget = m_widget;

        qt_tablet_target = widget;
    }

    if (qt_tablet_target) {
        QPointF delta = event->globalPosF() - event->globalPos();
        QPointF mapped = qt_tablet_target->mapFromGlobal(event->globalPos()) + delta;
        QTabletEvent ev(event->type(), mapped, event->globalPosF(), event->device(), event->pointerType(),
                        event->pressure(), event->xTilt(), event->yTilt(), event->tangentialPressure(),
                        event->rotation(), event->z(), event->modifiers(), event->uniqueId());
        ev.setTimestamp(event->timestamp());
        QGuiApplication::sendSpontaneousEvent(qt_tablet_target, &ev);
    }

    if (event->type() == QEvent::TabletRelease)
        qt_tablet_target = 0;
}
#endif // QT_NO_TABLETEVENT

void QWidgetWindow::updateObjectName()
{
    QString name = m_widget->objectName();
    if (name.isEmpty())
        name = QString::fromUtf8(m_widget->metaObject()->className()) + QStringLiteral("Class");
    name += QStringLiteral("Window");
    setObjectName(name);
}

QT_END_NAMESPACE
