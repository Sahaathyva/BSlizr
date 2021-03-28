/* B.Slizr
 * Step Sequencer Effect Plugin
 *
 * Copyright (C) 2018, 2019 by Sven Jähnichen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <exception>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include "BWidgets/Widget.hpp"
#include "BWidgets/Window.hpp"
#include "BWidgets/Label.hpp"
#include "BWidgets/DrawingSurface.hpp"
#include "BWidgets/HSwitch.hpp"
#include "BWidgets/VSlider.hpp"
#include "BWidgets/VSliderValue.hpp"
#include "BWidgets/HSliderValue.hpp"
#include "BWidgets/DialValue.hpp"

#include "main.h"
#include "screen.h"

#ifndef MESSAGENR_
#define MESSAGENR_
enum MessageNr
{
	NO_MSG		= 0,
	JACK_STOP_MSG	= 1,
	MAX_MSG		= 1
};
#endif /* MESSAGENR_ */

#define SCALEMIN -60
#define SCALEMAX 30

#ifndef UWU
#define CAIRO_BG_COLOR 0.0, 0.0, 0.0, 1.0
#define CAIRO_TRANSPARENT 0.0, 0.0, 0.0, 0.0
#else
#define CAIRO_BG_COLOR 1.0, 0.8, 0.8, 1.0
#define CAIRO_TRANSPARENT 1.0, 0.8, 0.8, 0.0
#endif

#define CAIRO_BG_COLOR2 0.2, 0.2, 0.2, 1.0
#define CAIRO_FG_COLOR 1.0, 1.0, 1.0, 1.0

#ifndef UWU
#define CAIRO_INK1 0.0, 1.0, 0.4
#define CAIRO_INK2 0.8, 0.6, 0.2
#else
#define CAIRO_INK1 1.0, 0.2, 0.4
#define CAIRO_INK2 1.0, 0.2, 0.4
#endif



#define BG_FILE "surface.png"

#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)
#define CO_DB(g) ((g) > 0.0001f ? logf((g)) / 0.05f : -90.0f)
#define LIM(g , max) ((g) > (max) ? (max) : (g))
#define INT(g) (int) (g + 0.5)
#define RESIZE(widget, x, y, w, h, sz) (widget).moveTo ((x) * (sz), (y) * (sz)); (widget).resize ((w) * (sz), (h) * (sz));

const std::string messageStrings[MAX_MSG + 1] =
{
	"",
	"*** Jack transport off or halted. ***"
};

class BSlizr_GUI : public BWidgets::Window
{
public:
	BSlizr_GUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeView parentWindow);
	~BSlizr_GUI ();
	void portEvent (uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer);
	void send_record_on ();
	void send_record_off ();
	virtual void onConfigureRequest (BEvents::ExposeEvent* event) override;
	void applyTheme (BStyles::Theme& theme) override;

	LV2UI_Controller controller;
	LV2UI_Write_Function write_function;


private:
	void resizeGUI ();
	void rearrange_step_controllers (float nrSteps_newf);
	static void valueChangedCallback (BEvents::Event* event);
	bool init_Stepshape ();
	void destroy_Stepshape ();
	void redrawStepshape ();
	bool init_mainMonitor ();
	void destroy_mainMonitor ();
	void add_monitor_data (BSlizrNotifications* notifications, uint32_t notificationsCount, uint32_t& end);
	void redrawMainMonitor ();


	BWidgets::Widget mContainer;
	BWidgets::Widget sContainer;
	BWidgets::HSwitch monitorSwitch;
	BWidgets::DrawingSurface monitorDisplay;
	BWidgets::Label monitorLabel;
	BWidgets::VSlider scaleControl;
	BWidgets::DrawingSurface stepshapeDisplay;
	BWidgets::DialValue attackControl;
	BWidgets::Label attackLabel;
	BWidgets::DialValue releaseControl;
	BWidgets::Label releaseLabel;
	BWidgets::HSliderValue sequencesperbarControl;
	BWidgets::Label sequencesperbarLabel;
	BWidgets::HSliderValue nrStepsControl;
	BWidgets::Label nrStepsLabel;
	BWidgets::Label stepshapeLabel;
	BWidgets::Label sequencemonitorLabel;
	BWidgets::Label messageLabel;
	std::array<BWidgets::VSliderValue, MAXSTEPS> stepControl;

	cairo_surface_t* surface;
	cairo_t* cr1;
	cairo_t* cr2;
	cairo_t* cr3;
	cairo_t* cr4;
	cairo_pattern_t* pat1;
	cairo_pattern_t* pat2;
	cairo_pattern_t* pat3;
	cairo_pattern_t* pat4;
	cairo_pattern_t* pat5;

	struct
	{
		bool record_on;
		uint32_t width;
		uint32_t height;
		std::array<BSlizrNotifications, MONITORBUFFERSIZE> data;
		uint32_t horizonPos;
	}  mainMonitor;

	std::string pluginPath;
	double sz;
	cairo_surface_t* bgImageSurface;

	float scale;
	float attack;
	float release;
	float nrSteps;
	float sequencesperbar;
	std::array<float, MAXSTEPS> step;

	LV2_Atom_Forge forge;
	BSlizrURIs uris;
	LV2_URID_Map* map;



	// Definition of styles
#ifndef UWU
	BColors::ColorSet fgColors = {{{0.0, 0.75, 0.2, 1.0}, {0.2, 1.0, 0.6, 1.0}, {0.0, 0.2, 0.0, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BColors::ColorSet txColors = {{{0.0, 1.0, 0.4, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.5, 0.0, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BColors::ColorSet bgColors = {{{0.15, 0.15, 0.15, 1.0}, {0.3, 0.3, 0.3, 1.0}, {0.05, 0.05, 0.05, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BColors::Color ink = {0.0, 0.75, 0.2, 1.0};

	BStyles::Border border = {{ink, 1.0}, 0.0, 2.0, 0.0};
	BStyles::Fill widgetBg = BStyles::noFill;
	BStyles::Fill screenBg = BStyles::Fill (BColors::Color (0.0, 0.0, 0.0, 0.75));
	BStyles::Border screenBorder = BStyles::Border (BStyles::Line (BColors::Color (0.0, 0.0, 0.0, 0.75), 4.0));
#else
	BColors::ColorSet fgColors = {{{1.0, 0.2, 0.4, 1.0}, {1.0, 0.4, 0.8, 1.0}, {0.5, 0.0, 0.2, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BColors::ColorSet txColors = {{{1.0, 0.1, 0.2, 1.0}, {1.0, 0.2, 0.4, 1.0}, {0.5, 0.0, 0.2, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BColors::ColorSet bgColors = {{{0.5, 0.5, 0.5, 1.0}, {0.8, 0.8, 0.8, 1.0}, {0.1, 0.1, 0.1, 1.0}, {0.0, 0.0, 0.0, 0.0}}};
	BColors::Color ink = {0.0, 0.75, 0.2, 1.0};

	BStyles::Border border = {{ink, 1.0}, 0.0, 2.0, 0.0};
	BStyles::Fill widgetBg = BStyles::noFill;
	BStyles::Fill screenBg = BStyles::Fill (BColors::Color (1.0, 0.8, 0.8, 0.75));
	BStyles::Border screenBorder = BStyles::Border (BStyles::Line (BColors::Color (0.0, 0.0, 0.0, 0.75), 4.0));
#endif
	BStyles::Font defaultFont = BStyles::Font ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 12.0,
						   BStyles::TEXT_ALIGN_CENTER, BStyles::TEXT_VALIGN_MIDDLE);
	BStyles::StyleSet defaultStyles = {"default", {{"background", STYLEPTR (&BStyles::noFill)},
						       {"border", STYLEPTR (&BStyles::noBorder)}}};
	BStyles::StyleSet labelStyles = {"labels", {{"background", STYLEPTR (&BStyles::noFill)},
						    {"border", STYLEPTR (&BStyles::noBorder)},
						    {"textcolors", STYLEPTR (&txColors)},
						    {"font", STYLEPTR (&defaultFont)}}};

	BStyles::Theme theme = BStyles::Theme ({
		defaultStyles,
		{"B.Slizr", 	{{"background", STYLEPTR (&BStyles::blackFill)},
				 {"border", STYLEPTR (&BStyles::noBorder)}}},
		{"main", 	{{"background", STYLEPTR (&widgetBg)},
				 {"border", STYLEPTR (&BStyles::noBorder)}}},
		{"widget", 	{{"uses", STYLEPTR (&defaultStyles)}}},
		{"monitor", 	{{"background", STYLEPTR (&BStyles::blackFill)},
				 {"border", STYLEPTR (&border)}}},
		{"dial", 	{{"uses", STYLEPTR (&defaultStyles)},
				 {"fgcolors", STYLEPTR (&fgColors)},
				 {"bgcolors", STYLEPTR (&bgColors)},
				 {"textcolors", STYLEPTR (&fgColors)},
				 {"font", STYLEPTR (&defaultFont)}}},
		{"dial/focus", 	{{"background", STYLEPTR (&screenBg)},
				 {"border", STYLEPTR (&screenBorder)},
				 {"textcolors", STYLEPTR (&txColors)},
				 {"font", STYLEPTR (&defaultFont)}}},
		{"slider",	{{"uses", STYLEPTR (&defaultStyles)},
				 {"fgcolors", STYLEPTR (&fgColors)},
				 {"bgcolors", STYLEPTR (&bgColors)},
				 {"textcolors", STYLEPTR (&fgColors)},
				 {"font", STYLEPTR (&defaultFont)}}},
		{"slider/focus",{{"background", STYLEPTR (&screenBg)},
				 {"border", STYLEPTR (&screenBorder)},
				 {"textcolors", STYLEPTR (&txColors)},
				 {"font", STYLEPTR (&defaultFont)}}},
		{"switch",	{{"uses", STYLEPTR (&defaultStyles)},
				 {"fgcolors", STYLEPTR (&fgColors)},
				 {"bgcolors", STYLEPTR (&bgColors)}}},
		{"switch/focus",{{"background", STYLEPTR (&screenBg)},
				 {"border", STYLEPTR (&screenBorder)},
				 {"textcolors", STYLEPTR (&txColors)},
				 {"font", STYLEPTR (&defaultFont)}}},
		{"label",	{{"uses", STYLEPTR (&labelStyles)}}},
		{"hilabel",	{{"uses", STYLEPTR (&labelStyles)},
				 {"textcolors", STYLEPTR (&BColors::whites)}}},
	});
};


BSlizr_GUI::BSlizr_GUI (const char *bundle_path, const LV2_Feature *const *features, PuglNativeView parentWindow) :
	Window (800, 560, "B.Slizr", parentWindow, true, PUGL_MODULE, 0),
	controller (NULL), write_function (NULL),

#ifndef UWU
	mContainer (0, 0, 800, 560, "main"),
	sContainer (260, 330, 480, 130, "widget"),
	monitorSwitch (690, 25, 40, 16, "switch", 0.0),
	monitorDisplay (260, 70, 480, 240, "monitor"),
	monitorLabel (680, 45, 60, 20, "label", "Monitor"),
	scaleControl (760, 80, 14, 230, "slider", 0.0, SCALEMIN, SCALEMAX, 0.1),
	stepshapeDisplay (30, 320, 180, 140, "monitor"),
	attackControl (40, 465, 50, 60, "dial", 0.2, 0.01, 1.0, 0.01, "%1.2f"),
	attackLabel (20, 520, 90, 20, "label", "Attack"),
	releaseControl (150, 465, 50, 60, "dial", 0.2, 0.01, 1.0, -0.01, "%1.2f"),
	releaseLabel (130, 520, 90, 20, "label", "Release"),
	sequencesperbarControl (260, 492, 120, 28, "slider", 1.0, 1.0, 8.0, 1.0, "%1.0f"),
	sequencesperbarLabel (250, 520, 140, 20, "label", "Sequences per bar"),
	nrStepsControl (400, 492, 380, 28, "slider", 1.0, 1.0, MAXSTEPS, 1.0, "%2.0f"),
	nrStepsLabel (400, 520, 380, 20, "label", "Number of steps"),
	stepshapeLabel (33, 323, 80, 20, "label", "Step shape"),
	sequencemonitorLabel (263, 73, 120, 20, "label", "Sequence monitor"),
	messageLabel (420, 73, 280, 20, "hilabel", ""),
#else
	mContainer (0, 0, 800, 560, "main"),
	sContainer (260, 330, 480, 130, "widget"),
	monitorSwitch (690, 25, 40, 16, "switch", 0.0),
	monitorDisplay (260, 70, 480, 240, "monitor"),
	monitorLabel (680, 45, 60, 20, "label", "Monitow"),
	scaleControl (760, 80, 14, 230, "slider", 0.0, SCALEMIN, SCALEMAX, 0.1),
	stepshapeDisplay (30, 320, 180, 140, "monitor"),
	attackControl (40, 465, 50, 60, "dial", 0.2, 0.01, 1.0, 0.01, "%1.2f"),
	attackLabel (20, 520, 90, 20, "label", "Attack"),
	releaseControl (150, 465, 50, 60, "dial", 0.2, 0.01, 1.0, -0.01, "%1.2f"),
	releaseLabel (130, 520, 90, 20, "label", "Wewease"),
	sequencesperbarControl (260, 492, 120, 28, "slider", 1.0, 1.0, 8.0, 1.0, "%1.0f"),
	sequencesperbarLabel (250, 520, 140, 20, "label", "Sequences pew baw"),
	nrStepsControl (400, 492, 380, 28, "slider", 1.0, 1.0, MAXSTEPS, 1.0, "%2.0f"),
	nrStepsLabel (400, 520, 380, 20, "label", "Numbew of steps"),
	stepshapeLabel (33, 323, 80, 20, "label", "Step shape"),
	sequencemonitorLabel (263, 73, 120, 20, "label", "Sequence monitow"),
	messageLabel (420, 73, 280, 20, "hilabel", ""),
#endif

	surface (NULL), cr1 (NULL), cr2 (NULL), cr3 (NULL), cr4 (NULL), pat1 (NULL), pat2 (NULL), pat3 (NULL), pat4 (NULL), pat5 (NULL),
	pluginPath (bundle_path ? std::string (bundle_path) : std::string ("")),  sz (1.0), bgImageSurface (nullptr),
	scale (DB_CO(0.0)), attack (0.2), release (0.2), nrSteps (16.0), sequencesperbar (4.0), step (),
	map (NULL)


{
	if (!init_mainMonitor () || !init_Stepshape ())
	{
		std::cerr << "BSlizr.lv2#GUI: Failed to init monitor." <<  std::endl;
		destroy_mainMonitor ();
		destroy_Stepshape ();
		throw std::bad_alloc ();
	}

	//Initialialize and configure stepControllers
	for (int i = 0; i < MAXSTEPS; ++i)
	{
		stepControl[i] = BWidgets::VSliderValue ((i + 0.5) * 480 / MAXSTEPS - 10, 0, 28, 130, "slider", 1.0, 0.0, 1.0, 0.01, "%1.2f");
		stepControl[i].setHardChangeable (false);
		stepControl[i].setScrollable (true);
		stepControl[i].rename ("slider");
		stepControl[i].setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, BSlizr_GUI::valueChangedCallback);
		stepControl[i].applyTheme (theme, "slider");
		stepControl[i].getDisplayLabel ()->setState (BColors::ACTIVE);
		sContainer.add (stepControl[i]);
	}

	// Set callbacks
	monitorSwitch.setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, BSlizr_GUI::valueChangedCallback);
	scaleControl.setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, BSlizr_GUI::valueChangedCallback);
	attackControl.setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, BSlizr_GUI::valueChangedCallback);
	releaseControl.setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, BSlizr_GUI::valueChangedCallback);
	sequencesperbarControl.setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, BSlizr_GUI::valueChangedCallback);
	nrStepsControl.setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, BSlizr_GUI::valueChangedCallback);

	// Configure widgets
	bgImageSurface = cairo_image_surface_create_from_png ((pluginPath + BG_FILE).c_str());
	widgetBg.loadFillFromCairoSurface (bgImageSurface);
	scaleControl.setScrollable (true);
	attackControl.setScrollable (true);
	attackControl.setHardChangeable (false);
	releaseControl.setScrollable (true);
	releaseControl.setHardChangeable (false);
	sequencesperbarControl.setScrollable (true);
	nrStepsControl.setScrollable (true);
	applyTheme (theme);

	// Pack widgets
	mContainer.add (monitorSwitch);
	mContainer.add (monitorDisplay);
	mContainer.add (monitorLabel);
	mContainer.add (scaleControl);
	mContainer.add (stepshapeDisplay);
	mContainer.add (attackControl);
	mContainer.add (attackLabel);
	mContainer.add (releaseControl);
	mContainer.add (releaseLabel);
	mContainer.add (sequencesperbarControl);
	mContainer.add (sequencesperbarLabel);
	mContainer.add (nrStepsControl);
	mContainer.add (nrStepsLabel);
	mContainer.add (stepshapeLabel);
	mContainer.add (sequencemonitorLabel);
	mContainer.add (messageLabel);
	mContainer.add (sContainer);
	add (mContainer);

	//Scan host features for URID map
	LV2_URID_Map* m = NULL;
	for (int i = 0; features[i]; ++i)
	{
		if (strcmp(features[i]->URI, LV2_URID__map) == 0) m = (LV2_URID_Map*) features[i]->data;
	}
	if (!m) throw std::invalid_argument ("Host does not support urid:map");

	//Map URIS
	map = m;
	getURIs (map, &uris);

	// Initialize forge
	lv2_atom_forge_init (&forge,map);
}

BSlizr_GUI::~BSlizr_GUI()
{
	send_record_off ();
	destroy_mainMonitor ();
	destroy_Stepshape ();
}

void BSlizr_GUI::portEvent(uint32_t port_index, uint32_t buffer_size, uint32_t format, const void* buffer)
{
	// Notify port
	if ((format == uris.atom_eventTransfer) && (port_index == Notify))
	{
		const LV2_Atom* atom = (const LV2_Atom*) buffer;
		if ((atom->type == uris.atom_Blank) || (atom->type == uris.atom_Object))
		{
			const LV2_Atom_Object* obj = (const LV2_Atom_Object*) atom;

			// Monitor notification
			if (obj->body.otype == uris.notify_event)
			{
				const LV2_Atom* data = NULL;
				lv2_atom_object_get(obj, uris.notify_key, &data, 0);
				if (data && (data->type == uris.atom_Vector))
				{
					const LV2_Atom_Vector* vec = (const LV2_Atom_Vector*) data;
					if (vec->body.child_type == uris.atom_Float)
					{
						uint32_t notificationsCount = (uint32_t) ((data->size - sizeof(LV2_Atom_Vector_Body)) / sizeof (BSlizrNotifications));
						BSlizrNotifications* notifications = (BSlizrNotifications*) (&vec->body + 1);
						if (notificationsCount > 0)
						{
							add_monitor_data (notifications, notificationsCount, mainMonitor.horizonPos);
							redrawMainMonitor ();
						}
					}
				}
				else std::cerr << "BSlizr.lv2#GUI: Corrupt audio message." << std::endl;
			}

			// Message notification
			else if (obj->body.otype == uris.notify_messageEvent)
			{
				const LV2_Atom* data = NULL;
				lv2_atom_object_get(obj, uris.notify_message, &data, 0);
				if (data && (data->type == uris.atom_Int))
				{
					const int messageNr = ((LV2_Atom_Int*)data)->body;
					std::string msg = ((messageNr >= NO_MSG) && (messageNr <= MAX_MSG) ? messageStrings[messageNr] : "");
					messageLabel.setText (msg);
				}
			}
		}
	}

	// Scan remaining ports
	else if ((format == 0) && (port_index >= Attack) && (port_index < Step_ + MAXSTEPS))
	{
	float* pval = (float*) buffer;
	switch (port_index) {
		case Attack:
			attack = *pval;
			attackControl.setValue (*pval);
			break;
		case Release:
			release = *pval;
			releaseControl.setValue (*pval);
			break;
		case SequencesPerBar:
			sequencesperbar = *pval;
			sequencesperbarControl.setValue (*pval);
			break;
		case NrSteps:
			if (nrSteps != *pval)
			{
				rearrange_step_controllers (*pval);
				nrSteps = *pval;
			}
			redrawMainMonitor ();
			nrStepsControl.setValue (*pval);
			break;
		default:
			if ((port_index >= Step_) and (port_index < Step_ + MAXSTEPS))
			{
				step[port_index-Step_] = *pval;
				stepControl[port_index-Step_].setValue (*pval);
			}
		}
	}

}

void BSlizr_GUI::resizeGUI()
{
	hide ();

	// Resize Fonts
	defaultFont.setFontSize (12 * sz);

	// Resize Background
	cairo_surface_t* surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 800 * sz, 560 * sz);
	cairo_t* cr = cairo_create (surface);
	cairo_scale (cr, sz, sz);
	cairo_set_source_surface(cr, bgImageSurface, 0, 0);
	cairo_paint(cr);
	widgetBg.loadFillFromCairoSurface(surface);
	cairo_destroy (cr);
	cairo_surface_destroy (surface);

	// Resize widgets
	RESIZE (mContainer, 0, 0, 800, 560, sz);
	RESIZE (monitorSwitch, 690, 25, 40, 16, sz);
	RESIZE (monitorDisplay, 260, 70, 480, 240, sz);
	RESIZE (monitorLabel, 680, 45, 60, 20, sz);
	RESIZE (scaleControl, 760, 80, 14, 230, sz);
	RESIZE (stepshapeDisplay, 30, 320, 180, 140, sz);
	RESIZE (attackControl, 40, 465, 50, 60, sz);
	RESIZE (attackLabel, 20, 520, 90, 20, sz);
	RESIZE (releaseControl, 150, 465, 50, 60, sz);
	RESIZE (releaseLabel, 130, 520, 90, 20, sz);
	RESIZE (sequencesperbarControl, 260, 492, 120, 28, sz);
	RESIZE (sequencesperbarLabel, 250, 520, 140, 20, sz);
	RESIZE (nrStepsControl, 400, 492, 380, 28, sz);
	RESIZE (nrStepsLabel, 400, 520, 380, 20, sz);
	RESIZE (stepshapeLabel, 33, 323, 80, 20, sz);
	RESIZE (sequencemonitorLabel, 263, 73, 120, 20, sz);
	RESIZE (messageLabel, 420, 73, 280, 20,sz);
	RESIZE (sContainer, 260, 330, 480, 130, sz);
	for (int i = 0; i < MAXSTEPS; ++i) {RESIZE (stepControl[i], (i + 0.5) * 480 / nrSteps - 10, 0, 28, 130, sz);}

	// Update monitors
	destroy_Stepshape ();
	init_Stepshape ();
	redrawStepshape ();
	destroy_mainMonitor ();
	init_mainMonitor ();
	redrawMainMonitor ();

	// Apply changes
	applyTheme (theme);
	show ();
}

void BSlizr_GUI::applyTheme (BStyles::Theme& theme)
{
	mContainer.applyTheme (theme);
	monitorSwitch.applyTheme (theme);
	monitorDisplay.applyTheme (theme);
	monitorLabel.applyTheme (theme);
	scaleControl.applyTheme (theme);
	stepshapeDisplay.applyTheme (theme);
	attackControl.applyTheme (theme);
	attackLabel.applyTheme (theme);
	releaseControl.applyTheme (theme);
	releaseLabel.applyTheme (theme);
	sequencesperbarControl.applyTheme (theme);
	sequencesperbarLabel.applyTheme (theme);
	nrStepsControl.applyTheme (theme);
	nrStepsLabel.applyTheme (theme);
	stepshapeLabel.applyTheme (theme);
	sequencemonitorLabel.applyTheme (theme);
	messageLabel.applyTheme (theme);
	sContainer.applyTheme (theme);
	for (int i = 0; i < MAXSTEPS; ++i)
	{
		stepControl[i].applyTheme (theme);
		stepControl[i].update ();	// TODO Remove if fixed in BWidgets TK
	}
}

void BSlizr_GUI::onConfigureRequest (BEvents::ExposeEvent* event)
{
	Window::onConfigureRequest (event);

	sz = (getWidth() / 800 > getHeight() / 560 ? getHeight() / 560 : getWidth() / 800);
	resizeGUI ();
}

void BSlizr_GUI::send_record_on ()
{
	uint8_t obj_buf[64];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, uris.ui_on);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, Control_2, lv2_atom_total_size(msg), uris.atom_eventTransfer, msg);
	monitorSwitch.setValue (1.0);
}

void BSlizr_GUI::send_record_off ()
{
	uint8_t obj_buf[64];
	lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf));

	LV2_Atom_Forge_Frame frame;
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, uris.ui_off);
	lv2_atom_forge_pop(&forge, &frame);
	write_function(controller, Control_2, lv2_atom_total_size(msg), uris.atom_eventTransfer, msg);
	monitorSwitch.setValue (0.0);
}

void BSlizr_GUI::rearrange_step_controllers (float nrSteps_newf)
{
	int nrSteps_old = INT (nrSteps);
	int nrSteps_new = INT (nrSteps_newf);

	if ((nrSteps_old < 1) || (nrSteps_old > MAXSTEPS) || (nrSteps_new < 1) || (nrSteps_old > MAXSTEPS)) return;

	for (int i = 0; i < MAXSTEPS; ++i)
	{
		if (i < nrSteps_new)
		{
			stepControl[i].moveTo (((i + 0.5) * 480 / nrSteps_new - 10) * sz, 0);
			stepControl[i].show ();
		}
		else stepControl[i].hide ();
	}
}

void BSlizr_GUI::valueChangedCallback (BEvents::Event* event)
{
	if ((event) && (event->getWidget ()))
	{
		BWidgets::ValueWidget* widget = (BWidgets::ValueWidget*) event->getWidget ();

		if (widget->getMainWindow ())
		{
			BSlizr_GUI* ui = (BSlizr_GUI*) widget->getMainWindow ();

			// monitor on/off changed
			if (widget == &ui->monitorSwitch)
			{
				int value = INT (widget->getValue ());
				if (value == 1)
				{
					ui->mainMonitor.record_on = true;
					ui->send_record_on ();
				}
				else
				{
					ui->mainMonitor.record_on = false;
					ui->send_record_off ();
				}
				return;
			}

			// Scale changed
			if (widget == &ui->scaleControl)
			{
				float value = (float) widget->getValue ();
				ui->scale = DB_CO (value);
				if (ui->scale < 0.0001f) ui->scale = 0.0001f;
				ui->redrawMainMonitor ();
			}

			// Attack changed
			if (widget == &ui->attackControl)
			{
				ui->attack = (float) widget->getValue ();
				ui->write_function(ui->controller, Attack, sizeof(ui->attack), 0, &ui->attack);
				ui->redrawStepshape ();
				return;
			}

			// Release changed
			if (widget == &ui->releaseControl)
			{
				ui->release = (float) widget->getValue ();
				ui->write_function(ui->controller, Release, sizeof(ui->release), 0, &ui->release);
				ui->redrawStepshape ();
				return;
			}

			// Step size changed
			if (widget == &ui->sequencesperbarControl)
			{
				ui->sequencesperbar = (float) widget->getValue ();
				ui->write_function(ui->controller, SequencesPerBar, sizeof(ui->sequencesperbar), 0, &ui->sequencesperbar);
				return;
			}

			// nrSteps changed
			if (widget == &ui->nrStepsControl)
			{
				float nrSteps_new = (float) widget->getValue ();
				if (nrSteps_new != ui->nrSteps) ui->rearrange_step_controllers (nrSteps_new);
				ui->nrSteps = nrSteps_new;
				ui->write_function(ui->controller, NrSteps, sizeof(ui->nrSteps), 0, &ui->nrSteps);
				ui->redrawMainMonitor ();
				return;
			}

			// Step controllers changed
			for (int i = 0; i < ui->nrSteps; i++)
			{
				if (widget == &ui->stepControl[i])
				{
					ui->step[i] = (float) widget->getValue ();
					ui->write_function(ui->controller, Step_+i , sizeof(ui->step[i]), 0, &ui->step[i]);
					return;
				}
			}
		}
	}
}

bool BSlizr_GUI::init_Stepshape ()
{
	double height = stepshapeDisplay.getEffectiveHeight ();
	pat5 = cairo_pattern_create_linear (0, 0, 0, height);

	return (pat5 && (cairo_pattern_status (pat5) == CAIRO_STATUS_SUCCESS));
}

void BSlizr_GUI::destroy_Stepshape ()
{
	//Destroy also mainMonitors cairo data
	if (pat5 && (cairo_pattern_status (pat5) == CAIRO_STATUS_SUCCESS)) cairo_pattern_destroy (pat5);
}

void BSlizr_GUI::redrawStepshape ()
{
	double width = stepshapeDisplay.getEffectiveWidth ();
	double height = stepshapeDisplay.getEffectiveHeight ();

	cairo_t* cr = cairo_create (stepshapeDisplay.getDrawingSurface ());
	if (cairo_status (cr) != CAIRO_STATUS_SUCCESS) return;

	// Draw background
	cairo_set_source_rgba (cr, CAIRO_BG_COLOR);
	cairo_rectangle (cr, 0.0, 0.0, width, height);
	cairo_fill (cr);
	cairo_set_source_rgba (cr, CAIRO_BG_COLOR2);
	cairo_set_line_width (cr, 1);
	cairo_move_to (cr, 0, 0.2 * height);
	cairo_line_to (cr, width, 0.2 * height);
	cairo_move_to (cr, 0, 0.55 * height);
	cairo_line_to (cr, width, 0.55 * height);
	cairo_move_to (cr, 0, 0.9 * height);
	cairo_line_to (cr, width, 0.9 * height);
	cairo_move_to (cr, 0.25 * width, 0);
	cairo_line_to (cr, 0.25 * width, height);
	cairo_move_to (cr, 0.5 * width, 0);
	cairo_line_to (cr, 0.5 * width, height);
	cairo_move_to (cr, 0.75 * width, 0);
	cairo_line_to (cr, 0.75 * width, height);
	cairo_stroke (cr);

	// Draw step shape
	cairo_set_source_rgba (cr, CAIRO_INK1, 1.0);
	cairo_set_line_width (cr, 3);

	cairo_move_to (cr, 0, 0.9 * height);
	cairo_line_to (cr, width * 0.25, 0.9 * height);
	if ((attack + release) > 1)
	{
		float crosspointX = attack / (attack + release);
		float crosspointY = crosspointX / attack - (crosspointX - (1 - release)) / release;
		cairo_line_to (cr, width* 0.25 + crosspointX * width * 0.5, 0.9 * height - 0.7 * height * crosspointY);
	}
	else
	{
		cairo_line_to (cr, width * 0.25 + attack * width * 0.5 , 0.2 * height);
		cairo_line_to (cr, width * 0.75  - release * width * 0.5, 0.2 * height);

	}
	cairo_line_to (cr, width * 0.75, 0.9 * height);
	cairo_line_to (cr, width, 0.9 * height);

	cairo_stroke_preserve (cr);

	cairo_pattern_add_color_stop_rgba (pat5, 0.1, CAIRO_INK1, 1);
	cairo_pattern_add_color_stop_rgba (pat5, 0.9, CAIRO_INK1, 0);
	cairo_set_source (cr, pat5);
	cairo_line_to(cr, 0, 0.9 * height);
	cairo_set_line_width (cr, 0);
	cairo_fill (cr);

	cairo_destroy (cr);

	stepshapeDisplay.update ();
}

bool BSlizr_GUI::init_mainMonitor ()
{
	//Initialize mainMonitor
	mainMonitor.record_on = true;
	mainMonitor.width = 0;
	mainMonitor.height = 0;
	mainMonitor.data.fill (defaultNotification);
	mainMonitor.horizonPos = 0;

	//Initialize mainMonitors cairo data
	double width = monitorDisplay.getEffectiveWidth ();
	double height = monitorDisplay.getEffectiveHeight ();
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
	cr1 = cairo_create (surface);
	cr2 = cairo_create (surface);
	cr3 = cairo_create (surface);
	cr4 = cairo_create (surface);
	pat1 = cairo_pattern_create_linear (0, 0, 0, height);
	cairo_pattern_add_color_stop_rgba (pat1, 0.1, CAIRO_INK1, 1);
	cairo_pattern_add_color_stop_rgba (pat1, 0.6, CAIRO_INK1, 0);
	pat2 = cairo_pattern_create_linear (0, 0, 0, height);
	cairo_pattern_add_color_stop_rgba (pat2, 0.1, CAIRO_INK2, 1);
	cairo_pattern_add_color_stop_rgba (pat2, 0.6, CAIRO_INK2, 0);
	pat3 = cairo_pattern_create_linear (0, height, 0, 0);
	cairo_pattern_add_color_stop_rgba (pat3, 0.1, CAIRO_INK1, 1);
	cairo_pattern_add_color_stop_rgba (pat3, 0.6, CAIRO_INK1, 0);
	pat4 = cairo_pattern_create_linear (0, height, 0, 0);
	cairo_pattern_add_color_stop_rgba (pat4, 0.1, CAIRO_INK2, 1);
	cairo_pattern_add_color_stop_rgba (pat4, 0.6, CAIRO_INK2, 0);

	return (pat4 && (cairo_pattern_status (pat4) == CAIRO_STATUS_SUCCESS) &&
			pat3 && (cairo_pattern_status (pat3) == CAIRO_STATUS_SUCCESS) &&
			pat2 && (cairo_pattern_status (pat2) == CAIRO_STATUS_SUCCESS) &&
			pat1 && (cairo_pattern_status (pat1) == CAIRO_STATUS_SUCCESS) &&
			cr4 && (cairo_status (cr4) == CAIRO_STATUS_SUCCESS) &&
			cr3 && (cairo_status (cr3) == CAIRO_STATUS_SUCCESS)&&
			cr2 && (cairo_status (cr2) == CAIRO_STATUS_SUCCESS) &&
			cr1 && (cairo_status (cr1) == CAIRO_STATUS_SUCCESS) &&
			surface && (cairo_surface_status (surface) == CAIRO_STATUS_SUCCESS));
}

void BSlizr_GUI::destroy_mainMonitor ()
{
	//Destroy also mainMonitors cairo data
	if (pat4 && (cairo_pattern_status (pat4) == CAIRO_STATUS_SUCCESS)) cairo_pattern_destroy (pat4);
	if (pat3 && (cairo_pattern_status (pat3) == CAIRO_STATUS_SUCCESS)) cairo_pattern_destroy (pat3);
	if (pat2 && (cairo_pattern_status (pat2) == CAIRO_STATUS_SUCCESS)) cairo_pattern_destroy (pat2);
	if (pat1 && (cairo_pattern_status (pat1) == CAIRO_STATUS_SUCCESS)) cairo_pattern_destroy (pat1);
	if (cr4 && (cairo_status (cr4) == CAIRO_STATUS_SUCCESS)) cairo_destroy (cr4);
	if (cr3 && (cairo_status (cr3) == CAIRO_STATUS_SUCCESS)) cairo_destroy (cr3);
	if (cr2 && (cairo_status (cr2) == CAIRO_STATUS_SUCCESS)) cairo_destroy (cr2);
	if (cr1 && (cairo_status (cr1) == CAIRO_STATUS_SUCCESS)) cairo_destroy (cr1);
	if (surface && (cairo_surface_status (surface) == CAIRO_STATUS_SUCCESS)) cairo_surface_destroy (surface);
}

void BSlizr_GUI::add_monitor_data (BSlizrNotifications* notifications, uint32_t notificationsCount, uint32_t& end)
{
	for (uint32_t i = 0; i < notificationsCount; ++i)
	{
		int monitorpos = notifications[i].position;
		if (monitorpos >= MONITORBUFFERSIZE) monitorpos = MONITORBUFFERSIZE;
		if (monitorpos < 0) monitorpos = 0;

		mainMonitor.data[monitorpos].inputMin = notifications[i].inputMin;
		mainMonitor.data[monitorpos].inputMax = notifications[i].inputMax;
		mainMonitor.data[monitorpos].outputMin = notifications[i].outputMin;
		mainMonitor.data[monitorpos].outputMax = notifications[i].outputMax;
		mainMonitor.horizonPos = monitorpos;
	}
}

void BSlizr_GUI::redrawMainMonitor ()
{
	double width = monitorDisplay.getEffectiveWidth ();
	double height = monitorDisplay.getEffectiveHeight ();

	cairo_t* cr = cairo_create (monitorDisplay.getDrawingSurface ());
	if (cairo_status (cr) != CAIRO_STATUS_SUCCESS) return;

	// Draw background
	cairo_set_source_rgba (cr, CAIRO_BG_COLOR);
	cairo_rectangle (cr, 0, 0, width, height);
	cairo_fill (cr);

	cairo_set_source_rgba (cr, CAIRO_BG_COLOR2);
	cairo_set_line_width (cr, 1);
	cairo_move_to (cr, 0, 0.1 * height);
	cairo_line_to (cr, width, 0.1 * height);
	cairo_move_to (cr, 0, 0.5 * height);
	cairo_line_to (cr, width, 0.5 * height);
	cairo_move_to (cr, 0, 0.9 * height);
	cairo_line_to (cr, width, 0.9 * height);

	uint32_t steps = (uint32_t) nrSteps;
	for (uint32_t i = 1; i < steps; ++i)
	{
		uint32_t x = uint32_t (i * width / steps);
		cairo_move_to (cr, x, 0);
		cairo_line_to (cr, x, height);
	}
	cairo_stroke (cr);

	if (mainMonitor.record_on)
	{
		cairo_surface_clear (surface);

		// Draw input (cr, cr3) and output (cr2, cr4) curves
		cairo_move_to (cr1, 0, height * (0.5  - (0.4 * LIM ((mainMonitor.data[0].inputMax / scale), 1.0f))));
		cairo_move_to (cr2, 0, height * (0.5  - (0.4 * LIM ((mainMonitor.data[0].outputMax / scale), 1.0f))));
		cairo_move_to (cr3, 0, height * (0.5  + (0.4 * LIM (-(mainMonitor.data[0].inputMin / scale), 1.0f))));
		cairo_move_to (cr4, 0, height * (0.5  + (0.4 * LIM (-(mainMonitor.data[0].outputMin / scale), 1.0f))));

		for (int i = 0; i < MONITORBUFFERSIZE; ++i)
		{
			double pos = ((double) i) / (MONITORBUFFERSIZE - 1.0f);
			cairo_line_to (cr1, pos * width, height * (0.5  - (0.4 * LIM ((mainMonitor.data[i].inputMax / scale), 1.0f))));
			cairo_line_to (cr2, pos * width, height * (0.5  - (0.4 * LIM ((mainMonitor.data[i].outputMax / scale), 1.0f))));
			cairo_line_to (cr3, pos * width, height * (0.5  + (0.4 * LIM (-(mainMonitor.data[i].inputMin / scale), 1.0f))));
			cairo_line_to (cr4, pos * width, height * (0.5  + (0.4 * LIM (-(mainMonitor.data[i].outputMin / scale), 1.0f))));
		}

		// Visualize input (cr, cr3) and output (cr2, cr4) curves
		cairo_set_source_rgba (cr1, CAIRO_INK1, 1.0);
		cairo_set_line_width (cr1, 3);
		cairo_set_source_rgba (cr2, CAIRO_INK2, 1.0);
		cairo_set_line_width (cr2, 3);
		cairo_stroke_preserve (cr1);
		cairo_stroke_preserve (cr2);
		cairo_set_source_rgba (cr3, CAIRO_INK1, 1.0);
		cairo_set_line_width (cr3, 3);
		cairo_set_source_rgba (cr4, CAIRO_INK2, 1.0);
		cairo_set_line_width (cr4, 3);
		cairo_stroke_preserve (cr3);
		cairo_stroke_preserve (cr4);

		// Visualize input (cr, cr3) and output (cr2, cr4) areas under the curves
		cairo_line_to (cr1, width, height * 0.5);
		cairo_line_to (cr1, 0, height * 0.5);
		cairo_close_path (cr1);
		cairo_line_to (cr2, width, height * 0.5);
		cairo_line_to (cr2, 0, height * 0.5);
		cairo_close_path (cr2);
		cairo_set_source (cr1, pat1);
		cairo_set_line_width (cr1, 0);
		cairo_set_source (cr2, pat2);
		cairo_set_line_width (cr2, 0);
		cairo_fill (cr1);
		cairo_fill (cr2);
		cairo_line_to (cr3, width, height * 0.5);
		cairo_line_to (cr3, 0, height * 0.5);
		cairo_close_path (cr3);
		cairo_line_to (cr4, width, height * 0.5);
		cairo_line_to (cr4, 0, height * 0.5);
		cairo_close_path (cr4);
		cairo_set_source (cr3, pat3);
		cairo_set_line_width (cr3, 0);
		cairo_set_source (cr4, pat4);
		cairo_set_line_width (cr4, 0);
		cairo_fill (cr3);
		cairo_fill (cr4);

		// Draw fade out
		double horizon = ((double) mainMonitor.horizonPos) / (MONITORBUFFERSIZE - 1.0f);
		cairo_pattern_t* pat6 = cairo_pattern_create_linear (horizon * width, 0, horizon * width + 63, 0);
		if (cairo_pattern_status (pat6) == CAIRO_STATUS_SUCCESS)
		{
			cairo_pattern_add_color_stop_rgba (pat6, 0.0, CAIRO_BG_COLOR);
			cairo_pattern_add_color_stop_rgba (pat6, 1.0, CAIRO_TRANSPARENT);
			cairo_set_line_width (cr1, 0.0);
			cairo_set_source (cr1, pat6);
			cairo_rectangle (cr1, horizon * width, 0, 63, height);
			cairo_fill (cr1);
			cairo_pattern_destroy (pat6);
		}

		if (horizon * width > width - 63)
		{
			cairo_pattern_t* pat6 = cairo_pattern_create_linear ((horizon - 1) * width, 0, (horizon - 1) * width + 63, 0);
			if (cairo_pattern_status (pat6) == CAIRO_STATUS_SUCCESS)
			{
				cairo_pattern_add_color_stop_rgba (pat6, 0.0, CAIRO_BG_COLOR);
				cairo_pattern_add_color_stop_rgba (pat6, 1.0, CAIRO_TRANSPARENT);
				cairo_set_line_width (cr1, 0.0);
				cairo_set_source (cr1, pat6);
				cairo_rectangle (cr1, (horizon - 1) * width, 0, 63, height);
				cairo_fill (cr1);
				cairo_pattern_destroy (pat6);
			}
		}

		// Draw horizon line
		cairo_set_source_rgba (cr1, CAIRO_FG_COLOR);
		cairo_set_line_width (cr1, 1);
		cairo_move_to (cr1, horizon * width, 0);
		cairo_line_to (cr1, horizon * width, height);
		cairo_stroke (cr1);
	}

	cairo_set_source_surface (cr, surface, 0, 0);
	cairo_paint (cr);

	cairo_destroy (cr);

	monitorDisplay.update ();
}

static LV2UI_Handle instantiate (const LV2UI_Descriptor *descriptor, const char *plugin_uri, const char *bundle_path,
						  LV2UI_Write_Function write_function, LV2UI_Controller controller, LV2UI_Widget *widget,
						  const LV2_Feature *const *features)
{
	PuglNativeView parentWindow = 0;
	LV2UI_Resize* resize = NULL;

	if (strcmp(plugin_uri, BSLIZR_URI) != 0)
	{
		std::cerr << "BSlizr.lv2#GUI: GUI does not support plugin with URI " << plugin_uri << std::endl;
		return NULL;
	}

	for (int i = 0; features[i]; ++i)
	{
		if (!strcmp(features[i]->URI, LV2_UI__parent)) parentWindow = (PuglNativeView) features[i]->data;
		else if (!strcmp(features[i]->URI, LV2_UI__resize)) resize = (LV2UI_Resize*)features[i]->data;
	}
	if (parentWindow == 0) std::cerr << "BSlizr.lv2#GUI: No parent window.\n";

	// New instance
	BSlizr_GUI* ui;
	try {ui = new BSlizr_GUI (bundle_path, features, parentWindow);}
	catch (std::exception& exc)
	{
		std::cerr << "BSlizr.lv2#GUI: Instantiation failed. " << exc.what () << std::endl;
		return NULL;
	}

	ui->controller = controller;
	ui->write_function = write_function;

	// Reduce min GUI size for small displays
	double sz = 1.0;
	int screenWidth  = getScreenWidth ();
	int screenHeight = getScreenHeight ();
	if ((screenWidth < 820) || (screenHeight < 600)) sz = 0.66;

	/*
	std::cerr << "BSlizr_GUI.lv2 screen size " << screenWidth << " x " << screenHeight <<
			". Set GUI size to " << 800 * sz << " x " << 560 * sz << ".\n";
	*/

	if (resize) resize->ui_resize(resize->handle, 800 * sz, 560 * sz);

	*widget = (LV2UI_Widget) puglGetNativeWindow (ui->getPuglView ());
	ui->send_record_on();
	return (LV2UI_Handle) ui;
}

static void cleanup(LV2UI_Handle ui)
{
	BSlizr_GUI* pluginGui = (BSlizr_GUI*) ui;
	if (pluginGui) delete pluginGui;
}

static void portEvent(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size,
	uint32_t format, const void* buffer)
{
	BSlizr_GUI* pluginGui = (BSlizr_GUI*) ui;
	if (pluginGui) pluginGui->portEvent(port_index, buffer_size, format, buffer);
}

static int callIdle (LV2UI_Handle ui)
{
	BSlizr_GUI* pluginGui = (BSlizr_GUI*) ui;
	if (pluginGui) pluginGui->handleEvents ();
	return 0;
}

static int callResize (LV2UI_Handle ui, int width, int height)
{
	BSlizr_GUI* self = (BSlizr_GUI*) ui;
	if (!self) return 0;

	BEvents::ExposeEvent* ev = new BEvents::ExposeEvent (self, self, BEvents::CONFIGURE_REQUEST_EVENT, self->getPosition().x, self->getPosition().y, width, height);
	self->addEventToQueue (ev);
	return 0;
}

static const LV2UI_Idle_Interface idle = {callIdle};
static const LV2UI_Resize resize = {nullptr, callResize};

static const void* extensionData(const char* uri)
{
	if (!strcmp(uri, LV2_UI__idleInterface)) return &idle;
	else if(!strcmp(uri, LV2_UI__resize)) return &resize;
	else return NULL;
}

static const LV2UI_Descriptor guiDescriptor = {
		BSLIZR_GUI_URI,
		instantiate,
		cleanup,
		portEvent,
		extensionData
};

// LV2 Symbol Export
LV2_SYMBOL_EXPORT const LV2UI_Descriptor *lv2ui_descriptor(uint32_t index)
{
	switch (index) {
	case 0: return &guiDescriptor;
	default:return NULL;
    }
}
