#ifndef TASK_WEEK_EDITOR_H_SFMXVN
#define TASK_WEEK_EDITOR_H_SFMXVN

#include <QWidget>
#include <qpushbutton.h>

#include <stack>

typedef void (*taskChangedCallback)(void *callback_data, void *task_data, const char *label, time_t time);

class QLineEdit;
class TaskList;
class LoggedTask;
class UserSettings;

class UndoAction
{
public:
	UndoAction(LoggedTask *logged_task, time_t original_start_time );
	void apply_undo();
private:
	LoggedTask *_logged_task;
	time_t _original_start_time;
};

class TaskWeekEditor : public QWidget
{
    Q_OBJECT //if we include this, we need to use the moc tool to generate some c++ code for us. ugh.

public:
    TaskWeekEditor(TaskList *tasks, UserSettings *user_settings, QWidget *parent = 0);

	void addTask( time_t start_time, const char *task_name, void *data);

	void setWeek( time_t time );
	inline void setTaskChangedCallback(taskChangedCallback cb, void *callback_data) { _task_changed_cb = cb; _task_changed_cb_data = callback_data; }

	bool paint_light;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void resizeEvent(QResizeEvent *event);

public slots:
	void undo();

private slots:
    void handleButton();

private:
	class VisibleTask;

	TaskList *_tasks;

	std::stack< UndoAction > _undo_actions;

	taskChangedCallback _task_changed_cb;
	void *_task_changed_cb_data;

	//returns a displayed Task if the position (x,y) overlaps with the starting time of the task
	LoggedTask* getTaskByStartPos( int x, int y, int y_tolerance = 1 );

	//returns a displayed Task if the position (x,y) is contained in the task rectangle
	LoggedTask* getTaskByPos( int x, int y );

	VisibleTask* getVisibleTaskByPos( int x, int y );

	inline int dayOfWeekByPos( int x ) const {
		int x_rel = (x - _margin - _time_column_width);
		if( x_rel < 0 )
			return -1;
		int day_of_week = x_rel / _day_column_width;
		if( day_of_week < 0 || day_of_week > 6 )
			day_of_week = -1;
		return day_of_week;
	}

	time_t mousePosToTimestamp( int x, int y ) const;

	void setTaskTimestamp(LoggedTask *task, time_t timestamp, time_t min_time_padding = 60 );

	//grid position variables
	int _top_heading_size;
	int _margin;
	int _time_column_width;
	int _day_column_width;
	int _grid_width;
	int _grid_height;

	//in seconds since midnight localtime
	int _day_start_time;
	int _day_end_time;

	//UTC epoch time
	int _week_start_time;
	int _week_end_time;

	//when dragging the start time of a task, this is set to that task
	LoggedTask *_selected_boundary_task;
	//initial value before editing a task (used for undo)
	time_t _selected_boundary_task_initial_value;

	//when selecting the task (to rename), this is set
	LoggedTask *_selected_task;
	VisibleTask *_selected_visible_task;

	QLineEdit *_task_name_line_edit;

	void setTaskEditorPosition();

	QPushButton *_prev_week_button;
	QPushButton *_next_week_button;
	QPushButton *_this_week_button;

	UserSettings *_user_settings;

	//all tasks
	//std::vector< Task > _tasks;
	//tasks visible on the selected week/time
	std::vector< VisibleTask > _visible_tasks;

	void calcWindowResizeValues();
	void calcVisibleTasks();

	//returns number of seconds per tick to achive a minimum of 'tick_spacing' pixels per plot tick
	int getSecondsPerPlotTick( int tick_spacing = 50 ) const;

	time_t localDayTimeToUTC(int day_of_week, int seconds_since_midnight) const;

	inline int getYFromTime( int seconds_since_midnight_localtime )
	{
		const int day_seconds_displayed = _day_end_time - _day_start_time;
		float tick_pct = static_cast<float>(seconds_since_midnight_localtime - _day_start_time) / day_seconds_displayed;
		int tick_y = tick_pct * _grid_height + _margin + _top_heading_size;
		return tick_y;
	}

	
	class VisibleTask
	{
	public:
		//0 = sun, 1 = mon, ... 6 = sat
		int day_of_week;
		int start_y;
		int stop_y;
		bool continued_task;

		//1 - spring forward, 0 - no, -1 fall back
		int daylight_saving;
		LoggedTask *task;
		int task_seconds;
		bool in_progress;

		//counter corresponding to position of task (used for consistent coloring)
		int task_i;
	};
};

#endif //TASK_WEEK_EDITOR_H_SFMXVN