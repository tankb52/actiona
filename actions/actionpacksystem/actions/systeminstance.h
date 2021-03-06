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

#ifndef SYSTEMINSTANCE_H
#define SYSTEMINSTANCE_H

#include "actioninstance.h"
#include "script.h"
#include "../systemsession.h"

namespace Actions
{
	class SystemInstance : public ActionTools::ActionInstance
	{
		Q_OBJECT
		Q_ENUMS(Operation)

	public:
		enum Operation
		{
			Logout,
			Restart,
			Shutdown,
			Suspend,
			Hibernate,
			LockScreen,
			StartScreenSaver
		};
		enum Exceptions
		{
			NotAvailable = ActionTools::ActionException::UserException
		};

		SystemInstance(const ActionTools::ActionDefinition *definition, QObject *parent = 0)
			: ActionTools::ActionInstance(definition, parent) {}

		static ActionTools::StringListPair operations;
		static ActionTools::StringListPair modes;

		void startExecution();

	private:
		Q_DISABLE_COPY(SystemInstance)
	};
}

#endif // SYSTEMINSTANCE_H
