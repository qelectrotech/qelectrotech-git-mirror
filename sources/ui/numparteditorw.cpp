/*
	Copyright 2006-2016 The QElectroTech Team
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
#include <QRegExp>
#include "numparteditorw.h"
#include "ui_numparteditorw.h"

/**
 * Constructor
 */
NumPartEditorW::NumPartEditorW(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::NumPartEditorW),
	intValidator (new QIntValidator(0,99999,this))
{
	ui -> setupUi(this);
	setVisibleItems();
	setType(NumPartEditorW::unit, true);
}

/**
 * Constructor
 * Build with value of @context at position i
 */
NumPartEditorW::NumPartEditorW (NumerotationContext &context, int i, QWidget *parent):
	QWidget(parent),
	ui(new Ui::NumPartEditorW),
	intValidator (new QIntValidator(0,99999,this))
{
	ui -> setupUi(this);
	setVisibleItems();
	if(context.size()==0) setType(NumPartEditorW::unit, true);

	else {
		QStringList strl = context.itemAt(i);
		if (strl.at(0)=="unit") setType(NumPartEditorW::unit, true);
		else if (strl.at(0)=="unitfolio") setType(NumPartEditorW::unitfolio, true);
		else if (strl.at(0)=="ten") setType(NumPartEditorW::ten, true);
		else if (strl.at(0)=="tenfolio") setType(NumPartEditorW::tenfolio, true);
		else if (strl.at(0)=="hundred") setType(NumPartEditorW::hundred, true);
		else if (strl.at(0)=="hundredfolio") setType(NumPartEditorW::hundredfolio, true);
		else if (strl.at(0)=="string") setType(NumPartEditorW::string);
		else if (strl.at(0)=="idfolio") setType(NumPartEditorW::idfolio);
		else if (strl.at(0)=="folio") setType(NumPartEditorW::folio);
		else if (strl.at(0)=="elementline") setType(NumPartEditorW::elementline);
		else if (strl.at(0)=="elementcolumn") setType(NumPartEditorW::elementcolumn);
		else if (strl.at(0)=="elementprefix") setType(NumPartEditorW::elementprefix);
		ui -> value_field -> setText(strl.at(1));
		ui -> increase_spinBox -> setValue(strl.at(2).toInt());
	}
}

/**
 * Destructor
 */
NumPartEditorW::~NumPartEditorW()
{
	delete intValidator;
	delete ui;
}

void NumPartEditorW::setVisibleItems() {
	ui->type_cb->setInsertPolicy(QComboBox::InsertAtBottom);
	QStringList items;
	if (parentWidget()->parentWidget()->objectName()=="FolioTab") {
		items	<< tr("Chiffre 1") << tr("Chiffre 01")
				<< tr("Chiffre 001")
				<< tr("Texte") << tr("N° folio");
	}
	else if (parentWidget()->parentWidget()->objectName()=="ConductorTab") {
		items	<< tr("Chiffre 1") << tr("Chiffre 01")
				<< tr("Chiffre 001")
				<< tr("Texte") << tr("N° folio") << tr("Folio");
	}
	else
		items << tr("Chiffre 1") << tr("Chiffre 1 - Folio") << tr("Chiffre 01")
			  << tr("Chiffre 01 - Folio") << tr("Chiffre 001") << tr("Chiffre 001 - Folio")
			  << tr("Texte") << tr("N° folio") << tr("Folio")
			  << tr("Element Line") << tr("Element Column") << tr("Element Prefix");
	ui->type_cb->insertItems(0,items);
}

/**
 * @brief NumPartEditorW::toNumContext
 * @return the display to NumerotationContext
 */
NumerotationContext NumPartEditorW::toNumContext() {
	NumerotationContext nc;
	QString type_str;
	switch (type_) {
		case unit:
			type_str = "unit";
			break;
		case unitfolio:
			type_str = "unitfolio";
			break;
		case ten:
			type_str = "ten";
			break;
		case tenfolio:
			type_str = "tenfolio";
			break;
		case hundred:
			type_str = "hundred";
			break;
		case hundredfolio:
			type_str = "hundredfolio";
			break;
		case string:
			type_str = "string";
			break;
		case idfolio:
			type_str = "idfolio";
			break;
		case folio:
			type_str = "folio";
			break;
		case elementline:
			type_str = "elementline";
			break;
		case elementcolumn:
			type_str = "elementcolumn";
			break;
		case elementprefix:
			type_str = "elementprefix";
			break;
	}
	if (type_str == "unitfolio" || type_str == "tenfolio" || type_str == "hundredfolio")
		nc.addValue(type_str, ui -> value_field -> displayText(), ui -> increase_spinBox -> value(), ui->value_field->displayText().toInt());
	else
	nc.addValue(type_str, ui -> value_field -> displayText(), ui -> increase_spinBox -> value());
	return nc;
}

/**
 * @brief NumPartEditorW::isValid
 * @return true if value field isn't empty or if type is folio
 */
bool NumPartEditorW::isValid() {
	if (type_ == folio || type_ == idfolio || type_ == elementline ||
		type_ == elementcolumn || type_ == elementprefix) {return true;}
	else if(ui -> value_field -> text().isEmpty()) {return false;}
	else return true;
}

/**
 * @brief NumPartEditorW::on_type_cb_activated
 * Action when user change the type comboBox
 */
void NumPartEditorW::on_type_cb_activated(int) {
	if (ui->type_cb->currentText() == tr("Chiffre 1"))
		setType(unit);
	else if (ui->type_cb->currentText() == tr("Chiffre 1 - Folio"))
		setType(unitfolio);
	else if (ui->type_cb->currentText() ==  tr("Chiffre 01"))
		setType(ten);
	else if (ui->type_cb->currentText() == tr("Chiffre 01 - Folio"))
		setType(tenfolio);
	else if (ui->type_cb->currentText() == tr("Chiffre 001"))
		setType(hundred);
	else if (ui->type_cb->currentText() == tr("Chiffre 001 - Folio"))
		setType(hundredfolio);
	else if (ui->type_cb->currentText() == tr("Texte"))
		setType(string);
	else if (ui->type_cb->currentText() == tr("N° folio"))
		setType(idfolio);
	else if (ui->type_cb->currentText() == tr("Folio"))
		setType(folio);
	else if (ui->type_cb->currentText() == tr("Element Line"))
		setType(elementline);
	else if (ui->type_cb->currentText() == tr("Element Column"))
		setType(elementcolumn);
	else if (ui->type_cb->currentText() == tr("Element Prefix"))
		setType(elementprefix);
	emit changed();
}

/**
 * @brief NumPartEditorW::on_value_field_textChanged
 * emit changed when @value_field text changed
 */
void NumPartEditorW::on_value_field_textEdited() {
	emit changed();
}

/**
 * @brief NumPartEditorW::on_increase_spinBox_valueChanged
 * emit changed when @increase_spinBox value changed
 */
void NumPartEditorW::on_increase_spinBox_valueChanged(int) {
	if (!ui -> value_field -> text().isEmpty()) emit changed();
}

/**
 * @brief NumPartEditorW::setType
 * Set good behavior by type @t
 * @param t, type used
 * @param fnum, force the behavior of numeric type
 */
void NumPartEditorW::setType(NumPartEditorW::type t, bool fnum) {
//	ui -> type_cb -> setCurrentIndex(t);

	//if @t is a numeric type and preview type @type_ isn't a numeric type
	//or @fnum is true, we set numeric behavior
	if ( ((t==unit || t==unitfolio || t==ten || t==tenfolio || t==hundred || t==hundredfolio) &&
		  (type_==string || type_==folio || type_==idfolio ||
		   type_==elementcolumn || type_==elementline || type_==elementprefix))
		 || fnum) {
		ui -> value_field -> clear();
		ui -> value_field -> setEnabled(true);
		ui -> value_field -> setValidator(intValidator);
		ui -> increase_spinBox -> setEnabled(true);
		ui -> increase_spinBox -> setValue(1);
	}
	//@t isn't a numeric type
	else if (t == string || t == folio || t == idfolio || t == elementline ||
			 t == elementcolumn || t == elementprefix) {
		ui -> value_field -> clear();
		ui -> increase_spinBox -> setDisabled(true);
		if (t==string) {
			ui -> value_field -> setValidator(0);
			ui -> value_field -> setEnabled(true);
		}
		else if (t==folio) {
			ui -> value_field -> setDisabled(true);
			ui -> increase_spinBox -> setDisabled(true);
		}
		else if (t==idfolio) {
			ui -> value_field -> setDisabled(true);
			ui -> increase_spinBox -> setDisabled(true);
		}
		else if (t==elementcolumn) {
			ui -> value_field -> setDisabled(true);
			ui -> increase_spinBox -> setDisabled(true);
		}
		else if (t==elementline) {
			ui -> value_field -> setDisabled(true);
			ui -> increase_spinBox -> setDisabled(true);
		}
		else if (t==elementprefix) {
			ui -> value_field -> setDisabled(true);
			ui -> increase_spinBox -> setDisabled(true);
		}
	}
	type_= t;
}
