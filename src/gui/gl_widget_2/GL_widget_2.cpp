/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures. 
 * More info: http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: GL_widget_2.cpp 586 2009-04-03 20:01:21Z miklosb $
 */

#include <algorithm>
#include <iostream>
#include <math.h>

#include <QtCore/QSettings>

#include <GL/glew.h>
#include <QtOpenGL/QGLPixelBuffer>

#include <constants.h>
#include <gui/app/static/Application_settings.h>
#include <gui/app/static/Color_map_factory.h>

#include <gui/gl_widget_2/GL_widget_2.h>
#include <gui/gl_widget/Visualization_layer.h>

//#include "Computation_thread.h"



double round(double x) {
	if(x>=0.5){return ceil(x);}else{return floor(x);}
}

GL_widget_2::GL_widget_2(QWidget * parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent), renderer_type(OPEN_GL), use_display_lists(true) {
	xmin = xmin_old = -300;
	xmax = xmax_old = 100;
	ymin = ymin_old = -300;
	ymax = ymax_old = 300;
	layers.clear();
	default_cursor = cursor();
	default_bounding_box = true;
	bounding_ymin = bounding_xmin = -1;
	bounding_ymax = bounding_xmax = 1;
	painter = 0;
	pbuffer = 0;
	dynamic_texture = 0;
	rendering_effect = RENDERING_NORMAL;
	want_repaint = false;
	
//	full_screen = false;

	QSettings settings;
	if (!settings.contains("npr-render")) {
		settings.setValue("npr-render", false);
	} Application_settings::add_setting("npr-render", AS_BOOL);
	if (!settings.contains("npr-render-paper")) {
		settings.setValue("npr-render-paper", true);
	} Application_settings::add_setting("npr-render-paper", AS_BOOL);
	if (!settings.contains("npr-render-distortion")) {
		settings.setValue("npr-render-distortion", (double)0.015);
	} Application_settings::add_setting("npr-render-distortion", AS_DOUBLE);
	if (!settings.contains("npr-render-edge-detection-width")) {
		settings.setValue("npr-render-edge-detection-width", (double)0.001);
	} Application_settings::add_setting("npr-render-edge-detection-width", AS_DOUBLE);

	if (settings.value("npr-render").toBool()) 	rendering_effect = RENDERING_PENCIL;

	Application_settings::add_int_setting("background-color-red", 255);
	Application_settings::add_int_setting("background-color-green", 255);
	Application_settings::add_int_setting("background-color-blue", 255);

	
}

void GL_widget_2::switch_render_effect() {
	if (rendering_effect == RENDERING_NORMAL)
		rendering_effect = RENDERING_PENCIL;
	else rendering_effect = RENDERING_NORMAL;
	resizeGL(width(),height());
	glBindTexture( GL_TEXTURE_2D, 0);
	repaintGL();
}

GL_widget_2::~GL_widget_2() {}


void GL_widget_2::update_bounding_box(const QRectF& r) {
	if (default_bounding_box || r.x() < bounding_xmin) bounding_xmin = r.x();
	if (default_bounding_box || r.x()+r.width() > bounding_xmax) bounding_xmax = r.x()+r.width();
	if (default_bounding_box || r.y() < bounding_ymin) bounding_ymin = r.y();
	if (default_bounding_box || r.y()+r.height() > bounding_ymax) bounding_ymax = r.y()+r.height();
	default_bounding_box = false;
}

void GL_widget_2::set_window_to_boundingbox() {
	if (default_bounding_box) return;
	double margin_percent=BOUNDING_BOX_MARGIN_PERCENT;
	double x_min = bounding_xmin;
	double x_max = bounding_xmax;
	double y_min = bounding_ymin;
	double y_max = bounding_ymax;
	double x_margin = (x_max - x_min) * margin_percent;
	double y_margin = (y_max - y_min) * margin_percent;
	x_min -= x_margin;	x_max += x_margin;
	y_min -= y_margin;	y_max += y_margin;
	set_window(x_min, x_max, y_min, y_max);
}

void GL_widget_2::export_to_image(QString image_filepath) {

	bool old_dlists = get_use_display_lists();
	set_use_display_lists(false);
	repaint();
	QImage image = grabFrameBuffer();
	set_use_display_lists(old_dlists);
	repaint();

	image.save(image_filepath,"PNG");
	std::cout << "Image written to " << image_filepath.toStdString() << std::endl;
}

void GL_widget_2::export_to_vector(QString file_name) {
	QPrinter printer;
	printer.setOutputFileName(file_name);
	printer.setResolution(72);
	printer.setPageSize(QPrinter::A0);
	//	printer.setOrientation(QPrinter::Landscape); //ps would be not correct
	printer.setFullPage(true);
	printer.setColorMode(QPrinter::Color);


	QPainter painter;
	set_renderer_type(GL_widget_2::QT_PAINTER);
	set_painter(&painter);
	painter.begin(&printer);
	paint_to_painter();
	painter.end();
	set_painter(0);
	set_renderer_type(GL_widget_2::OPEN_GL);

	if (printer.printerState() < 2) // idle or active
		std::cout << "Image written to " << file_name.toStdString() << std::endl;
	else
		std::cout << "Could not write image to " << file_name.toStdString() << std::endl;
}

void GL_widget_2::print_to_printer() {
	QPrinter printer;

	QPrintDialog print_dialog(&printer, this);
    if (print_dialog.exec() == QDialog::Accepted) {
		QPainter painter;
		set_renderer_type(GL_widget_2::QT_PAINTER);
		set_painter(&painter);
		painter.begin(&printer);
		paint_to_painter();
		painter.end();
		set_painter(0);
		set_renderer_type(GL_widget_2::OPEN_GL);
	}
}



GL_widget_2::Renderer_type GL_widget_2::get_renderer_type() { return renderer_type; }
void GL_widget_2::set_renderer_type(Renderer_type r) { renderer_type = r; }

QPainter* GL_widget_2::get_painter() { return painter; }
void GL_widget_2::set_painter(QPainter* p) { painter = p; }

void GL_widget_2::attach(Visualization_layer *l) {
	layers.push_back(l);
	l->attach(this);
//	l->activate();
}

void GL_widget_2::detach(Visualization_layer *l) {
	std::vector<Visualization_layer*>::iterator to_delete = std::find(layers.begin(),layers.end(),l);
	if (to_delete!=layers.end()) 
		layers.erase(to_delete);
}

void GL_widget_2::repaintGL() {
	set_scales();
	updateGL();
	emit repaitedGL();
}

void GL_widget_2::paintGL () {
	bool is_valid;
	std::vector<Visualization_layer *>::iterator it, end;
	//int infologLength = 0;
	//int charsWritten  = 0;
	QSettings settings;
	
	switch (rendering_effect) {
		case RENDERING_PENCIL:
			// render into pbuffer
			pbuffer->makeCurrent();
			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			default_bounding_box = true;
			end=layers.end();
			for (it=layers.begin(); it!=end; it++) {
				Visualization_layer *l = *it;
				if (l->is_active()) {
					l->draw();
					QRectF bbox = l->get_bounding_box(&is_valid);
					if (is_valid) 
						update_bounding_box(bbox);
				}
			}
			glFlush();

			//pbuffer->toImage().save("D:/pbuffer.png");
			// on the screen now
			makeCurrent();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//draw a quad with the texture on it
				//glGenTextures(1, &noise_texture);
				//GLubyte *noise_data = make_2d_noise_texture(128,4,4);
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, noise_data);
				//glUniform1i(noise_uniform,6);



			//
			//glActiveTexture(GL_TEXTURE0+6);
			//std::cout << "noise texture: " << noise_texture << std::endl;
			//glBindTexture(GL_TEXTURE_2D, noise_texture);

			//

			//glGetProgramiv(npr_shader_program, GL_INFO_LOG_LENGTH,&infologLength);

			//if (infologLength > 0)
			//{
			//	infoLog = (char *)malloc(infologLength);
			//	glGetProgramInfoLog(npr_shader_program, infologLength, &charsWritten, infoLog);
			//	std::cout << infoLog << std::endl;
			//	free(infoLog);
			//}

			if (settings.value("npr-render-paper").toBool()) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,paper_texture);
				glBegin(GL_QUADS);
				glTexCoord2d(0,0);
				glVertex2d(0, 0);
				glTexCoord2d(10,0);
				glVertex2d(2048, 0);
				glTexCoord2d(10,10);
				glVertex2d(2048, 2048);
				glTexCoord2d(0,10);
				glVertex2d(0, 2048);
				glEnd();
			}
			glActiveTexture(GL_TEXTURE0+6);
			glBindTexture(GL_TEXTURE_2D, noise_texture);
//			glActiveTexture(GL_TEXTURE0);
//			glBindTexture(GL_TEXTURE_2D, paper_texture);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, dynamic_texture);

			// activate shader program
			glUseProgram(npr_shader_program);

			glBegin(GL_QUADS);
				glTexCoord2d(0,0);
				glVertex2d(0, 0);
				glTexCoord2d(1,0);
				glVertex2d(2048, 0);
				glTexCoord2d(1,1);
				glVertex2d(2048, 2048);
				glTexCoord2d(0,1);
				glVertex2d(0, 2048);
			glEnd();
			glUseProgram(0);
			glActiveTexture(GL_TEXTURE0);

			glFlush();
			break;
		default:
			// render directly into the widget
			makeCurrent();
			glBindTexture(GL_TEXTURE_2D, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			default_bounding_box = true;
			end=layers.end();


			//do computation for layers
			//if (! computation_thread->isRunning()) {
			//	computation_thread->start();
			//} else {
			//	std::cout << LOG_GREEN << "thread is running" << std::endl;

			//	//			std::cout << LOG_RED << "DONE Preparing layers" << std::endl;


			//	//			std::cout << LOG_BLUE << "START Drawing layers" << std::endl;

			//}

			//render them
			makeCurrent();
			for (it=layers.begin(); it!=end; it++) {
				Visualization_layer *l = *it;
				if (l->is_active()) {
					l->draw();
					QRectF bbox = l->get_bounding_box(&is_valid);
					if (is_valid) 
						update_bounding_box(bbox);
				}
			}
			glFlush();

//			std::cout << LOG_BLUE << "DONE Drawing layers" << std::endl;
			break;
	}


}

void GL_widget_2::paint_to_painter() {
	painter->setClipRegion(QRegion(0,0,width(), height()));
	std::vector<Visualization_layer *>::iterator it, end=layers.end();
	for (it=layers.begin(); it!=end; it++) {
		Visualization_layer *l = *it;
		if (l->is_active())
			l->draw();
	}
}

void GL_widget_2::set_point_pen(QPen pen) {
	point_pen = pen;
}
void GL_widget_2::set_line_pen(QPen pen) {
	line_pen = pen;
}


void GL_widget_2::resizeGL ( int w, int h ) {

	xmin = xmin_old;
	xmax = xmax_old;
	ymin = ymin_old;
	ymax = ymax_old;
	emit widget_resized_to(QRect(0,0,w,h));

	set_scales();

	double xratio,yratio,bxmin,bxmax,bymin,bymax, half_width,half_height;
	switch (rendering_effect) {
		case RENDERING_PENCIL:
			// free old pencil buffer if exists
			if (pbuffer) {
				pbuffer->releaseFromDynamicTexture();
				glDeleteTextures(1, &dynamic_texture);
				delete pbuffer;
			}
			// create new pencil buffer and init
			pbuffer = new QGLPixelBuffer(QSize(2048, 2048), format(), this);
			pbuffer->makeCurrent();
			init_common();
		
			// generate texture that has the same size/format as the pbuffer
			dynamic_texture = pbuffer->generateDynamicTexture();

			// bind the dynamic texture to the pbuffer - this is a no-op under X11
			pbuffer->bindToDynamicTexture(dynamic_texture);

			// set projection matrix such that the 2048x2048 covers the view screen
			xratio = 2048.0/w;
			bxmin = ((1-xratio)*xmax + (1+xratio)*xmin)/2;
			bxmax = ((1+xratio)*xmax + (1-xratio)*xmin)/2;
			yratio = 2048.0/h;
			bymin = ((1-yratio)*ymax + (1+yratio)*ymin)/2;
			bymax = ((1+yratio)*ymax + (1-yratio)*ymin)/2;
			glViewport(0,0,2048,2048);
			glMatrixMode(GL_PROJECTION);
		    glLoadIdentity();
			glOrtho(bxmin, bxmax, bymin, bymax, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// update the widget projection such that it is a window in the middle of 2048x2048
			makeCurrent();
			glViewport(0, 0, w, h);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			half_width=width()/2;
			half_height=height()/2;
		    glOrtho(1024-half_width, 1024+half_width, 1024-half_height, 1024+half_height, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			break;
		default:
			makeCurrent();
			glViewport(0, 0, w, h);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(xmin, xmax, ymin, ymax, -1, 1);
			break;
	}
}

char *read_text_from_file(const QString &name) {
	QFile file(name);
	if (!file.open(QIODevice::ReadOnly)) {
		std::cout << "File " << name.toStdString() << " could not be open" << std::endl;
		return 0;
	}
	int count = file.bytesAvailable ();
	char* data = (char *)malloc(sizeof(char) * (count+1));
	file.read(data, count);
	data[count] = '\0';
	file.close();
	return data;
}

void GL_widget_2::initializeGL () {
	makeCurrent();
	init_common();

	set_scales();
	//init shaders
	glewInit();
	if (glewIsSupported("GL_VERSION_2_0")) {
		std::cout << "OpenGL 2.0 is supported" << std::endl;
		GLint texSize; 
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
		std::cout << "Maximum texture size: " << texSize << std::endl;

		char *vs,*fs;

		GLuint f,v;

		v = glCreateShader(GL_VERTEX_SHADER);
		f = glCreateShader(GL_FRAGMENT_SHADER);

		vs = read_text_from_file(":/pencil.vert");
		fs = read_text_from_file(":/pencil.frag");

		const char * vv = vs;
		const char * ff = fs;

		glShaderSource(v, 1, &vv,NULL);
		glShaderSource(f, 1, &ff,NULL);

		free(vs);free(fs);

		glCompileShader(v);
		int success;
		glGetShaderiv(v, GL_COMPILE_STATUS, &success);
		if (success!=GL_TRUE) std::cout << LOG_ERROR << "Vertex shader NOT compiled" << std::endl;
		glCompileShader(f);
		glGetShaderiv(f, GL_COMPILE_STATUS, &success);
		if (success!=GL_TRUE) std::cout << LOG_ERROR << "Fragment shader NOT compiled" << std::endl;

		npr_shader_program = glCreateProgram();

		glAttachShader(npr_shader_program,v);
		glAttachShader(npr_shader_program,f);

		glLinkProgram(npr_shader_program);
		glGetProgramiv(npr_shader_program, GL_LINK_STATUS, &success);
		if (success!=GL_TRUE) std::cout << LOG_ERROR << "Shader program NOT compiled" << std::endl;

		glUseProgram(npr_shader_program);
		GLint noise_uniform = glGetUniformLocation(npr_shader_program, "hatchmap");


		//				glGenTextures(1, &noise_texture);
		glActiveTexture(GL_TEXTURE0+6);

		noise_texture = bindTexture(QPixmap(":/perlin.png"), GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, noise_texture);

		noise_uniform = glGetUniformLocation(npr_shader_program, "hatchmap");
		glUniform1i(noise_uniform,6);

		glActiveTexture(GL_TEXTURE0);
		paper_texture = bindTexture(QPixmap(":/paper.png"), GL_TEXTURE_2D);

		QSettings settings;
		GLint hatchscale_location = glGetUniformLocation(npr_shader_program, "hatchScale");
		glUniform1f(hatchscale_location, settings.value("npr-render-distortion").toDouble());

		GLint edge_location = glGetUniformLocation(npr_shader_program, "edgeSize");
		glUniform1f(edge_location, settings.value("npr-render-edge-detection-width").toDouble());

		//glBindTexture(GL_TEXTURE_2D, paper_texture);
		//glBindTexture( GL_TEXTURE_2D, 0);

		glUseProgram(0);
	} else {
		std::cout << LOG_ERROR << "OpenGL 2.0 is not supported" << std::endl;
	}

	double xratio,yratio,bxmin,bxmax,bymin,bymax, half_width,half_height;
	switch (rendering_effect) {
		case RENDERING_PENCIL:
			// free old pencil buffer if exists
			if (pbuffer) {
				pbuffer->releaseFromDynamicTexture();
				glDeleteTextures(1, &dynamic_texture);
				delete pbuffer;
			}
			// create new pencil buffer and init
			pbuffer = new QGLPixelBuffer(QSize(2048, 2048), format(), this);
			pbuffer->makeCurrent();
			init_common();
		
			// generate texture that has the same size/format as the pbuffer
			dynamic_texture = pbuffer->generateDynamicTexture();

			// bind the dynamic texture to the pbuffer - this is a no-op under X11
			pbuffer->bindToDynamicTexture(dynamic_texture);

			// set projection matrix such that the 2048x2048 covers the view screen
			xratio = 2048.0/width();
			bxmin = ((1-xratio)*xmax + (1+xratio)*xmin)/2;
			bxmax = ((1+xratio)*xmax + (1-xratio)*xmin)/2;
			yratio = 2048.0/height();
			bymin = ((1-yratio)*ymax + (1+yratio)*ymin)/2;
			bymax = ((1+yratio)*ymax + (1-yratio)*ymin)/2;
			glViewport(0,0,2048,2048);
			glMatrixMode(GL_PROJECTION);
		    glLoadIdentity();
			glOrtho(bxmin, bxmax, bymin, bymax, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// update the widget projection such that it is a window in the middle of 2048x2048
			makeCurrent();
			glViewport(0, 0, width(), height());
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			half_width=width()/2;
			half_height=height()/2;
		    glOrtho(1024-half_width, 1024+half_width, 1024-half_height, 1024+half_height, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			break;
		default:

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, width(), 0, height(), -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			break;
	}


}

void GL_widget_2::init_common() {
		glClearColor(Application_settings::get_int_setting("background-color-red")/255.0,
					 Application_settings::get_int_setting("background-color-green")/255.0,
					 Application_settings::get_int_setting("background-color-blue")/255.0,
					 1.0f);
    glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);		// Antialias the lines
	glEnable(GL_BLEND);										// Enable Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		// Type Of Blending To Use
//	glEnable(GL_MULTISAMPLE);
}

void GL_widget_2::set_window(double x_min, double x_max,
							 double y_min, double y_max) {
	xmin_old = xmin = x_min;
	xmax_old = xmax = x_max;
	ymin_old = ymin = y_min;
	ymax_old = ymax = y_max;
	set_scales();


	double xratio,yratio,bxmin,bxmax,bymin,bymax;
	switch (rendering_effect) {
		case RENDERING_PENCIL:
			pbuffer->makeCurrent();
			xratio = 2048.0/width();
			bxmin = ((1-xratio)*xmax + (1+xratio)*xmin)/2;
			bxmax = ((1+xratio)*xmax + (1-xratio)*xmin)/2;
			yratio = 2048.0/height();
			bymin = ((1-yratio)*ymax + (1+yratio)*ymin)/2;
			bymax = ((1+yratio)*ymax + (1-yratio)*ymin)/2;
			glViewport(0,0,2048,2048);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(bxmin, bxmax, bymin, bymax, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			break;
		default:
			makeCurrent();
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(xmin, xmax, ymin, ymax, -1, 1);
			break;
	}


	repaintGL();
//	updateGL();
}

void GL_widget_2::set_scales() {
	xscal = yscal =  (width()/(xmax - xmin) < height()/(ymax - ymin) ) ? width() / (xmax - xmin) : height() / (ymax - ymin);
	double xcenter = xmin + (xmax - xmin) / 2;
	double ycenter = ymin + (ymax - ymin) / 2;

	//if(xscal<1) {
	//	// if xscal < 1, width()/xscal > width(). then we can round it 
	//	// with loosing precision.
	//	xmin = xcenter - (int)(width()/xscal)/2;
	//	xmax = xcenter + (int)(width()/xscal)/2;
	//	ymin = ycenter - (int)(height()/yscal)/2;
	//	ymax = ycenter + (int)(height()/yscal)/2;
	//} else {
		xmin = xcenter - (width()/xscal)/2;
		xmax = xcenter + (width()/xscal)/2;
		ymin = ycenter - (height()/yscal)/2;
		ymax = ycenter + (height()/yscal)/2;
	//}
}

void GL_widget_2::move_center(const double distx, const double disty) {
	xmin += distx; xmin_old += distx;
	xmax += distx; xmax_old += distx;
	ymin += disty; ymin_old += disty;
	ymax += disty; ymax_old += disty;

	double xratio,yratio,bxmin,bxmax,bymin,bymax;
	switch (rendering_effect) {
		case RENDERING_PENCIL:
			pbuffer->makeCurrent();
			xratio = 2048.0/width();
			bxmin = ((1-xratio)*xmax + (1+xratio)*xmin)/2;
			bxmax = ((1+xratio)*xmax + (1-xratio)*xmin)/2;
			yratio = 2048.0/height();
			bymin = ((1-yratio)*ymax + (1+yratio)*ymin)/2;
			bymax = ((1+yratio)*ymax + (1-yratio)*ymin)/2;
			glViewport(0,0,2048,2048);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(bxmin, bxmax, bymin, bymax, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			break;
		default:
			makeCurrent();
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(xmin, xmax, ymin, ymax, -1, 1);
			break;
	}


	repaintGL();
//	updateGL();
}


void GL_widget_2::set_center(const double x, const double y) {
	if(xscal<1) {
		xmin = x - (int)(width()/xscal)/2;
		xmax = x + (int)(width()/xscal)/2;
		ymin = y - (int)(height()/yscal)/2;
		ymax = y + (int)(height()/yscal)/2;
	} else {
		xmin = x - (width()/xscal)/2;
		xmax = x + (width()/xscal)/2;
		ymin = y - (height()/yscal)/2;
		ymax = y + (height()/yscal)/2;
	}

	// this is so that we can not zoom out higher than the maximum rendering area (for infinite lines and rays)
	bool need_set_scales = false; double new_min, new_max;
	if (xmax > GL_MAX_DRAWING_COORDINATE) { 
		xmax = GL_MAX_DRAWING_COORDINATE; 
		new_min=(ymin+ymax)/2-(ymax-ymin)/100;
		new_max=(ymin+ymax)/2+(ymax-ymin)/100;
		ymin=new_min;
		ymax=new_max;
		need_set_scales = true;
	}
	if (xmin < -GL_MAX_DRAWING_COORDINATE) { 
		xmin = -GL_MAX_DRAWING_COORDINATE;
		new_min=(ymin+ymax)/2-(ymax-ymin)/100;
		new_max=(ymin+ymax)/2+(ymax-ymin)/100;
		ymin=new_min;
		ymax=new_max;
		need_set_scales = true;
	}
	if (ymax > GL_MAX_DRAWING_COORDINATE) { 
		ymax = GL_MAX_DRAWING_COORDINATE;
		new_min=(xmin+xmax)/2-(xmax-xmin)/100;
		new_max=(xmin+xmax)/2+(xmax-xmin)/100;
		xmin=new_min;
		xmax=new_max;
		need_set_scales = true;
	}
	if (ymin < -GL_MAX_DRAWING_COORDINATE) { 
		ymin = -GL_MAX_DRAWING_COORDINATE;
		new_min=(xmin+xmax)/2-(xmax-xmin)/100;
		new_max=(xmin+xmax)/2+(xmax-xmin)/100;
		xmin=new_min;
		xmax=new_max;
		need_set_scales = true;
	}
	if (need_set_scales) set_scales();

	xmin_old = xmin;
	xmax_old = xmax;
	ymin_old = ymin;
	ymax_old = ymax;

	double xratio,yratio,bxmin,bxmax,bymin,bymax;
	switch (rendering_effect) {
		case RENDERING_PENCIL:
			pbuffer->makeCurrent();
			xratio = 2048.0/width();
			bxmin = ((1-xratio)*xmax + (1+xratio)*xmin)/2;
			bxmax = ((1+xratio)*xmax + (1-xratio)*xmin)/2;
			yratio = 2048.0/height();
			bymin = ((1-yratio)*ymax + (1+yratio)*ymin)/2;
			bymax = ((1+yratio)*ymax + (1-yratio)*ymin)/2;
			glViewport(0,0,2048,2048);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(bxmin, bxmax, bymin, bymax, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			break;
		default:
			makeCurrent();
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(xmin, xmax, ymin, ymax, -1, 1);
			break;
	}

	repaintGL();
}

void GL_widget_2::zoom(double ratio, double xc, double yc) {  
	xscal = xscal*ratio; yscal = yscal*ratio;
	set_center(xc, yc);
}

void GL_widget_2::zoom(double ratio) {
	zoom(ratio,
		xmin + (xmax - xmin) / 2 ,
		ymin + (ymax - ymin) / 2 );
}

double GL_widget_2::x_real(int x) const {
	if(xscal<1)
		return(xmin+(int)(x/xscal));
	else
		return (xmin+x/xscal);
}

double GL_widget_2::y_real(int y) const {
	if(yscal<1)
		return(ymax-(int)(y/yscal));
	else
		return (ymax-y/yscal);
}

double GL_widget_2::x_pixel_d(double x) {
	return (x-xmin)*xscal;
}
double GL_widget_2::y_pixel_d(double y) {
  return -1*(y-ymax)*yscal;
}

double GL_widget_2::x_pixel_dist_d(double d) {
    return d*xscal;
}

double GL_widget_2::y_pixel_dist_d(double d) {
    return d*yscal;
}



void GL_widget_2::selection_copied_by(double x, double y)
{
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
			(*it)->selection_copied_by(x,y);
	emit points_modified();
}

void GL_widget_2::selection_moved_by(double x, double y)
{
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
			(*it)->selection_moved_by(x,y);
	emit points_modified();
}

bool GL_widget_2::selection_deleted()
{
	bool something_deleted = false;
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
			something_deleted = something_deleted || (*it)->selection_deleted();
	if (hasFocus() && something_deleted) {
		emit points_modified();
		return true;
	}
	return false;
}

void GL_widget_2::clear_selection()
{
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
			(*it)->clear_selection();
}


void GL_widget_2::ray_selection(double x, double y, bool* hit) {
	*hit = false; 
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active()) {
			bool layer_hit = false;
			(*it)->ray_selection(x, y, &layer_hit);
			*hit = *hit || layer_hit;
		}
}

void GL_widget_2::disk_selection(double x, double y, double radius, bool* hit) {
	*hit = false; 
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active()) {
			bool layer_hit = false;
			(*it)->disk_selection(x, y, radius, &layer_hit);
			*hit = *hit || layer_hit;
		}
}

void GL_widget_2::box_selection(double x, double y, double width, double height, bool* hit) {
	*hit = false; 
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active()) {
			bool layer_hit = false;
			(*it)->box_selection(x, y, width, height, &layer_hit);
			*hit = *hit || layer_hit;
		}
}

void  GL_widget_2::request_repaint() {
	want_repaint = true;
}

void GL_widget_2::application_settings_changed(const QString& settings_name) {
	std::vector<Visualization_layer*>::iterator it;

	for(it = layers.begin(); it!= layers.end(); it++)
		(*it)->application_settings_changed(settings_name);

	if (want_repaint) {
		repaintGL();
		want_repaint = false;
	}

	if (settings_name=="npr-render") { switch_render_effect(); repaint(); }
	if (settings_name=="npr-render-paper") repaint();
	if (settings_name=="npr-render-distortion") {
		QSettings settings;
		glUseProgram(npr_shader_program);
		GLint hatchscale_location = glGetUniformLocation(npr_shader_program, "hatchScale");
		glUniform1f(hatchscale_location, settings.value("npr-render-distortion").toDouble());
		glUseProgram(0);
		repaint();
	}
	if (settings_name=="npr-render-edge-detection-width") {
		QSettings settings;
		glUseProgram(npr_shader_program);
		GLint edge_location = glGetUniformLocation(npr_shader_program, "edgeSize");
		glUniform1f(edge_location, settings.value("npr-render-edge-detection-width").toDouble());
		glUseProgram(0);
		repaint();
	}

	if (settings_name.startsWith("background-color")) {
		glClearColor(Application_settings::get_int_setting("background-color-red")/255.0,
					 Application_settings::get_int_setting("background-color-green")/255.0,
					 Application_settings::get_int_setting("background-color-blue")/255.0,
					 1.0f);
		repaintGL();
	}

	if (settings_name == "random-colormap-resolution") {
		Color_map_factory::add_color_map(new Random_color_map(Application_settings::get_int_setting("random-colormap-resolution"), "Random"));
		repaint();
	}


}


void GL_widget_2::add_point_to_selection(double x, double y) {
	emit add_point_to_selection(QPointF(x,y));
}
void GL_widget_2::add_line_to_selection(double x1, double y1, double x2, double y2) {
	emit add_line_to_selection(QLineF(x1,y1,x2,y2));
}
void GL_widget_2::add_circle_to_selection(double x, double y, double r) {
	emit add_circle_to_selection(QPointF(x,y), r);
}

void GL_widget_2::remove_point_from_selection(double x, double y) {
	emit remove_point_from_selection(QPointF(x,y));
}
void GL_widget_2::remove_line_from_selection(double x1, double y1, double x2, double y2) {
	emit remove_line_from_selection(QLineF(x1,y1,x2,y2));
}
void GL_widget_2::remove_circle_from_selection(double x, double y, double r) {
	emit remove_circle_from_selection(QPointF(x,y), r);
}

void GL_widget_2::smooth_zoom(double x_min, double x_max, double y_min, double y_max) {
	double o_x_min = xmin;
	double o_x_max = xmax;
	double o_y_min = ymin;
	double o_y_max = ymax;
	if (o_x_min == x_min && o_x_max == x_max && o_y_min == y_min && o_y_max == y_max) return;

	QSettings settings;
	int min_squared_distance = settings.value("navigation-smooth-zoom-steps").toInt();
//	std::cout << "navigation-smooth-zoom-steps: " << min_squared_distance << std::endl;

	// these parameters should be moved outside, ini file or user interface...
	double uniform_ratio = 1.00;
	double total_steps = min_squared_distance;//SMOOTH_ZOOM_STEPS;
	
	double uniform_steps = (int) total_steps * uniform_ratio;
	double accel_steps = (int) total_steps - uniform_steps;

	double c_x_min = o_x_min; double x_min_step = uniform_ratio * (x_min - o_x_min)/uniform_steps;
	double c_x_max = o_x_max; double x_max_step = uniform_ratio * (x_max - o_x_max)/uniform_steps;
	double c_y_min = o_y_min; double y_min_step = uniform_ratio * (y_min - o_y_min)/uniform_steps;
	double c_y_max = o_y_max; double y_max_step = uniform_ratio * (y_max - o_y_max)/uniform_steps;
	for (int i=0; i<uniform_steps; i++) {
		c_x_min += x_min_step;
		c_x_max += x_max_step;
		c_y_min += y_min_step;
		c_y_max += y_max_step;
		set_window(c_x_min, c_x_max, c_y_min, c_y_max);
//		msg("ren");
	}

	if (accel_steps==0) return;
	double acc_x_min = ((1-uniform_ratio) * (x_min - o_x_min) - x_min_step * accel_steps) / accel_steps / accel_steps;
	double acc_x_max = ((1-uniform_ratio) * (x_max - o_x_max) - x_max_step * accel_steps) / accel_steps / accel_steps;
	double acc_y_min = ((1-uniform_ratio) * (y_min - o_y_min) - y_min_step * accel_steps) / accel_steps / accel_steps;
	double acc_y_max = ((1-uniform_ratio) * (y_max - o_y_max) - y_max_step * accel_steps) / accel_steps / accel_steps;
	double a_x_min = c_x_min;
	double a_x_max = c_x_max;
	double a_y_min = c_y_min;
	double a_y_max = c_y_max;
	for (int i=1; i<accel_steps+1; i++) {
		a_x_min = c_x_min + x_min_step*i + acc_x_min * i*i;
		a_x_max = c_x_max + x_max_step*i + acc_x_max * i*i;
		a_y_min = c_y_min + y_min_step*i + acc_y_min * i*i;
		a_y_max = c_y_max + y_max_step*i + acc_y_max * i*i;
		set_window(a_x_min, a_x_max, a_y_min, a_y_max);
	}
}

void GL_widget_2::mousePressEvent(QMouseEvent *e)
{
	emit(s_mousePressEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->mousePressEvent(e);
}

void GL_widget_2::mouseReleaseEvent(QMouseEvent *e)
{
	emit(s_mouseReleaseEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->mouseReleaseEvent(e);
}

void GL_widget_2::mouseMoveEvent(QMouseEvent *e)
{
	emit(s_mouseMoveEvent(e));
	emit mouse_at(QPointF(x_real(e->x()), y_real(e->y())));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->mouseMoveEvent(e);
}

void GL_widget_2::wheelEvent(QWheelEvent *e)
{
	emit(s_wheelEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->wheelEvent(e);
}

void GL_widget_2::mouseDoubleClickEvent(QMouseEvent *e)
{
	emit(s_mouseDoubleClickEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->mouseDoubleClickEvent(e);
}

void GL_widget_2::keyPressEvent(QKeyEvent *e)
{
	emit(s_keyPressEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->keyPressEvent(e);
}

void GL_widget_2::save_view(bool) {
	QString bounding_box = tr("%1 %2 %3 %4").arg(x_min()).arg(x_max()).arg(y_min()).arg(y_max());

	QSettings settings;
	QString file_name = QFileDialog::getSaveFileName(
		this,
		"Choose a filename to save view",
		settings.value("last-view-directory",QString()).toString(),
		"View files (*.2view)");
	if (file_name=="") return;
	if (!file_name.endsWith(".2view")) file_name += ".2view";
	QString save_directory = file_name;
	save_directory.truncate(file_name.lastIndexOf('/'));
	settings.setValue("last-view-directory",save_directory);

	QFile f(file_name);
	if ( !f.open( QIODevice::WriteOnly ) ) {
		std::cout << LOG_ERROR << tr("File `%1' could not be open for writing!").arg(file_name).toStdString() << std::endl;
		return;
	}
	QTextStream fs( &f );
	fs << bounding_box;
	f.close();

	std::cout << "Camera view stored in " << file_name.toStdString()  << std::endl;

}
void GL_widget_2::load_view(bool) {
	QSettings settings;

	QString file_name = QFileDialog::getOpenFileName(
		this,
		"Choose a filename to load view",
		settings.value("last-view-directory",QString()).toString(),
		"View files (*.2view)");
	if (file_name!="") {
		if (!file_name.endsWith(".2view")) file_name += ".2view";
		QString save_directory = file_name;
		save_directory.truncate(file_name.lastIndexOf('/'));
		settings.setValue("last-view-directory",save_directory);

		QFile file(file_name);
		if (!file.open(QIODevice::ReadOnly)) {
			std::cout << LOG_ERROR << tr("File `%1' could not be open for reading!").arg(file_name).toStdString() << std::endl;
			return;
		}

		QTextStream ts(&file);
		double xmin, xmax, ymin, ymax;
		ts >> xmin; ts >> xmax; ts >> ymin; ts >> ymax;
		file.close();

		smooth_zoom(xmin, xmax,ymin,ymax);
		std::cout << "Camera view reloaded from " << file_name.toStdString() << std::endl;
	}

}

void GL_widget_2::keyReleaseEvent(QKeyEvent *e)
{
	emit(s_keyReleaseEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->keyReleaseEvent(e);
	if (e->key()==Qt::Key_Space && e->modifiers() == Qt::ControlModifier) { 
		QSettings settings;
		settings.setValue("npr-render",!settings.value("npr-render").toBool());
		switch_render_effect(); 
		emit settings_changed();
	}
	if (e->key()==Qt::Key_F && e->modifiers() == Qt::ControlModifier) { switch_full_screen(); }

	//if (e->key() == Qt::Key_V && e->modifiers() == (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier)) {
	//	QString bounding_box = tr("%1 %2 %3 %4").arg(x_min()).arg(x_max()).arg(y_min()).arg(y_max());
	//	QSettings settings;
	//	settings.setValue("viewer-buffer-2d-view-state",bounding_box);
	//	std::cout << "Current view stored" << std::endl;
	//}
	//if (e->key() == Qt::Key_V && e->modifiers() == (Qt::ControlModifier | Qt::AltModifier)) {
	//	QSettings settings;
	//	QString doc_text = settings.value("viewer-buffer-2d-view-state").toString();
	//	QTextStream ts(&doc_text);
	//	double xmin, xmax, ymin, ymax;
	//	ts >> xmin; ts >> xmax; ts >> ymin; ts >> ymax;
	//	smooth_zoom(xmin, xmax,ymin,ymax);
	//	std::cout << "Camera view reloaded with "<< doc_text.toStdString() << std::endl;
	//}

	//if (e->key() == Qt::Key_P && e->modifiers() == (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier)) {
	//	QString bounding_box = tr("%1 %2 %3 %4").arg(x_min()).arg(x_max()).arg(y_min()).arg(y_max());
	//	
	//	QSettings settings;
	//	QString file_name = QFileDialog::getSaveFileName(
	//		this,
	//		"Choose a filename to save view",
	//		settings.value("last-data-directory",QString()).toString(),
	//		"View files (*.2view)");
	//	if (file_name=="") return;
	//	if (!file_name.endsWith(".2view")) file_name += ".2view";
	//	QString save_directory = file_name;
	//	save_directory.truncate(file_name.lastIndexOf('/'));
	//	settings.setValue("last-data-directory",save_directory);

	//	QFile f(file_name);
	//	if ( !f.open( QIODevice::WriteOnly ) ) {
	//		std::cout << LOG_ERROR << tr("File `%1' could not be open for writing!").arg(file_name).toStdString() << std::endl;
	//		return;
	//	}
	//	QTextStream fs( &f );
	//	fs << bounding_box;
	//	f.close();

	//	std::cout << "Camera view stored in " << file_name.toStdString()  << std::endl;
	//}
	//if (e->key() == Qt::Key_P && e->modifiers() == (Qt::ControlModifier | Qt::AltModifier)) {
	//	QSettings settings;

	//	QString file_name = QFileDialog::getOpenFileName(
	//		this,
	//		"Choose a filename to load view",
	//		settings.value("last-data-directory",QString()).toString(),
	//		"View files (*.2view)");
	//	if (file_name!="") {
	//		if (!file_name.endsWith(".2view")) file_name += ".2view";
	//		QString save_directory = file_name;
	//		save_directory.truncate(file_name.lastIndexOf('/'));
	//		settings.setValue("last-data-directory",save_directory);

	//		QFile file(file_name);
	//		if (!file.open(QIODevice::ReadOnly)) {
	//			std::cout << LOG_ERROR << tr("File `%1' could not be open for reading!").arg(file_name).toStdString() << std::endl;
	//			return;
	//		}

	//		QTextStream ts(&file);
	//		double xmin, xmax, ymin, ymax;
	//		ts >> xmin; ts >> xmax; ts >> ymin; ts >> ymax;
	//		file.close();

	//		smooth_zoom(xmin, xmax,ymin,ymax);
	//		std::cout << "Camera view reloaded from " << file_name.toStdString() << std::endl;
	//	}


	//}

}

void GL_widget_2::switch_full_screen() {
	std::cout << "should switch to full screen" << std::endl;
	
//	QWidget* tlw = topLevelWidget();
	//if (! isFullScreen())
	//{
	//	old_parent = parentWidget();
	//	setParent(0);
	//	setWindowFlags(windowFlags() | Qt::Window | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	//	std::cout << "Widget is window: " << isWindow() << std::endl;


	//	showFullScreen();
	//	move(0,0);
	//}
	//else
	//{
	//	setParent(old_parent);
	//	showNormal();
	//	//tlw->move(prev_pos);
	//}

	//full_screen = ! full_screen;
	//setWindowState(windowState() ^ Qt::WindowFullScreen);
	//if (full_scren) {
	//	w->setWindowState(w->windowState() & ~WindowMinimized | WindowActive);
	//} else {
	//}
}

void GL_widget_2::enterEvent(QEvent *e)
{
	emit(s_enterEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->enterEvent(e);
}

void GL_widget_2::leaveEvent(QEvent *e)
{
	emit(s_leaveEvent(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->leaveEvent(e);
}

bool GL_widget_2::event(QEvent *e)
{
	emit(s_event(e));
	std::vector<Visualization_layer*>::iterator it;
	for(it = layers.begin(); it!= layers.end(); it++)
		if((*it)->is_active())
			(*it)->event(e);
	return QWidget::event(e);
}

void GL_widget_2::set_default_cursor(QCursor c) { default_cursor = c; }
QCursor GL_widget_2::get_default_cursor() { return default_cursor; }



