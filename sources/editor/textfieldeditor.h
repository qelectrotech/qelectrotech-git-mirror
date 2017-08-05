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
#ifndef TEXTFIELD_EDITOR_H
#define TEXTFIELD_EDITOR_H

#include "elementitemeditor.h"

class PartTextField;
class QTextOrientationSpinBoxWidget;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

/**
	This class provides a widget to edit text fields within the element editor.
*/
class TextFieldEditor : public ElementItemEditor
{
	Q_OBJECT

		// Constructors, destructor
	public:
		TextFieldEditor(QETElementEditor *, PartTextField * = nullptr, QWidget * = nullptr);
		virtual ~TextFieldEditor();
	private:
		TextFieldEditor(const TextFieldEditor &);
	
		// attributes
	private:
		PartTextField *part;
		QLineEdit *qle_text;
		QComboBox *m_tagg_cb;
		QSpinBox *font_size;
		QDoubleSpinBox *qle_x, *qle_y;
		QCheckBox *rotate;
		QTextOrientationSpinBoxWidget *rotation_angle_;
		bool m_locked;
	
		// methods
	public:
		virtual bool setPart(CustomElementPart *);
		virtual CustomElementPart *currentPart() const;
	
	public slots:
		void updateTextFieldT();
		void updateTextFieldS();
		void updateTextFieldR();
		void updateTextFieldRotationAngle();
		void updateTagg();
		void updatePos();
		void updateForm();
	
	private:
		void activeConnections(bool);
};
#endif
