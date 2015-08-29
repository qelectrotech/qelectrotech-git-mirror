/*
	Copyright 2006-2014 The QElectroTech team
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
#include "conductorautonumerotation.h"
#include "numerotationcontextcommands.h"
#include "qetdiagrameditor.h"
#include "conductor.h"
#include "diagram.h"
#include "qet.h"
#include "QPropertyUndoCommand/qpropertyundocommand.h"
#include "potentialselectordialog.h"

/**
 * @brief ConductorAutoNumerotation::ConductorAutoNumerotation
 * Constructor of autonum, after create a class, call numerate to apply the autonum.
 * When autonum is applyed, they do with an undo command added to the stack of diagram.
 * If you give a parent_undo at constructor, the undo command create in this class have parent_undo for parent,
 * and wasn't added to the stack of diagram (it's the responsabillty of the parent_undo)
 * @param conductor   : the conductor to apply automatic numerotation
 * @param diagram     : the diagram of conductor
 * @param parent_undo : parent undo command
 */
ConductorAutoNumerotation::ConductorAutoNumerotation(Conductor *conductor, Diagram *diagram, QUndoCommand *parent_undo) :
	m_diagram      (diagram),
	conductor_     (conductor),
	conductor_list (conductor -> relatedPotentialConductors()),
	m_parent_undo  (parent_undo)
{}

/**
 * @brief ConductorAutoNumerotation::numerate
 * execute the automatic numerotation
 */
void ConductorAutoNumerotation::numerate() {
	if (!conductor_) return;
	if (conductor_list.size() >= 1 ) numeratePotential();
	else if (conductor_ -> properties().type == ConductorProperties::Multi) numerateNewConductor();
}

/**
 * @brief ConductorAutoNumerotation::applyText
 * apply the text @t to @conductor_ and all conductors at the same potential
 */
void ConductorAutoNumerotation::applyText(QString t)
{
	if (!conductor_) return;

	QVariant old_value, new_value;
	ConductorProperties cp = conductor_ -> properties();
	old_value.setValue(cp);
	cp.text = t;
	new_value.setValue(cp);

	QPropertyUndoCommand *undo = new QPropertyUndoCommand(conductor_, "properties", old_value, new_value, m_parent_undo);
	undo->setText(QObject::tr("Modifier les propriétés d'un conducteur", "undo caption"));

	if (!conductor_list.isEmpty())
	{
		undo->setText(QObject::tr("Modifier les propriétés de plusieurs conducteurs", "undo caption"));
		foreach (Conductor *cond, conductor_list)
		{
			ConductorProperties cp2 = cond -> properties();
			old_value.setValue(cp2);
			cp2.text = t;
			new_value.setValue(cp2);
			new QPropertyUndoCommand(cond, "properties", old_value, new_value, undo);
		}
	}

	if (!m_parent_undo)
		m_diagram->undoStack().push(undo);
}

/**
 * @brief ConductorAutoNumerotation::numeratePotential
 * Numerate a conductor on an existing potential
 */
void ConductorAutoNumerotation::numeratePotential()
{
	QStringList strl;
	foreach (const Conductor *cc, conductor_list) strl<<(cc->text());

		//the texts is identicals
	if (QET::eachStrIsEqual(strl))
	{
		ConductorProperties cp = conductor_ -> properties();
		cp.text = strl.at(0);
		conductor_ -> setProperties(cp);
		conductor_ -> setText(strl.at(0));
	}
		//the texts isn't identicals
	else
	{
		PotentialSelectorDialog psd(conductor_, m_parent_undo, conductor_->diagramEditor());
		psd.exec();
	}
}

/**
 * @brief ConductorAutoNumerotation::numerateNewConductor
 * create and apply a new numerotation to @conductor_
 */
void ConductorAutoNumerotation::numerateNewConductor() {
	if (!conductor_ || m_diagram->conductorsAutonumName().isEmpty()) return;

	NumerotationContext context = m_diagram->project()->conductorAutoNum(m_diagram -> conductorsAutonumName());
	if (context.isEmpty()) return;

	NumerotationContextCommands ncc (context, m_diagram);
	applyText(ncc.toRepresentedString());
	m_diagram->project()->addConductorAutoNum(m_diagram -> conductorsAutonumName(), ncc.next());
}
