#include "tracked_task_line_edit.h"

#include "tracked_tasks_list_model.h"
#include "tracked_task_completer.h"
#include "tracked_task_item_delegate.h"
#include "task_list.h"

#include <QtGui\QKeyEvent>
#include <QtWidgets\qcompleter.h>

#include <qabstractitemview.h>

TrackedTaskLineEdit::TrackedTaskLineEdit( TaskList *task_list, UserSettings *user_settings, QWidget *parent ) :
	QLineEdit(parent),
	_current_prefix_match( false )
{
	//create a model which is used to filter the completed results
	_tracked_tasks_list_model = new TrackedTasksListModel( user_settings, task_list, this );

	//special completer that does more than just prefix
	//pointer must explicitly be deleted by this widget
	_completer = new TrackedTasksCompleter( _tracked_tasks_list_model, this );
	setCompleter( _completer );

	//delegator to draw task including last active + total time today
	//pointer must explicitly be deleted by this widget
	_item_delegate = new TrackedTaskItemDelegate( _tracked_tasks_list_model, this );
	_completer->popup()->setItemDelegate( _item_delegate );
}

TrackedTaskLineEdit::~TrackedTaskLineEdit()
{
	delete _tracked_tasks_list_model;
	delete _item_delegate;
	delete _completer;
}

bool TrackedTaskLineEdit::event( QEvent *event )
{
	if( event->type() == QEvent::KeyPress ) {
		QKeyEvent *key_event = static_cast<QKeyEvent*>( event );
		if( key_event->key() == Qt::Key_Tab ) {

			if( selectedText().length() == 0 )
				_current_prefix_match = false;

			if( _current_prefix_match ) {
				//TODO make sure the user hasn't pushed up/down (or rather make sure up/down sets this to false

				//deselect text (moving cursor to end)
				int text_size = text().length();
				setSelection( text_size, text_size );
				_current_prefix_match = false;

				const int number_of_sub_tasks = _tracked_tasks_list_model->number_of_sub_tasks( 0 );
				if( number_of_sub_tasks > 0 ) {
					completer()->setCompletionPrefix( text() );
					completer()->complete();
					//selectCompletion();
				} else {
					//deselect
					completer()->popup()->hide();
				}
			} else {
				const int current_selection_row = completer()->popup()->currentIndex().row();
				if( current_selection_row >= 0 ) {
					const int number_of_sub_tasks = _tracked_tasks_list_model->number_of_sub_tasks( current_selection_row );
					if( number_of_sub_tasks == 0 ) {
						completer()->popup()->hide();
					} else {
						completer()->setCompletionPrefix( text() );
						completer()->complete();
					}
				}
			}
			return true;
		}
	}
	return QLineEdit::event( event );
}

void TrackedTaskLineEdit::keyPressEvent( QKeyEvent * e )
{
	int key = e->key();
	bool has_modifiers = e->modifiers() & ~Qt::ShiftModifier;
	if( Qt::Key_Escape == key ) {
		if( completer()->popup()->isVisible() ) {
			completer()->popup()->hide();
		} else {
			QLineEdit::keyPressEvent(e);
		}
	} else {
		int text_len_before = text().length() - selectedText().length();
		QLineEdit::keyPressEvent(e);
		int text_len_after = text().length() - selectedText().length();
		if( text_len_before < text_len_after &&
			key != Qt::Key_Backspace && 
			key != Qt::Key_Left && 
			key != Qt::Key_Right && 
			key != Qt::Key_Delete && 
			key != Qt::Key_Home &&
			key != Qt::Key_End &&
			!has_modifiers ) {
			selectCompletion();
		}
	}
}

void TrackedTaskLineEdit::selectCompletion()
{
	int start = text().length();
	QString completed_string = completer()->currentCompletion();
	if( start > 0 && completed_string.startsWith( text() ) ) {
		setText( completed_string );
		setSelection( start, text().length() - start );
		_current_prefix_match = true;
	} else {
		_current_prefix_match = false;
	}
}
