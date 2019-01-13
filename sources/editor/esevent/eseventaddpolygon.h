/*
	Copyright 2006-2019 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ESEVENTADDPOLYGON_H
#define ESEVENTADDPOLYGON_H

#include "eseventinterface.h"

class ElementScene;
class PartPolygon;
class QGraphicsSceneMouseEvent;

/**
 * @brief The ESEventAddPolygon class
 * This ESEvent manage creation of polygon in an ElementScene
 */
class ESEventAddPolygon : public ESEventInterface
{
	public:
		ESEventAddPolygon(ElementScene *scene);
		~ESEventAddPolygon() override;

		bool mousePressEvent       (QGraphicsSceneMouseEvent *event) override;
		bool mouseMoveEvent        (QGraphicsSceneMouseEvent *event) override;
		bool mouseReleaseEvent     (QGraphicsSceneMouseEvent *event) override;
		bool mouseDoubleClickEvent (QGraphicsSceneMouseEvent *event) override;

	private:
		PartPolygon *m_polygon;
};

#endif // ESEVENTADDPOLYGON_H
