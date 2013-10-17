#include "qitemdelegate.h"

class TrackedTasksListModel;

class TrackedTaskItemDelegate : public QItemDelegate
{
public:
	TrackedTaskItemDelegate( const TrackedTasksListModel *tracked_tasks_list_model, QWidget *parent = 0) : 
		QItemDelegate( parent ),
		_tracked_tasks_list_model( tracked_tasks_list_model ),
		item_height( 40 )
	{}

    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const ;

    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
private:
	const int item_height;
	const TrackedTasksListModel *_tracked_tasks_list_model;
};