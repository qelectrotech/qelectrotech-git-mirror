#include "diagramview.h"
#include "diagram.h"
#include "customelement.h"
#include "exportdialog.h"

/**
	Initialise le DiagramView
*/
void DiagramView::initialise() {
	setInteractive(true);
	setAntialiasing(true);
	setScene(scene = new Diagram(this));
	setDragMode(RubberBandDrag);
	setAcceptDrops(true);
	setWindowTitle(tr("Nouveau sch\351ma") + "[*]");
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setResizeAnchor(QGraphicsView::AnchorUnderMouse);
	setAlignment(Qt::AlignLeft | Qt::AlignTop);
	setSceneRect(QRectF(0.0, 0.0, scene -> border_and_inset.borderWidth() + 10.0, scene -> border_and_inset.borderHeight() + 10.0));
	connect(scene, SIGNAL(selectionChanged()), this, SLOT(slot_selectionChanged()));
}

/**
	Constructeur par defaut
*/
DiagramView::DiagramView() : QGraphicsView() {
	initialise();
}

/**
	Constructeur
	@param parent Le QWidegt parent de cette vue de schema
*/
DiagramView::DiagramView(QWidget *parent) : QGraphicsView(parent) {
	initialise();
}

/**
	Permet de savoir si le rendu graphique du DiagramView est antialiase ou non.
	@return Un booleen indiquant si le DiagramView est antialiase
*/
bool DiagramView::antialiased() const {
	return(antialiasing);
}

/**
	Active ou desactive l'antialiasing pour le rendu graphique du DiagramView.
	@param aa un booleen indiquant si le DiagramView doit etre antialiase ou non
*/
void DiagramView::setAntialiasing(bool aa) {
	antialiasing = aa;
	setRenderHint(QPainter::Antialiasing, aa);
	setRenderHint(QPainter::TextAntialiasing, aa);
	setRenderHint(QPainter::SmoothPixmapTransform, aa);
	repaint();
}

/**
	appelle la methode select sur tous les elements de la liste d'elements
*/
void DiagramView::selectAll() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(true);
}

/**
	appelle la methode deselect sur tous les elements de la liste d'elements
*/
void DiagramView::selectNothing() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(false);
}

/**
	Inverse l'etat de selection de tous les elements de la liste d'elements
 */
void DiagramView::selectInvert() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(!item -> isSelected());
}

/**
	Supprime les composants selectionnes
*/
void DiagramView::supprimer() {
	
	QList<QGraphicsItem *> garbage_elmt;
	QList<QGraphicsItem *> garbage_conducers;
	
	// creation de deux listes : une pour les conducteurs, une pour les elements
	foreach (QGraphicsItem *qgi, scene -> selectedItems()) {
		// pour chaque qgi selectionne, il s'agit soit d'un element soit d'un conducteur
		if (qgraphicsitem_cast<Conducer *>(qgi)) {
			// s'il s'agit d'un conducteur, on le met dans la liste des conducteurs
			if (!garbage_conducers.contains(qgi)) garbage_conducers.append(qgi);
		} else if (qgraphicsitem_cast<Element *>(qgi)) {
			// s'il s'agit d'un element, on veille a enlever ses conducteurs
			if (!garbage_elmt.contains(qgi)) garbage_elmt.append(qgi);
			// pour chaque enfant de l'element
			foreach (QGraphicsItem *child, qgi -> children()) {
				// si cet enfant est une borne
				if (Terminal *p = qgraphicsitem_cast<Terminal *>(child)) {
					// alors chaque conducteur de la borne est recense
					foreach (Conducer *f, p -> conducers()) {
						if (!garbage_conducers.contains(f)) garbage_conducers.append(f);
					}
				}
			}
		}
	}
	scene -> clearSelection();
	
	// "destroying" the wires, removing them from the scene and stocking them into the � garbage �
	foreach (QGraphicsItem *qgi, garbage_conducers) {
		if (Conducer *f = qgraphicsitem_cast<Conducer *>(qgi)) {
			f -> destroy();
			scene -> removeItem(f);
			throwToGarbage(f);
		}
	}
	
	// removing the elements from the scene and stocking them into the � garbage �
	foreach (QGraphicsItem *qgi, garbage_elmt) {
		scene -> removeItem(qgi);
		throwToGarbage(qgi);
	}
	resetCachedContent();
	QTimer::singleShot(5000, this, SLOT(flushGarbage()));
}

/**
	Envoie un item vers le "garbage" pour qu'il soit supprime plus tard
	@param qgi L'item a supprimer
*/
void DiagramView::throwToGarbage(QGraphicsItem *qgi) {
	// pas de doublon dans le garbage (sinon ca va sentir la segfault)
	bool qgi_deja_dans_le_garbage = false;
	foreach(QGraphicsItem *gbg_qgi, garbage) {
		if ((void *)gbg_qgi == (void *)qgi) {
			qgi_deja_dans_le_garbage = true;
			break;
		}
	}
	if (!qgi_deja_dans_le_garbage) garbage.append(qgi);
}

/**
	Supprime tous les elements du "garbage"
*/
void DiagramView::flushGarbage() {
	foreach(QGraphicsItem *qgi, garbage) {
		delete(qgi);
		garbage.removeAll(qgi);
	}
}

/**
	Pivote les composants selectionnes
*/
void DiagramView::pivoter() {
	if (scene -> selectedItems().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> selectedItems()) {
		if (Element *elt = qgraphicsitem_cast<Element *>(item)) {
			elt -> setOrientation(elt -> nextAcceptableOrientation());
			elt -> update();
		}
	}
}

/**
	accepte ou refuse le drag'n drop en fonction du type de donnees entrant
	@param e le QDragEnterEvent correspondant au drag'n drop tente
*/
void DiagramView::dragEnterEvent(QDragEnterEvent *e) {
	if (e -> mimeData() -> hasFormat("text/plain")) e -> acceptProposedAction();
	else e-> ignore();
}

/**
	gere les dragleaveevent
	@param e le QDragEnterEvent correspondant au drag'n drop sortant
*/
void DiagramView::dragLeaveEvent(QDragLeaveEvent *) {}

/**
	accepte ou refuse le drag'n drop en fonction du type de donnees entrant
	@param e le QDragMoveEvent correspondant au drag'n drop tente
*/
void DiagramView::dragMoveEvent(QDragMoveEvent *e) {
	if (e -> mimeData() -> hasFormat("text/plain")) e -> acceptProposedAction();
	else e-> ignore();
}

/**
	gere les depots (drop) acceptes sur le Diagram
	@param e le QDropEvent correspondant au drag'n drop effectue
*/
void DiagramView::dropEvent(QDropEvent *e) {
	QString fichier = e -> mimeData() -> text();
	int etat;
	Element *el = new CustomElement(fichier, 0, 0, &etat);
	if (etat != 0) delete el;
	else {
		scene -> addItem(el);
		el -> setPos(mapToScene(e -> pos().x(), e -> pos().y()));
		el -> setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	}
}

/**
	Passe le Diagram en mode visualisation
*/
void DiagramView::setVisualisationMode() {
	setDragMode(ScrollHandDrag);
	emit(modeChanged());
}

/**
	Passe le Diagram en mode Selection
*/
void DiagramView::setSelectionMode() {
	setDragMode(RubberBandDrag);
	viewport() -> setCursor(Qt::ArrowCursor);
	emit(modeChanged());
}

/**
	Agrandit le schema (+33% = inverse des -25 % de zoomMoins())
*/
void DiagramView::zoomPlus() {
	scale(4.0/3.0, 4.0/3.0);
}

/**
	Retrecit le schema (-25% = inverse des +33 % de zoomPlus())
*/
void DiagramView::zoomMoins() {
	scale(0.75, 0.75);
}

/**
	Agrandit ou rectrecit le schema de facon a ce que tous les elements du
	schema soient visibles a l'ecran. S'il n'y a aucun element sur le schema,
	le zoom est reinitialise
*/
void DiagramView::zoomFit() {
	if (scene -> items().isEmpty()) {
		zoomReset();
		return;
	}
	QRectF vue = scene -> itemsBoundingRect();
	// la marge  = 5 % de la longueur necessaire
	qreal marge = 0.05 * vue.width();
	vue.translate(-marge, -marge);
	vue.setWidth(vue.width() + 2.0 * marge);
	vue.setHeight(vue.height() + 2.0 * marge);
	fitInView(vue, Qt::KeepAspectRatio);
}

/**
	Reinitialise le zoom
*/
void DiagramView::zoomReset() {
	resetMatrix();
}

/**
	copie les elements selectionnes du schema dans le presse-papier puis les supprime
*/
void DiagramView::couper() {
	copier();
	supprimer();
}

/**
	copie les elements selectionnes du schema dans le presse-papier
*/
void DiagramView::copier() {
	QClipboard *presse_papier = QApplication::clipboard();
	QString contenu_presse_papier = scene -> toXml(false).toString(4);
	if (presse_papier -> supportsSelection()) presse_papier -> setText(contenu_presse_papier, QClipboard::Selection);
	presse_papier -> setText(contenu_presse_papier);
}

/**
	importe les elements contenus dans le presse-papier dans le schema
*/
void DiagramView::coller() {
	QString texte_presse_papier;
	QDomDocument document_xml;
	if ((texte_presse_papier = QApplication::clipboard() -> text()) == QString()) return;
	if (!document_xml.setContent(texte_presse_papier)) return;
	scene -> fromXml(document_xml, QPointF(), false);
}

/**
	gere les clics et plus particulierement le clic du milieu (= coller pour X11)
*/
void DiagramView::mousePressEvent(QMouseEvent *e) {
	if (e -> buttons() == Qt::MidButton) {
		QString texte_presse_papier;
		QDomDocument document_xml;
		if ((texte_presse_papier = QApplication::clipboard() -> text(QClipboard::Selection)) == QString()) return;
		if (!document_xml.setContent(texte_presse_papier)) return;
		scene -> fromXml(document_xml, mapToScene(e -> pos()), false);
	}
	QGraphicsView::mousePressEvent(e);
}

/**
	Ouvre un fichier *.qet dans cette DiagramView
	@param nom_fichier Nom du fichier a ouvrir
	@param erreur Si le pointeur est specifie, cet entier est mis a 0 en cas de reussite de l'ouverture, 1 si le fichier n'existe pas, 2 si le fichier n'est pas lisible, 3 si le fichier n'est pas un element XML, 4 si l'ouverture du fichier a echoue pour une autre raison (c'est pas ca qui manque ^^)
	@return true si l'ouverture a reussi, false sinon
*/
bool DiagramView::ouvrir(QString n_fichier, int *erreur) {
	// verifie l'existence du fichier
	if (!QFileInfo(n_fichier).exists()) {
		if (erreur != NULL) *erreur = 1;
		return(false);
	}
	
	// ouvre le fichier
	QFile fichier(n_fichier);
	if (!fichier.open(QIODevice::ReadOnly)) {
		if (erreur != NULL) *erreur = 2;
		return(false);
	}
	
	// lit son contenu dans un QDomDocument
	QDomDocument document;
	if (!document.setContent(&fichier)) {
		if (erreur != NULL) *erreur = 3;
		fichier.close();
		return(false);
	}
	fichier.close();
	
	// construit le schema a partir du QDomDocument
	QDomDocument &doc = document;
	if (scene -> fromXml(doc)) {
		if (erreur != NULL) *erreur = 0;
		nom_fichier = n_fichier;
		setWindowTitle(nom_fichier + "[*]");
		return(true);
	} else {
		if (erreur != NULL) *erreur = 4;
		return(false);
	}
}

void DiagramView::slot_selectionChanged() {
	emit(selectionChanged());
}

void DiagramView::closeEvent(QCloseEvent *event) {
	// demande d'abord a l'utilisateur s'il veut enregistrer le schema en cours
	QMessageBox::StandardButton reponse = QMessageBox::question(
		this,
		tr("Enregistrer le sch\351ma en cours ?"),
		tr("Voulez-vous enregistrer le sch\351ma ") + windowTitle() + tr(" ?"),
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
		QMessageBox::Cancel
	);
	bool retour;
	switch(reponse) {
		case QMessageBox::Cancel: retour = false;         break; // l'utilisateur annule : echec de la fermeture
		case QMessageBox::Yes:    retour = enregistrer(); break; // l'utilisateur dit oui : la reussite depend de l'enregistrement
		default:                  retour = true;                 // l'utilisateur dit non ou ferme le dialogue: c'est reussi
	}
	if (retour) event -> accept();
	else event -> ignore();
}

/**
	Methode enregistrant le schema dans le dernier nom de fichier connu.
	Si aucun nom de fichier n'est connu, cette methode appelle la methode enregistrer_sous
	@return true si l'enregistrement a reussi, false sinon
*/
bool DiagramView::enregistrer() {
	if (nom_fichier == QString()) return(enregistrer_sous());
	else return(private_enregistrer(nom_fichier));
}

/**
	Cette methode demande un nom de fichier a l'utilisateur pour enregistrer le schema
	Si aucun nom n'est entre, elle renvoie faux.
	Si le nom ne se termine pas par l'extension .qet, celle-ci est ajoutee.
	Si l'enregistrement reussit, le nom du fichier est conserve et la fonction renvoie true.
	Sinon, faux est renvoye.
	@return true si l'enregistrement a reussi, false sinon
*/
bool DiagramView::enregistrer_sous() {
	// demande un nom de fichier a l'utilisateur pour enregistrer le schema
	QString n_fichier = QFileDialog::getSaveFileName(
		this,
		tr("Enregistrer sous"),
		QDir::homePath(),
		tr("Diagram QelectroTech (*.qet)")
	);
	// si aucun nom n'est entre, renvoie faux.
	if (n_fichier == "") return(false);
	// si le nom ne se termine pas par l'extension .qet, celle-ci est ajoutee
	if (!n_fichier.endsWith(".qet", Qt::CaseInsensitive)) n_fichier += ".qet";
	// tente d'enregistrer le fichier
	bool resultat_enregistrement = private_enregistrer(n_fichier);
	// si l'enregistrement reussit, le nom du fichier est conserve
	if (resultat_enregistrement) {
		nom_fichier = n_fichier;
		setWindowTitle(nom_fichier + "[*]");
	}
	// retourne un booleen representatif de la reussite de l'enregistrement
	return(resultat_enregistrement);
}

/**
	Methode privee gerant l'enregistrement du fichier XML. S'il n'est pas possible
	d'ecrire dans le fichier, cette fonction affiche un message d'erreur et renvoie false.
	Autrement, elle renvoie true.
	@param nom_fichier Nom du fichier dans lequel l'arbre XML doit etre ecrit
	@return true si l'enregistrement a reussi, false sinon
*/
bool DiagramView::private_enregistrer(QString &n_fichier) {
	QFile fichier(n_fichier);
	if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(this, tr("Erreur"), tr("Impossible d'ecrire dans ce fichier"));
		return(false);
	}
	QTextStream out(&fichier);
	out.setCodec("UTF-8");
	out << scene -> toXml().toString(4);
	fichier.close();
	return(true);
}

void DiagramView::dialogExport() {
	ExportDialog ed(scene, this);
	ed.exec();
}

void DiagramView::dialogPrint() {
	QPrinter qprin;
	qprin.setOutputFormat(QPrinter::PdfFormat);
	qprin.setOrientation(QPrinter::Landscape);
	qprin.setPageSize(QPrinter::A4);
	QPrintDialog qpd(&qprin, this);
	
	if (qpd.exec() == QDialog::Accepted) {
		QPainter qp(&qprin);
		scene -> setAffichageGrille(false);
		scene -> render(&qp);
		scene -> setAffichageGrille(true);
	}
}

void DiagramView::dialogEditInfos() {
	// recupere le cartouche du schema
	BorderInset *inset = &(scene -> border_and_inset);
	
	// construit le dialogue
	QDialog popup;
	popup.setMinimumWidth(400);
	popup.setWindowTitle(tr("Cartouche du sch\351ma"));
	
	QLineEdit *titre = new QLineEdit(inset -> title(), &popup);
	QLineEdit *auteur = new QLineEdit(inset -> author(), &popup);
	QDate date_diagram = QDate(inset -> date());
	if (date_diagram.isNull() || !date_diagram.isValid()) date_diagram = QDate::currentDate();
	QDateEdit *date = new QDateEdit(date_diagram, &popup);
	date -> setCalendarPopup(true);
	QLineEdit *fichier = new QLineEdit(inset -> fileName(), &popup);
	QLineEdit *folio = new QLineEdit(inset -> folio(), &popup);
	QWidget bidon(&popup);
	QGridLayout layout_champs(&bidon);
	layout_champs.addWidget(new QLabel(tr("Titre : ")),   0, 0);
	layout_champs.addWidget(titre,                        0, 1);
	layout_champs.addWidget(new QLabel(tr("Auteur : ")),  1, 0);
	layout_champs.addWidget(auteur,                       1, 1);
	layout_champs.addWidget(new QLabel(tr("Date : ")),    2, 0);
	layout_champs.addWidget(date,                         2, 1);
	layout_champs.addWidget(new QLabel(tr("Fichier : ")), 3, 0);
	layout_champs.addWidget(fichier,                      3, 1);
	layout_champs.addWidget(new QLabel(tr("Folio : ")),   4, 0);
	layout_champs.addWidget(folio,                        4, 1);
	
	// boutons
	QDialogButtonBox boutons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(&boutons, SIGNAL(accepted()), &popup, SLOT(accept()));
	connect(&boutons, SIGNAL(rejected()), &popup, SLOT(accept()));
	
	// ajout dans une disposition verticale
	QVBoxLayout layout_v(&popup);
	layout_v.addWidget(&bidon);
	layout_v.addWidget(&boutons);
	if (popup.exec() == QDialog::Accepted) {
		inset -> setTitle(titre -> text());
		inset -> setAuthor(auteur -> text());
		inset -> setDate(date -> date());
		inset -> setFileName(fichier -> text());
		inset -> setFolio(folio -> text());
	}
}

bool DiagramView::hasSelectedItems() {
	return(scene -> selectedItems().size() > 0);
}

void DiagramView::addColumn() {
	// ajoute la colonne
	scene -> border_and_inset.addColumn();
	
	// met a jour la zone affichee par la vue
	QRectF sr = sceneRect();
	sr.setWidth(5.0 + scene -> border_and_inset.borderWidth());
	setSceneRect(sr);
	
	// rafraichit la vue
	scene -> update(sceneRect());
}

void DiagramView::removeColumn() {
	scene -> border_and_inset.removeColumn();
	
	// on pourrait mettre a jour la zone affichee par la vue
	
	// rafraichit la vue
	scene -> update(sceneRect());
}

void DiagramView::expand() {
	// ajoute 10 pixels
	scene -> border_and_inset.setColumnsHeight(scene -> border_and_inset.columnsHeight() + 20.0);
	
	// met a jour la zone affichee par la vue
	QRectF sr = sceneRect();
	sr.setHeight(20.0 + sr.height());
	setSceneRect(sr);
	
	// rafraichit la vue
	scene -> update(sceneRect());
}

void DiagramView::shrink() {
	// enleve 10 pixels
	scene -> border_and_inset.setColumnsHeight(scene -> border_and_inset.columnsHeight() - 20.0);
	
	// rafraichit la vue
	scene -> update(sceneRect());
}
