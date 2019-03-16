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
#include "elementsmover.h"
#include "conductor.h"
#include "conductortextitem.h"
#include "diagram.h"
#include "diagramcommands.h"
#include "element.h"
#include "independenttextitem.h"
#include "diagramimageitem.h"
#include "conductorautonumerotation.h"
#include "dynamicelementtextitem.h"
#include "elementtextitemgroup.h"

/**
 * @brief ElementsMover::ElementsMover Constructor
 */
ElementsMover::ElementsMover() :
	movement_running_(false),
	current_movement_(),
	diagram_(nullptr),
	m_movement_driver(nullptr),
	m_moved_content()
{
	
}

/**
 * @brief ElementsMover::~ElementsMover Destructor
 */
ElementsMover::~ElementsMover() {
}

/**
 * @brief ElementsMover::isReady
 * @return True if this element mover is ready to be used.
 * A element mover is ready when the previous managed movement is finish.
 */
bool ElementsMover::isReady() const {
	return(!movement_running_);
}

/**
 * @brief ElementsMover::beginMovement
 * Start a new movement
 * @param diagram diagram where the movement is applied
 * @param driver_item item moved by mouse and don't be moved by Element mover
 * @return the numbers of items to be moved or -1 if movement can't be init.
 */
int ElementsMover::beginMovement(Diagram *diagram, QGraphicsItem *driver_item)
{
		// They must be no movement in progress
	if (movement_running_) return(-1);
	
		// Be sure we have diagram to work
	if (!diagram) return(-1);
	diagram_ = diagram;
	
		// Take count of driver item
	m_movement_driver = driver_item;
	
		// At the beginning of movement, move is NULL
	current_movement_ = QPointF(0.0, 0.0);
	
	m_moved_content = DiagramContent(diagram);
	m_moved_content.removeNonMovableItems();

		//Remove element text, if the parent element is selected.
	QList<DynamicElementTextItem *> deti_list = m_moved_content.m_element_texts.toList();
	for(DynamicElementTextItem *deti : deti_list) {
		if(m_moved_content.m_elements.contains(deti->parentElement())) {
			m_moved_content.m_element_texts.remove(deti);
		}
	}

	QList<ElementTextItemGroup *> etig_list = m_moved_content.m_texts_groups.toList();
	for(ElementTextItemGroup *etig : etig_list) {
		if (m_moved_content.m_elements.contains(etig->parentElement())) {
			m_moved_content.m_texts_groups.remove(etig);
		}
	}

	if (!m_moved_content.count()) return(-1);
	
	/* At this point, we've got all info to manage movement.
	 * There is now a move in progress */
	movement_running_ = true;
	
	return(m_moved_content.count());
}

/**
 * @brief ElementsMover::continueMovement
 * Add a move to the current movement.
 * @param movement movement to applied
 */
void ElementsMover::continueMovement(const QPointF &movement) {
	if (!movement_running_ || movement.isNull()) return;

	current_movement_ += movement;

	//Move every movable item, except conductor
	typedef DiagramContent dc;
	for (QGraphicsItem *qgi : m_moved_content.items(dc::Elements | dc::TextFields | dc::Images | dc::Shapes | dc::ElementTextFields | dc::TextGroup))
	{
		if (qgi == m_movement_driver)
			continue;
		qgi -> setPos(qgi->pos() + movement);
	}
	
	// Move some conductors
	foreach(Conductor *conductor, m_moved_content.m_conductors_to_move) {
		conductor -> setPos(conductor -> pos() + movement);
	}
	
	// Recalcul the path of other conductors
	foreach(Conductor *conductor, m_moved_content.m_conductors_to_update) {
		conductor -> updatePath();
	}
}

/**
 * @brief ElementsMover::endMovement
 * Ended the current movement by creating an undo added to the undostack of the diagram.
 * If there is only one element moved, we try to auto-connect new conductor from this element
 * and other possible element.
 */
void ElementsMover::endMovement()
{
		// A movement must be inited
	if (!movement_running_) return;

		//empty command to be used has parent of commands below
	QUndoCommand *undo_object = new QUndoCommand();

		//Create undo move if there is a movement
	if (!current_movement_.isNull()) {
		QUndoCommand *quc = new MoveElementsCommand(diagram_, m_moved_content, current_movement_, undo_object);
		undo_object->setText(quc->text());
	}

		//There is only one element moved, and project authorize auto conductor,
		//we try auto connection of conductor;
	typedef DiagramContent dc;
	if (m_moved_content.items(dc::TextFields | dc::Images | dc::Shapes).size() == 0 &&
		m_moved_content.items(dc::Elements).size() == 1 &&
		diagram_ -> project() -> autoConductor())
	{
		Element *elmt = m_moved_content.m_elements.first();

		int acc = elmt->AlignedFreeTerminals().size();

		while (!elmt -> AlignedFreeTerminals().isEmpty())
		{
			QPair <Terminal *, Terminal *> pair = elmt -> AlignedFreeTerminals().takeFirst();

			Conductor *conductor = new Conductor(pair.first, pair.second);

				//Create an undo object for each new auto conductor, with undo_object for parent
			new AddItemCommand<Conductor *>(conductor, diagram_, QPointF(), undo_object);
				if (undo_object->text().isEmpty())
					undo_object->setText(QObject::tr("Ajouter %n conducteur(s)", "add a numbers of conductor one or more", acc));

				//Get all conductors at the same potential of conductor
			QSet <Conductor *> conductors_list = conductor->relatedPotentialConductors();

				//Compare the properties of every conductors stored in conductors_list,
				//if every conductors properties is equal, we use this properties for conductor.
			ConductorProperties others_properties;
			bool use_properties = false;
			if (!conductors_list.isEmpty())
			{
				use_properties = true;
				others_properties = (*conductors_list.begin())->properties();
				foreach (Conductor *cond, conductors_list)
					if (cond->properties() != others_properties)
						use_properties = false;
			}

			if (use_properties)
				conductor->setProperties(others_properties);
			else
			{
				conductor -> setProperties(diagram_ -> defaultConductorProperties);
					//Autonum the new conductor, the undo command associated for this, have for parent undo_object
				ConductorAutoNumerotation can  (conductor, diagram_, undo_object);
				can.numerate();
			}
		};
	}

		//Add undo_object if have child
	if (undo_object->childCount() >= 1)
		diagram_ -> undoStack().push(undo_object);
	else
		delete undo_object;
	
		// There is no movement in progress now
	movement_running_ = false;
	m_moved_content.clear();
}
