#include "tracked_task_item_delegate.h"

#include "tracked_tasks_list_model.h"
#include "human_text.h"

#include <QPainter>
#include <sstream>

#ifdef WIN32
#include <Windows.h>
#endif //WIN32

void TrackedTaskItemDelegate::paint( QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index ) const
{
	// prepare
    painter->save();
  
	drawBackground(painter, option, index);

	const int y = option.rect.y();
	const int y_end = y + item_height - 1;

	//painter->setPen( QColor(0,128,0) );
	//painter->drawLine(0, y, 10, y_end);

	QVariant data = index.data();
	QString qs = data.toString();
	std::string utf8_text = qs.toUtf8().constData();

	const char *s = utf8_text.c_str();

	painter->setPen( QColor(0,0,0) );
	QPoint first_line_point = QPoint( 0, y + item_height/2 - 2 );
	painter->drawText( first_line_point, s );

	//const TrackedTasksListModel *model = static_cast<const TrackedTasksListModel*>( index.model() );
	time_t seconds_last_active = _tracked_tasks_list_model->seconds_last_active( index );
	time_t seconds_today = _tracked_tasks_list_model->seconds_today( index );

	int number_of_sub_tasks = _tracked_tasks_list_model->number_of_sub_tasks( index );

	std::ostringstream oss;
	if( seconds_last_active < 0 ) {
		oss << "no work ever";
	} else if( seconds_last_active < 60 ) {
		oss << "last started under 1 minute ago";
	} else {
		std::string human_elapsed_time = human_time_elapsed( seconds_last_active, true );
		oss << "last started over " << human_elapsed_time << " ago";
	}

	if( number_of_sub_tasks > 0 )
		oss << "  |||  " << number_of_sub_tasks << " hidden sub tasks";

	QPoint second_line_point = QPoint( 20, y + item_height - 4 );
	painter->drawText( second_line_point, oss.str().c_str() );
	
    // done
    painter->restore();
}

QSize TrackedTaskItemDelegate::sizeHint( const QStyleOptionViewItem &option,
                           const QModelIndex &index ) const
{
	//This controls how large each item is
	return QSize(50, 40);
}
