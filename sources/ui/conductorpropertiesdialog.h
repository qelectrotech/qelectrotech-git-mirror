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

#ifndef CONDUCTORPROPERTIESDIALOG_H
#define CONDUCTORPROPERTIESDIALOG_H

#include <QDialog>
#include "conductorproperties.h"

class Conductor;
class ConductorPropertiesWidget;

namespace Ui {
	class ConductorPropertiesDialog;
}

class ConductorPropertiesDialog : public QDialog
{
		Q_OBJECT

	public:
		explicit ConductorPropertiesDialog (Conductor *conductor, QWidget *parent = nullptr);
		static void PropertiesDialog (Conductor *conductor, QWidget *parent = nullptr);
		~ConductorPropertiesDialog() override;

		ConductorProperties properties() const;
		bool applyAll() const;

	private:
		Ui::ConductorPropertiesDialog *ui;
		ConductorPropertiesWidget *m_cpw;
};

#endif // CONDUCTORPROPERTIESDIALOG_H
