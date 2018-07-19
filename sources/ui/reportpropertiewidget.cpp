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
#include "reportpropertiewidget.h"
#include "ui_reportpropertiewidget.h"

ReportPropertieWidget::ReportPropertieWidget(const QString& value, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ReportPropertieWidget)
{
	ui->setupUi(this);
	ui->line_edit->setText(value);
}

ReportPropertieWidget::~ReportPropertieWidget()
{
	delete ui;
}

void ReportPropertieWidget::toSettings(QSettings &settings, const QString &prefix) {
	settings.setValue(prefix + "label", ui->line_edit->text());
}

void ReportPropertieWidget::setReportProperties(const QString& label) {
	ui->line_edit->setText(label);
}

QString ReportPropertieWidget::ReportProperties() const {
	return ui->line_edit->text();
}
