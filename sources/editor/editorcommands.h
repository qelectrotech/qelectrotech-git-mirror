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
#ifndef EDITOR_COMMANDS_H
#define EDITOR_COMMANDS_H
#include "customelementpart.h"
#include "elementview.h"
#include "elementscene.h"
#include "elementcontent.h"
#include "qgimanager.h"

/**
 * @brief The ElementEditionCommand class
 * 	ElementEditionCommand is the base class for all commands classes involved in
	the edition of an electrical element. It provides commonly required methods
	and attributes, such as accessors to the modified scene and view.
 */
class ElementEditionCommand : public QUndoCommand
{
		// constructors, destructor
	public:
		ElementEditionCommand(ElementScene * = nullptr, ElementView * = nullptr, QUndoCommand * = nullptr);
		ElementEditionCommand(const QString &, ElementScene * = nullptr, ElementView * = nullptr, QUndoCommand * = nullptr);
		~ElementEditionCommand() override;
	private:
		ElementEditionCommand(const ElementEditionCommand &);
	
		// methods
	public:
		ElementScene *elementScene() const;
		void setElementScene(ElementScene *);
		ElementView *elementView() const;
		void setElementView(ElementView *);
	
		// attributes
	protected:
		/// Element editor/view/scene the command should take place on
		ElementScene *editor_scene_;
		ElementView *editor_view_;
};

/**
	This command deletes one or several primitives/parts when editing an
	electrical element.
*/
class DeletePartsCommand : public ElementEditionCommand {
	// constructors, destructor
	public:
	DeletePartsCommand(ElementScene *, const QList<QGraphicsItem *>, QUndoCommand * = nullptr);
	~DeletePartsCommand() override;
	private:
	DeletePartsCommand(const DeletePartsCommand &);
	
	// methods
	public:
	void undo() override;
	void redo() override;
	
	// attributes
	private:
	/// Deleted primitives
	QList<QGraphicsItem *> deleted_parts;
};

/**
	This command pastes primitives when editing an electrical element.
*/
class PastePartsCommand : public ElementEditionCommand {
	// constructors, destructor
	public:
	PastePartsCommand(ElementView *, const ElementContent &, QUndoCommand * = nullptr);
	~PastePartsCommand() override;
	private:
	PastePartsCommand(const PastePartsCommand &);
	
	// methods
	public:
	void undo() override;
	void redo() override;
	virtual void setOffset(int, const QPointF &, int, const QPointF &);
	
	// attributes
	private:
	/// Pasted content
	ElementContent content_;
	/// Data required to undo a copy/paste with offset
	int old_offset_paste_count_;
	QPointF old_start_top_left_corner_;
	int new_offset_paste_count_;
	QPointF new_start_top_left_corner_;
	bool uses_offset;
	/// Prevent the first call to redo()
	bool first_redo;
};

/**
	This command cut primitives when editing an electrical element.
*/
class CutPartsCommand : public DeletePartsCommand {
	// constructors, destructor
	public:
	CutPartsCommand(ElementScene *, const QList<QGraphicsItem *>, QUndoCommand * = nullptr);
	~CutPartsCommand() override;
	private:
	CutPartsCommand(const CutPartsCommand &);
};

/**
	This command moves primitives when editing an electrical element.
*/
class MovePartsCommand : public ElementEditionCommand {
	// constructors, destructor
	public:
	MovePartsCommand(const QPointF &, ElementScene *, const QList<QGraphicsItem *>, QUndoCommand * = nullptr);
	~MovePartsCommand() override;
	private:
	MovePartsCommand(const MovePartsCommand &);
	
	// methods
	public:
	void undo() override;
	void redo() override;
	
	// attributes
	private:
	/// List of moved primitives
	QList<QGraphicsItem *> moved_parts;
	/// applied movement
	QPointF movement;
	/// Prevent the first call to redo()
	bool first_redo;
};

/**
	This command adds a primitive when editing an electrical element.
*/
class AddPartCommand : public ElementEditionCommand {
	// constructors, destructor
	public:
	AddPartCommand(const QString &, ElementScene *, QGraphicsItem *, QUndoCommand * = nullptr);
	~AddPartCommand() override;
	private:
	AddPartCommand(const AddPartCommand &);
	
	// methods
	public:
	void undo() override;
	void redo() override;
	
	// attributes
	private:
	/// Added primitive
	QGraphicsItem *part;
	/// Prevent the first call to redo()
	bool first_redo;
};

/**
	This command changes the translated names of an electrical element.
*/
class ChangeNamesCommand : public ElementEditionCommand {
	// constructors, destructor
	public:
	ChangeNamesCommand(ElementScene *, const NamesList &, const NamesList &, QUndoCommand * = nullptr);
	~ChangeNamesCommand() override;
	private:
	ChangeNamesCommand(const ChangeNamesCommand &);
	
	// methods
	public:
	void undo() override;
	void redo() override;
	
	// attributes
	private:
	/// List of former names
	NamesList names_before;
	/// List of new names
	NamesList names_after;
};

/**
	This command changes the zValue of a set of primitives when editing an
	electrical element.
*/
class ChangeZValueCommand : public ElementEditionCommand {
	// constructors, destructor
	public:
	/// List the various kind of changes for the zValue
	enum Option {
		BringForward, ///< Bring primitives to the foreground so they have the highest zValue
		Raise,        ///< Raise primitives one layer above their current one; zValues are incremented
		Lower,        ///< Send primitives one layer below their current one; zValues are decremented
		SendBackward  ///< Send primitives to the background so they have the lowest zValue
	};
	ChangeZValueCommand(ElementScene *, Option, QUndoCommand * = nullptr);
	~ChangeZValueCommand() override;
	private:
	ChangeZValueCommand(const ChangeZValueCommand &);
	
	// methods
	public:
	void undo() override;
	void redo() override;
	
	private:
	void applyBringForward(const QList<QGraphicsItem *> &);
	void applyRaise(const QList<QGraphicsItem *> &);
	void applyLower(const QList<QGraphicsItem *> &);
	void applySendBackward(const QList<QGraphicsItem *> &);
	
	// attributes
	private:
	/// associates impacted primitives with their former zValues
	QHash<QGraphicsItem *, qreal> undo_hash;
	/// associates impacted primitives with their new zValues
	QHash<QGraphicsItem *, qreal> redo_hash;
	/// kind of treatment to apply
	Option option;
};

/**
	This command changes extra information carried by an electrical element.
*/
class ChangeInformationsCommand : public ElementEditionCommand {
	// constructors, destructor
	public:
	ChangeInformationsCommand(ElementScene *, const QString &, const QString &, QUndoCommand * = nullptr);
	~ChangeInformationsCommand() override;
	private:
	ChangeInformationsCommand(const ChangeInformationsCommand &);
	
	// methods
	public:
	void undo() override;
	void redo() override;
	
	// attributes
	private:
	/// Former information
	QString old_informations_;
	/// New information
	QString new_informations_;
};

/**
	This command scales primitives when editing an electrical element.
*/
class ScalePartsCommand : public ElementEditionCommand {
	// constructors, destructor
	public:
	ScalePartsCommand(ElementScene * = nullptr, QUndoCommand * = nullptr);
	~ScalePartsCommand() override;
	private:
	ScalePartsCommand(const ScalePartsCommand &);
	
	// methods
	public:
	void undo() override;
	void redo() override;
	ElementScene *elementScene() const;
	void setScaledPrimitives(const QList<CustomElementPart *> &);
	QList<CustomElementPart *> scaledPrimitives() const;
	void setTransformation(const QRectF &, const QRectF &);
	QPair<QRectF, QRectF> transformation();
	
	protected:
	void scale(const QRectF &before, const QRectF &after);
	void adjustText();
	
	// attributes
	private:
	/// List of moved primitives
	QList<CustomElementPart *> scaled_primitives_;
	/// original rect items fit in
	QRectF original_rect_;
	/// new rect items should fit in
	QRectF new_rect_;
	/// Prevent the first call to redo()
	bool first_redo;
};

class ChangePropertiesCommand : public ElementEditionCommand {
	public:
	ChangePropertiesCommand (ElementScene *scene, QString type, DiagramContext info, QUndoCommand *parent=nullptr);
	~ChangePropertiesCommand () override;

	void undo() override;
	void redo() override;

	private:
	QList <QString> m_type;
	QList <DiagramContext> m_info;
};

#endif
