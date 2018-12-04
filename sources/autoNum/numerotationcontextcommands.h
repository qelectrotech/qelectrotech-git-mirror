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
#ifndef NUMEROTATIONCONTEXTCOMMANDS_H
#define NUMEROTATIONCONTEXTCOMMANDS_H

#include "numerotationcontext.h"

class NumStrategy;
class Diagram;

/**
 * this class provide methods to handle content of NumerotationContext.
 */
class NumerotationContextCommands
{
	public:
	NumerotationContextCommands (const NumerotationContext &, Diagram * = nullptr);
	~NumerotationContextCommands ();
	NumerotationContext next ();
	NumerotationContext previous ();
	QString toRepresentedString ();

	private:
	void setNumStrategy (const QString &);

	Diagram *diagram_;
	NumerotationContext context_;
	NumStrategy *strategy_;
};

class NumStrategy
{
	public:
	NumStrategy (Diagram * = nullptr);
	virtual ~NumStrategy ();
	virtual QString toRepresentedString (const QString) const = 0;
	virtual NumerotationContext next     (const NumerotationContext &, const int) const = 0;
	virtual NumerotationContext previous (const NumerotationContext &, const int) const = 0;

	protected:
	NumerotationContext nextString (const NumerotationContext &, const int) const;
	NumerotationContext nextNumber (const NumerotationContext &, const int) const;
	NumerotationContext previousNumber (const NumerotationContext &, const int) const;

	Diagram *diagram_;
};

class UnitNum: public NumStrategy
{
	public:
	UnitNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class UnitFNum: public NumStrategy
{
	public:
	UnitFNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class TenNum: public NumStrategy
{
	public:
	TenNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class TenFNum: public NumStrategy
{
	public:
	TenFNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class HundredNum: public NumStrategy
{
	public:
	HundredNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class HundredFNum: public NumStrategy
{
	public:
	HundredFNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class StringNum: public NumStrategy
{
	public:
	StringNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class IdFolioNum: public NumStrategy
{
	public:
	IdFolioNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class FolioNum: public NumStrategy
{
	public:
	FolioNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class PlantNum: public NumStrategy
{
	public:
    PlantNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class LocmachNum: public NumStrategy
{
	public:
	LocmachNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};


class ElementLineNum: public NumStrategy
{
	public:
	ElementLineNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class ElementColumnNum: public NumStrategy
{
	public:
	ElementColumnNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};

class ElementPrefixNum: public NumStrategy
{
	public:
	ElementPrefixNum (Diagram *);
	QString toRepresentedString(const QString) const override;
	NumerotationContext next     (const NumerotationContext &, const int) const override;
	NumerotationContext previous (const NumerotationContext &, const int) const override;
};


#endif // NUMEROTATIONCONTEXTCOMMANDS_H
