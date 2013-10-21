#include <qcompleter.h>

class TrackedTasksListModel;

/*
This custom completer is used to fool QT's qcompleter
to allow searching for more than prefixes

It is somewhat inspired by http://www.mimec.org/node/304
and uses splitPath to send any entered text to the underlying TrackedTasksListModel
so that when it is used to display a list, it will return all strings matching the entered text

*/
class TrackedTasksCompleter : public QCompleter
{
public:
	TrackedTasksCompleter(TrackedTasksListModel *model, QObject *parent = 0);

	  virtual QStringList splitPath(const QString& s) const;
protected:
	TrackedTasksListModel *_model;
};
