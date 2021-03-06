// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <QKeyEvent>

#include "DolphinQt2/Host.h"
#include "DolphinQt2/RenderWidget.h"
#include "DolphinQt2/Settings.h"

RenderWidget::RenderWidget(QWidget* parent) : QWidget(parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setAttribute(Qt::WA_NoSystemBackground, true);

  connect(Host::GetInstance(), &Host::RequestTitle, this, &RenderWidget::setWindowTitle);
  connect(this, &RenderWidget::StateChanged, Host::GetInstance(), &Host::SetRenderFullscreen);
  connect(this, &RenderWidget::HandleChanged, Host::GetInstance(), &Host::SetRenderHandle);
  emit HandleChanged((void*)winId());

  connect(&Settings::Instance(), &Settings::HideCursorChanged, this,
          &RenderWidget::OnHideCursorChanged);
  OnHideCursorChanged();
}

void RenderWidget::OnHideCursorChanged()
{
  setCursor(Settings::Instance().GetHideCursor() ? Qt::BlankCursor : Qt::ArrowCursor);
}

bool RenderWidget::event(QEvent* event)
{
  switch (event->type())
  {
  case QEvent::KeyPress:
  {
    QKeyEvent* ke = static_cast<QKeyEvent*>(event);
    if (ke->key() == Qt::Key_Escape)
      emit EscapePressed();
    break;
  }
  case QEvent::WinIdChange:
    emit HandleChanged((void*)winId());
    break;
  case QEvent::WindowActivate:
    Host::GetInstance()->SetRenderFocus(true);
    break;
  case QEvent::WindowDeactivate:
    Host::GetInstance()->SetRenderFocus(false);
    break;
  case QEvent::WindowStateChange:
    emit StateChanged(isFullScreen());
    break;
  case QEvent::Close:
    emit Closed();
    break;
  default:
    break;
  }
  return QWidget::event(event);
}
