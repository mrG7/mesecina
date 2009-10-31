/* This source file is part of Mesecina, a software for visualization and studying of
 * the medial axis and related computational geometry structures.
 * More info at http://www.agg.ethz.ch/~miklosb/mesecina
 * Copyright Balint Miklos, Applied Geometry Group, ETH Zurich
 *
 * $Id: constants.h 737 2009-05-16 15:40:46Z miklosb $
 */


#ifndef MESECINA_CONSTANTS_H
#define MESECINA_CONSTANTS_H

#define COMPANY_NAME "ETH-AGG-BalintMiklos"
#ifdef GINA_2
#define APPLICATION_NAME "Gina"
#else
#define APPLICATION_NAME "mesecina"
#endif
#define APPLICATION_VERSION "seven"
#define SVN_REVISION "$Rev: 737 $ - $Date: 2009-05-16 17:40:46 +0200 (Sat, 16 May 2009) $"
#define COUNT_RECENT_FILES 9
#define GL_MAX_DRAWING_COORDINATE 1700000000
#define BOUNDING_BOX_MARGIN_PERCENT 0.03
#define MIN_SQUARED_DISTANCE_SPRAY 100
#define SMOOTH_ZOOM_STEPS 100
#define IMAGE_FROM_SETTINGS true
#define DEFAULT_TOOLTIPS_FOR_LAYERS true

// shared stuctures

#define SHARED_POINTS "shared input points"
#define SHARED_INNER_VORONOI_BALLS "inner voronoi balls"
#define SHARED_INNER_VORONOI_BALLS_FLOATS "inner voronoi balls floats"
#define SHARED_INNER_POLES "inner poles"
#define SHARED_ONESTEP_CURVE "onestep curve reconstruction"
#define SHARED_INPUT_TRIANGULATION "input triangulation"
#define SHARED_LANDMARK_TRIANGULATION "landmark triangulation"
#define SHARED_GROWN_BALLS_FROM_MOEBIUS "grown moebius balls"

#define PROGRESS_HEADER "Progress"
#define PROGRESS_ON "ProgressOn"
#define PROGRESS_OFF "ProgressOff"
#define PROGRESS_MINIMUM "ProgressMinimum: "
#define PROGRESS_MAXIMUM "ProgressMaximum: "
#define PROGRESS_VALUE "ProgressValue: "
#define PROGRESS_STATUS "ProgressStatus: "
#define PROGRESS_DONE "ProgressDone"
#define LOG_ERROR "LogError: "
#define LOG_WARNING "LogWarning: "
#define LOG_GREEN "LogGreen: "
#define LOG_BLUE "LogBlue: "
#define LOG_RED "LogRed: "

#define MAX_CROSSING_V_VORONOI_SQUARED 0.0000002


#define LAYER_IN_SOUND "D:/Hero.wav"
#define LAYER_OUT_SOUND "D:/Blow.wav"
#define BUNDLE_IN_SOUND "D:/drip.wav"


#endif //MESECINA_CONSTANTS_H