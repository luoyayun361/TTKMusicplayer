#include "musicgrabwidget.h"
#include "musicwidgetutils.h"
#include "musicsinglemanager.h"

#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>

#ifdef Q_CC_GNU
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

MusicGrabWidget::MusicGrabWidget(QWidget *parent)
    : QWidget(nullptr)
{
    Q_UNUSED(parent);

    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_QuitOnClose, true);

    setWindowFlags( Qt::Widget | Qt::FramelessWindowHint );
    resize(QApplication::desktop()->width(), QApplication::desktop()->height());
    setCursor(Qt::CrossCursor);
    m_isDrawing = false;

#ifndef TTK_GREATER_NEW
    m_originPixmap = QPixmap::grabWindow(QApplication::desktop()->winId(), 0, 0, width(), height());
#else
    m_originPixmap = QApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId(), 0, 0, width(), height());
#endif
}

MusicGrabWidget::~MusicGrabWidget()
{
    M_SINGLE_MANAGER_PTR->removeObject(getClassName());
}

void MusicGrabWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    m_ptCursor.setX(event->x());
    m_ptCursor.setY(event->y());
    update();
}

void MusicGrabWidget::musicCreateRightMenu()
{
    const QString &filename = MusicUtils::Widget::getSaveFileDialog(this, "Jpeg(*.jpg)");
    if(!filename.isEmpty())
    {
        musicCreateRightMenu(filename);
    }
}

void MusicGrabWidget::musicCreateRightMenu(const QString &path)
{
    if(!m_grabPixmap.isNull())
    {
        m_grabPixmap.save(path, 0, 100);
        close();
    }
}

void MusicGrabWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    QPen pen(QColor("#158FE1"), 1);
    painter.setPen(pen);

    painter.drawPixmap(0, 0, width(), height(), m_originPixmap);

    int w, h;
    if(m_isDrawing)
    {
        w = m_ptCursor.x() - m_ptStart.x();
        h = m_ptCursor.y() - m_ptStart.y();
        painter.drawRect(m_ptStart.x(), m_ptStart.y(), w, h);
    }
    else
    {
        if(m_ptEnd != m_ptStart)
        {
            w = m_ptEnd.x() - m_ptStart.x();
            h = m_ptEnd.y() - m_ptStart.y();
            painter.drawRect(m_ptStart.x(), m_ptStart.y(), w, h);
        }
    }

    QPolygon listMarker;
    listMarker.push_back(QPoint(m_ptStart.x(), m_ptStart.y()));
    listMarker.push_back(QPoint(m_ptStart.x() + w, m_ptStart.y()));
    listMarker.push_back(QPoint(m_ptStart.x() , h + m_ptStart.y()));
    listMarker.push_back(QPoint(m_ptStart.x() + w, h + m_ptStart.y()));

    listMarker.push_back(QPoint(m_ptStart.x() + (w >> 1), m_ptStart.y()));
    listMarker.push_back(QPoint(m_ptStart.x() + (w >> 1), h + m_ptStart.y()));
    listMarker.push_back(QPoint(m_ptStart.x(), m_ptStart.y() + (h >> 1)));
    listMarker.push_back(QPoint(m_ptStart.x() + w, m_ptStart.y() + (h >> 1)));

    pen.setWidth(4);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawPoints(listMarker);
}

void MusicGrabWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    if(event->button() == Qt::LeftButton)
    {
        m_ptStart = event->pos();
        m_ptCursor = m_ptStart;
        m_isDrawing = true;
    }
    update();
}

void MusicGrabWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton)
    {
        m_ptEnd = event->pos();
        m_isDrawing = false;
        grabPixmap();
    }
}

void MusicGrabWidget::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        const QString &filename = MusicObject::getAppDir() + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + JPG_FILE;
        musicCreateRightMenu(filename);
    }
    else if(event->key() == Qt::Key_Escape)
    {
        close();
    }
}

void MusicGrabWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QWidget::contextMenuEvent(event);
    QMenu menu(this);
    menu.addAction(tr("Save"), this, SLOT(musicCreateRightMenu()));
    menu.exec(QCursor::pos());
}

void MusicGrabWidget::grabPixmap()
{
    int width = m_ptEnd.x() - m_ptStart.x();
    int height = m_ptEnd.y() - m_ptStart.y();
#ifndef TTK_GREATER_NEW
    m_grabPixmap = QPixmap::grabWindow(QApplication::desktop()->winId(), m_ptStart.x(), m_ptStart.y(), width, height);
#else
    m_grabPixmap = QApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId(), m_ptStart.x(), m_ptStart.y(), width, height);
#endif
}
