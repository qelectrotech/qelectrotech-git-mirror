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
#include "assignvariables.h"
#include "diagram.h"
#include "element.h"
#include "diagramposition.h"
#include "qetapp.h"
#include "qetxml.h"

#include <QVariant>
#include <QStringList>
#include <utility>

namespace autonum
{
	/**
	 * @brief sequentialNumbers::sequentialNumbers
	 */
	sequentialNumbers::sequentialNumbers()
	{}

	sequentialNumbers::sequentialNumbers(const sequentialNumbers &other)
	{
		unit          = other.unit;
		unit_folio    = other.unit_folio;
		ten           = other.ten;
		ten_folio     = other.ten_folio;
		hundred       = other.hundred;
		hundred_folio = other.hundred_folio;
	}

	sequentialNumbers::~sequentialNumbers() {}

	sequentialNumbers &sequentialNumbers::operator=(const sequentialNumbers &other)
	{
		if (&other == this || other == *this)
			return (*this);

		unit          = other.unit;
		unit_folio    = other.unit_folio;
		ten           = other.ten;
		ten_folio     = other.ten_folio;
		hundred       = other.hundred;
		hundred_folio = other.hundred_folio;

		return (*this);
	}

	bool sequentialNumbers::operator==(const sequentialNumbers &other) const
	{
		if (unit          == other.unit && \
			unit_folio    == other.unit_folio && \
			ten           == other.ten && \
			ten_folio     == other.ten_folio && \
			hundred       == other.hundred && \
			hundred_folio == other.hundred_folio)
			return true;
		else
			return false;
	}

	bool sequentialNumbers::operator!=(const sequentialNumbers &other) const
	{
		if (*this == other)
			return false;
		else
			return true;
	}

	/**
	 * @brief sequentialNumbers::toXml
	 * export this sequential numbers into a QDomElement.
	 * @param document : QDomDocument used to create the QDomElement
	 * @param tag_name : the tag name used for the QDomElement.
	 * @return A QDomElement, if this sequential have no value, the returned QDomELement is empty
	 */
	QDomElement sequentialNumbers::toXml(QDomDocument &document, const QString& tag_name) const
	{
		QDomElement element = document.createElement(tag_name);

		if (!unit.isEmpty())
			element.appendChild(QETXML::textToDomElement(document, "unit", unit.join(";")));
		if (!unit_folio.isEmpty())
			element.appendChild(QETXML::textToDomElement(document, "unitFolio", unit_folio.join(";")));
		if(!ten.isEmpty())
			element.appendChild(QETXML::textToDomElement(document, "ten", ten.join(";")));
		if(!ten_folio.isEmpty())
			element.appendChild(QETXML::textToDomElement(document, "tenFolio", ten_folio.join(";")));
		if(!hundred.isEmpty())
			element.appendChild(QETXML::textToDomElement(document, "hundred", hundred.join(";")));
		if(!hundred_folio.isEmpty())
			element.appendChild(QETXML::textToDomElement(document, "hundredFolio", hundred_folio.join(";")));

		return element;
	}

	/**
	 * @brief sequentialNumbers::fromXml
	 * Import sequential values from a QDomElement
	 * @param element
	 */
	void sequentialNumbers::fromXml(const QDomElement &element)
	{
		if (!element.hasChildNodes())
			return;

		QDomElement from;

		from = element.firstChildElement("unit");
		unit = from.text().split(";");

		from = element.firstChildElement("unitFolio");
		unit_folio = from.text().split(";");

		from = element.firstChildElement("ten");
		ten = from.text().split(";");

		from = element.firstChildElement("tenFolio");
		ten_folio = from.text().split(";");

		from = element.firstChildElement("hundred");
		hundred = from.text().split(";");

		from = element.firstChildElement("hundredFolio");
		hundred_folio = from.text().split(";");
	}
	
		//Clear this sequence
	void sequentialNumbers::clear()
	{
		unit.clear();
		unit_folio.clear();
		ten.clear();
		ten_folio.clear();
		hundred.clear();
		hundred_folio.clear();
	}

	/**
	 * @brief AssignVariables::formulaToLabel
	 * Return the @formula with variable assigned (ready to be displayed)
	 * @param formula - the formula to work
	 * @param seqStruct - struct where is stocked int values (struct is passed as a reference and modified by this static method)
	 * @param diagram - the diagram where occure the formula.
	 * @param elmt - parent element (if any) of the formula
	 * @return the string with variable assigned.
	 */
	QString AssignVariables::formulaToLabel(QString formula, sequentialNumbers &seqStruct, Diagram *diagram, const Element *elmt)
	{
		AssignVariables av(std::move(formula), seqStruct, diagram, elmt);
		seqStruct = av.m_seq_struct;
		return av.m_assigned_label;
	}
	
	/**
	 * @brief AssignVariables::replaceVariable
	 * Replace the variables in @formula in form %{my-var} to the corresponding value stored in @dc
	 * @param formula
	 * @param dc
	 * @return 
	 */
	QString AssignVariables::replaceVariable(const QString &formula, const DiagramContext &dc)
	{
		QString str = formula;
		str.replace("%{label}", dc.value("label").toString());
		str.replace("%{comment}", dc.value("comment").toString());
		str.replace("%{description}", dc.value("description").toString());
		str.replace("%{designation}", dc.value("designation").toString());
		str.replace("%{manufacturer}", dc.value("manufacturer").toString());
		str.replace("%{manufacturer-reference}", dc.value("manufacturer-reference").toString());
		str.replace("%{provider}", dc.value("provider").toString());
		str.replace("%{auxiliary1}", dc.value("auxiliary1").toString());
		str.replace("%{auxiliary2}", dc.value("auxiliary2").toString());
		str.replace("%{machine-manufacturer-reference}", dc.value("machine-manufacturer-reference").toString());
		str.replace("%{location}", dc.value("location").toString());
		str.replace("%{function}", dc.value("function").toString());
		str.replace("%{void}", QString());

		return str;
	}
	
	
	AssignVariables::AssignVariables(const QString& formula, const sequentialNumbers& seqStruct , Diagram *diagram, const Element *elmt):
	m_diagram(diagram),
	m_arg_formula(formula),
	m_assigned_label(formula),
	m_seq_struct(seqStruct),
	m_element(elmt)
	
	{
		if (m_diagram)
		{
			m_assigned_label.replace("%F",  m_diagram -> border_and_titleblock.folio());
			m_assigned_label.replace("%f",     QString::number(m_diagram->folioIndex()+1));
			m_assigned_label.replace("%id",    QString::number(m_diagram->folioIndex()+1));
			m_assigned_label.replace("%total", QString::number(m_diagram->border_and_titleblock.folioTotal()));
			m_assigned_label.replace("%M",  m_diagram -> border_and_titleblock.machine());
			m_assigned_label.replace("%LM", m_diagram -> border_and_titleblock.locmach());

	QSettings settings;
			if (m_element)
			{
			if (settings.value("border-columns_0", true).toBool()){
				m_assigned_label.replace("%c", QString::number(m_diagram->convertPosition(m_element->scenePos()).number() - 1));
				}else{
				m_assigned_label.replace("%c", QString::number(m_diagram->convertPosition(m_element->scenePos()).number()));
				}
				m_assigned_label.replace("%l", m_diagram->convertPosition(m_element->scenePos()).letter());
				m_assigned_label.replace("%prefix", m_element->getPrefix());
			}

			assignTitleBlockVar();
			assignProjectVar();
			assignSequence();
		}
	}

	void AssignVariables::assignTitleBlockVar()
	{
		for (int i = 0; i < m_diagram->border_and_titleblock.additionalFields().count(); i++)
		{
			QString folio_variable = m_diagram->border_and_titleblock.additionalFields().keys().at(i);
			QVariant folio_value = m_diagram->border_and_titleblock.additionalFields().operator [](folio_variable);

			if (m_assigned_label.contains(folio_variable)) {
				m_assigned_label.replace("%{" + folio_variable + "}", folio_value.toString());
				m_assigned_label.replace("%"  + folio_variable      , folio_value.toString());
			}
		}
	}

	void AssignVariables::assignProjectVar()
	{
		for (int i = 0; i < m_diagram->project()->projectProperties().count(); i++)
		{
			QString folio_variable = m_diagram->project()->projectProperties().keys().at(i);
			QVariant folio_value = m_diagram->project()->projectProperties().operator [](folio_variable);

			if (m_assigned_label.contains(folio_variable)) {
				m_assigned_label.replace("%{" + folio_variable + "}", folio_value.toString());
				m_assigned_label.replace("%"  + folio_variable      , folio_value.toString());
			}
		}
	}

	void AssignVariables::assignSequence()
	{
		int max = qMax(
						qMax(
							qMax(m_seq_struct.unit_folio.size(),
								 m_seq_struct.ten_folio.size()),
							qMax(m_seq_struct.hundred_folio.size(),
								 m_seq_struct.unit.size())),
						qMax(m_seq_struct.hundred.size(),
							 m_seq_struct.ten.size())
					);

		for (int i=1; i<=max ; i++)
		{
			if (m_assigned_label.contains("%sequ_" + QString::number(i)) && m_seq_struct.unit.size() >= i) {
				m_assigned_label.replace("%sequ_" + QString::number(i),m_seq_struct.unit.at(i-1));
			}
			if (m_assigned_label.contains("%seqt_" + QString::number(i)) && m_seq_struct.ten.size() >= i) {
				m_assigned_label.replace("%seqt_" + QString::number(i),m_seq_struct.ten.at(i-1));
			}
			if (m_assigned_label.contains("%seqh_" + QString::number(i)) && m_seq_struct.hundred.size() >= i) {
				m_assigned_label.replace("%seqh_" + QString::number(i),m_seq_struct.hundred.at(i-1));
			}
			if (m_assigned_label.contains("%sequf_" + QString::number(i)) && m_seq_struct.unit_folio.size() >= i) {
				m_assigned_label.replace("%sequf_" + QString::number(i),m_seq_struct.unit_folio.at(i-1));
			}
			if (m_assigned_label.contains("%seqtf_" + QString::number(i)) && m_seq_struct.ten_folio.size() >= i) {
				m_assigned_label.replace("%seqtf_" + QString::number(i),m_seq_struct.ten_folio.at(i-1));
			}
			if (m_assigned_label.contains("%seqhf_" + QString::number(i)) && m_seq_struct.hundred_folio.size() >= i) {
				m_assigned_label.replace("%seqhf_" + QString::number(i),m_seq_struct.hundred_folio.at(i-1));
			}
		}
	}

	/**
	 * @brief setSequentialToList
	 * Append all sequential of type @type owned by @context in list
	 * @param list : list to have value inserted
	 * @param context : numerotation context to retrieve value
	 * @param type : type of sequential (unit, unitfolio, ten, tenfolio, hundred, hundredfolio)
	 */
	void setSequentialToList(QStringList &list, NumerotationContext &context, const QString& type)
	{
		for (int i = 0; i < context.size(); i++)
		{
			if (context.itemAt(i).at(0) == type)
			{
				QString number;
				if (type == "ten" || type == "tenfolio")
					number = QString("%1").arg(context.itemAt(i).at(1).toInt(), 2, 10, QChar('0'));
				else if (type == "hundred" || type == "hundredfolio")
					number = QString("%1").arg(context.itemAt(i).at(1).toInt(), 3, 10, QChar('0'));
				else number = QString::number(context.itemAt(i).at(1).toInt());
					list.append(number);
			}
		}
	}

	/**
	 * @brief setFolioSequentialToHash
	 * Insert all value of @list in @hash with key @autoNumName
	 * @param list : list to get values from
	 * @param hash : hash to have values inserted
	 * @param autoNumName : name to use as key of hash
	 */
	void setFolioSequentialToHash(QStringList &list, QHash<QString, QStringList> &hash, const QString& autoNumName)
	{
		if (hash.isEmpty() || !hash.contains(autoNumName))
		{
			QStringList max;
			for (int i = 0; i < list.size(); i++)
			{
				max.append(list.at(i));
			}
			hash.insert(autoNumName,max);
		}
		else if (hash.contains(autoNumName))
		{
				//Load the String List and update it
			QStringList max = hash.value(autoNumName);
			for (int i = 0; i < list.size(); i++)
			{
				if ((list.at(i).toInt()) > max.at(i).toInt())
				{
					max.replace(i,list.at(i));
					hash.remove(autoNumName);
					hash.insert(autoNumName,max);
				}
			}
		}
	}

	/**
	 * @brief setSequential
	 * Fill seqStruct
	 * @param label : label of sequential to fill (%sequ_, %sequf_, %seqt_, ect....)
	 * @param seqStruct : struct to fill
	 * @param context : numerotation context use to know the current sequential num.
	 * @param diagram : diagram where the sequential occur, notably use when label is folio type (%sequf_, %seqtf_, %seqhf_),
	 * to keep up to date the current sequential of folio.
	 * @param hashKey : the hash key used to store the sequential for folio type.
	 */
	void setSequential(const QString& label, sequentialNumbers &seqStruct, NumerotationContext &context, Diagram *diagram, const QString& hashKey)
	{
		if (!context.isEmpty())
		{
			if (label.contains("%sequ_"))
			{
				autonum::setSequentialToList(seqStruct.unit, context,"unit");
			}
			if (label.contains("%sequf_"))
			{
				autonum::setSequentialToList(seqStruct.unit_folio, context,"unitfolio");
				autonum::setFolioSequentialToHash(seqStruct.unit_folio, diagram->m_elmt_unitfolio_max, hashKey);
			}
			if (label.contains("%seqt_"))
			{
				autonum::setSequentialToList(seqStruct.ten, context,"ten");
			}
			if (label.contains("%seqtf_"))
			{
				autonum::setSequentialToList(seqStruct.ten_folio, context,"tenfolio");
				autonum::setFolioSequentialToHash(seqStruct.ten_folio, diagram->m_elmt_tenfolio_max, hashKey);
			}
			if (label.contains("%seqh_"))
			{
				autonum::setSequentialToList(seqStruct.hundred, context,"hundred");
			}
			if (label.contains("%seqhf_"))
			{
				autonum::setSequentialToList(seqStruct.hundred_folio, context,"hundredfolio");
				autonum::setFolioSequentialToHash(seqStruct.hundred_folio, diagram->m_elmt_hundredfolio_max, hashKey);
			}
		}
	}

	/**
	 * @brief numerotationContextToFormula
	 * @param nc
	 * @return the numerotation context, converted to formula
	 */
	QString numerotationContextToFormula(const NumerotationContext &nc)
	{
		QString type;
		QString value;
		QString formula;
		int count_unit = 0;
		int count_unitf = 0;
		int count_ten = 0;
		int count_tenf = 0;
		int count_hundred = 0;
		int count_hundredf = 0;

		for(int i=0 ; i<nc.size() ; i++)
		{
			type = nc.itemAt(i).at(0);
			value = nc.itemAt(i).at(1);

			if (type == "idfolio") {
				formula.append("%id");
			}
			else if (type == "folio") {
				formula.append("%F");
			}
			else if (type == "machine") {
				formula.append("%M");
			}
			else if (type == "locmach") {
				formula.append("%LM");
			}


			else if (type == "elementcolumn") {
				formula.append("%c");
			}
			else if (type == "elementline") {
				formula.append("%l");
			}
			else if (type == "elementprefix") {
				formula.append("%prefix");
			}
			else if (type == "string") {
				formula.append(value);
			}
			else if (type == "unit") {
				count_unit++;
				formula.append("%sequ_" + QString::number(count_unit));
			}
			else if (type == "unitfolio") {
				count_unitf++;
				formula.append("%sequf_" + QString::number(count_unitf));
			}
			else if (type == "ten") {
				count_ten++;
				formula.append("%seqt_" + QString::number(count_ten));
			}
			else if (type == "tenfolio") {
				count_tenf++;
				formula.append("%seqtf_" + QString::number(count_tenf));
			}
			else if (type == "hundred") {
				count_hundred++;
				formula.append("%seqh_" + QString::number(count_hundred));
			}
			else if (type == "hundredfolio") {
				count_hundredf++;
				formula.append("%seqhf_" + QString::number(count_hundredf));
			}
		}

		return formula;
	}

	/**
	 * @brief elementPrefixForLocation
	 * @param location
	 * @return the prefix for an element represented by location, prefix can be null.
	 * Search for a prefix only if @location represent an element embedded in a project
	 */
	QString elementPrefixForLocation(const ElementsLocation &location)
	{
		if (!location.isProject())
			return QString();

		QXmlStreamReader rxml;
		QString path[10];
		int i = -1;
		ElementsLocation current_location = location;
		int dirLevel = -1;

			//Add location name to path array
		while((current_location.parent() != current_location) && (current_location.parent().fileName() != "import"))
		{
			i++;
			path[i]=current_location.fileName();
			current_location = current_location.parent();
			dirLevel++;
		}
			//User Element without folder treatment
		if (i == -1)
		{
			i = 0;
			path[i]=current_location.fileName();
			current_location = current_location.parent();
			dirLevel = 0;
		}

		// Create Custom labels if qet_labels.xml if exit in customElementsDir
		if (current_location.fileName() != "10_electric"){
		QString custom_labels = "qet_labels.xml";
		QString customfilepath = QETApp::customElementsDir().append(custom_labels);
		
		QFile file(customfilepath);
		file.isReadable();
		if (!file.open(QFile::ReadOnly | QFile::Text))
			return QString();
				rxml.setDevice(&file);
				rxml.readNext();

		while(!rxml.atEnd())
		{
			if (rxml.attributes().value("name").toString() == path[i])
			{
				rxml.readNext();
				i=i-1;
					//reached element directory
				if (i==0)
				{
					for (int j=i; j<= dirLevel; j = j +1)
					{
							//if there is a prefix available apply prefix
						if(rxml.name()=="prefix")
						{
							return rxml.readElementText();
						}
							//if there isn't a prefix available, find parent prefix in parent folder
						else
						{
							while (rxml.readNextStartElement() && rxml.name()!="prefix")
							{
								rxml.skipCurrentElement();
								rxml.readNext();
							}
						}
					}
				}
			}
			rxml.readNext();
			}
		}
		else
		{
		QString qet_labels = "10_electric/qet_labels.xml";
		QString filepath = QETApp::commonElementsDir().append(qet_labels);
		QFile file(filepath);
		file.isReadable();
		if (!file.open(QFile::ReadOnly | QFile::Text))
			return QString();
			
		rxml.setDevice(&file);
		rxml.readNext();

		while(!rxml.atEnd())
		{
			if (rxml.attributes().value("name").toString() == path[i])
			{
				rxml.readNext();
				i=i-1;
					//reached element directory
				if (i==0)
				{
					for (int j=i; j<= dirLevel; j = j +1)
					{
							//if there is a prefix available apply prefix
						if(rxml.name()=="prefix")
						{
							return rxml.readElementText();
						}
							//if there isn't a prefix available, find parent prefix in parent folder
						else
						{
							while (rxml.readNextStartElement() && rxml.name()!="prefix")
							{
								rxml.skipCurrentElement();
								rxml.readNext();
							}
						}
					}
				}
			}
			rxml.readNext();
		}
}
		return QString();
	}
}
