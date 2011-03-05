/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. 
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_widget_3.cpp 737 2009-05-16 15:40:46Z miklosb $
 */


#include <GL/glew.h>

#include <gui/gl_widget_3/GL_draw_layer_3.h>
#include <gui/gl_widget_3/GL_widget_3.h>
#include <gui/gl_widget/Visualization_layer.h>
#include <gui/app/static/Application_settings.h>

#include <constants.h>
//#include "../textfile.h"

#include <algorithm>
#include <iostream>
#include <math.h>

#include <GL/glut.h>
#include <QtXml/QDomElement>

//double round(double x) {
//	if(x>=0.5){return ceil(x);}else{return floor(x);}
//}

GL_widget_3::GL_widget_3(QWidget * parent) : QGLViewer(parent), use_display_lists(true) {
	QSettings settings;
	if (!settings.contains("camera-projection-orthographic")) 
		settings.setValue("camera-projection-orthographic", false);
	Application_settings::add_setting("camera-projection-orthographic", AS_BOOL);
	if (!settings.contains("camera-projection-perspective-fieldofview")) 
		settings.setValue("camera-projection-perspective-fieldofview", M_PI/4);
	Application_settings::add_setting("camera-projection-perspective-fieldofview", AS_DOUBLE);
	if (!settings.contains("sphere-rendering-resolution")) 
		settings.setValue("sphere-rendering-resolution", SPHERE_RENDERING_RENDERING);
	Application_settings::add_setting("sphere-rendering-resolution", AS_INTEGER);
	layers.clear();
	default_bounding_box = true;
	bounding_xmin = bounding_xmax = bounding_ymin = bounding_ymax = bounding_zmin = bounding_zmax = 0;
	want_repaint =false;

	sphere_dl = 0;
	sphere_wire_dl = 0;
}


void  GL_widget_3::request_repaint() {
	want_repaint = true;
}

void GL_widget_3::postSelection(const QPoint& point) {
//	std::cout << "post selection" << std::endl;
	bool found;
    qglviewer::Vec selectedPoint = camera()->pointUnderPixel(point, found);
//	std::cout << " The selected point is: (" << selectedPoint.x << "," << selectedPoint.y << "," << selectedPoint.z << ")" << std::endl;
	std::vector<Visualization_layer *>::iterator it, end=layers.end();
	for (it=layers.begin(); it!=end; it++) {
		GL_draw_layer_3 *l = (GL_draw_layer_3*)*it;
		if (l->is_active()) {
			l->selection_at(selectedPoint.x, selectedPoint.y, selectedPoint.z);
		}
	}
}

//GL_widget_3::~GL_widget_3() {}


void GL_widget_3::update_bounding_box(const Box3D& r) {
	if (default_bounding_box || r.xmin < bounding_xmin) bounding_xmin = r.xmin;
	if (default_bounding_box || r.xmax > bounding_xmax) bounding_xmax = r.xmax;
	if (default_bounding_box || r.ymin < bounding_ymin) bounding_ymin = r.ymin;
	if (default_bounding_box || r.ymax > bounding_ymax) bounding_ymax = r.ymax;
	if (default_bounding_box || r.zmin < bounding_zmin) bounding_zmin = r.zmin;
	if (default_bounding_box || r.zmax > bounding_zmax) bounding_zmax = r.zmax;
	default_bounding_box = false;
}

void GL_widget_3::set_window_to_boundingbox() {
	showEntireScene();
}


void GL_widget_3::export_to_image(QString image_filepath) {


	//setSnapshotFormat("PNG");
	//saveSnapshot(image_filepath, true);

	bool old_dlists = get_use_display_lists();
	set_use_display_lists(false);
	repaint();
	QImage image = grabFrameBuffer();
	set_use_display_lists(old_dlists);
	repaint();

	image.save(image_filepath,"PNG");
	std::cout << "Image written to " << image_filepath.toStdString() << std::endl;
}

void GL_widget_3::export_to_vector(QString file_name) {
	if (file_name.toUpper().endsWith(".PS")) setSnapshotFormat("PS");
	if (file_name.toUpper().endsWith(".PDF")) setSnapshotFormat("PDF");

	saveSnapshot(file_name, true);
	// something different here
}

void GL_widget_3::print_to_printer() {
	QPrinter printer;

	QPrintDialog print_dialog(&printer/*, this*/);
    if (print_dialog.exec() == QDialog::Accepted) {
		//something different here
	}
}

void GL_widget_3::attach(Visualization_layer *l) {
	layers.push_back(l);
	l->attach(this);
//	l->activate();
}

void GL_widget_3::detach(Visualization_layer *l) {
	std::vector<Visualization_layer*>::iterator to_delete = std::find(layers.begin(),layers.end(),l);
	if (to_delete!=layers.end()) 
		layers.erase(to_delete);
}

void GL_widget_3::repaintGL() {
	updateGL();
	emit repaitedGL();
}

void GL_widget_3::resizeGL( int w, int h ) {
	emit widget_resized_to(QRect(0,0,w,h));
	QGLViewer::resizeGL(w,h);
}

void GL_widget_3::init() {
	setBackgroundColor(Qt::white);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);										// Enable Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		// Type Of Blending To Use
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);		// Antialias the lines

	
	glDisable( GL_COLOR_MATERIAL );
	
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
//	glShadeModel(GL_FLAT);
	glEnable(GL_NORMALIZE);

	setMouseBinding(Qt::LeftButton,  ZOOM_ON_PIXEL,  true);
//	setMouseBinding(Qt::LeftButton,  RAP_FROM_PIXEL,  true);
	setMouseBinding(Qt::RightButton, ZOOM_TO_FIT,    true);
	setMouseBinding(Qt::ALT + Qt::LeftButton, ALIGN_CAMERA,   true);
	
//	setMouseBinding(Qt::RightButton, RAP_IS_CENTER,    true);

	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50);
	setManipulatedFrame(0);
	
	set_camera_projection();

	create_sphere_display_list();
}

void GL_widget_3::set_camera_projection() {
	QSettings settings;
	if (settings.value("camera-projection-orthographic").toBool())
		camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);
	else camera()->setType(qglviewer::Camera::PERSPECTIVE);

	camera()->setFieldOfView(settings.value("camera-projection-perspective-fieldofview").toDouble());
}

void GL_widget_3::add_point_to_selection(double x, double y, double z) {
	emit s_add_point_to_selection(x,y,z);
}
void GL_widget_3::remove_point_from_selection(double x, double y, double z) {
	emit s_remove_point_from_selection(x,y,z);
}
void GL_widget_3::clear_selection() {
	emit s_clear_selection();
}

void GL_widget_3::create_sphere_display_list() {
	// generate a display list for a sphere
	if (sphere_dl!=0) glDeleteLists(sphere_dl,1);
	sphere_dl = glGenLists(1);
	QSettings settings;
	glNewList(sphere_dl,GL_COMPILE);
		glutSolidSphere(1, settings.value("sphere-rendering-resolution").toInt(), settings.value("sphere-rendering-resolution").toInt());
	glEndList();

	if (sphere_wire_dl!=0) glDeleteLists(sphere_wire_dl,1);
	sphere_wire_dl = glGenLists(1);
	glNewList(sphere_wire_dl,GL_COMPILE);
		glutWireSphere(1, settings.value("sphere-rendering-resolution").toInt(), settings.value("sphere-rendering-resolution").toInt());
	glEndList();


//	std::cout << LOG_BLUE <<"new sphere rendering with " << settings.value("sphere-rendering-resolution").toInt() << " resolution: " << sphere_dl << std::endl;
}


void GL_widget_3::draw () {
	default_bounding_box = true;

	bool is_valid;
	std::vector<Visualization_layer *>::iterator it, end=layers.end();
	for (it=layers.begin(); it!=end; it++) {
		GL_draw_layer_3 *l = (GL_draw_layer_3*)*it;
		if (l->is_active()) {
			l->draw();
			Box3D bbox = l->get_3d_bounding_box(&is_valid);
			if (is_valid) 
				update_bounding_box(bbox);
		}
	}
	setSceneBoundingBox(qglviewer::Vec((float)bounding_xmin, (float)bounding_ymin, (float)bounding_zmin),
						qglviewer::Vec((float)bounding_xmax, (float)bounding_ymax, (float)bounding_zmax));
//	std::cout << "BBox: [" << bounding_xmin << ", " << bounding_ymin << ", " << bounding_zmin << "] - [" << bounding_xmax << ", " << bounding_ymax << ", " << bounding_zmax << "]" << std::endl;

	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);

}

//void GL_widget_3::paint_to_painter() {
//	painter->setClipRegion(QRegion(0,0,width(), height()));
//	std::vector<Visualization_layer *>::iterator it, end=layers.end();
//	for (it=layers.begin(); it!=end; it++) {
//		Visualization_layer *l = *it;
//		if (l->is_active())
//			l->draw();
//	}
//}



void GL_widget_3::mousePressEvent(QMouseEvent *e)
{
//	emit(s_mousePressEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->mousePressEvent(e);
	QGLViewer::mousePressEvent(e);
}

void GL_widget_3::mouseReleaseEvent(QMouseEvent *e)
{
//	emit(s_mouseReleaseEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->mouseReleaseEvent(e);
	QGLViewer::mouseReleaseEvent(e);
}

void GL_widget_3::mouseMoveEvent(QMouseEvent *e)
{
//	emit(s_mouseMoveEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->mouseMoveEvent(e);
	QGLViewer::mouseMoveEvent(e);
}

void GL_widget_3::wheelEvent(QWheelEvent *e)
{
//	emit(s_wheelEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->wheelEvent(e);
	QGLViewer::wheelEvent(e);
}

void GL_widget_3::mouseDoubleClickEvent(QMouseEvent *e)
{
//	emit(s_mouseDoubleClickEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->mouseDoubleClickEvent(e);
	QGLViewer::mouseDoubleClickEvent(e);
}

void GL_widget_3::keyPressEvent(QKeyEvent *e)
{
//	emit(s_keyPressEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->keyPressEvent(e);
	if (e->key() == Qt::Key_V && e->modifiers() == (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier)) {
		qglviewer::ManipulatedCameraFrame* frame = camera()->frame();
		QDomDocument doc;
		QDomElement el = frame->domElement("qglview-camera",doc);
		doc.appendChild(el);
		QSettings settings;
		settings.setValue("viewer-buffer-3d-view-state",doc.toString());
		std::cout << "Camera view stored" << std::endl;
	}
	if (e->key() == Qt::Key_V && e->modifiers() == (Qt::ControlModifier | Qt::AltModifier)) {
		QSettings settings;
		QString doc_text = settings.value("viewer-buffer-3d-view-state").toString();
		QDomDocument doc;
		doc.setContent(doc_text);
		QDomElement camera_element = doc.documentElement();
		camera()->frame()->initFromDOMElement(camera_element);
		repaint();
		std::cout << "Camera view reloaded" << std::endl;
	}

	if (e->key() == Qt::Key_O && e->modifiers() == (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier)) {
	}
	if (e->key() == Qt::Key_O && e->modifiers() == (Qt::ControlModifier | Qt::AltModifier)) {


	}

	QGLViewer::keyPressEvent(e);
}


void GL_widget_3::save_view(bool) {
	qglviewer::ManipulatedCameraFrame* frame = camera()->frame();
	QDomDocument doc;
	QDomElement el = frame->domElement("qglview-camera",doc);
	doc.appendChild(el);

	QSettings settings;
	QString file_name = QFileDialog::getSaveFileName(
		this,
		"Choose a filename to save view",
		settings.value("last-view-directory",QString()).toString(),
		"View files (*.3view)");
	if (file_name=="") return;
	if (!file_name.endsWith(".3view")) file_name += ".3view";
	QString save_directory = file_name;
	save_directory.truncate(file_name.lastIndexOf('/'));
	settings.setValue("last-view-directory",save_directory);

	QFile f(file_name);
	if ( !f.open( QIODevice::WriteOnly ) ) {
		std::cout << LOG_ERROR << tr("File `%1' could not be open for writing!").arg(file_name).toStdString() << std::endl;
		return;
	}
	QTextStream fs( &f );
	fs << doc.toString();
	f.close();

	std::cout << "Camera view stored in " << file_name.toStdString()  << std::endl;
}

void GL_widget_3::load_view(bool) {
	QSettings settings;

	QString file_name = QFileDialog::getOpenFileName(
		this,
		"Choose a filename to load view",
		settings.value("last-view-directory",QString()).toString(),
		"View files (*.3view)");
	if (file_name!="") {
		if (!file_name.endsWith(".3view")) file_name += ".3view";
		QString save_directory = file_name;
		save_directory.truncate(file_name.lastIndexOf('/'));
		settings.setValue("last-view-directory",save_directory);

		QFile file(file_name);
		if (!file.open(QIODevice::ReadOnly)) {
			std::cout << LOG_ERROR << tr("File `%1' could not be open for reading!").arg(file_name).toStdString() << std::endl;
			return;
		}
		QDomDocument doc;
		if (!doc.setContent(&file)) {
			std::cout << LOG_ERROR << tr("File `%1' can not be parsed to an xml").arg(file_name).toStdString() << std::endl;
			file.close();
			return;
		}
		file.close();

		QDomElement camera_element = doc.documentElement();
		camera()->frame()->initFromDOMElement(camera_element);
		repaint();
		std::cout << "Camera view reloaded from " << file_name.toStdString() << std::endl;
	}
}


void GL_widget_3::keyReleaseEvent(QKeyEvent *e)
{
//	emit(s_keyReleaseEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->keyReleaseEvent(e);
	QGLViewer::keyReleaseEvent(e);
}

void GL_widget_3::enterEvent(QEvent *e)
{
//	emit(s_enterEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->enterEvent(e);
	QGLViewer::enterEvent(e);
}

void GL_widget_3::leaveEvent(QEvent *e)
{
//	emit(s_leaveEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->leaveEvent(e);
	QGLViewer::leaveEvent(e);
}

bool GL_widget_3::event(QEvent *e)
{
//	emit(s_event(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->event(e);
	return QGLViewer::event(e);
}

void GL_widget_3::application_settings_changed(const QString& settings_name) {
	//send it to the layers
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		(*it)->application_settings_changed(settings_name);

	if (settings_name == "sphere-rendering-resolution") {
		create_sphere_display_list();
		want_repaint = true;
	}

	if (settings_name.startsWith("camera-projection")) {
		set_camera_projection();
		want_repaint = true;
	}

	if (want_repaint) {
		repaintGL();
		want_repaint = false;
	}

}


