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

#include "dialogwaiting.h"
#include "ui_dialogwaiting.h"
#include <QPushButton>


DialogWaiting *DialogWaiting::m_static_dialog = nullptr;
/**
 * @brief DialogWaiting::DialogWaiting
 * @param parent
 */
DialogWaiting::DialogWaiting(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogWaiting)
{
	ui->setupUi(this);
	setTitle(  "..." );
	setDetail( "..." );
}

/**
 * @brief DialogWaiting::~DialogWaiting
 */
DialogWaiting::~DialogWaiting() {
	delete ui;
}

/**
 * @brief DialogWaiting::setProgressBar
 * @param val is the progressBar value
 */
void DialogWaiting::setProgressBar(int val){
	ui->progressBar->setValue(val);
	qApp->processEvents();
}

/**
 * @brief DialogWaiting::setProgressReset, clear progressBar and reset
 */
void DialogWaiting::setProgressReset(){
	ui->progressBar->reset();
}

/**
 * @brief DialogWaiting::setProgressBarRange
 * @param min is the minimum of progressBar
 * @param max is the maximun of progressBar
 */
void DialogWaiting::setProgressBarRange(int min, int max){
	ui->progressBar->setRange(min,max);
	ui->progressBar->setFormat(QObject::tr("%p% effectué (%v sur %m)"));
}

/**
 * @brief DialogWaiting::setTitle of action
 * @param val is the string of action
 */
void DialogWaiting::setTitle(const QString& val){
	ui->labelTitle->setText(val);
}

/**
 * @brief DialogWaiting::setDetail of action
 * @param val is the string of detail action
 */
void DialogWaiting::setDetail(const QString& val){
	ui->label_detail->setText(val);
}

/**
 * @brief DialogWaiting::progressBarValue
 * @return The current vcalue of the progress bar
 */
int DialogWaiting::progressBarValue() const
{
	return ui->progressBar->value();
}

