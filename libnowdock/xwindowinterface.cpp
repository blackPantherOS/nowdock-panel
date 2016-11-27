#include "xwindowinterface.h"

#include <KWindowInfo>
#include <KWindowSystem>

namespace NowDock
{

XWindowInterface::XWindowInterface(QQuickWindow *parent) :
    AbstractInterface(parent),
    m_demandsAttention(-1)
{
    m_activeWindow = KWindowSystem::activeWindow();

    connect(KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)), this, SLOT(activeWindowChanged(WId)));
    connect(KWindowSystem::self(), SIGNAL(windowChanged (WId,NET::Properties,NET::Properties2)), this, SLOT(windowChanged (WId,NET::Properties,NET::Properties2)));
    connect(KWindowSystem::self(), SIGNAL(windowRemoved(WId)), this, SLOT(windowRemoved(WId)));
}

XWindowInterface::~XWindowInterface()
{
}

void XWindowInterface::showDockOnTop()
{
    KWindowSystem::clearState(m_dockWindow->winId(), NET::KeepBelow);
    KWindowSystem::setState(m_dockWindow->winId(), NET::KeepAbove);
}

void XWindowInterface::showDockAsNormal()
{
    //    qDebug() << "reached make normal...";
    KWindowSystem::clearState(m_dockWindow->winId(), NET::KeepAbove);
    KWindowSystem::clearState(m_dockWindow->winId(), NET::KeepBelow);
}

void XWindowInterface::showDockOnBottom()
{
    //    qDebug() << "reached make bottom...";
    KWindowSystem::clearState(m_dockWindow->winId(), NET::KeepAbove);
    KWindowSystem::setState(m_dockWindow->winId(), NET::KeepBelow);
}


bool XWindowInterface::isDesktop(WId id) const
{
    KWindowInfo info(id, NET::WMWindowType);

    if ( !info.valid() ) {
        return false;
    }

    NET::WindowType type = info.windowType(NET::DesktopMask|NET::DockMask|NET::DialogMask);

    return type == NET::Desktop;
}


bool XWindowInterface::isMaximized(WId id) const
{
    KWindowInfo info(id, NET::WMState);

    if ( !info.valid() ) {
        return false;
    }
    return ( info.hasState(NET::Max) );
}

bool XWindowInterface::isNormal(WId id) const
{
    return ( !isOnBottom(id) && !isOnTop(id) );
}

bool XWindowInterface::isOnBottom(WId id) const
{
    KWindowInfo info(id, NET::WMState);

    if ( !info.valid() ) {
        return false;
    }

    return ( info.hasState(NET::KeepBelow) );
}

bool XWindowInterface::isOnTop(WId id) const
{
    KWindowInfo info(id, NET::WMState);

    if ( !info.valid() ) {
        return false;
    }

    return ( info.hasState(NET::KeepAbove) );
}

bool XWindowInterface::activeIsMaximized() const
{
    return isMaximized(m_activeWindow);
}


bool XWindowInterface::desktopIsActive() const
{
    return isDesktop(m_activeWindow);
}

bool XWindowInterface::dockIsOnTop() const
{
    return isOnTop(m_dockWindow->winId());

}

bool XWindowInterface::dockInNormalState() const
{
    return isNormal(m_dockWindow->winId());
}

bool XWindowInterface::dockIsBelow() const
{
    return isOnBottom(m_dockWindow->winId());
}

bool XWindowInterface::dockIntersectsActiveWindow() const
{
    KWindowInfo activeInfo(m_activeWindow, NET::WMGeometry);

    if ( activeInfo.valid() ) {
        QRect maskSize;

        if ( !m_maskArea.isNull() ) {
            maskSize = QRect(m_dockWindow->x()+m_maskArea.x(), m_dockWindow->y()+m_maskArea.y(), m_maskArea.width(), m_maskArea.height());
        } else {
            maskSize = QRect(m_dockWindow->x(), m_dockWindow->y(), m_dockWindow->width(), m_dockWindow->height());
        }

        return maskSize.intersects(activeInfo.geometry());
    } else {
        return false;
    }
}


bool XWindowInterface::dockIsCovered() const
{
    int currentDockPos = -1;

    QList<WId> windows = KWindowSystem::stackingOrder();
    int size = windows.count();

    for(int i=size-1; i>=0; --i) {
        WId window = windows.at(i);
        if (window == m_dockWindow->winId()) {
            currentDockPos = i;
            break;
        }
    }

    if (currentDockPos >=0) {
        QRect maskSize;

        if ( m_maskArea.isNull() ) {
            maskSize = QRect(m_dockWindow->x()+m_maskArea.x(), m_dockWindow->y()+m_maskArea.y(), m_maskArea.width(), m_maskArea.height());
        } else {
            maskSize = QRect(m_dockWindow->x(), m_dockWindow->y(), m_dockWindow->width(), m_dockWindow->height());
        }

        WId transient;

        if (m_dockWindow->transientParent()) {
            transient = m_dockWindow->transientParent()->winId();
        }

        for(int j=size-1; j>currentDockPos; --j) {
            WId window = windows.at(j);

            KWindowInfo info(window, NET::WMState | NET::XAWMState | NET::WMGeometry);

            if ( info.valid() && !isDesktop(window) && transient!=window && !info.isMinimized() && maskSize.intersects(info.geometry()) ) {
                return true;
            }
        }
    }

    return false;
}

bool XWindowInterface::dockIsCovering() const
{
    int currentDockPos = -1;

    QList<WId> windows = KWindowSystem::stackingOrder();
    int size = windows.count();

    for(int i=size-1; i>=0; --i) {
        WId window = windows.at(i);
        if (window == m_dockWindow->winId()) {
            currentDockPos = i;
            break;
        }
    }

    if (currentDockPos >=0) {
        QRect maskSize;

        if ( !m_maskArea.isNull() ) {
            maskSize = QRect(m_dockWindow->x()+m_maskArea.x(), m_dockWindow->y()+m_maskArea.y(), m_maskArea.width(), m_maskArea.height());
        } else {
            maskSize = QRect(m_dockWindow->x(), m_dockWindow->y(), m_dockWindow->width(), m_dockWindow->height());
        }

        WId transient;

        if (m_dockWindow->transientParent()) {
            transient = m_dockWindow->transientParent()->winId();
        }

        for(int j=currentDockPos-1; j>=0; --j) {
            WId window = windows.at(j);

            KWindowInfo info(window, NET::WMState | NET::XAWMState | NET::WMGeometry);

            if ( info.valid() && !isDesktop(window) && transient!=window && !info.isMinimized() && maskSize.intersects(info.geometry()) ) {
                return true;
            }
        }
    }

    return false;
}

/*
 * SLOTS
 */

void XWindowInterface::activeWindowChanged(WId win)
{
    m_activeWindow = win;

    emit AbstractInterface::activeWindowChanged();
}

void XWindowInterface::windowChanged (WId id, NET::Properties properties, NET::Properties2 properties2)
{
    KWindowInfo info(id, NET::WMState|NET::CloseWindow);

    if (info.valid()) {
        if ((m_demandsAttention == -1) && info.hasState(NET::DemandsAttention)) {
            m_demandsAttention = id;
            emit windowInAttention(true);
        } else if ((m_demandsAttention == id) && !info.hasState(NET::DemandsAttention)) {
            m_demandsAttention = -1;
            emit windowInAttention(false);
        }
    }

    emit AbstractInterface::windowChanged();

    if (id==m_activeWindow) {
        emit AbstractInterface::activeWindowChanged();
    }
}

void XWindowInterface::windowRemoved (WId id)
{
    if (id==m_demandsAttention) {
        m_demandsAttention = -1;
        emit AbstractInterface::windowInAttention(false);
    }
}


}