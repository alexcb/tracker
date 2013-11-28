#include "tracked_task_completer.h"

#include "tracked_tasks_list_model.h"

#include <QStringList>

TrackedTasksCompleter::TrackedTasksCompleter(TrackedTasksListModel *model, QObject *parent) :
QCompleter( model, parent ),
	_model( model )
{
}

//somewhat inspired by http://www.mimec.org/node/304
QStringList TrackedTasksCompleter::splitPath(const QString& s) const
{
	QString search_term = completionPrefix();

	//change model to only return results matching `search_term'
	_model->setSearchTerm(search_term);

	return QStringList();
}
