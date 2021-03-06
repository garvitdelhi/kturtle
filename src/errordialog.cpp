/*
	Copyright (C) 2003-2008 Cies Breijs <cies AT kde DOT nl>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this program; if not, write to the Free
	Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
	Boston, MA 02110-1301, USA.
*/

#include "errordialog.h"

#include <QHeaderView>

#include <kdebug.h>

#include <kdialog.h>
#include <kglobalsettings.h>
#include <klocale.h>


ErrorDialog::ErrorDialog(QWidget* parent)
	: KDialog(parent)
{
	errorList = 0;

	setCaption(i18n("Errors"));
	setModal(false);
	setButtons(User1 | Help);
	setButtonGuiItem(User1, KGuiItem(i18n("Hide Errors"), "dialog-close", i18n("This button hides the Errors tab")));
// 	setButtonGuiItem(User1, i18n("Help on &Error"));  // TODO context help in the error dialog
	setDefaultButton(User1);
	showButtonSeparator(false);
	setHelp("reference", "kturtle");

	QWidget *baseWidget = new QWidget(this);
	setMainWidget(baseWidget);
	baseLayout = new QVBoxLayout(baseWidget); 
	
	label = new QLabel(baseWidget);
	label->setText(i18n("In this list you find the error(s) that resulted from running your code.\nGood luck!"));
	// \nYou can select an error and click the 'Help on Error' button for help.
	label->setScaledContents(true);
	baseLayout->addWidget(label);
	
	spacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
	baseLayout->addItem(spacer);
	
	errorTable = new QTableWidget(baseWidget);
	errorTable->setSelectionMode(QAbstractItemView::SingleSelection);
	errorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	errorTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	errorTable->setShowGrid(false);

	errorTable->setColumnCount(3);
	QStringList horizontalHeaderTexts;
	horizontalHeaderTexts << i18n("line") << i18n("description") << i18n("code");
	errorTable->setHorizontalHeaderLabels(horizontalHeaderTexts);
	errorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

	baseLayout->addWidget(errorTable);

	clear();
}



void ErrorDialog::clear()
{
	disable();
	errorList = 0;
	errorTable->clearContents();

	// put a friendly 'nothing to see here' notice in the empty table
	errorTable->setRowCount(1);
	QTableWidgetItem* emptyItem = new QTableWidgetItem(i18n("No errors occurred yet."));
	QFont emptyFont(KGlobalSettings::generalFont());
	emptyFont.setItalic(true);
	emptyItem->setFont(emptyFont);
	errorTable->setItem(0, 1, emptyItem);

	errorTable->resizeColumnsToContents();
// 	errorTable->setColumnWidth(0, errorTable->fontMetrics().width("88888"));
// 	errorTable->setColumnWidth(2, errorTable->fontMetrics().width("88888"));
// 	errorTable->setColumnWidth(1, errorTable->width() - errorTable->verticalHeader()->width() - errorTable->columnWidth(0) - errorTable->columnWidth(2));
}

void ErrorDialog::enable()
{
	Q_ASSERT (errorList != 0);
	errorTable->setEnabled(true);
	enableButton(Help, true);
	connect (errorTable, SIGNAL(itemSelectionChanged()), this, SLOT(selectedErrorChangedProxy()));
	errorTable->selectRow(0);
}

void ErrorDialog::disable()
{
	disconnect (errorTable, SIGNAL(itemSelectionChanged()), this, SLOT(selectedErrorChangedProxy()));
	errorTable->setEnabled(false);
	enableButton(Help, false);
	errorTable->clearSelection();
}


void ErrorDialog::setErrorList(ErrorList *list)
{
	errorList = list;
	errorTable->setRowCount(errorList->size());
	int row = 0;
	foreach (const ErrorMessage &error, *errorList) {
		int col = 0;
		QStringList itemTexts;
		itemTexts << QString::number(error.token().startRow()) << error.text() << QString::number(error.code());
		foreach (const QString &itemText, itemTexts) {
			errorTable->setItem(row, col, new QTableWidgetItem(itemText));
			col++;
		}
		row++;
	}
	errorTable->clearSelection();
	errorTable->resizeColumnsToContents();
	enable();
}

void ErrorDialog::selectedErrorChangedProxy()
{
	Q_ASSERT (errorList != 0);
	const Token* t = &errorList->at(errorTable->selectedItems().first()->row()).token();
	emit currentlySelectedError(t->startRow(), t->startCol(), t->endRow(), t->endCol());
	// kDebug() << "EMITTED: " << t->startRow() << ", " << t->startCol() << ", " << t->endRow() << ", " << t->endCol();
}


#include "errordialog.moc"

