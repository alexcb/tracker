#include "editor_widget.h"

#include "task_week_editor.h"

#include <QWidget>
#include <QVBoxLayout>

EditorWidget::EditorWidget( TaskList *tasks, UserSettings *user_settings, QWidget *parent )
	: QWidget( parent )
{
	QVBoxLayout *vbox = new QVBoxLayout( this );
	vbox->setSpacing( 1 );

	task_week_editor = new TaskWeekEditor( tasks, user_settings, this );
	vbox->addWidget( task_week_editor );

	setLayout( vbox );
}