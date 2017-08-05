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
#ifndef INDEPENDENT_TEXT_ITEM_H
#define INDEPENDENT_TEXT_ITEM_H

#include "diagramtextitem.h"

/**
	This class represents an independent text field on a particular diagram.
	It may be moved, edited, and rotated.
*/
class IndependentTextItem : public DiagramTextItem {
	Q_OBJECT
	// constructors, destructor
	public:
	IndependentTextItem();
	IndependentTextItem(const QString &);
	~IndependentTextItem() override;
	
	// attributes
	public:
	enum { Type = UserType + 1005 };
	
	// methods
	public:
	/**
		Enable the use of qgraphicsitem_cast to safely cast a QGraphicsItem into an
		IndependentTextItem.
		@return le type de QGraphicsItem
	*/
	int type() const override { return Type; }
	void fromXml(const QDomElement &) override;
	QDomElement toXml(QDomDocument &) const override;
	
	private:
	QPointF mouse_to_origin_movement_;
};
#endif
