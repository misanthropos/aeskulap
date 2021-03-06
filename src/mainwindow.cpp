/*
    Aeskulap - DICOM image viewer and network client
    Copyright (C) 2005  Alexander Pipelka

    This file is part of Aeskulap.

    Aeskulap is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Aeskulap is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Aeskulap; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Alexander Pipelka
*/

#include "mainwindow.h"
#include "aiconfactory.h"
#include "abusycursor.h"
#include "astudytab.h"
#include "aconfiguration.h"
#include "awindowleveltoolbutton.h"

#include "poolstudy.h"

#include "imagepool.h"
#include "studyview.h"
#include "studymanager.h"
#include "settings.h"
#include "prescandialog.h"
#include "aboutdialog.h"
#include "windowleveldialog.h"

#include "afloatwidget.h"

#include "assert.h"
#include "gettext.h"
#include <iostream>

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade) :
Gtk::Window(cobject),
m_refGlade(refGlade),
m_dialogFile(gettext("Open DICOM Image files")),
m_raise_opened(true),
m_netloader(Aeskulap::Configuration::get_instance().get_local_aet())
{
	/*Glib::RefPtr<Gdk::Pixbuf> icon = Aeskulap::IconFactory::load_from_file("aeskulap.png");

	if(icon) {
		set_icon(icon);
	}*/

	m_windowlevel = NULL;
	m_refGlade->get_widget_derived("windowlevel_add", m_windowlevel);

	m_about = NULL;
	m_refGlade->get_widget_derived("aboutdialog", m_about);

	m_prescandialog = NULL;
	m_refGlade->get_widget_derived("prescandialog", m_prescandialog);

	m_studymanager = NULL;
	m_refGlade->get_widget_derived("vbox_studymanager", m_studymanager);
	assert(m_studymanager != NULL);
	m_studymanager->signal_open_study.connect(sigc::mem_fun(*this, &MainWindow::on_study_open));

	m_settings = NULL;
	m_refGlade->get_widget_derived("settingswindow", m_settings);
	assert(m_settings != NULL);
	m_settings->signal_apply.connect(sigc::mem_fun(*this, &MainWindow::on_edit_settings_apply));

	m_mainNotebook = NULL;
	m_refGlade->get_widget("notebook_main", m_mainNotebook);
	m_mainNotebook->popup_enable();
	m_mainNotebook->signal_switch_page().connect(sigc::mem_fun(*this, &MainWindow::on_switch_page));

	Gtk::ImageMenuItem* item = NULL;

	m_refGlade->get_widget("file_open", item);
	item->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_file_open));

	m_refGlade->get_widget("dicomdir_open", item);
	item->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_dicomdir_open));

	m_refGlade->get_widget("file_exit", item);
	item->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_file_exit));

	m_itemViewFullscreen = NULL;
	m_refGlade->get_widget("view_fullscreen", m_itemViewFullscreen);
	m_itemViewFullscreen->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::on_view_fullscreen));

	m_refGlade->get_widget("edit_settings", item);
	item->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_edit_settings));

	{
		Gtk::ImageMenuItem* item = NULL;
		m_refGlade->get_widget("aeskulap_info", item);
		item->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_about));
	}

	m_dialogFile.set_select_multiple();

	m_dialog_check = manage(new Gtk::CheckButton(gettext("Bring opened files to front")));
	m_dialog_check->set_active(true);
	m_dialog_check->show();
	m_dialogFile.add_action_widget (*m_dialog_check, 1001);

	m_dialogFile.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	m_dialogFile.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	m_filter_dicom.set_name(gettext("DICOM files"));
	m_filter_dicom.add_pattern("*.dcm");

	m_filter_any.set_name(gettext("Any files"));
	m_filter_any.add_pattern("*");

	m_filter_dicomdir.set_name(gettext("DICOMDIR files"));
	m_filter_dicomdir.add_pattern("DICOMDIR");
	m_filter_dicomdir.add_pattern("dicomdir");

	m_netloader.signal_study_added.connect(sigc::mem_fun(*this, &MainWindow::on_study_added));
	m_netloader.signal_error.connect(sigc::mem_fun(*this, &MainWindow::on_study_error));

	m_fileloader.signal_study_added.connect(sigc::mem_fun(*this, &MainWindow::on_study_added));
	m_fileloader.signal_prescan_progress.connect(sigc::mem_fun(*m_prescandialog, &PrescanDialog::set_progress));

	m_dicomdirloader.signal_study_added.connect(sigc::mem_fun(*this, &MainWindow::on_study_added));

	//Aeskulap::FloatWidget* f = new Aeskulap::FloatWidget(*this, 100,100);
	//f->show();
}

MainWindow::~MainWindow() {
	delete m_settings;
	delete m_prescandialog;
}

void MainWindow::on_dicomdir_open() {
	bool bExit = false;

    m_dialogFile.set_title(gettext("Open DICOMDIR"));
	m_dialogFile.set_select_multiple(false);
    m_dialogFile.add_filter(m_filter_dicomdir);
    m_dialogFile.set_filter(m_filter_dicomdir);
    m_dialogFile.remove_filter(m_filter_dicom);
    m_dialogFile.remove_filter(m_filter_any);
	m_dialogFile.show();

	int rc = 0;

	while(!bExit) {
		rc = m_dialogFile.run();
		bExit = (rc == Gtk::RESPONSE_CANCEL) || (rc == Gtk::RESPONSE_OK);
	}

	m_raise_opened = m_dialog_check->get_active();

	m_dialogFile.hide();

	if(rc == Gtk::RESPONSE_CANCEL) {
		return;
	}
	open_dicomdir(m_dialogFile.get_filename());
}

void MainWindow::on_file_open() {
	bool bExit = false;

    m_dialogFile.set_title(gettext("Open DICOM Image files"));
	m_dialogFile.set_select_multiple(true);
    m_dialogFile.remove_filter(m_filter_dicomdir);
    m_dialogFile.add_filter(m_filter_dicom);
    m_dialogFile.add_filter(m_filter_any);
    m_dialogFile.set_filter(m_filter_dicom);
	m_dialogFile.show();

	int rc = 0;

	while(!bExit) {
		rc = m_dialogFile.run();
		bExit = (rc == Gtk::RESPONSE_CANCEL) || (rc == Gtk::RESPONSE_OK);
	}

	m_raise_opened = m_dialog_check->get_active();

	m_dialogFile.hide();

	if(rc == Gtk::RESPONSE_CANCEL) {
		return;
	}

	load_files(m_dialogFile.get_filenames());
}

void MainWindow::load_files(std::list< Glib::ustring > list) {
	m_prescandialog->show();

	while(Gtk::Main::events_pending()) Gtk::Main::iteration(false);

	if(!m_fileloader.load(list)) {
		//set_busy_cursor(false);
	}

	m_prescandialog->hide();
}

void MainWindow::open_dicomdir(const Glib::ustring &dicomdir) {

	while(Gtk::Main::events_pending()) Gtk::Main::iteration(false);
	m_studymanager->open_dicomdir(dicomdir);
}

void MainWindow::on_study_open(const Glib::RefPtr< ImagePool::Study >& study, const std::string& server) {
	m_raise_opened = true;

	Aeskulap::set_busy_cursor();

    if( server.substr(0, 9) == "DICOMDIR:" ) {
        if( !m_dicomdirloader.load(study->studyinstanceuid(), server.substr(9)) )
       		Aeskulap::set_busy_cursor(false);
    }
    else if(!m_netloader.load(study, server)) {
		Aeskulap::set_busy_cursor(false);
	}
}

void MainWindow::on_study_error() {
	std::cout << "MainWindow::on_study_error()" << std::endl;
	Aeskulap::set_busy_cursor(false);

	Gtk::MessageDialog error(
				*this,
				gettext("<span weight=\"bold\" size=\"larger\">Unable to receive the\n"
				"requested images</span>\n\n"
				"The request was sent to the server but no response has been received."),
				true,
				Gtk::MESSAGE_ERROR,
				Gtk::BUTTONS_OK,
				true);

	error.show();
	error.run();
	error.hide();
}

void MainWindow::on_file_exit() {
	Gtk::Main::quit();
}

void MainWindow::on_view_fullscreen() {
	if(m_itemViewFullscreen->property_active()) {
		fullscreen();
	}
	else {
		unfullscreen();
	}
}

void MainWindow::on_study_added(const Glib::RefPtr<ImagePool::Study>& study) {
	std::cout << "new study " << study->studyinstanceuid() << std::endl;

	StudyView* frame = manage(new StudyView(study));
	frame->signal_windowlevel_add.connect(sigc::mem_fun(*this, &MainWindow::on_windowlevel_add));

	m_studyview[study->studyinstanceuid()] = frame;

	Aeskulap::StudyTab* tab = manage(new Aeskulap::StudyTab(study, frame));
	tab->signal_close.connect(sigc::mem_fun(*this, &MainWindow::on_study_closed));
	study->signal_progress.connect(sigc::mem_fun(*tab, &Aeskulap::StudyTab::on_progress));

	m_mainNotebook->append_page(*frame, *tab);
	frame->show();

	if(m_raise_opened) {
		m_raise_opened = false;
		m_mainNotebook->set_current_page(-1);
		frame->accelerate(*this);
	}
	study->signal_series_added.connect(sigc::mem_fun(*frame, &StudyView::on_series_added));

	Aeskulap::set_busy_cursor(false);
	while(Gtk::Main::events_pending()) Gtk::Main::iteration(false);
}

void MainWindow::on_study_closed(StudyView* page) {
	std::cout << "MainWindow::on_study_closed" << std::endl;
	m_mainNotebook->remove(*page);
	m_studyview.erase(find_pageuid(page));
	delete page;
}

void MainWindow::on_edit_settings() {
	m_settings->show();
}

void MainWindow::on_edit_settings_apply() {
	m_studymanager->update_grouplist();
	Aeskulap::WindowLevelToolButton::update_all();
}

const std::string& MainWindow::find_pageuid(Gtk::Widget* page) {
	static std::string empty = "";

	std::map<std::string, StudyView*>::iterator i = m_studyview.begin();

	while(i != m_studyview.end()) {
		if(i->second == page) {
			return i->first;
		}
		i++;
	}

	return empty;
}

void MainWindow::on_about() {
	m_about->show();
	m_about->run();
	m_about->hide();
}

bool MainWindow::on_windowlevel_add(const Aeskulap::WindowLevel& level) {
	m_windowlevel->set(level);
	m_windowlevel->show();
	int result = m_windowlevel->run();
	m_windowlevel->hide();

	if(result != Gtk::RESPONSE_OK) {
		return false;
	}

	m_configuration.set_windowlevel(m_windowlevel->get());
	return true;
}

void MainWindow::on_switch_page(GtkNotebookPage* page, guint index) {
	std::cerr << "page: " << index << std::endl;
	if(index == 0) {
		return;
	}

	StudyView* study = static_cast<StudyView*>(m_mainNotebook->get_nth_page(index));
	study->accelerate(*this);
}
