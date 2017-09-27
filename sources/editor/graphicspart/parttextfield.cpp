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
#include "parttextfield.h"
#include "textfieldeditor.h"
#include "elementprimitivedecorator.h"
#include "qetapp.h"
#include "QPropertyUndoCommand/qpropertyundocommand.h"
#include "elementscene.h"

/**
	Constructeur
	@param editor L'editeur d'element concerne
	@param parent Le QGraphicsItem parent de ce champ de texte
*/
PartTextField::PartTextField(QETElementEditor *editor, QGraphicsItem *parent) :
	QGraphicsTextItem(parent),
	CustomElementPart(editor),
	follow_parent_rotations(true),
	m_tagg("none"),
    previous_text()
{
	setDefaultTextColor(Qt::black);
	setFont(QETApp::diagramTextsFont());
	real_font_size_ = font().pointSize();
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemIsMovable);
	setAcceptHoverEvents(true);
	setPlainText(QObject::tr("_", "default text when adding a textfield in the element editor"));
	
	adjustItemPosition(1);
	// adjust textfield position after line additions/deletions
	connect(document(), SIGNAL(blockCountChanged(int)), this, SLOT(adjustItemPosition(int)));
	connect(document(), SIGNAL(contentsChanged()),      this, SLOT(adjustItemPosition()));
}

/// Destructeur
PartTextField::~PartTextField() {
}

/**
	Importe les proprietes d'un champ de texte depuis un element XML
	@param xml_element Element XML a lire
*/
void PartTextField::fromXml(const QDomElement &xml_element) {
	bool ok;
	int font_size = xml_element.attribute("size").toInt(&ok);
	if (!ok || font_size < 1) font_size = 20;
	
	setProperty("size", font_size);
	setPlainText(xml_element.attribute("text"));

	m_tagg = xml_element.attribute("tagg", "none");
	
	qreal default_rotation_angle = 0.0;
	if (QET::attributeIsAReal(xml_element, "rotation", &default_rotation_angle)) {
		setRotationAngle(default_rotation_angle);
	}
	
	setPos(
		xml_element.attribute("x").toDouble(),
		xml_element.attribute("y").toDouble()
	);
	
	follow_parent_rotations = (xml_element.attribute("rotate") == "true");
}

/**
	Exporte le champ de texte en XML
	@param xml_document Document XML a utiliser pour creer l'element XML
	@return un element XML decrivant le champ de texte
*/
const QDomElement PartTextField::toXml(QDomDocument &xml_document) const {
	QDomElement xml_element = xml_document.createElement("input");
	xml_element.setAttribute("x", QString("%1").arg(pos().x()));
	xml_element.setAttribute("y", QString("%1").arg(pos().y()));
	xml_element.setAttribute("text", toPlainText());
	xml_element.setAttribute("size", font().pointSize());
	xml_element.setAttribute("tagg", m_tagg);

	// angle de rotation du champ de texte
	if (rotation()) {
		xml_element.setAttribute("rotation", QString("%1").arg(rotation()));
	}
	// suivi (ou non) des rotations de l'element parent par le champ de texte
	if (follow_parent_rotations) {
		xml_element.setAttribute("rotate", "true");
	}
	return(xml_element);
}

void PartTextField::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) && (flags() & QGraphicsItem::ItemIsMovable))
    {
        QPointF pos = event->scenePos() + (m_origine_pos - event->buttonDownScenePos(Qt::LeftButton));
        event->modifiers() == Qt::ControlModifier ? setPos(pos) : setPos(elementScene()->snapToGrid(pos));
    }
    else
        QGraphicsObject::mouseMoveEvent(event);
}

void PartTextField::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        m_origine_pos = this->pos();

    QGraphicsObject::mousePressEvent(event);
}

void PartTextField::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->button() & Qt::LeftButton) && (flags() & QGraphicsItem::ItemIsMovable) && m_origine_pos != pos())
    {
        QPropertyUndoCommand *undo = new QPropertyUndoCommand(this, "pos", QVariant(m_origine_pos), QVariant(pos()));
        undo->setText(tr("Déplacer un champ texte"));
        undo->enableAnimation();
        elementScene()->undoStack().push(undo);
    }

    QGraphicsObject::mouseReleaseEvent(event);
}

/**
 * @brief PartTextField::focusInEvent
 * @param e The QFocusEvent object describing the focus gain.
    Start text edition when the item gains focus.
 */
void PartTextField::focusInEvent(QFocusEvent *e) {
	startEdition();
	QGraphicsTextItem::focusInEvent(e);
}

/**
	Permet a l'element texte de redevenir deplacable a la fin de l'edition de texte
	@param e Le QFocusEvent decrivant la perte de focus
*/
void PartTextField::focusOutEvent(QFocusEvent *e) {
	QGraphicsTextItem::focusOutEvent(e);
	endEdition();
}

/**
	@reimp QGraphicsTextItem::keyPressEvent()
	Used to handle the escape key when the event is delivered to the field, not
	to the decorator.
*/
void PartTextField::keyPressEvent(QKeyEvent *event) {
	if (event -> key() == Qt::Key_Escape) {
		endEdition();
	}
	else {
		QGraphicsTextItem::keyPressEvent(event);
	}
}

/**
	Permet a l'element texte de devenir editable lorsqu'on double-clique dessus
	@param e Le QGraphicsSceneMouseEvent qui decrit le double-clic
*/
void PartTextField::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) {
	QGraphicsTextItem::mouseDoubleClickEvent(e);
	if (e -> button() == Qt::LeftButton) {
		setEditable(true);
	}
}

/**
	Gere les changements intervenant sur cette partie
	@param change Type de changement
	@param value Valeur numerique relative au changement
*/
QVariant PartTextField::itemChange(GraphicsItemChange change, const QVariant &value) {
	if (change == QGraphicsItem::ItemPositionHasChanged || change == QGraphicsItem::ItemSceneHasChanged) {
		updateCurrentPartEditor();
	} else if (change == QGraphicsItem::ItemSelectedHasChanged) {
		if (value.toBool() == true) {
			updateCurrentPartEditor();
		}
	}
	return(QGraphicsTextItem::itemChange(change, value));
}

/**
	@return le rectangle delimitant cette partie.
*/
QRectF PartTextField::boundingRect() const {
	QRectF r = QGraphicsTextItem::boundingRect();
	r.adjust(0.0, -1.1, 0.0, 0.0);
	return(r);
}

/**
	@return true si cette partie n'est pas pertinente et ne merite pas d'etre
	conservee / enregistree.
	Un champ de texte est toujours pertinent ; cette fonction renvoie donc
	toujours false
*/
bool PartTextField::isUseless() const {
	return(false);
}

/**
	@return the minimum, margin-less rectangle this part can fit into, in scene
	coordinates. It is different from boundingRect() because it is not supposed
	to imply any margin, and it is different from shape because it is a regular
	rectangle, not a complex shape.
*/
QRectF PartTextField::sceneGeometricRect() const {
	return(sceneBoundingRect());
}

/**
	Start the user-induced transformation, provided this primitive is contained
	within the \a initial_selection_rect bounding rectangle.
*/
void PartTextField::startUserTransformation(const QRectF &initial_selection_rect) {
	Q_UNUSED(initial_selection_rect)
	saved_point_ = pos(); // scene coordinates, no need to mapFromScene()
	saved_font_size_ = real_font_size_;
}

/**
	Handle the user-induced transformation from \a initial_selection_rect to \a new_selection_rect
*/
void PartTextField::handleUserTransformation(const QRectF &initial_selection_rect, const QRectF &new_selection_rect) {
	// let's try the naive approach
	QPointF new_pos = mapPoints(initial_selection_rect, new_selection_rect, QList<QPointF>() << saved_point_).first();
	setPos(new_pos);
	
	// adjust the font size following the vertical scale factor
	qreal sy = new_selection_rect.height() / initial_selection_rect.height();
	qreal new_font_size = saved_font_size_ * sy;
	setProperty("real_size", qMax(1, qRound(new_font_size)));
}

/**
	Cette methode s'assure que la position du champ de texte est coherente
	en repositionnant son origine (c-a-d le milieu du bord gauche du champ de
	texte) a la position originale. Cela est notamment utile lorsque le champ
	de texte est agrandi ou retreci verticalement (ajout ou retrait de lignes).
	@param new_block_count Nombre de blocs dans le PartTextField
*/
void PartTextField::adjustItemPosition(int new_block_count) {
	Q_UNUSED(new_block_count);
	qreal origin_offset = boundingRect().bottom() / 2.0;
	
	QTransform base_translation;
	base_translation.translate(0.0, -origin_offset);
	setTransform(base_translation, false);
	setTransformOriginPoint(0.0, origin_offset);
}

/**
	@param editable Whether this text item should be interactively editable.
*/
void PartTextField::setEditable(bool editable) {
	if (editable) {
		setFlag(QGraphicsItem::ItemIsFocusable, true);
		setTextInteractionFlags(Qt::TextEditorInteraction);
		setFocus(Qt::MouseFocusReason);
	}
	else {
		setTextInteractionFlags(Qt::NoTextInteraction);
		setFlag(QGraphicsItem::ItemIsFocusable, false);
	}
}

/**
	Start text edition by storing the former value of the text.
*/
void PartTextField::startEdition() {
	// !previous_text.isNull() means the text is being edited
	previous_text = toPlainText();
}

/**
	End text edition, potentially generating a ChangePartCommand if the text
	has changed.
*/
void PartTextField::endEdition()
{
	if (!previous_text.isNull())
	{
			// the text was being edited
		QString new_text = toPlainText();
		if (previous_text != new_text)
		{
			QPropertyUndoCommand *undo = new QPropertyUndoCommand(this, "text", previous_text, new_text);
			undo->setText(tr("Modifier un champ texte"));
			undoStack().push(undo);
			previous_text = QString();
		}
	}
	
	// deselectionne le texte
	QTextCursor qtc = textCursor();
	qtc.clearSelection();
	setTextCursor(qtc);
	
	setEditable(false);
}
