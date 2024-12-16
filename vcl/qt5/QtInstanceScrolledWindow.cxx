/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QtInstanceScrolledWindow.hxx>
#include <QtInstanceScrolledWindow.moc>

#include <QtWidgets/QScrollBar>

QtInstanceScrolledWindow::QtInstanceScrolledWindow(QScrollArea* pScrollArea)
    : QtInstanceWidget(pScrollArea)
    , m_pScrollArea(pScrollArea)
{
    assert(m_pScrollArea);
}

void QtInstanceScrolledWindow::move(weld::Widget*, weld::Container*)
{
    assert(false && "Not implemented yet");
}

css::uno::Reference<css::awt::XWindow> QtInstanceScrolledWindow::CreateChildFrame()
{
    assert(false && "Not implemented yet");
    return nullptr;
}

void QtInstanceScrolledWindow::child_grab_focus() { assert(false && "Not implemented yet"); }

void QtInstanceScrolledWindow::hadjustment_configure(int, int, int, int, int, int)
{
    assert(false && "Not implemented yet");
}

int QtInstanceScrolledWindow::hadjustment_get_value() const
{
    assert(false && "Not implemented yet");
    return 0;
}

void QtInstanceScrolledWindow::hadjustment_set_value(int)
{
    assert(false && "Not implemented yet");
}

int QtInstanceScrolledWindow::hadjustment_get_upper() const
{
    assert(false && "Not implemented yet");
    return 0;
}

void QtInstanceScrolledWindow::hadjustment_set_upper(int)
{
    assert(false && "Not implemented yet");
}

int QtInstanceScrolledWindow::hadjustment_get_page_size() const
{
    assert(false && "Not implemented yet");
    return 0;
}

void QtInstanceScrolledWindow::hadjustment_set_page_size(int)
{
    assert(false && "Not implemented yet");
}

void QtInstanceScrolledWindow::hadjustment_set_page_increment(int)
{
    assert(false && "Not implemented yet");
}

void QtInstanceScrolledWindow::hadjustment_set_step_increment(int)
{
    assert(false && "Not implemented yet");
}

void QtInstanceScrolledWindow::set_hpolicy(VclPolicyType)
{
    assert(false && "Not implemented yet");
}

VclPolicyType QtInstanceScrolledWindow::get_hpolicy() const
{
    assert(false && "Not implemented yet");
    return VclPolicyType::ALWAYS;
}

void QtInstanceScrolledWindow::vadjustment_configure(int, int, int, int, int, int)
{
    assert(false && "Not implemented yet");
}

int QtInstanceScrolledWindow::vadjustment_get_value() const
{
    assert(false && "Not implemented yet");
    return 0;
}

void QtInstanceScrolledWindow::vadjustment_set_value(int)
{
    assert(false && "Not implemented yet");
}

int QtInstanceScrolledWindow::vadjustment_get_upper() const
{
    assert(false && "Not implemented yet");
    return 0;
}

void QtInstanceScrolledWindow::vadjustment_set_upper(int)
{
    assert(false && "Not implemented yet");
}

int QtInstanceScrolledWindow::vadjustment_get_page_size() const
{
    assert(false && "Not implemented yet");
    return 0;
}

void QtInstanceScrolledWindow::vadjustment_set_page_size(int)
{
    assert(false && "Not implemented yet");
}

void QtInstanceScrolledWindow::vadjustment_set_page_increment(int)
{
    assert(false && "Not implemented yet");
}

void QtInstanceScrolledWindow::vadjustment_set_step_increment(int nSize)
{
    SolarMutexGuard g;

    GetQtInstance().RunInMainThread([&] {
        if (QScrollBar* pScrollBar = m_pScrollArea->verticalScrollBar())
            pScrollBar->setSingleStep(nSize);
    });
}

int QtInstanceScrolledWindow::vadjustment_get_lower() const
{
    assert(false && "Not implemented yet");
    return 0;
}

void QtInstanceScrolledWindow::vadjustment_set_lower(int)
{
    assert(false && "Not implemented yet");
}

void QtInstanceScrolledWindow::set_vpolicy(VclPolicyType)
{
    assert(false && "Not implemented yet");
}

VclPolicyType QtInstanceScrolledWindow::get_vpolicy() const
{
    assert(false && "Not implemented yet");
    return VclPolicyType::ALWAYS;
}

int QtInstanceScrolledWindow::get_scroll_thickness() const
{
    assert(false && "Not implemented yet");
    return 0;
}

void QtInstanceScrolledWindow::set_scroll_thickness(int) { assert(false && "Not implemented yet"); }

void QtInstanceScrolledWindow::customize_scrollbars(const Color&, const Color&, const Color&)
{
    assert(false && "Not implemented yet");
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
