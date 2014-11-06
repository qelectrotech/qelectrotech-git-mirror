/*
	Copyright 2006-2014 The QElectroTech Team
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
#include <QObject>

#include "eseventaddrect.h"
#include "elementscene.h"
#include "partrectangle.h"
#include "editorcommands.h"

/**
 * @brief ESEventAddRect::ESEventAddRect
 * @param scene
 */
ESEventAddRect::ESEventAddRect(ElementScene *scene) :
	ESEventInterface(scene),
	m_rect(nullptr)
{}

/**
 * @brief ESEventAddRect::~ESEventAddRect
 */
ESEventAddRect::~ESEventAddRect() {
	if (m_running || m_abort)
		delete m_rect;
}

/**
 * @brief ESEventAddRect::mousePressEvent
 * @param event
 * @return
 */
bool ESEventAddRect::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	if (event -> button() == Qt::LeftButton) {
		if(!m_running) m_running = true;
		QPointF pos = m_scene->snapToGrid(event -> scenePos());

		//create new rectangle
		if (!m_rect) {
			m_rect = new PartRectangle(m_editor, 0, m_scene);
			m_rect -> setRect(QRectF(pos, pos));
			return true;
		}

		//Add rectangle to scene
		m_rect  -> setRect(m_rect -> rect().normalized());
		m_scene -> undoStack().push(new AddPartCommand(QObject::tr("Rectangle"), m_scene, m_rect));

		//Set m_rect to nullptr for create new rectangle at next mouse press
		m_rect = nullptr;

		return true;
	}
	return false;
}

/**
 * @brief ESEventAddRect::mouseMoveEvent
 * @param event
 * @return
 */
bool ESEventAddRect::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	updateHelpCross(event -> scenePos());
	if (!m_rect) return false;

	QRectF rect(m_rect -> rect().topLeft(), m_scene->snapToGrid(event -> scenePos()));
	m_rect -> setRect(rect);
	return true;
}

/**
 * @brief ESEventAddRect::mouseReleaseEvent
 * @param event
 * @return
 */
bool ESEventAddRect::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	if (event -> button() == Qt::RightButton) {
		if (m_rect) {delete m_rect; m_rect = nullptr;}
		else        {m_running = false;}
		return true;
	}
	return false;
}
