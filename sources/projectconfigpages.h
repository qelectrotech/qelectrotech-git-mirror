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
#ifndef PROJECTCONFIGPAGES_H
#define PROJECTCONFIGPAGES_H
#include "configpage.h"

class QLabel;
class QLineEdit;
class QETProject;
class BorderPropertiesWidget;
class ConductorPropertiesWidget;
class DiagramContextWidget;
class ReportPropertieWidget;
class XRefPropertiesWidget;
class SelectAutonumW;
class FolioAutonumberingW;
class FormulaAutonumberingW;
class AutoNumberingManagementW;

/**
	This class, derived from ConfigPage, aims at providing the basic skeleton
	for a project configuration page.
*/
class ProjectConfigPage : public ConfigPage {
	Q_OBJECT
	// Constructor, destructor
	public:
	ProjectConfigPage(QETProject *, QWidget * = nullptr);
	~ProjectConfigPage() override;
	private:
	ProjectConfigPage(const ProjectConfigPage &);
	
	// methods
	public:
	virtual QETProject *project() const;
	virtual QETProject *setProject(QETProject *project, bool = true);
	void applyConf() override;
	/**
		Apply configuration to the project after user input. This method is
		automatically called when the ConfigDialog is validated, and only if the
		project is both non-zero and not read-only.
	*/
	virtual void applyProjectConf() = 0;
	
	protected:
	virtual void init();
	/**
		Use this pure virtual method to initialize your page widgets.
	*/
	virtual void initWidgets() = 0;
	/**
		Use this pure virtual method to initialize your page layout. This method is
		always called after initWidgets().
	*/
	virtual void initLayout() = 0;
	/**
		Use this pure virtual method to fill widgets with project values.
	*/
	virtual void readValuesFromProject()  = 0;
	/**
		Use this pure virtual method to adjust the "read only" state of your page
		widgets according to the currently edited project.
	*/
	virtual void adjustReadOnly() = 0;
	
	// attributes
	protected:
	QETProject *m_project; ///< Currently edited project
};

/**
	This page enables users to configure the main properties of a project.
*/
class ProjectMainConfigPage : public ProjectConfigPage {
	Q_OBJECT
	// Constructor, destructor
	public:
	ProjectMainConfigPage(QETProject *, QWidget * = nullptr);
	~ProjectMainConfigPage() override;
	private:
	ProjectMainConfigPage(const ProjectMainConfigPage &);
	
	// methods
	public:
	QString title() const override;
	QIcon icon() const override;
	void applyProjectConf() override;
	QString projectTitle() const;
	
	protected:
	void initWidgets() override;
	void initLayout() override;
	void readValuesFromProject() override;
	void adjustReadOnly() override;
	
	// attributes
	protected:
	QLabel *title_label_;
	QLineEdit *title_value_;
	QLabel *title_information_;
	QLabel *project_variables_label_;
	DiagramContextWidget *project_variables_;
};

class ProjectAutoNumConfigPage : public ProjectConfigPage {
		Q_OBJECT

	//Methods
	public:
		ProjectAutoNumConfigPage (QETProject *project, QWidget *parent = nullptr);

		QString title() const override;
		QIcon   icon() const override;
		void    applyProjectConf() override;
		virtual void    changeToTab(int);
	protected:
		void initWidgets() override;
		void initLayout() override {}
		void readValuesFromProject() override;
		void adjustReadOnly() override;
	private:
		void buildConnections();
	private slots:
		void updateContextConductor(const QString&);//conductor
		void saveContextConductor();
		void removeContextConductor();
		void updateContextFolio(const QString&);//folio
		void saveContextFolio();
		void removeContextFolio();
		void updateContextElement(const QString&);//element
		void saveContextElement();
		void removeContextElement();

		void applyAutoNum();
		void applyManagement();

	signals:
		void setAutoNum(QString);
		void setAutoNum(int,int);
		void saveCurrentTbp();
		void loadSavedTbp();

	//Attributes
	private:
		SelectAutonumW        *m_saw_conductor;
		SelectAutonumW        *m_saw_folio;
		SelectAutonumW        *m_saw_element;
		FolioAutonumberingW   *m_faw;
		AutoNumberingManagementW *m_amw;

};

#endif
