
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __EVENT_BROWSER_TAB_REOPEN_CLOSED_DECLARE__
#include "EventBrowserTabReopenClosed.h"
#undef __EVENT_BROWSER_TAB_REOPEN_CLOSED_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserTabReopenClosed
 * \brief Event to open a recently closed tab in a window
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventBrowserTabReopenClosed::EventBrowserTabReopenClosed(const int32_t browserWindowIndex)
: Event(EventTypeEnum::EVENT_BROWSER_TAB_REOPEN_CLOSED),
m_browserWindowIndex(browserWindowIndex)
{
    
}

/**
 * Destructor.
 */
EventBrowserTabReopenClosed::~EventBrowserTabReopenClosed()
{
}

/**
 * @return Index of browser window that is requested reopening last closed tab
 */
int32_t
EventBrowserTabReopenClosed::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * @return Content of reopened tab or NULL if tab could not be reopened
 */
BrowserTabContent*
EventBrowserTabReopenClosed::getBrowserTabContent() const
{
    return m_browserTabContent;
}

/**
 * Set the browser tab content for the reopened browser tab.
 *
 * @param browserTabContent
 *     Content of the reopened browser tab
 */
void
EventBrowserTabReopenClosed::setBrowserTabContent(BrowserTabContent* browserTabContent)
{
    m_browserTabContent = browserTabContent;
}

