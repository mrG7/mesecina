/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: Evolution_slider_toolbar.cpp 104 2007-05-20 07:42:36Z miklosb $
 */

#include <gui/app/Evolution_slider_toolbar.h>
#include <gui/app/Evolution.h>
#include <gui/app/Main_window.h>
#include <constants.h>

Evolution_slider_toolbar::Evolution_slider_toolbar(Main_window * parent) : QToolBar() {
	this->setObjectName("Evolution slider");
	setMovable(false);
	main_window = parent;

	val_label = new QLabel;
	val_label->setText("0");
	val_label->setAlignment(Qt::AlignCenter);
	val_label->setMaximumWidth(30);
	val_label->setMinimumWidth(30);

	evolutions_combo_box = new QComboBox();
	evolutions_combo_box->setMinimumWidth(150);
	evolutions_combo_box->setMaximumWidth(150);

	min_line_edit = new QLineEdit();
	min_line_edit->setEnabled(false);
	min_line_edit->setMaximumWidth(30);
	min_line_edit->setText("0");
	min_validator = new QIntValidator (min_line_edit);
	min_line_edit->setValidator(min_validator);

	slider = new QSlider(Qt::Horizontal);
	slider->setMinimum(1);
	slider->setMaximum(20);

	max_line_edit = new QLineEdit();
	max_line_edit->setEnabled(true);
	max_line_edit->setMaximumWidth(30);
	max_line_edit->setText("20");
	max_validator = new QIntValidator (max_line_edit);
	max_line_edit->setValidator(min_validator);

	play_backward_button = new QPushButton();
	play_backward_button->setText("<");
	play_backward_button->setMaximumWidth(50);

	play_forward_button = new QPushButton();
	play_forward_button->setText(">");
	play_forward_button->setMaximumWidth(50);	

	layout = new QHBoxLayout();
	layout->addWidget(val_label);
	layout->addWidget(evolutions_combo_box);
	layout->addWidget(min_line_edit);
	layout->addWidget(slider);
	layout->addWidget(max_line_edit);
	layout->addWidget(play_backward_button);
	layout->addWidget(play_forward_button);
	layout->setSpacing(6);
	layout->setMargin(0);

	wrapper = new QWidget();
	wrapper->setLayout(layout);
	addWidget(wrapper);

	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(value_changed(int)));
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(change_geometries(int)));
	connect(max_line_edit, SIGNAL(returnPressed()), this, SLOT(max_value_entered()));

	connect(play_forward_button, SIGNAL(clicked()), this, SLOT(play_forward()));
	connect(play_backward_button, SIGNAL(clicked()), this, SLOT(play_backward()));

	connect(evolutions_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(evolution_changed(int)));
}


void Evolution_slider_toolbar::connect_evolution_change(bool flag) {
	if (flag) connect(evolutions_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(evolution_changed(int)));
	else disconnect(evolutions_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(evolution_changed(int)));
}
void Evolution_slider_toolbar::value_changed(int val) {
	val_label->setText(tr("%1").arg(val-1));
}

void Evolution_slider_toolbar::min_value_entered() {
}

void Evolution_slider_toolbar::max_value_entered() {
	slider->setMaximum(max_line_edit->text().toInt()+1);
}

void Evolution_slider_toolbar::play_forward() {
	max_value_entered();
	while (slider->value() < slider->maximum()) {
		slider->setValue(slider->value()+1);
		repaint();
	}
}

void Evolution_slider_toolbar::keyPressEvent(QKeyEvent *) {
}

void Evolution_slider_toolbar::keyReleaseEvent(QKeyEvent *e) {
	if (e->modifiers() != Qt::ControlModifier) return;
	switch(e->key()) {
		case Qt::Key_Up: 
			evolutions_combo_box->setCurrentIndex(evolutions_combo_box->currentIndex()+1);
			break;
		case Qt::Key_Down: 
			evolutions_combo_box->setCurrentIndex(evolutions_combo_box->currentIndex()-1);
			break;
		case Qt::Key_Left: 
			slider->setValue(slider->value()-1);
			break;
		case Qt::Key_Right:
			slider->setValue(slider->value()+1);
			break;
	}

}

void Evolution_slider_toolbar::play_backward() {
	max_value_entered();
	while (slider->value() > slider->minimum()) {
		slider->setValue(slider->value()-1);
		repaint();
	}
}

//void Evolution_slider_toolbar::add_evolution(Evolution* e) {
//	evolutions.push_back(e);
//	evolutions_combo_box->insertItem(evolutions_combo_box->count(),e->get_name().c_str());
//}

void Evolution_slider_toolbar::put_evolution(const std::string& name, std::list<Geometry*>* init_geometries) {
	std::vector<Evolution*>::iterator e_it, e_end = evolutions.end();
	for (e_it = evolutions.begin(); e_it!=e_end; ++e_it) {
		if ((*e_it)->get_name() == name) return;
	}
	evolutions.push_back(new Evolution(name, init_geometries));
	evolutions_combo_box->insertItem(evolutions_combo_box->count(),evolutions.back()->get_name().c_str());
//	std::cout << "Evolution combo_box has " << evolutions_combo_box->count() << " entries, last one added: " << evolutions.back()->get_name() << std::endl;
}

void Evolution_slider_toolbar::init_evolutions(std::list<Geometry*>* igl) {
	std::set<Geometry*> throw_geometries;
	std::vector<Evolution*>::iterator e_it, e_end = evolutions.end();
	for (e_it = evolutions.begin(); e_it!=e_end; ++e_it) {
		Evolution::Geometry_list_vector::iterator gl_it = (*e_it)->gll.begin(), gl_end = (*e_it)->gll.end();
		for (;gl_it!=gl_end; gl_it++) {
			Evolution::Geometry_list::iterator g_it = gl_it->begin(), g_end = gl_it->end();
			for (;g_it!=g_end; g_it++) {
				throw_geometries.insert(*g_it);
			}
		}
	}
//	std::cout << LOG_BLUE << "Total geometries in evolutions: " << throw_geometries.size() << std::endl;
	std::list<Geometry*>::iterator kg_it = igl->begin(), kg_end = igl->end(); 
	for (; kg_it!=kg_end; kg_it++) {
		std::set<Geometry*>::iterator loc = throw_geometries.find(*kg_it);
		if (loc != throw_geometries.end()) {
			throw_geometries.erase(loc);
//			std::cout << LOG_BLUE << " found geometry which should not be deleted" << std::endl;
		}
	}

//	std::cout << LOG_BLUE << "Geometries to free up: " << throw_geometries.size() << std::endl;
	std::set<Geometry*>::iterator tg_it = throw_geometries.begin(), tg_end = throw_geometries.end();
	for (;tg_it!=tg_end; tg_it++) {
		if (*tg_it)	delete *tg_it;
	}
	for (e_it = evolutions.begin(); e_it!=e_end; ++e_it) {
		(*e_it)->init_geometries(igl);
	}
	move_evolutions_to_zero();

 


}

void Evolution_slider_toolbar::clear_evolutions() {
	//std::vector<Evolution*>::iterator  e_it, e_end = evolutions.end();
	//for (e_it = evolutions.begin(); e_it != e_end; e_it++) {
	//	Evolution* e = *e_it;
	//	delete e;
	//}
	evolutions.clear();
	evolutions_combo_box->clear();
}

void Evolution_slider_toolbar::move_evolutions_to_zero() {
	std::vector<Evolution*>::iterator  e_it, e_end = evolutions.end();
	for (e_it = evolutions.begin(); e_it != e_end; e_it++) {
		Evolution* e = *e_it;
		e->current = 1;
	}
	slider->setValue(1);
}

void Evolution_slider_toolbar::change_geometries(int index) {
	if (evolutions.size() < 1) return;
	index--; // so that it fits to the 0..n range
	int e_index  = evolutions_combo_box->currentIndex();
	Evolution* e = evolutions[e_index];
	int max_computed = e->get_max_computed();
	// make sure we have computed one by one everything until the current index
	if (index >= max_computed) {
		for (int i=max_computed; i<index; i++) {
			e->add_geometries(main_window->clone_and_bind_geometries());
//			e->step_up_operation();
		}
	}
	// set the current index
	std::list<Geometry*> *gl = e->get_geometries(index);
	main_window->set_geometries(gl);
}

void Evolution_slider_toolbar::evolution_changed(int index) {
	if (index == -1) 
		return;
	Evolution* e = evolutions[index];
	if (e->show_max > e->current) {
		slider->setValue(e->current);
		slider->setMaximum(e->show_max);
	} else {
		slider->setMaximum(e->show_max);
		slider->setValue(e->current);
	}
	max_line_edit->setText(tr("%1").arg(e->show_max));
}
