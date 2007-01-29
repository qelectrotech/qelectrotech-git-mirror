#ifndef ELEMENTFIXE_H
	#define ELEMENTFIXE_H
	#include "element.h"
	class ElementFixe : public Element {
		public:
		ElementFixe(QGraphicsItem * = 0, Diagram * = 0);
		int nbTerminalsMin() const;
		int nbTerminalsMax() const;
		virtual bool fromXml(QDomElement &, QHash<int, Terminal *>&);
		virtual int nbTerminals() const = 0;
		virtual void paint(QPainter *, const QStyleOptionGraphicsItem *) = 0;
		virtual QString typeId() = 0;
		virtual QString  nom() = 0;
	};
#endif
