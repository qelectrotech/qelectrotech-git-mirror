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
#include "conductorpropertieswidget.h"
#include "ui_conductorpropertieswidget.h"
#include "conductorproperties.h"
#include "qetapp.h"
#include "qtextorientationspinboxwidget.h"

/**
 * @brief ConductorPropertiesWidget::ConductorPropertiesWidget
 * Constructor
 * @param parent, paretn widget
 */
ConductorPropertiesWidget::ConductorPropertiesWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ConductorPropertiesWidget)
{
	ui->setupUi(this);
	initWidget();
}




/**
 * @brief ConductorPropertiesWidget::ConductorPropertiesWidget
 * Constructor with properties
 * @param properties, properties to edit
 * @param parent, parent widget
 */
ConductorPropertiesWidget::ConductorPropertiesWidget(const ConductorProperties &properties, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ConductorPropertiesWidget)
{
	ui -> setupUi(this);
	initWidget();
	setProperties(properties);
}

/**
 * @brief ConductorPropertiesWidget::~ConductorPropertiesWidget
 * destructor
 */
ConductorPropertiesWidget::~ConductorPropertiesWidget()
{
	delete ui;
}

/**
 * @brief ConductorPropertiesWidget::setProperties
 * Set the properties
 * @param properties
 */
void ConductorPropertiesWidget::setProperties(const ConductorProperties &properties)
{
	if (m_properties == properties)
		return;
	
	m_properties = properties;

	setColorButton(m_properties.color);
	setColorButton2(m_properties.m_color_2);
	int index = ui -> m_line_style_cb -> findData(QPen(m_properties.style));
	if (index != -1) ui -> m_line_style_cb -> setCurrentIndex(index);

	ui->m_color_2_gb            -> setChecked  (m_properties.m_bicolor);
	ui->m_dash_size_sb          -> setValue    (m_properties.m_dash_size);
	ui->m_formula_le            -> setText    (m_properties.m_formula);
	ui->m_text_le               -> setText    (m_properties.text);
	ui->m_function_le           -> setText    (m_properties.m_function);
	ui->m_tension_protocol_le   -> setText    (m_properties.m_tension_protocol);
	ui->m_text_size_sb          -> setValue   (m_properties.text_size);
	ui->m_cond_size_sb          -> setValue   (m_properties.cond_size);
	ui->m_show_text_cb          -> setChecked (m_properties.m_show_text);
	ui->m_one_text_per_folio_cb -> setChecked (m_properties.m_one_text_per_folio);
	ui->m_earth_cb              -> setChecked (m_properties.singleLineProperties.hasGround);
	ui->m_neutral_cb            -> setChecked (m_properties.singleLineProperties.hasNeutral);
	ui->m_pen_cb                -> setChecked (m_properties.singleLineProperties.isPen());
	ui->m_phase_cb              -> setChecked (m_properties.singleLineProperties.phasesCount());
	ui->m_phase_slider          -> setValue   (m_properties.singleLineProperties.phasesCount());

	ui->m_horiz_cb->setCurrentIndex(m_properties.m_horizontal_alignment == Qt::AlignTop? 0 : 1);
	ui->m_verti_cb->setCurrentIndex(m_properties.m_vertical_alignment == Qt::AlignLeft? 0 : 1);
	m_verti_select -> setValue (m_properties.verti_rotate_text);
	m_horiz_select -> setValue (m_properties.horiz_rotate_text);

	setConductorType(m_properties.type);
	updatePreview(false);
}

/**
 * @brief ConductorPropertiesWidget::properties
 * @return the edited properties
 */
ConductorProperties ConductorPropertiesWidget::properties() const
{
	ConductorProperties properties_;
	if (ui -> m_multiwires_gb  -> isChecked()) properties_.type = ConductorProperties::Multi;
	else if (ui -> m_singlewire_gb -> isChecked()) properties_.type = ConductorProperties::Single;

	properties_.color                   = ui -> m_color_pb->palette().color(QPalette::Button);
	properties_.m_bicolor               = ui->m_color_2_gb->isChecked();
	properties_.m_color_2               = ui->m_color_2_pb->palette().color(QPalette::Button);
	properties_.m_dash_size             = ui->m_dash_size_sb->value();
	properties_.style                   = ui -> m_line_style_cb->itemData(ui->m_line_style_cb->currentIndex()).value<QPen>().style();
	properties_.m_formula               = ui->m_formula_le->text();
	properties_.text                    = ui -> m_text_le -> text();
	properties_.m_function              = ui -> m_function_le->text();
	properties_.m_tension_protocol      = ui -> m_tension_protocol_le->text();
	properties_.text_size               = ui -> m_text_size_sb -> value();
	properties_.cond_size               = ui -> m_cond_size_sb -> value();
	properties_.m_show_text             = ui -> m_show_text_cb -> isChecked();
	properties_.m_one_text_per_folio    = ui -> m_one_text_per_folio_cb -> isChecked();
	properties_.verti_rotate_text       = m_verti_select -> value();
	properties_.horiz_rotate_text       = m_horiz_select -> value();
	properties_.m_vertical_alignment    = ui->m_verti_cb->currentIndex() == 0? Qt::AlignLeft : Qt::AlignRight;
	properties_.m_horizontal_alignment  = ui->m_horiz_cb->currentIndex() == 0? Qt::AlignTop : Qt::AlignBottom;

	properties_.singleLineProperties.hasGround  = ui -> m_earth_cb -> isChecked();
	properties_.singleLineProperties.hasNeutral = ui -> m_neutral_cb -> isChecked();
	properties_.singleLineProperties.is_pen     = ui -> m_pen_cb -> isChecked();
	properties_.singleLineProperties.setPhasesCount(ui -> m_phase_cb -> isChecked() ? ui -> m_phase_sb -> value() : 0);

	return properties_;
}

/**
 * @brief ConductorPropertiesWidget::event
 * @param event
 * @return
 */
bool ConductorPropertiesWidget::event(QEvent *event)
{
	if (event->type() == QEvent::WindowActivate) {
		ui -> m_formula_le -> setFocus();
	}
	return(QWidget::event(event));
}


/**
 * @brief ConductorPropertiesWidget::setReadOnly
 * @param ro if true this widget is disabled
 */
void ConductorPropertiesWidget::setReadOnly(const bool &ro) {
	this->setDisabled(ro);
}

/**
 * @brief ConductorPropertiesWidget::addAutonumWidget
 * @param widget
 */
void ConductorPropertiesWidget::addAutonumWidget(QWidget *widget) {
	ui->m_autonum_layout->addWidget(widget);
}

/**
 * @brief ConductorPropertiesWidget::setHiddenOneTextPerFolio
 * @param hide
 */
void ConductorPropertiesWidget::setHiddenOneTextPerFolio(const bool &hide) {
	ui -> m_one_text_per_folio_cb -> setHidden(hide);
}

/**
 * @brief ConductorPropertiesWidget::setDisabledShowText
 * @param disable
 */
void ConductorPropertiesWidget::setDisabledShowText(const bool &disable) {
	ui->m_show_text_cb->setDisabled(disable==true? true : false);
}

/**
 * @brief ConductorPropertiesWidget::setHiddenAvailableAutonum
 * Hide the label, combo box and push button of available autonum
 * @param hide
 */
void ConductorPropertiesWidget::setHiddenAvailableAutonum(const bool &hide)
{
	ui->m_autonum_label->setHidden(hide);
	ui->m_available_autonum_cb->setHidden(hide);
	ui->m_edit_autonum_pb->setHidden(hide);
}

QComboBox *ConductorPropertiesWidget::autonumComboBox() const
{
	return ui->m_available_autonum_cb;
}

QPushButton *ConductorPropertiesWidget::editAutonumPushButton() const
{
	return ui->m_edit_autonum_pb;
}

/**
 * @brief ConductorPropertiesWidget::initWidget
 */
void ConductorPropertiesWidget::initWidget() {
	m_verti_select = QETApp::createTextOrientationSpinBoxWidget();
	ui -> m_text_angle_gl -> addWidget(m_verti_select, 2, 0, Qt::AlignHCenter);
	m_horiz_select = QETApp::createTextOrientationSpinBoxWidget();
	ui -> m_text_angle_gl -> addWidget(m_horiz_select, 2, 1, Qt::AlignHCenter);

	ui -> m_line_style_cb -> addItem(tr("Trait plein", "conductor style: solid line"), QPen(Qt::SolidLine));
	ui -> m_line_style_cb -> addItem(tr("Trait en pointillés", "conductor style: dashed line"), QPen(Qt::DashLine));
	ui -> m_line_style_cb -> addItem(tr("Traits et points", "conductor style: dashed and dotted line"), QPen(Qt::DashDotLine));

	ui -> m_update_preview_pb -> setHidden(true);

	connect(ui->m_multiwires_gb, &QGroupBox::toggled, [this](bool toggle) {this->ui->m_singlewire_gb->setChecked(!toggle);});
	connect(ui->m_singlewire_gb, &QGroupBox::toggled, [this](bool toggle) {this->ui->m_multiwires_gb->setChecked(!toggle);});
	connect(ui->m_formula_le, &QLineEdit::textChanged, [this](QString text) {this->ui->m_text_le->setEnabled(text.isEmpty());});
	ui->m_multiwires_gb->setChecked(true);
	ui->m_singlewire_gb->setChecked(true);
}

/**
 * @brief ConductorPropertiesWidget::setConductorType
 * Update this widget to the new conductor type
 * @param type
 */
void ConductorPropertiesWidget::setConductorType(ConductorProperties::ConductorType type) {
	switch (type) {
		case ConductorProperties::Multi:
			ui -> m_multiwires_gb -> setChecked(true);
			break;
		case ConductorProperties::Single:
			ui -> m_singlewire_gb -> setChecked(true);
			break;
		default:
			ui -> m_multiwires_gb -> setChecked(true);
			break;
	}
}

/**
 * @brief ConductorPropertiesWidget::updatePreview
 * Update the preview for single lien
 * @param b true: update from the value displayed by this widget
 *          false: update from the properties given at the constructor of this widget
 */
void ConductorPropertiesWidget::updatePreview(bool b) {
	const QRect pixmap_rect(0, 0, 96, 96);
	QPixmap pixmap(pixmap_rect.width(), pixmap_rect.height());
	QPainter painter;
	painter.begin(&pixmap);
	painter.eraseRect(pixmap_rect);
	painter.drawRect(pixmap_rect.adjusted(0,0,-1,-1));
	painter.drawLine(QLineF(0, pixmap_rect.height() / 2, pixmap_rect.width(), pixmap_rect.height() / 2));

	if (b) properties().singleLineProperties.draw(&painter, QET::Horizontal, pixmap_rect);
	else   m_properties.singleLineProperties.draw(&painter, QET::Horizontal, pixmap_rect);

	painter.end();
	ui -> m_preview_label -> setPixmap(pixmap);
}

/**
 * @brief ConductorPropertiesWidget::on_m_earth_cb_toggled
 * Update the widget when toggled
 * @param checked
 */
void ConductorPropertiesWidget::on_m_earth_cb_toggled(bool checked) {
	if (checked && ui -> m_neutral_cb -> isChecked())
		 ui -> m_pen_cb -> setEnabled(true);
	else ui -> m_pen_cb -> setDisabled(true);
}

/**
 * @brief ConductorPropertiesWidget::on_m_neutral_cb_toggled
 * Update the widget when toggled
 * @param checked
 */
void ConductorPropertiesWidget::on_m_neutral_cb_toggled(bool checked) {
	if (checked && ui -> m_earth_cb -> isChecked())
		ui -> m_pen_cb -> setEnabled(true);
	else ui -> m_pen_cb -> setDisabled(true);
}

/**
 * @brief ConductorPropertiesWidget::on_m_color_pb_clicked
 * Open a color dialog, for choose the color of conductor
 */
void ConductorPropertiesWidget::on_m_color_pb_clicked() {
	QColor color = QColorDialog::getColor(m_properties.color, this);
	if (color.isValid())
		setColorButton(color);
}

/**
 * @brief ConductorPropertiesWidget::setColorButton
 * Set m_color_pb to @color
 * @param color
 */
void ConductorPropertiesWidget::setColorButton(const QColor &color){
	QPalette palette;
	palette.setColor(QPalette::Button, color);
	ui -> m_color_pb -> setStyleSheet(QString("background-color: %1; min-height: 1.5em; border-style: outset; border-width: 2px; border-color: gray; border-radius: 4px;").arg(color.name()));
}

/**
 * @brief ConductorPropertiesWidget::setColorButton2
 * Set m_color_2_pb to @color
 * @param color
 */
void ConductorPropertiesWidget::setColorButton2(const QColor &color)
{
	QPalette palette;
	palette.setColor(QPalette::Button, color);
	ui->m_color_2_pb->setStyleSheet(QString("background-color: %1; min-height: 1.5em; border-style: outset; border-width: 2px; border-color: gray; border-radius: 4px;").arg(color.name()));
}

/**
 * @brief ConductorPropertiesWidget::on_m_update_preview_pb_clicked
 * Update the preview of single line.
 * m_update_preview_pb is a no used button and hidden, his role is only
 * to centralize signal from various widget to edit single ligne properties,
 * for update the preview
 */
void ConductorPropertiesWidget::on_m_update_preview_pb_clicked() {
	updatePreview();
}

/**
 * @brief ConductorPropertiesWidget::on_m_color_2_pb_clicked
 * Open a color dialog, for choose the second color of conductor
 */
void ConductorPropertiesWidget::on_m_color_2_pb_clicked()
{
	QColor color = QColorDialog::getColor(m_properties.m_color_2, this);
	if (color.isValid())
		setColorButton2(color);
}
