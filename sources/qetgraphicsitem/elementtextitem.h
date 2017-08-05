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
#ifndef ELEMENT_TEXT_ITEM_H
#define ELEMENT_TEXT_ITEM_H

#include "diagramtextitem.h"

class Element;

/**
	This class represents a text item attached to an element. Users can change its
	value, adjust its position (defined relatively to its parent element), and
	direct it to any angle.
*/
class ElementTextItem : public DiagramTextItem {
	Q_OBJECT
		// constructors, destructor
	public:
		ElementTextItem(Element * = nullptr);
		ElementTextItem(const QString &, Element * = nullptr);
		virtual ~ElementTextItem();
	
		// attributes
	public:
		enum { Type = UserType + 1003 };
		virtual int type () const { return Type; }
	
	private:
		Element *parent_element_;
		bool     follow_parent_rotations;
		QPointF  original_position;
		qreal    original_rotation_angle_;
		QString  tagg_;
	
		// methods
	public:	
		Element     *parentElement            () const;
		void         fromXml                  ( const QDomElement & );
		QDomElement  toXml                    ( QDomDocument & ) const;
		void         setOriginalPos           ( const QPointF & );
		QPointF      originalPos              () const;
		void         setOriginalRotationAngle ( const qreal & );
		qreal        originalRotationAngle    () const;
		virtual void setFont                  ( const QFont & );
		void         setTagg                  ( const QString &str ) {tagg_ = str;}
		QString      tagg                     () const               {return tagg_;}
		void         setFollowParentRotations ( bool fpr);
		bool         followParentRotations    () const;
	
	public slots:
		void adjustItemPosition(int = 0);
	
	protected:
		virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent *event );
		virtual void mouseMoveEvent        ( QGraphicsSceneMouseEvent *event );
		virtual void mouseReleaseEvent     ( QGraphicsSceneMouseEvent *event );
		virtual void hoverEnterEvent       ( QGraphicsSceneHoverEvent *event );
		virtual void hoverLeaveEvent       ( QGraphicsSceneHoverEvent *event );

	private:
		void build ();
};

/**
	Element text items can optionally be applied a counter-rotation when their
	parent element is rotated, thus preserving their readability.
	@return whether this text item follows the rotations of its parent element.
*/
inline bool ElementTextItem::followParentRotations() const {
	return(follow_parent_rotations);
}

/**
	Element text items can optionally be applied a counter-rotation when their
	parent element is rotated, thus preserving their readability.
	@param frp whether this text item should follow the rotations of its parent
	element.
*/
inline void ElementTextItem::setFollowParentRotations(bool frp) {
	follow_parent_rotations = frp;
}

#endif
