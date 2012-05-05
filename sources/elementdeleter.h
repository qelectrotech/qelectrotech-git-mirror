/*
	Copyright 2006-2012 Xavier Guerrin
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
#ifndef ELEMENT_DELETER_H
#define ELEMENT_DELETER_H
#include "elementscategory.h"
#include "elementslocation.h"
#include <QtGui>
/**
	Cette classe represente une couche d'abstraction pour supprimer
	un element de la collection d'elements.
	Elle demande notamment confirmation a l'utilisateur.
*/
class ElementDeleter : public QWidget {
	Q_OBJECT
	// constructeurs, destructeur
	public:
	ElementDeleter(const ElementsLocation &, QWidget * = 0);
	virtual ~ElementDeleter();
	private:
	ElementDeleter(const ElementsCategory &);
	
	// methodes
	public slots:
	bool exec();
	
	// attributs
	private:
	ElementsCollectionItem *element;
};
#endif
