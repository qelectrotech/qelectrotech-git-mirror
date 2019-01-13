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
#ifndef ELEMENTINFOPARTWIDGET_H
#define ELEMENTINFOPARTWIDGET_H

#include <QWidget>

namespace Ui {
	class ElementInfoPartWidget;
}

class ElementInfoPartWidget : public QWidget
{
	Q_OBJECT

	//METHODS
	public:
		explicit ElementInfoPartWidget(QString key, const QString& translated_key, QWidget *parent = nullptr);
		~ElementInfoPartWidget() override;

		QString key         () const {return key_;}
		void    setText     (const QString &);
		QString text        () const;
		void setPlaceHolderText (const QString &text);
		void    setFocusTolineEdit();
		void    setEnabled(bool e);
		void    setDisabled(bool d);
		void setEraseTextVisible (bool visible);
		void setEraseTextChecked (bool check);
		Qt::CheckState EraseTextCheckState ()const;

	signals:
		void textEdited  (const QString & text);
		void textChanged (const QString & text);

		//ATTRIBUTES
	private slots:
		void on_m_erase_text_clicked();
		
	private:
		Ui::ElementInfoPartWidget *ui;
		QString key_;
		bool m_show_erase = false;
};

#endif // ELEMENTINFOPARTWIDGET_H
