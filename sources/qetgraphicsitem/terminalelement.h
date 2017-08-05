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
#ifndef TERMINALELEMENT_H
#define TERMINALELEMENT_H

#include "customelement.h"

class CommentItem;
class QETProject;

class TerminalElement : public CustomElement
{
		Q_OBJECT
	public:
		TerminalElement(const ElementsLocation &, QGraphicsItem * = nullptr, int * = nullptr);
		~TerminalElement() override;
		void initLink(QETProject *project) override;

	public slots:
		void updateLabel(DiagramContext old_info, DiagramContext new_info);

	private:
		CommentItem *m_comment_item;
		CommentItem *m_location_item;
};

#endif // TERMINALELEMENT_H
