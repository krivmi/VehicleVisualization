/*
*
* This file is part of QMapControl,
* an open-source cross-platform map widget
*
* Copyright (C) 2007 - 2008 Kai Winter
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QMapControl. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: kaiwinter@gmx.de
* Program URL   : http://qmapcontrol.sourceforge.net/
*
*/

#pragma once

// Qt includes.
#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>

// STL includes.
#include <memory>

// Local includes.
#include "qmapcontrol_global.h"
#include "Layer.h"
#include "MapAdapter.h"

namespace qmapcontrol
{
    //! Layer class
    /*!
     * Layer that can display a MapAdapter.
     *
     * @author Kai Winter <kaiwinter@gmx.de>
     * @author Chris Stylianou <chris5287@gmail.com>
     */
    class QMAPCONTROL_EXPORT LayerMapAdapter : public Layer
    {
        Q_OBJECT
    public:
        //! Layer constructor
        /*!
         * This is used to construct a layer.
         * @param name The name of the layer.
         * @param mapadapter The Map Adapter of the layer.
         * @param zoom_minimum The minimum zoom level to show this geometry at.
         * @param zoom_maximum The maximum zoom level to show this geometry at.
         * @param parent QObject parent ownership.
         */
        LayerMapAdapter(const std::string& name, const std::shared_ptr<MapAdapter>& mapadapter = nullptr, const int& zoom_minimum = 0, const int& zoom_maximum = 19, QObject* parent = 0);

        //! Disable copy constructor.
        ///LayerMapAdapter(const LayerMapAdapter&) = delete; @todo re-add once MSVC supports default/delete syntax.

        //! Disable copy assignment.
        ///LayerMapAdapter& operator=(const LayerMapAdapter&) = delete; @todo re-add once MSVC supports default/delete syntax.

        //! Destructor.
        virtual ~LayerMapAdapter() { } /// = default; @todo re-add once MSVC supports default/delete syntax.

        /*!
         * Returns the Map Adapter from this Layer
         * @return the map adapter that are on this Layer.
         */
        const std::shared_ptr<MapAdapter> getMapAdapter() const;

        /*!
         * Sets the Map Adapter for this layer.
         * @param mapadapter The Map Adapter to set.
         */
        void setMapAdapter(const std::shared_ptr<MapAdapter>& mapadapter);

        /*!
         * Handles mouse press events (such as left-clicking an item on the layer).
         * @param mouse_event The mouse event.
         * @param mouse_point_coord The mouse point on the map in coord.
         * @param controller_zoom The current controller zoom.
         */
        bool mousePressEvent(const QMouseEvent* mouse_event, const PointWorldCoord& mouse_point_coord, const int& controller_zoom) const final;

        /*!
         * Draws each map adapter and geometry to a pixmap using the provided painter.
         * @param painter The painter that will draw to the pixmap.
         * @param backbuffer_rect_px Only draw map tiles/geometries that are contained in the backbuffer rect (pixels).
         * @param controller_zoom The current controller zoom.
         */
        void draw(QPainter& painter, const RectWorldPx& backbuffer_rect_px, const int& controller_zoom) const final;

    private:
        /// The map adapter drawn by this layer.
        std::shared_ptr<MapAdapter> m_mapadapter;

        /// Mutex to protect map adapter.
        mutable QReadWriteLock m_mapadapter_mutex;
    };
}
