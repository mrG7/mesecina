/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Evolution_slider_toolbar.h 104 2007-05-20 07:42:36Z miklosb $
 */

#ifndef EVOLUTION_SLIDER_TOOLBAR_H
#define EVOLUTION_SLIDER_TOOLBAR_H

#include <QtGui/QToolBar>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QIntValidator>

#include <vector>

class Evolution;
class Geometry;
class Main_window;

class Evolution_slider_toolbar : public QToolBar {
	Q_OBJECT

public:
	Evolution_slider_toolbar(Main_window * parent = 0);

	QWidget* wrapper;
	
	QHBoxLayout* layout;
	QLabel* val_label;
	QComboBox* evolutions_combo_box;
	QLineEdit* min_line_edit;
	QIntValidator  *min_validator;
	QSlider* slider;
	QLineEdit* max_line_edit;
	QIntValidator  *max_validator;
	QPushButton* play_forward_button;
	QPushButton* play_backward_button;

//	void add_evolution(Evolution* e);
	void clear_evolutions();
	void move_evolutions_to_zero();
	void connect_evolution_change(bool flag);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);
	void put_evolution(const std::string& name, std::list<Geometry*>* init_geometries);
	void init_evolutions(std::list<Geometry*>* init_geometries);
	bool has_evolutions() { return !evolutions.empty(); }

private slots:
	void min_value_entered();
	void value_changed(int val);
	void max_value_entered();
	void play_forward();
	void play_backward();
	void change_geometries(int index);

public slots:
	void evolution_changed(int index);

signals:
	void clone_and_bind_geometries(std::list<Geometry*>* &new_gl);
	void set_geometries(std::list< Geometry* > *gl);


private:
	std::vector<Evolution*> evolutions;
	Main_window* main_window;
};

#endif //EVOLUTION_SLIDER_TOOLBAR_H