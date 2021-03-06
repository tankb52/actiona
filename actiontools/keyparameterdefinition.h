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

#ifndef KEYPARAMETERDEFINITION_H
#define KEYPARAMETERDEFINITION_H

#include "parameterdefinition.h"
#include "actiontools_global.h"

namespace ActionTools
{
	class KeyEdit;

	class ACTIONTOOLSSHARED_EXPORT KeyParameterDefinition : public ParameterDefinition
	{
		Q_OBJECT

	public:
        KeyParameterDefinition(const Name &name, QObject *parent);

		void buildEditors(Script *script, QWidget *parent);
		void load(const ActionInstance *actionInstance);
		void save(ActionInstance *actionInstance);
		void setDefaultValues(ActionInstance *actionInstance);
		
	private:
		QVariant defaultValue(QVariant defaultValue = QVariant()) const			{ Q_UNUSED(defaultValue); return QVariant(); }
		void setDefaultValue(const QVariant &defaultValue)						{ Q_UNUSED(defaultValue); }
		
		KeyEdit *mKeyEdit;

		Q_DISABLE_COPY(KeyParameterDefinition)
	};
}

#endif // KEYPARAMETERDEFINITION_H
