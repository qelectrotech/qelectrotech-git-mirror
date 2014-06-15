/*
	Copyright 2006-2014 The QElectroTech Team
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
#include "diagramcommands.h"
#include "qetgraphicsitem/element.h"
#include "qetgraphicsitem/conductor.h"
#include "qetgraphicsitem/conductortextitem.h"
#include "diagram.h"
#include "qetgraphicsitem/elementtextitem.h"
#include "qetgraphicsitem/independenttextitem.h"
#include "qgimanager.h"
#include "diagram.h"
#include "qetgraphicsitem/diagramtextitem.h"
#include "qetgraphicsitem/diagramimageitem.h"
#include "conductorautonumerotation.h"

/**
	Constructeur
	@param d Schema auquel on ajoute un element
	@param elmt Element ajoute
	@param p Position a laquelle l'element est ajoute
	@param parent QUndoCommand parent
*/
AddElementCommand::AddElementCommand(
	Diagram *d,
	Element *elmt,
	const QPointF &p,
	QUndoCommand *parent
) :
	QUndoCommand(QString(QObject::tr("ajouter 1 %1", "undo caption - %1 is an element name")).arg(elmt -> name()), parent),
	element(elmt),
	diagram(d),
	position(p)
{
	diagram -> qgiManager().manage(element);
}

/// Destructeur
AddElementCommand::~AddElementCommand() {
	diagram -> qgiManager().release(element);
}

/// Annule l'ajout
void AddElementCommand::undo() {
	diagram -> showMe();
	diagram -> removeElement(element);
}

/// Refait l'ajout
void AddElementCommand::redo() {
	diagram -> showMe();
	diagram -> addElement(element);
	element -> setPos(position);
	element -> setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

/**
	Constructeur
	@param dia Schema auquel on ajoute du texte
	@param text Texte ajoute
	@param pos Position a laquelle le texte est ajoute
	@param parent QUndoCommand parent
*/
AddTextCommand::AddTextCommand(Diagram *dia, IndependentTextItem *text, const QPointF &pos, QUndoCommand *parent) :
	QUndoCommand(QObject::tr("Ajouter un champ de texte", "undo caption"), parent),
	textitem(text),
	diagram(dia),
	position(pos)
{
	diagram -> qgiManager().manage(textitem);
}

/// Destructeur
AddTextCommand::~AddTextCommand() {
	diagram -> qgiManager().release(textitem);
}

/// Annule l'ajout
void AddTextCommand::undo() {
	diagram -> showMe();
	diagram -> removeIndependentTextItem(textitem);
}

/// Refait l'ajout
void AddTextCommand::redo() {
	diagram -> showMe();
	diagram -> addIndependentTextItem(textitem);
	textitem -> setPos(position);
}

/**
	Constructeur
	@param dia Schema auquel on ajoute une image
	@param image Image ajoute
	@param pos Position a laquelle l'image est ajoute
	@param parent QUndoCommand parent
 */
AddImageCommand::AddImageCommand(Diagram *dia, DiagramImageItem *image, const QPointF &pos, QUndoCommand *parent):
	QUndoCommand(QObject::tr("Ajouter une image", "undo caption"), parent),
	imageitem(image),
	diagram(dia),
	position(pos)
{
	diagram -> qgiManager().manage(imageitem);
}

///Destructor
AddImageCommand::~AddImageCommand() {
	diagram -> qgiManager().release(imageitem);
}

///Annule l'ajout
void AddImageCommand::undo() {
	diagram -> showMe();
	diagram -> removeItem(imageitem);
}

///Refait l'ajout
void AddImageCommand::redo() {
	diagram -> showMe();
	diagram -> addDiagramImageItem(imageitem);
	imageitem -> setPos(position - imageitem -> boundingRect().center());
}

/**
	Constructeur
	@param dia Schema auquel on ajoute une shape
	@param shape Shape ajoute
	@param pos Position a laquelle l'shape est ajoute
	@param parent QUndoCommand parent
 */
AddShapeCommand::AddShapeCommand(Diagram *dia, QetShapeItem *shape, const QPointF &pos, QUndoCommand *parent):
	QUndoCommand(QObject::tr("Ajouter une Shape", "undo caption"), parent),
	shapeitem(shape),
	diagram(dia),
	position(pos)
{
	diagram -> qgiManager().manage(shapeitem);
}

///Destructor
AddShapeCommand::~AddShapeCommand() {
	diagram -> qgiManager().release(shapeitem);
}

///Annule l'ajout
void AddShapeCommand::undo() {
	diagram -> showMe();
	diagram -> removeItem(shapeitem);
}

///Refait l'ajout
void AddShapeCommand::redo() {
	diagram -> showMe();
	if (shapeitem ->diagram() != diagram)
		diagram -> addItem(shapeitem);
	//diagram -> addDiagramImageItem(imageitem);
	//imageitem -> setPos(position - imageitem -> boundingRect().center());
}


/**
	Constructeur
	@param d Schema auquel on ajoute un conducteur
	@param c Conducteur ajoute
	@param parent QUndoCommand parent
*/
AddConductorCommand::AddConductorCommand(
	Diagram *d,
	Conductor *c,
	QUndoCommand *parent
) :
	QUndoCommand(QObject::tr("ajouter un conducteur", "undo caption"), parent),
	conductor(c),
	diagram(d)
{
	diagram -> qgiManager().manage(conductor);
}

/// Destructeur
AddConductorCommand::~AddConductorCommand() {
	diagram -> qgiManager().release(conductor);
}

/// Annule l'ajout
void AddConductorCommand::undo() {
	diagram -> showMe();
	diagram -> removeConductor(conductor);
}

/// Refait l'ajout
void AddConductorCommand::redo() {
	diagram -> showMe();
	diagram -> addConductor(conductor);
}

/**
	Constructeur
	@param dia Schema dont on supprime des elements et conducteurs
	@param content Contenu supprime
	@param parent QUndoCommand parent
*/
DeleteElementsCommand::DeleteElementsCommand(
	Diagram *dia,
	const DiagramContent &content,
	QUndoCommand *parent
) :
	QUndoCommand(parent),
	removed_content(content),
	diagram(dia)
{
	setText(
		QString(
			QObject::tr(
				"supprimer %1",
				"undo caption - %1 is a sentence listing the removed content"
			)
		).arg(removed_content.sentence(DiagramContent::All))
	);
	diagram -> qgiManager().manage(removed_content.items(DiagramContent::All));
}

/// Destructeur
DeleteElementsCommand::~DeleteElementsCommand() {
	diagram -> qgiManager().release(removed_content.items(DiagramContent::All));
}

/// annule les suppressions
void DeleteElementsCommand::undo() {
	diagram -> showMe();
	// remet les elements
	foreach(Element *e, removed_content.elements) {
		diagram -> addElement(e);
	}
	
	// remet les conducteurs
	foreach(Conductor *c, removed_content.conductors(DiagramContent::AnyConductor)) {
		diagram -> addConductor(c);
	}
	
	// remet les textes
	foreach(IndependentTextItem *t, removed_content.textFields) {
		diagram -> addIndependentTextItem(t);
	}

	foreach(DiagramImageItem *dii, removed_content.images) {
		diagram -> addItem(dii);
	}

	foreach(QetShapeItem *dsi, removed_content.shapes) {
		diagram -> addItem(dsi);
	}
}

/// refait les suppressions
void DeleteElementsCommand::redo() {
	diagram -> showMe();
	// enleve les conducteurs
	foreach(Conductor *c, removed_content.conductors(DiagramContent::AnyConductor)) {
		diagram -> removeConductor(c);
	}
	
	// enleve les elements
	foreach(Element *e, removed_content.elements) {
		diagram -> removeElement(e);
	}
	
	// enleve les textes
	foreach(IndependentTextItem *t, removed_content.textFields) {
		diagram -> removeIndependentTextItem(t);
	}

	//enleve les images
	foreach(DiagramImageItem *dii, removed_content.images) {
		diagram -> removeItem(dii);
	}

	//enleve les shapes
	foreach(QetShapeItem *dsi, removed_content.shapes) {
		diagram -> removeItem(dsi);
	}
}

/**
	Constructeur
	@param dia Schema sur lequel on colle les elements et conducteurs
	@param c Contenu a coller sur le schema
	@param parent QUndoCommand parent
*/
PasteDiagramCommand::PasteDiagramCommand(
	Diagram *dia,
	const DiagramContent &c,
	QUndoCommand *parent
) :
	QUndoCommand(parent),
	content(c),
	diagram(dia),
	filter(DiagramContent::Elements|DiagramContent::TextFields|DiagramContent::Images|DiagramContent::ConductorsToMove),
	first_redo(true)
{
	
	setText(
		QString(
			QObject::tr(
				"coller %1",
				"undo caption - %1 is a sentence listing the content to paste"
			).arg(content.sentence(filter))
		)
	);
	diagram -> qgiManager().manage(content.items(filter));
}

/// Destructeur
PasteDiagramCommand::~PasteDiagramCommand() {
	diagram -> qgiManager().release(content.items(filter));
}

/// annule le coller
void PasteDiagramCommand::undo() {
	diagram -> showMe();
	// remove the conductors
	foreach(Conductor *c, content.conductorsToMove) diagram -> removeConductor(c);
	
	// remove the elements
	foreach(Element *e, content.elements) diagram -> removeElement(e);
	
	// remove the texts
	foreach(IndependentTextItem *t, content.textFields) diagram -> removeIndependentTextItem(t);

	// remove the images
	foreach(DiagramImageItem *dii, content.images) diagram -> removeItem(dii);
}

/// refait le coller
void PasteDiagramCommand::redo() {
	diagram -> showMe();
	if (first_redo) {
		first_redo = false;
		//this is the first paste, we make new uuid for each element
		//because old uuid are the uuid of the copied element
		foreach(Element *e, content.elements) e->newUuid();
	}
	else {
		// paste the elements
		foreach(Element *e, content.elements) diagram -> addElement(e);
		
		// paste the conductors
		foreach(Conductor *c, content.conductorsToMove) diagram -> addConductor(c);
		
		// paste the texts
		foreach(IndependentTextItem *t, content.textFields) diagram -> addIndependentTextItem(t);

		// paste the images
		foreach(DiagramImageItem *dii, content.images) diagram -> addDiagramImageItem(dii);
	}
	foreach(Element *e, content.elements) e -> setSelected(true);
	foreach(Conductor *c, content.conductorsToMove) c -> setSelected(true);
	foreach(IndependentTextItem *t, content.textFields) t -> setSelected(true);
	foreach(DiagramImageItem *dii, content.images) dii -> setSelected(true);
}

/**
	Constructeur
	@param dia Schema dont on coupe des elements et conducteurs
	@param content Contenu coupe
	@param parent QUndoCommand parent
*/
CutDiagramCommand::CutDiagramCommand(
	Diagram *dia,
	const DiagramContent &content,
	QUndoCommand *parent
) : 
	DeleteElementsCommand(dia, content, parent)
{
	setText(
		QString(
			QObject::tr(
				"couper %1",
				"undo caption - %1 is a sentence listing the content to cut"
			).arg(content.sentence(DiagramContent::All))
		)
	);
}

/// Destructeur
CutDiagramCommand::~CutDiagramCommand() {
}

/**
 * @brief MoveElementsCommand::MoveElementsCommand
 * Constructor
 * @param dia diagram
 * @param diagram_content diagram content (contain all items to be moved)
 * @param m movement to applied
 * @param parent parent undo command
 */
MoveElementsCommand::MoveElementsCommand(
	Diagram *dia,
	const DiagramContent &diagram_content,
	const QPointF &m,
	QUndoCommand *parent
) :
	QUndoCommand(parent),
	diagram(dia),
	content_to_move(diagram_content),
	movement(m),
	first_redo(true)
{
	QString moved_content_sentence = content_to_move.sentence(
		DiagramContent::Elements |
		DiagramContent::TextFields |
		DiagramContent::ConductorsToUpdate |
		DiagramContent::ConductorsToMove |
		DiagramContent::Images |
		DiagramContent::Shapes
	);
	
	setText(
		QString(
			QObject::tr(
				"d\351placer %1",
				"undo caption - %1 is a sentence listing the moved content"
			).arg(moved_content_sentence)
		)
	);
}

/**
 * @brief MoveElementsCommand::~MoveElementsCommand
 * Destructor
 */
MoveElementsCommand::~MoveElementsCommand() {
}

/**
 * @brief MoveElementsCommand::undo
 */
void MoveElementsCommand::undo() {
	diagram -> showMe();
	move(-movement);
}

/**
 * @brief MoveElementsCommand::redo
 */
void MoveElementsCommand::redo() {
	diagram -> showMe();
	if (first_redo) first_redo = false;
	else move(movement);
}

/**
 * @brief MoveElementsCommand::move
 * Move item and conductor to @actual_movement
 * @param actual_movement movement to be applied
 */
void MoveElementsCommand::move(const QPointF &actual_movement) {
	typedef DiagramContent dc;

	//Move every movable item, except conductor
	foreach (QGraphicsItem *qgi, content_to_move.items(dc::Elements | dc::TextFields | dc::Images | dc::Shapes)) {
		qgi -> setPos(qgi->pos() + actual_movement);
	}
	
	// Move some conductors
	foreach(Conductor *conductor, content_to_move.conductorsToMove) {
		conductor -> setPos(conductor -> pos() + actual_movement);
	}
	
	// Recalcul the path of other conductor
	foreach(Conductor *conductor, content_to_move.conductorsToUpdate) {
		conductor -> updatePath();
	}
}

/**
	Ajoute un champ de texte de conducteur a deplacer
	@param text_item Champ de texte a deplacer
	@param old_pos Position du champ de texte avant le deplacement
	@param new_pos Position du champ de texte apres le deplacement
*/
void MoveElementsCommand::addConductorTextItemMovement(ConductorTextItem *text_item, const QPointF &old_pos, const QPointF &new_pos) {
	if (moved_conductor_texts_.contains(text_item)) return;
	if (!text_item -> wasMovedByUser()) return;
	if (new_pos == old_pos) return;
	moved_conductor_texts_.insert(
		text_item,
		qMakePair(old_pos, new_pos)
	);
}

/**
	Constructeur
	@param diagram Schema sur lequel on deplace des champs de texte
	@param texts Liste des textes deplaces
	@param m translation subie par les elements
	@param parent QUndoCommand parent
*/
MoveElementsTextsCommand::MoveElementsTextsCommand(
	Diagram *diagram,
	const QSet<ElementTextItem *> &texts,
	const QPointF &m,
	QUndoCommand *parent
) :
	QUndoCommand(parent),
	diagram(diagram),
	texts_to_move(texts),
	movement(m),
	first_redo(true)
{
	QString moved_content_sentence = QET::ElementsAndConductorsSentence(0, 0, texts_to_move.count());
	
	setText(
		QString(
			QObject::tr(
				"d\351placer %1",
				"undo caption - %1 is a sentence listing the moved content"
			).arg(moved_content_sentence)
		)
	);
}

/// Destructeur
MoveElementsTextsCommand::~MoveElementsTextsCommand() {
}

/// annule le deplacement
void MoveElementsTextsCommand::undo() {
	diagram -> showMe();
	move(-movement);
}

/// refait le deplacement
void MoveElementsTextsCommand::redo() {
	diagram -> showMe();
	if (first_redo) first_redo = false;
	else move(movement);
}

/**
	deplace les elements et conducteurs
	@param actual_movement translation a effectuer sur les elements et conducteurs
*/
void MoveElementsTextsCommand::move(const QPointF &actual_movement) {
	// deplace les textes
	foreach(ElementTextItem *text, texts_to_move) {
		QPointF applied_movement = text -> mapMovementToParent(text -> mapMovementFromScene(actual_movement));
		text -> setPos(text -> pos() + applied_movement);
	}
}

/**
	Constructeur
	@param diagram Schema sur lequel on deplace des champs de texte
	@param texts Textes deplaces : chaque ConductorTextItem est associe a un
	couple de position : avant et apres le deplacement
	@param m translation subie par les elements
	@param parent QUndoCommand parent
*/
MoveConductorsTextsCommand::MoveConductorsTextsCommand(
	Diagram *diagram,
	QUndoCommand *parent
) :
	QUndoCommand(parent),
	diagram(diagram),
	first_redo(true)
{
}

/// Destructeur
MoveConductorsTextsCommand::~MoveConductorsTextsCommand() {
}

/// annule le deplacement
void MoveConductorsTextsCommand::undo() {
	diagram -> showMe();
	foreach(ConductorTextItem *cti, texts_to_move_.keys()) {
		QPointF movement = texts_to_move_[cti].first;
		bool was_already_moved = texts_to_move_[cti].second;
		
		cti -> forceMovedByUser(was_already_moved);
		if (was_already_moved) {
			cti -> setPos(cti -> pos() - movement);
		}
	}
}

/// refait le deplacement
void MoveConductorsTextsCommand::redo() {
	diagram -> showMe();
	if (first_redo) {
		first_redo = false;
	} else {
		foreach(ConductorTextItem *cti, texts_to_move_.keys()) {
			QPointF movement = texts_to_move_[cti].first;
			
			cti -> forceMovedByUser(true);
			cti -> setPos(cti -> pos() + movement);
		}
	}
}

/**
	Ajout un mouvement de champ de texte a cet objet
	@param text_item Champ de texte deplace ; si celui-ci est deja connu de l'objet d'annulation, il sera ignore
	@param old_pos Position du champ de texte avant le mouvement
	@param new_pos Position du champ de texte apres le mouvement
	@param alread_moved true si le champ de texte etait deja a une position personnalisee par l'utilisateur, false sinon
*/
void MoveConductorsTextsCommand::addTextMovement(ConductorTextItem *text_item, const QPointF &old_pos, const QPointF &new_pos, bool already_moved) {
	// si le champ de texte est deja connu de l'objet d'annulation, il sera ignore
	if (texts_to_move_.contains(text_item)) return;
	
	// on memorise le champ de texte, en l'associant au mouvement effectue et a son etat avant le deplacement
	texts_to_move_.insert(text_item, qMakePair(new_pos - old_pos, already_moved));
	
	// met a jour la description de l'objet d'annulation
	regenerateTextLabel();
}

/**
	Genere la description de l'objet d'annulation
*/
void MoveConductorsTextsCommand::regenerateTextLabel() {
	QString moved_content_sentence = QET::ElementsAndConductorsSentence(0, 0, texts_to_move_.count());
	
	setText(
		QString(
			QObject::tr(
				"d\351placer %1",
				"undo caption - %1 is a sentence listing the moved content"
			).arg(moved_content_sentence)
		)
	);
}

/**
	Constructeur
	@param dti Champ de texte modifie
	@param before texte avant
	@param after texte apres
	@param parent QUndoCommand parent
*/
ChangeDiagramTextCommand::ChangeDiagramTextCommand(
	DiagramTextItem *dti,
	const QString &before,
	const QString &after,
	QUndoCommand *parent
) :
	QUndoCommand(QObject::tr("modifier le texte", "undo caption"), parent),
	text_item(dti),
	text_before(before),
	text_after(after),
	first_redo(true),
	diagram(dti->diagram())
{
}

/// destructeur
ChangeDiagramTextCommand::~ChangeDiagramTextCommand() {
}

/// annule la modification de texte
void ChangeDiagramTextCommand::undo() {
	diagram -> showMe();
	text_item -> setHtml(text_before);
}

/// refait la modification de texte
void ChangeDiagramTextCommand::redo() {
	diagram -> showMe();
	if (first_redo) {
		first_redo = false;
	} else {
		text_item -> setHtml(text_after);
	}
}

/**
	Constructeur
	@param elements Elements a pivoter associes a leur orientation d'origine
	@param texts Textes a pivoter
	@param parent QUndoCommand parent
*/
RotateElementsCommand::RotateElementsCommand(const QList<Element *> &elements, const QList<DiagramTextItem *> &texts, const QList<DiagramImageItem *> &images, QUndoCommand *parent) :
	QUndoCommand(parent),
	elements_to_rotate(elements),
	texts_to_rotate(texts),
	images_to_rotate(images),
	applied_rotation_angle_(90.0)
{
	if(elements_to_rotate.size()) diagram = elements_to_rotate.first()->diagram();
	else if (texts_to_rotate.size()) diagram = texts_to_rotate.first()->diagram();
	else if (images_to_rotate.size()) diagram = images_to_rotate.first()->diagram();

	setText(
		QString(
			QObject::tr(
				"pivoter %1",
				"undo caption - %1 is a sentence listing the rotated content"
			)
		).arg(QET::ElementsAndConductorsSentence(elements.count(), 0, texts.count(), images.count()))
	);
}

/// Destructeur
RotateElementsCommand::~RotateElementsCommand() {
}

/// defait le pivotement
void RotateElementsCommand::undo() {
	diagram -> showMe();
	foreach(Element *e, elements_to_rotate) {
		e -> rotateBy(-applied_rotation_angle_);
	}
	foreach(DiagramTextItem *dti, texts_to_rotate) {
		//ConductorTextItem have a default rotation angle, we apply a specific treatment
		if (ConductorTextItem *cti = qgraphicsitem_cast<ConductorTextItem *>(dti)) {
			cti -> forceRotateByUser(previous_rotate_by_user_[cti]);
			(cti -> wasRotateByUser()) ? cti -> rotateBy(-applied_rotation_angle_) :
										 cti -> parentConductor() -> adjustTextItemPosition();
		}
		else {dti -> rotateBy(-applied_rotation_angle_);}
	}
	foreach(DiagramImageItem *dii, images_to_rotate) dii -> rotateBy(-applied_rotation_angle_);
}

/// refait le pivotement
void RotateElementsCommand::redo() {
	diagram -> showMe();
	foreach(Element *e, elements_to_rotate) {
		e -> rotateBy(applied_rotation_angle_);
	}
	foreach(DiagramTextItem *dti, texts_to_rotate) {
		//we grab the previous rotation by user of each ConductorTextItem
		if (ConductorTextItem *cti = qgraphicsitem_cast<ConductorTextItem *>(dti)) {
			previous_rotate_by_user_.insert(cti, cti -> wasRotateByUser());
			cti -> forceRotateByUser(true);
		}
		dti -> rotateBy(applied_rotation_angle_);
	}
	foreach(DiagramImageItem *dii, images_to_rotate) dii -> rotateBy(applied_rotation_angle_);
}

/**
	Constructeur
	@param previous_state Hash associant les textes impactes par l'action et leur angle de rotation avant l'action
	@param applied_rotation Nouvel angle de rotation, a appliquer au textes concernes
	@param parent QUndoCommand parent
*/
RotateTextsCommand::RotateTextsCommand(const QHash<DiagramTextItem *, double> &previous_state, double applied_rotation, QUndoCommand *parent) :
	QUndoCommand(parent),
	texts_to_rotate(previous_state),
	applied_rotation_angle_(applied_rotation),
	diagram(previous_state.key(0)->diagram())
{
	defineCommandName();
}

/**
	Constructeur
	@param texts Liste des textes impactes par l'action. L'objet retiendra leur angle de rotation au moment de sa construction.
	@param applied_rotation Nouvel angle de rotation, a appliquer au textes concernes
	@param parent QUndoCommand parent
*/
RotateTextsCommand::RotateTextsCommand(const QList<DiagramTextItem *> &texts, double applied_rotation, QUndoCommand *parent) :
	QUndoCommand(parent),
	applied_rotation_angle_(applied_rotation),
	diagram(texts.first()->diagram())
{
	foreach(DiagramTextItem *text, texts) {
		texts_to_rotate.insert(text, text -> rotationAngle());
	}
	defineCommandName();
}

/**
	Destructeur
*/
RotateTextsCommand::~RotateTextsCommand() {
}

/**
	Annule la rotation des textes
*/
void RotateTextsCommand::undo() {
	diagram -> showMe();
	foreach(DiagramTextItem *text, texts_to_rotate.keys()) {
		if (ConductorTextItem *cti = qgraphicsitem_cast<ConductorTextItem *>(text))
			cti -> forceRotateByUser(previous_rotate_by_user_[cti]);
		text -> setRotationAngle(texts_to_rotate[text]);
	}
}

/**
	Applique l'angle de rotation aux textes
*/
void RotateTextsCommand::redo() {
	diagram -> showMe();
	foreach(DiagramTextItem *text, texts_to_rotate.keys()) {
		if (ConductorTextItem *cti = qgraphicsitem_cast<ConductorTextItem *>(text)) {
			//we grab the previous rotation by user of each ConductorTextItem
			previous_rotate_by_user_.insert(cti, cti -> wasRotateByUser());
			cti -> forceRotateByUser(true);
		}
		text -> setRotationAngle(applied_rotation_angle_);
	}
}

/**
	Definit le nom de la commande d'annulation
*/
void RotateTextsCommand::defineCommandName() {
	setText(
		QString(
			QObject::tr(
				"orienter %1 \340 %2\260",
				"undo caption - %1 looks like '42 texts', %2 is a rotation angle"
			)
		).arg(QET::ElementsAndConductorsSentence(0, 0, texts_to_rotate.count()))
		.arg(applied_rotation_angle_)
	);
}

/**
	Constructeur
	@param c Conducteur modifie
	@param old_p ancien profil du conducteur
	@param new_p nouveau profil du conducteur
	@param path_t Trajectoire du trajet modifie
	@param parent QUndoCommand parent
*/
ChangeConductorCommand::ChangeConductorCommand(
	Conductor *c,
	const ConductorProfile &old_p,
	const ConductorProfile &new_p,
	Qt::Corner path_t,
	QUndoCommand *parent
) :
	QUndoCommand(QObject::tr("modifier un conducteur", "undo caption"), parent),
	conductor(c),
	old_profile(old_p),
	new_profile(new_p),
	path_type(path_t),
	first_redo(true),
	diagram (c->diagram())
{
}

/// Destructeur
ChangeConductorCommand::~ChangeConductorCommand() {
}

/// Annule la modification du conducteur
void ChangeConductorCommand::undo() {
	diagram -> showMe();
	conductor -> setProfile(old_profile, path_type);
	conductor -> textItem() -> setPos(text_pos_before_mov_);
}

/// Refait la modification du conducteur
void ChangeConductorCommand::redo() {
	diagram -> showMe();
	if (first_redo) {
		first_redo = false;
	} else {
		conductor -> setProfile(new_profile, path_type);
		conductor -> textItem() -> setPos(text_pos_after_mov_);
	}
}

/**
	Integre dans cet objet d'annulation le repositionnement du champ de texte
	du conducteur
	@param pos_before Position du texte avant la modification du conducteur
	@param pos_after  Position du texte apres la modification du conducteur
*/
void ChangeConductorCommand::setConductorTextItemMove(const QPointF &pos_before, const QPointF &pos_after) {
	text_pos_before_mov_ = pos_before;
	text_pos_after_mov_  = pos_after;
}

/**
	Constructeur
	@param cp Conducteurs reinitialises, associes a leur ancien profil
	@param parent QUndoCommand parent
*/
ResetConductorCommand::ResetConductorCommand(
	const QHash<Conductor *, ConductorProfilesGroup> &cp,
	QUndoCommand *parent
) :
	QUndoCommand(parent),
	conductors_profiles(cp),
	diagram(cp.keys().first()->diagram())
{
	setText(
		QObject::tr(
			"R\351initialiser %1",
			"undo caption - %1 is a sentence listing the reset content"
		).arg(QET::ElementsAndConductorsSentence(0, cp.count()))
	);
}

/// Destructeur
ResetConductorCommand::~ResetConductorCommand() {
}

/// Annule la reinitialisation des conducteurs
void ResetConductorCommand::undo() {
	diagram -> showMe();
	foreach(Conductor *c, conductors_profiles.keys()) {
		c -> setProfiles(conductors_profiles[c]);
	}
}

/// Refait la reinitialisation des conducteurs
void ResetConductorCommand::redo() {
	diagram -> showMe();
	foreach(Conductor *c, conductors_profiles.keys()) {
		c -> setProfiles(ConductorProfilesGroup());
	}
}

/**
	Constructeur
	@param d Schema dont on modifie le cartouche
	@param old_ip Anciennes proprietes du cartouche
	@param new_ip Nouvelles proprietes du cartouche
	@param parent QUndoCommand parent
*/
ChangeTitleBlockCommand::ChangeTitleBlockCommand(
	Diagram *d,
	const TitleBlockProperties &old_ip,
	const TitleBlockProperties &new_ip,
	QUndoCommand *parent
) :
	QUndoCommand(QObject::tr("modifier le cartouche", "undo caption"), parent),
	diagram(d),
	old_titleblock(old_ip),
	new_titleblock(new_ip)
{
}

/// Destructeur
ChangeTitleBlockCommand::~ChangeTitleBlockCommand() {
}

/// Annule la modification de cartouche
void ChangeTitleBlockCommand::undo() {
	diagram -> showMe();
	diagram -> border_and_titleblock.importTitleBlock(old_titleblock);
	diagram -> invalidate(diagram -> border());
}

/// Refait la modification de cartouche
void ChangeTitleBlockCommand::redo() {
	diagram -> showMe();
	diagram -> border_and_titleblock.importTitleBlock(new_titleblock);
	diagram -> invalidate(diagram -> border());
}

/**
	Constructeur
	@param dia Schema modifie
	@param old_bp Anciennes proprietes du cadre du schema
	@param new_bp Nouvelles proprietes du cadre du schema
	@param parent QUndoCommand parent
*/
ChangeBorderCommand::ChangeBorderCommand(Diagram *dia, const BorderProperties &old_bp, const BorderProperties &new_bp, QUndoCommand *parent) :
	QUndoCommand(QObject::tr("modifier les dimensions du sch\351ma", "undo caption"), parent),
	diagram(dia),
	old_properties(old_bp),
	new_properties(new_bp)
{
}

/// Destructeur
ChangeBorderCommand::~ChangeBorderCommand() {
}

/// Annule les changements apportes au schema
void ChangeBorderCommand::undo() {
	diagram -> showMe();
	diagram -> border_and_titleblock.importBorder(old_properties);
}

/// Refait les changements apportes au schema
void ChangeBorderCommand::redo() {
	diagram -> showMe();
	diagram -> border_and_titleblock.importBorder(new_properties);
}

/**
	Constructeur
	@param c Le conducteur dont on modifie les proprietes
	@param parent QUndoCommand parent
*/
ChangeConductorPropertiesCommand::ChangeConductorPropertiesCommand(Conductor *c, QUndoCommand *parent) :
	QUndoCommand(QObject::tr("modifier les propri\351t\351s d'un conducteur", "undo caption"), parent),
	conductor(c),
	old_settings_set(false),
	new_settings_set(false),
	diagram(c->diagram())
{
}

/// Destructeur
ChangeConductorPropertiesCommand::~ChangeConductorPropertiesCommand() {
}

/// definit l'ancienne configuration
void ChangeConductorPropertiesCommand::setOldSettings(const ConductorProperties &properties) {
	old_properties = properties;
	old_settings_set = true;
}

/// definit la nouvelle configuration
void ChangeConductorPropertiesCommand::setNewSettings(const ConductorProperties &properties) {
	new_properties = properties;
	new_settings_set = true;
}

/**
	Annule les changements - Attention : les anciens et nouveaux parametres
	doivent avoir ete definis a l'aide de setNewSettings et setOldSettings
*/
void ChangeConductorPropertiesCommand::undo() {
	diagram -> showMe();
	if (old_settings_set && new_settings_set) {
		conductor -> setProperties(old_properties);
		conductor -> update();
	}
}

/**
	Refait les changements - Attention : les anciens et nouveaux parametres
	doivent avoir ete definis a l'aide de setNewSettings et setOldSettings
*/
void ChangeConductorPropertiesCommand::redo() {
	diagram -> showMe();
	if (old_settings_set && new_settings_set) {
		conductor -> setProperties(new_properties);
		conductor -> update();
	}
}

/**
	Constructeur
	@param c La liste des conducteurs dont on modifie les proprietes
	@param parent QUndoCommand parent
*/
ChangeSeveralConductorsPropertiesCommand::ChangeSeveralConductorsPropertiesCommand(QList<Conductor *>c, QUndoCommand *parent) :
	QUndoCommand(QObject::tr("modifier les propri\351t\351s de plusieurs conducteurs", "undo caption"), parent),
	conductors(c),
	old_settings_set(false),
	new_settings_set(false),
	diagram(c.first()->diagram())
{
}

/// Destructeur
ChangeSeveralConductorsPropertiesCommand::~ChangeSeveralConductorsPropertiesCommand() {
}

/// definit l'ancienne configuration
void ChangeSeveralConductorsPropertiesCommand::setOldSettings(const QList<ConductorProperties> &properties) {
	if (!old_settings_set) {
		old_properties = properties;
		old_settings_set = true;
	}
}

/// definit la nouvelle configuration
void ChangeSeveralConductorsPropertiesCommand::setNewSettings(const QList<ConductorProperties> &properties) {
	if (!new_settings_set) {
		new_properties = properties;
		new_settings_set = true;
	}
}

void ChangeSeveralConductorsPropertiesCommand::setNewSettings(const ConductorProperties &properties) {
	if (!new_settings_set) {
		single_new_properties = properties;
		new_settings_set = true;
	}
}

/**
	Annule les changements - Attention : les anciens et nouveaux parametres
	doivent avoir ete definis a l'aide de setNewSettings et setOldSettings
*/
void ChangeSeveralConductorsPropertiesCommand::undo() {
	diagram -> showMe();
	if (old_settings_set && new_settings_set) {
		int i=0;
		foreach(Conductor *c, conductors) {
			c -> setProperties(old_properties.at(i));
			c -> update();
			i++;
		}
	}
}

/**
	Refait les changements - Attention : les anciens et nouveaux parametres
	doivent avoir ete definis a l'aide de setNewSettings et setOldSettings
*/
void ChangeSeveralConductorsPropertiesCommand::redo() {
	diagram -> showMe();
	if (old_settings_set && new_settings_set) {

		//new propertie are the same for each conductor
		if (new_properties.isEmpty()) {
			foreach(Conductor *c, conductors) {
				c -> setProperties(single_new_properties);
				c -> update();
			}
		}

		//new propertie are different for each conductor
		else {
			int i=0;
			foreach(Conductor *c, conductors) {
				c -> setProperties(new_properties.at(i));
				c -> update();
				i++;
			}
		}
	}
}

/**
 * @brief ItemResizerCommand::ItemResizerCommand
 * Change the size of @qgi
 * @param qgi item to resize
 * @param old_ old size
 * @param new_ new size
 * @param text text to display
 * @param parent undo parent
 */
ItemResizerCommand::ItemResizerCommand (QetGraphicsItem *qgi, qreal &old_, qreal &new_, const QString &text, QUndoCommand *parent):
	QUndoCommand(parent),
	m_qgi    ( qgi			  ),
	old_size ( old_			  ),
	new_size ( new_			  ),
	diagram  ( qgi->diagram() ),
	m_text   ( text			  )
{}

/**
 * @brief ItemResizerCommand::~ItemResizerCommand
 */
ItemResizerCommand::~ItemResizerCommand() {}

/**
 * @brief ItemResizerCommand::undo
 */
void ItemResizerCommand::undo() {
	diagram -> showMe();
	m_qgi -> setScale(old_size);
	QUndoCommand::undo();
}

/**
 * @brief ItemResizerCommand::redo
 */
void ItemResizerCommand::redo() {
	diagram -> showMe();
	if (old_size<new_size) setText(QObject::tr("Agrandire %1 \340 %2 %").arg(m_text).arg(new_size*100));
	else setText(QObject::tr("R\351duire %1 \340 %2 %").arg(m_text).arg(new_size*100));
	m_qgi -> setScale(new_size);
	QUndoCommand::redo();
}


/**
 * @brief ChangeShapeStyleCommand::ChangeShapeStyleCommand Constructor
 * @param shape
 * @param old_ old style of shape
 * @param new_ new style of shape
 * @param parent undocommand parent
 */
ChangeShapeStyleCommand::ChangeShapeStyleCommand(QetShapeItem *shape, Qt::PenStyle &old_, Qt::PenStyle &new_, QUndoCommand *parent):
	QUndoCommand(parent),
	shape_(shape),
	old_style (old_),
	new_style (new_),
	diagram(shape->diagram())
{
	setText(QObject::tr("Changer le style d'une shape"));
}

/**
 * @brief ChangeShapeStyleCommand::~ChangeShapeStyleCommand destructor
 */
ChangeShapeStyleCommand::~ChangeShapeStyleCommand() {}

/**
 * @brief ChangeShapeStyleCommand::undo set the old style
 */
void ChangeShapeStyleCommand::undo() {
	shape_ -> setStyle(old_style);
	diagram -> showMe();
	QUndoCommand::undo();
}

/**
 * @brief ChangeShapeStyleCommand::redo set the new style
 */
void ChangeShapeStyleCommand::redo() {
	shape_ -> setStyle(new_style);
	diagram -> showMe();
	QUndoCommand::redo();
}

/**
 * @brief LinkElementsCommand::LinkElementsCommand
 *Constructor
 * @param elmt1 element to Link
 * @param elmt2 element to link
 * @param parent parent undo command
 */
LinkElementsCommand::LinkElementsCommand(Element *elmt1, Element *elmt2, QUndoCommand *parent) :
	QUndoCommand(parent),
	diagram_(elmt1->diagram()),
	element_(elmt1),
	first_redo(true)
{
	elmt_list << elmt2;
	if (elmt1->linkType() & Element::AllReport &&
		elmt2->linkType() & Element::AllReport) {
		setText(QObject::tr("Lier deux reports de folio",
							"title for undo LinkElementsCommand if two elements are folio report"));
	}
	else if (element_->linkType() & (Element::Master|Element::Slave))
			setText(QObject::tr("Editer les r\351f\351rence crois\351", "edite the cross reference"));
	else	setText(QObject::tr("Lier deux éléments"));

	previous_linked = elmt1->linkedElements();
}

LinkElementsCommand::LinkElementsCommand(Element *elmt1, QList<Element *> &elmtList, QUndoCommand *parent) :
	QUndoCommand(parent),
	diagram_(elmt1->diagram()),
	element_(elmt1),
	elmt_list(elmtList),
	first_redo(true)
{
	if (element_->linkType() & (Element::Master|Element::Slave))
				 setText(QObject::tr("Editer les r\351f\351rence crois\351"));
	else setText(QObject::tr("Lier deux éléments"));
	previous_linked = elmt1->linkedElements();
}

/**
 * @brief LinkElementsCommand::~LinkElementsCommand
 *destructor
 */
LinkElementsCommand::~LinkElementsCommand(){}

/**
 * @brief LinkElementsCommand::undo
 *Undo command
 */
void LinkElementsCommand::undo() {
	diagram_->showMe();

	foreach (Element *elmt, elmt_list)
		element_->unlinkElement(elmt);

	foreach (Element *elmt, previous_linked)
		element_->linkToElement(elmt);

	QUndoCommand::undo();
}

/**
 * @brief LinkElementsCommand::redo
 *redo command
 */
void LinkElementsCommand::redo() {
	diagram_->showMe();

	foreach (Element *elmt, elmt_list)
		element_->linkToElement(elmt);

	//If element are report, check if text of this potential is identical.
	if ((element_->linkType() &Element::AllReport) && first_redo) {
		if(element_->conductors().count() && elmt_list.first()->conductors().count()) {
			ConductorAutoNumerotation::checkPotential(element_->conductors().first());
		}
		first_redo = false;
	}
	QUndoCommand::redo();
}

/**
 * @brief unlinkElementsCommand::unlinkElementsCommand
 * Constructor, unlink elmt2 or all elements if elmt2 isn't specified
 * @param elmt1 element to set undo command
 * @param elmt2 element to be unlinked
 * @param parent undo parent
 */
unlinkElementsCommand::unlinkElementsCommand(Element *elmt1, Element *elmt2, QUndoCommand *parent):
	QUndoCommand(parent),
	diagram_(elmt1->diagram()),
	element_(elmt1)
{
	if (elmt2) elmt_list << elmt2;
	else elmt_list << elmt1->linkedElements();
	setText(QObject::tr("D\351lier %n \351l\351ment(s)", "", elmt_list.size()));
}

/**
 * @brief unlinkElementsCommand::unlinkElementsCommand
 * @param elmt1 Element to set the link
 * @param elmtList list of all element to be linked to elmt1
 * @param parent undo command
 */
unlinkElementsCommand::unlinkElementsCommand(Element *elmt1, QList<Element *> &elmtList, QUndoCommand *parent):
	QUndoCommand(parent),
	diagram_(elmt1->diagram()),
	element_(elmt1),
	elmt_list(elmtList)
{
	setText(QObject::tr("D\351lier %n \351l\351ment(s)", "", elmt_list.size()));
}

/**
 * @brief unlinkElementsCommand::~unlinkElementsCommand
 * destructor
 */
unlinkElementsCommand::~unlinkElementsCommand(){}

/**
 * @brief unlinkElementsCommand::undo
 *undo command
 */
void unlinkElementsCommand::undo() {
	foreach (Element *elmt, elmt_list)
		element_->linkToElement(elmt);
	QUndoCommand::undo();
}

/**
 * @brief unlinkElementsCommand::redo
 *redo command
 */
void unlinkElementsCommand::redo() {
	foreach (Element *elmt, elmt_list)
		element_->unlinkElement(elmt);
	QUndoCommand::redo();
}
