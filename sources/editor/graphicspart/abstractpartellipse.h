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
#ifndef ABSTRACTPARTELLIPSE_H
#define ABSTRACTPARTELLIPSE_H

#include "customelementgraphicpart.h"

class QetGraphicsHandlerItem;

/**
 * @brief The AbstractPartEllipse class
 * This is the base class for all ellipse based item like ellipse, circle, arc.
 * This class only provide common method for edit the ellipse like rect that contain the ellipse.
 * All coordinates is in item coordinate, except pos(), center(), centerX() and centerY()
 * which are in parent coordinate (or scene if no parent).
 *
 * In several points, this class is a copy of QGraphicsEllipseItem with some change,
 * (the use of Q_PROPERTY) to be easily used with Element editor.
 */
class AbstractPartEllipse : public CustomElementGraphicPart
{
		Q_OBJECT
		Q_PROPERTY(int startAngle   READ startAngle WRITE setStartAngle)
		Q_PROPERTY(int spanAngle    READ spanAngle  WRITE setSpanAngle)
		Q_PROPERTY(QRectF rect READ rect WRITE setRect)

		// constructors, destructor
	public:
		AbstractPartEllipse(QETElementEditor *editor, QGraphicsItem * parent = 0);
		virtual ~AbstractPartEllipse();

	private:
		AbstractPartEllipse(const AbstractPartEllipse &);

	signals:
		void rectChanged();
		void startAngleChanged();
		void spanAngleChanged();

		// methods
	public:
		virtual void startUserTransformation  (const QRectF &);
		virtual void handleUserTransformation (const QRectF &, const QRectF &);

			//Coordinates
		virtual QRectF  boundingRect()       const;
		virtual QRectF  sceneGeometricRect() const;
		virtual QPointF sceneTopLeft()       const;

		QRectF rect() const;
		virtual void   setRect (const QRectF &rect);
		virtual bool isUseless() const;

		int  startAngle() const {return m_start_angle;}
		virtual void setStartAngle (const int &start_angle);

		int  spanAngle () const {return m_span_angle;}
		virtual void setSpanAngle (const int &span_angle);

	protected:
		QList<QPointF> saved_points_;
		QRectF m_rect;
		qreal m_start_angle;
		qreal m_span_angle;
		QVector<QetGraphicsHandlerItem *> m_handler_vector;
};

#endif // ABSTRACTPARTELLIPSE_H
