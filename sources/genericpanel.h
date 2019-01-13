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
#ifndef GENERIC_PANEL_H
#define GENERIC_PANEL_H
#include "qet.h"
#include <QTreeWidget>

class QTreeWidgetItem;
class QETProject;
class Diagram;
class TitleBlockTemplatesCollection;
class TitleBlockTemplateLocation;

/**
	The generic panel is a QTreeWidget subclass providing extra methods
	allowing developers to easily add objects (projects, diagrams, title block
	templates, elements, ...) to it; it also ensures the displayed information
	remains up to date.
*/
class GenericPanel : public QTreeWidget {
	Q_OBJECT
	
	public:
	enum PanelOption {
		AddChildDiagrams              =   1,
		AddChildTemplatesCollection   =   2,
		AddChildTemplates             =   4,
		AddAllChildTemplates          =   6,
		AddAllChild                   =  63,
		All                           = 127
	};
	Q_DECLARE_FLAGS(PanelOptions, PanelOption)
	
	enum MetaData {
		Item = Qt::UserRole + 1,
		AliasItem,
		Parent,
		PanelFlags
	};
	
	// Constructors, destructor
	public:
	GenericPanel(QWidget * = nullptr);
	~GenericPanel() override;
	
	public:
	// convenience methods to obtain what an item represents
	virtual int currentItemType();
	virtual QETProject *projectForItem(QTreeWidgetItem *) const;
	virtual Diagram *diagramForItem(QTreeWidgetItem *) const;
	virtual TitleBlockTemplateLocation templateLocationForItem(QTreeWidgetItem *) const;
	
	// convenience methods to obtain what the selected item represents
	virtual QETProject *selectedProject() const;
	virtual Diagram *selectedDiagram() const;
	virtual TitleBlockTemplateLocation selectedTemplateLocation() const;
	
	// project-related methods
	public:
	virtual QTreeWidgetItem *addProject(QETProject *, QTreeWidgetItem * = nullptr, PanelOptions = AddAllChild);
	virtual QTreeWidgetItem *itemForProject(QETProject *);
	protected:
	virtual QTreeWidgetItem *getItemForProject(QETProject *, bool * = nullptr);
	virtual QTreeWidgetItem *updateProjectItem(QTreeWidgetItem *, QETProject *, PanelOptions = AddAllChild, bool = false);
	virtual QTreeWidgetItem *fillProjectItem  (QTreeWidgetItem *, QETProject *, PanelOptions = AddAllChild, bool = false);
	
	// diagram-related methods
	public:
	virtual QTreeWidgetItem *addDiagram(Diagram *, QTreeWidgetItem * = nullptr, PanelOptions = AddAllChild);
	protected:
	virtual QTreeWidgetItem *getItemForDiagram(Diagram *, bool * = nullptr);
	virtual QTreeWidgetItem *updateDiagramItem(QTreeWidgetItem *, Diagram *, PanelOptions = AddAllChild, bool = false);
	virtual QTreeWidgetItem *fillDiagramItem  (QTreeWidgetItem *, Diagram *, PanelOptions = AddAllChild, bool = false);
	
	// title block templates collections methods
	public:
	virtual QTreeWidgetItem *addTemplatesCollection(TitleBlockTemplatesCollection *, QTreeWidgetItem * = nullptr, PanelOptions = AddAllChild);
	virtual QTreeWidgetItem *itemForTemplatesCollection(TitleBlockTemplatesCollection *);
	protected:
	virtual QTreeWidgetItem *getItemForTemplatesCollection(TitleBlockTemplatesCollection *, bool * = nullptr);
	virtual QTreeWidgetItem *updateTemplatesCollectionItem(QTreeWidgetItem *, TitleBlockTemplatesCollection *, PanelOptions = AddAllChild, bool = false);
	virtual QTreeWidgetItem *fillTemplatesCollectionItem  (QTreeWidgetItem *, TitleBlockTemplatesCollection *, PanelOptions = AddAllChild, bool = false);
	
	// title block templates methods
	public:
	virtual QTreeWidgetItem *addTemplate(const TitleBlockTemplateLocation &, QTreeWidgetItem * = nullptr, PanelOptions = AddAllChild);
	protected:
	virtual QTreeWidgetItem *getItemForTemplate(const TitleBlockTemplateLocation &, bool * = nullptr);
	virtual QTreeWidgetItem *updateTemplateItem(QTreeWidgetItem *, const TitleBlockTemplateLocation &, PanelOptions = AddAllChild, bool = false);
	virtual QTreeWidgetItem *fillTemplateItem  (QTreeWidgetItem *, const TitleBlockTemplateLocation &, PanelOptions = AddAllChild, bool = false);
	
	// generic methods
	protected:
	virtual QTreeWidgetItem *updateItem(QTreeWidgetItem *, PanelOptions = AddAllChild, bool = false);
	virtual QTreeWidgetItem *fillItem  (QTreeWidgetItem *, PanelOptions = AddAllChild, bool = false);
	
	// slots used to receive change notifications from added objects
	protected slots:
	virtual void projectInformationsChanged(QETProject *);
	virtual void diagramAdded(QETProject *, Diagram *);
	virtual void diagramRemoved(QETProject *, Diagram *);
	virtual void projectDiagramsOrderChanged(QETProject *, int, int);
	virtual void diagramTitleChanged(Diagram *, const QString &);
	virtual void templatesCollectionChanged(TitleBlockTemplatesCollection*, const QString &);
	virtual void diagramUsedTemplate(TitleBlockTemplatesCollection *, const QString &);
	
	// various other methods
	protected:
	virtual QString defaultText(QET::ItemType);
	virtual QIcon   defaultIcon(QET::ItemType);
	virtual QTreeWidgetItem *makeItem(QET::ItemType, QTreeWidgetItem * = nullptr, const QString & = QString(), const QIcon & = QIcon());
	virtual void deleteItem(QTreeWidgetItem *, bool = false);
	virtual void markItemAsUnused(QTreeWidgetItem *);
	virtual void reparent(QTreeWidgetItem *, QTreeWidgetItem *);
	QList<QTreeWidgetItem *> childItems(QTreeWidgetItem *, QET::ItemType, bool = false) const;
	template<typename T> void removeObsoleteItems(const QList<T> &, QTreeWidgetItem *, QET::ItemType, bool);
	template<typename T> T valueForItem(QTreeWidgetItem *) const;
	void unregisterItem(QTreeWidgetItem *);
	
	bool event(QEvent *) override;
	
	signals:
	bool firstActivated();
	void panelContentChanged();
	
	private slots:
	void emitFirstActivated();
	
	private:
	bool first_activation_; ///< boolean used to track the first time this widget is activated/shown
	QHash<QETProject *, QTreeWidgetItem *>               projects_;     ///< Allow quick retrieval of the item representing a given project
	QHash<Diagram *, QTreeWidgetItem *>                  diagrams_;     ///< Allow quick retrieval of the item representing a given diagram
	QHash<TitleBlockTemplateLocation, QTreeWidgetItem *> tb_templates_; ///< Allow quick retrieval of the item representing a title block template
};
#endif
