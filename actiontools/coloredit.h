/*
	Actiona
	Copyright (C) 2005-2017 Jonathan Mercier-Ganady

	Actiona is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Actiona is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.

	Contact : jmgr@jmgr.info
*/

#ifndef COLOREDIT_H
#define COLOREDIT_H

#include "actiontools_global.h"
#include "subparameter.h"
#include "abstractcodeeditor.h"

#include <QWidget>

namespace Ui
{
	class ColorEdit;
}

class QColorDialog;
class QRegExpValidator;

namespace ActionTools
{
	class CodeLineEdit;

	class ACTIONTOOLSSHARED_EXPORT ColorEdit : public QWidget, public AbstractCodeEditor
	{
		Q_OBJECT

	public:
		explicit ColorEdit(QWidget *parent = 0);
		~ColorEdit();

		void setText(const QString &text);
		QString text() const;
		CodeLineEdit *codeLineEdit() const;
		bool isCode() const;
		void setCode(bool code);
		void setFromSubParameter(const SubParameter &subParameter);
		void openEditor(int line = -1, int column = -1);
		void setCompletionModel(QAbstractItemModel *completionModel);
        void setParameterContainer(const ParameterContainer *parameterContainer);
        QSet<QString> findVariables() const;
		void setChooseByPositionButtonVisible(bool visible);
		
	signals:
        void positionChosen(QPointF position);
		
	public slots:
        void setPosition(QPointF position);
		
	private slots:
        void on_chooseByPosition_positionChosen(QPointF position);
		void on_choose_clicked();
		void on_colorLineEdit_textChanged(const QString &text);
		void on_colorLineEdit_codeChanged(bool code);
		void onColorSelected();

	private:
		QColor currentColor() const;

		Ui::ColorEdit *ui;
		QColorDialog *mColorDialog;
        QRegExpValidator *mValidator;

		Q_DISABLE_COPY(ColorEdit)
	};
}

#endif // COLOREDIT_H
