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
#ifndef COMMENTITEM_H
#define COMMENTITEM_H

#include <QGraphicsObject>
#include <QPicture>

class Element;

/**
 * @brief The CommentItem class
 * This item display the comment of an element
 * at the bottom of element diagram in a rounded rect
 */
class CommentItem : public QGraphicsObject
{
		Q_OBJECT
	public:
		explicit CommentItem(Element *elmt);

		QRectF boundingRect() const override;

	signals:

	public slots:
		void autoPos();
		void updateLabel();

	protected:
		void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
		void mouseDoubleClickEvent (QGraphicsSceneMouseEvent * event ) override;
		virtual bool setTextParent ();

	private:
		void addInfo(QPainter &, QString);

	private:
		Element *m_element;
		QPicture m_picture;
		QRectF   m_bounding_rect;
		QString  m_comment;
		QString  m_location;
		QPainterPath m_shape_path;
		bool     m_text_parent;
};

#endif // COMMENTITEM_H
