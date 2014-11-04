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
#include <QGraphicsSceneMouseEvent>
#include <QObject>

#include "eseventaddline.h"
#include "elementscene.h"
#include "qetelementeditor.h"
#include "partline.h"
#include "editorcommands.h"

/**
 * @brief ESEventAddLine::ESEventAddLine
 * Constructor
 * @param scene : scene where we operate this action
 */
ESEventAddLine::ESEventAddLine(ElementScene *scene) :
	ESEventInterface (scene),
	m_line           (nullptr)
{}

/**
 * @brief ESEventAddLine::~ESEventAddLine
 * destructor
 */
ESEventAddLine::~ESEventAddLine() {
	if (m_running || m_abort)
		delete m_line;
}

/**
 * @brief ESEventAddLine::mousePressEvent
 * @param event
 * @return
 */
bool ESEventAddLine::mousePressEvent(QGraphicsSceneMouseEvent *event) {

	if (event -> button() == Qt::LeftButton) {

		if (! m_running) m_running = true;
		QPointF pos = m_scene -> snapToGrid(event -> scenePos());

		//Create new line
		if (!m_line) {
			m_line = new PartLine(m_editor, 0, m_scene);
			m_line  -> setLine(QLineF(pos, pos));
			return true;
		}

		//Add new line to scene
		m_line  -> setLine(QLineF(m_line->line().p1(), pos));
		m_scene -> undoStack().push(new AddPartCommand(QObject::tr("ligne"), m_scene, m_line));

		//Set m_line to nullptr for create new line at next mouse press
		m_line = nullptr;

		return true;
	}

	//Remove the current line if exist
	//or finish if no line
	if (event -> button() == Qt::RightButton) {
		if (m_line) {
			delete m_line; m_line = nullptr;
		}
		else {
			m_running = false;
		}
		return true;
	}

	return false;
}

/**
 * @brief ESEventAddLine::mouseMoveEvent
 * @param event
 * @return
 */
bool ESEventAddLine::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	updateHelpCross (event -> scenePos());

	if (!m_line) return false;

	QPointF pos = m_scene -> snapToGrid(event -> scenePos());
	m_line -> setLine(QLineF(m_line->line().p1(), pos));
	return true;
}
