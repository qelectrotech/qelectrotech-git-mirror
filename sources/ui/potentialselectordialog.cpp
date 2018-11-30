/*
	Copyright 2006-2017 The QElectroTech Team
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
#include "potentialselectordialog.h"
#include "ui_potentialselectordialog.h"
#include "conductor.h"
#include "terminal.h"
#include <QRadioButton>
#include "QPropertyUndoCommand/qpropertyundocommand.h"
#include "diagram.h"
#include "element.h"
#include "reportelement.h"
#include "assignvariables.h"
#include "formulaassistantdialog.h"

//### BEGIN PRIVATE CLASS ###//

/**
 * @brief The NewConductorPotentialSelector class
 * Use for get the conductor propertie when two potentials is linked by a conductor
 */
class NewConductorPotentialSelector : public AbstractPotentialSelector
{
	public:
		NewConductorPotentialSelector(Conductor *conductor) :
			m_is_valid(false)
		{
			Terminal *terminal_1 = conductor->terminal1;
			Terminal *terminal_2 = conductor->terminal2;
				//We temporarily remove the conductor of his two terminals, to get the two existing potential
			terminal_1->removeConductor(conductor);
			terminal_2->removeConductor(conductor);

			getPotential(terminal_1, m_seq_num_1, m_conductor_number_1, m_properties_list_1, m_conductors_list_1);
			getPotential(terminal_2, m_seq_num_2, m_conductor_number_2, m_properties_list_2, m_conductors_list_2);

				//There isn't a potential at terminal 1 or 2.
			if (m_conductor_number_1 == 0 && m_conductor_number_2 == 0) return;

				//Re-add conductor to his terminals.
			terminal_1->addConductor(conductor);
			terminal_2->addConductor(conductor);
			m_is_valid = true;
		}

		bool isValid() const override {return m_is_valid;}

		/**
		 * @brief getPotential
		 * Get the conductor propertie of the potential at terminal, and the number of wire in this potential.
		 * @param terminal
		 * @param properties
		 * @param number
		 */
		void getPotential(Terminal *terminal, autonum::sequentialNumbers &seq_num , int &number, QList<ConductorProperties> &properties_list, QList<Conductor*> &c_list)
		{
			Conductor *conductor_in_potential = nullptr;

				//Terminal have conductor linked to, we get it.
			if (!terminal->conductors().isEmpty())
				conductor_in_potential = terminal->conductors().first();

				//Terminal haven't got a conductor, but if parent element is a folio report or a terminal element, we search a potential
				//through the report or in another terminal of the terminal element.
			else if (terminal->parentElement()->linkType() & (Element::Terminale | Element::AllReport))
			{
				Element *elmt_ = terminal->parentElement();

				if ((elmt_->linkType() & Element::Terminale) && !elmt_->terminals().isEmpty())
				{
					foreach(Terminal *t, elmt_->terminals())
					{
						if (t->conductors().isEmpty()) continue;
						conductor_in_potential = t->conductors().first();
						break;
					}
				}
				else if ((elmt_->linkType() & Element::AllReport) && !elmt_->isFree())
				{
					Element *other_report = elmt_->linkedElements().first();
					if (other_report->terminals().isEmpty()) return;
					Terminal *t = other_report->terminals().first();
					if (t->conductors().isEmpty()) return;
					conductor_in_potential= t->conductors().first();
				}
			}

			if (!conductor_in_potential)
				return;
			seq_num = conductor_in_potential->sequenceNum();
			number = conductor_in_potential->relatedPotentialConductors().size()+1; //We add +1 because conductor_in_potential isn't count by relatedPotentialConductors

			c_list = conductor_in_potential->relatedPotentialConductors().toList();
			c_list.append(conductor_in_potential);
			foreach(Conductor *c, c_list)
				properties_list.append(c->properties());
		}

		~NewConductorPotentialSelector() override {}

	private :
		bool m_is_valid;
};

/**
 * @brief The LinkReportPotentialSelector class
 * Use for get the conductor propertie when two potentials is linked with a folio report
 */
class LinkReportPotentialSelector : public AbstractPotentialSelector
{
	public:
		LinkReportPotentialSelector(Element *report) :
			m_is_valid(false)
		{
			if ((report->linkType() & Element::AllReport) && !report->isFree())
			{
					//We temporarily unlink report to get the two existing potential
				Element *other_report = report->linkedElements().first();
				report->unlinkAllElements();

				if (report->conductors().isEmpty() || other_report->conductors().isEmpty())
					return;

				m_conductor_number_1 = report->conductors().first()->relatedPotentialConductors().size() + 1;
				m_seq_num_1 = report->conductors().first()->sequenceNum();
				m_conductors_list_1.append(report->conductors().first()->relatedPotentialConductors().toList());
				m_conductors_list_1.append(report->conductors().first());
				foreach(Conductor *c, m_conductors_list_1)
					m_properties_list_1 << c->properties();

				m_conductor_number_2 = other_report->conductors().first()->relatedPotentialConductors().size() + 1;
				m_seq_num_2 = other_report->conductors().first()->sequenceNum();
				m_conductors_list_2.append(other_report->conductors().first()->relatedPotentialConductors().toList());
				m_conductors_list_2.append(other_report->conductors().first());
				foreach(Conductor *c, m_conductors_list_2)
					m_properties_list_2 << c->properties();

					//We relink the report
				report->linkToElement(other_report);
				m_is_valid = true;
			}
		}

		~LinkReportPotentialSelector() override {}

		bool isValid() const override {return m_is_valid;}

	private:
		bool m_is_valid;
};

//### END PRIVATE CLASS ###//

/**
 * @brief PotentialSelectorDialog::PotentialSelectorDialog
 * Constructor when we link two potentiels together, with a conductor
 * @param conductor : the new conductor who connect to existing potential
 * @param parent_undo : undo parent to use.
 * @param parent : parent widget.
 */
PotentialSelectorDialog::PotentialSelectorDialog(Conductor *conductor, QUndoCommand *parent_undo, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PotentialSelectorDialog),
	m_conductor(conductor),
	m_report(nullptr),
	m_parent_undo(parent_undo)
{
	ui->setupUi(this);
	m_potential_selector = new NewConductorPotentialSelector(conductor);
	buildWidget();
}

/**
 * @brief PotentialSelectorDialog::PotentialSelectorDialog
 * Constructor when we link two potentiels together, with a folio report.
 * @param report : one of the report used to link the potentials (report must be linked to another report)
 * @param parent_undo : undo parent to use
 * @param parent : parent widget
 */
PotentialSelectorDialog::PotentialSelectorDialog(Element *report, QUndoCommand *parent_undo, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PotentialSelectorDialog),
	m_conductor(nullptr),
	m_report(report),
	m_parent_undo(parent_undo)
{
	ui->setupUi(this);
	m_potential_selector = new LinkReportPotentialSelector(report);
	buildWidget();
}

PotentialSelectorDialog::~PotentialSelectorDialog()
{
    delete ui;
	delete m_potential_selector;
}

/**
 * @brief PotentialSelectorDialog::buildWidget
 * Build the dialog
 */
void PotentialSelectorDialog::buildWidget()
{
	QString text1(tr("%n conducteurs composent le potentiel suivant :", "", m_potential_selector->m_conductor_number_1));
	
	ConductorProperties cp1;
	if(!m_potential_selector->m_properties_list_1.isEmpty())
		cp1 = m_potential_selector->m_properties_list_1.first();;

	if(!cp1.text.isEmpty())
		text1.append(tr("\nNuméro : %1").arg(cp1.text));
	if(!cp1.m_function.isEmpty())
		text1.append(tr("\nFonction : %1").arg(cp1.m_function));
	if(!cp1.m_tension_protocol.isEmpty())
		text1.append(tr("\nTension/protocole : %1").arg(cp1.m_tension_protocol));
			
	QString text2(tr("%n conducteurs composent le potentiel suivant :", "", m_potential_selector->m_conductor_number_2));
	ConductorProperties cp2;
	if(!m_potential_selector->m_properties_list_2.isEmpty())
		cp2 = m_potential_selector->m_properties_list_2.first();
	
	if(!cp2.text.isEmpty())
		text2.append(tr("\nNuméro : %1").arg(cp2.text));
	if(!cp2.m_function.isEmpty())
		text2.append(tr("\nFonction : %1").arg(cp2.m_function));
	if(!cp2.m_tension_protocol.isEmpty())
		text2.append(tr("\nTension/protocole : %1").arg(cp2.m_tension_protocol));
			
	QRadioButton *rb1 = new QRadioButton(text1, this);
	QRadioButton *rb2 = new QRadioButton(text2, this);

	connect(rb1, &QRadioButton::toggled, [this](bool t)
	{
		if(t)
		{
			this->m_sequential_num = this->m_potential_selector->m_seq_num_1;
			this->m_properties_list = this->m_potential_selector->m_properties_list_1;
			this->m_conductors_to_change = this->m_potential_selector->m_conductors_list_2;
		}
	});
	connect(rb2, &QRadioButton::toggled, [this](bool t)
	{
		if(t)
		{
			this->m_sequential_num = this->m_potential_selector->m_seq_num_2;
			this->m_properties_list = this->m_potential_selector->m_properties_list_2;
			this->m_conductors_to_change = this->m_potential_selector->m_conductors_list_1;
		}
	});

		//Set the radio button of potential with the bigger number of conductors,
		//at first position, and check it
	if (m_potential_selector->m_conductor_number_1 >= m_potential_selector->m_conductor_number_2)
	{
		ui->verticalLayout->insertWidget(1, rb1);
		ui->verticalLayout->insertWidget(2, rb2);
		rb1->setChecked(true);
	}
	else
	{
		ui->verticalLayout->insertWidget(1, rb2);
		ui->verticalLayout->insertWidget(2, rb1);
		rb2->setChecked(true);
	}
}

/**
 * @brief PotentialSelectorDialog::on_buttonBox_accepted
 * Action when user click on OK button
 */
void PotentialSelectorDialog::on_buttonBox_accepted()
{
	if (!m_potential_selector->isValid())
		return;

	QUndoCommand *undo = nullptr;
	if (m_parent_undo)
		undo = m_parent_undo;
	else
		undo = new QUndoCommand(tr("Modifier les propriétés de plusieurs conducteurs", "undo caption"));

	Diagram * diagram = nullptr;

	if (m_report)
	{
		if ((m_report->linkType() & Element::AllReport) && !m_report->isFree())
		{
			if (m_report->diagram())
				diagram = m_report->diagram();

			QVariant old_value, new_value;
			QVariant old_seq, new_seq;
			new_seq.setValue(m_sequential_num);

				//Set the new properties for each conductors of the new potential
			foreach(Conductor *cond, m_conductors_to_change)
			{
				ConductorProperties new_properties = cond->properties();
				new_properties.applyForEqualAttributes(m_properties_list);
				old_value.setValue(cond->properties());
				new_value.setValue(new_properties);
				old_seq.setValue(cond->sequenceNum());
				new QPropertyUndoCommand(cond, "sequenceNum", old_seq, new_seq, undo);
				new QPropertyUndoCommand(cond, "properties", old_value, new_value, undo);
			}
			
				//Check if formula of the new potential have incompatible variable with folio report
			QRegularExpression rx ("%sequf_|%seqtf_|%seqhf_|%id|%F|%M|%FG|%LM");
			foreach(ConductorProperties cp, m_properties_list)
			{
				if (cp.m_formula.contains(rx))
				{
					QStringList forbidden_str;
					forbidden_str << "%sequf_" << "%seqtf_" << "%seqhf_" << "%id" << "%F" << "%M" << "%FG" << "%LM";
					
					QString text(tr("La formule du nouveau potentiel contient des variables incompatibles avec les reports de folio.\n"
									"Veuillez saisir une formule compatible pour ce potentiel.\n"
									"Les variables suivantes sont incompatibles :\n"
									"%sequf_  %seqtf_  %seqhf_  %id  %F  %M %FG %LM"));
					FormulaAssistantDialog fag(this);
					fag.setForbiddenVariables(forbidden_str);
					fag.setText(text);
					fag.setFormula(cp.m_formula);
					fag.exec();
					
					QString new_formula = fag.formula();
					QSet <Conductor *> c_list = m_report->conductors().first()->relatedPotentialConductors();
					c_list.insert(m_report->conductors().first());
					foreach(Conductor *cond, c_list)
					{
						old_value.setValue(cond->properties());
						ConductorProperties new_properties = cond->properties();
						new_properties.m_formula = new_formula;
						new_value.setValue(new_properties);
						new QPropertyUndoCommand(cond, "properties", old_value, new_value, undo);
					}
					
					break;
				}
			}
		}
	}

	else if (m_conductor)
	{
		if (m_conductor->diagram())
			diagram = m_conductor->diagram();

		ConductorProperties new_properties = m_conductor->properties();
		new_properties.applyForEqualAttributes(m_properties_list);

		QVariant old_value, new_value;
		old_value.setValue(m_conductor->properties());
		new_value.setValue(new_properties);

		QVariant old_seq, new_seq;
		old_seq.setValue(m_conductor->sequenceNum());
		new_seq.setValue(m_sequential_num);

		new QPropertyUndoCommand(m_conductor, "sequenceNum", old_seq, new_seq, undo);
		new QPropertyUndoCommand(m_conductor, "properties", old_value, new_value, undo);

			//Set the new properties for each conductors of the new potential
		foreach(Conductor *cond, m_conductor->relatedPotentialConductors())
		{
			new_properties = cond->properties();
			new_properties.applyForEqualAttributes(m_properties_list);
			old_value.setValue(cond->properties());
			new_value.setValue(new_properties);
			old_seq.setValue(cond->sequenceNum());
			new QPropertyUndoCommand(cond, "sequenceNum", old_seq, new_seq, undo);
			new QPropertyUndoCommand(cond, "properties", old_value, new_value, undo);
		}
	}

		//There is an undo parent, we stop here, the owner of m_parent_undo will push it to an undo stack
	if (m_parent_undo)
		return;
		//There isn't a parent, we push the undo command to diagram undo stack.
	if (diagram)
		diagram->undoStack().push(undo);
}
