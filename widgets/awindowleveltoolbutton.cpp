#include "awindowleveltoolbutton.h"
#include "aiconfactory.h"

namespace Aeskulap {

std::set<WindowLevelToolButton*> WindowLevelToolButton::m_widgetlist;

WindowLevelToolButton::WindowLevelToolButton() {
	Gtk::HBox* hbox = manage(new Gtk::HBox);

	Gtk::Image* image = manage(new Gtk::Image);
	Glib::RefPtr<Gdk::Pixbuf> p = Aeskulap::IconFactory::load_from_file("cursor_windowlevel.png");
	if(p) {
		image->set(p);
	}
	image->show();
	image->set_padding(6, 0);

	hbox->pack_start(*image, Gtk::PACK_SHRINK);

	Gtk::VBox* vbox = manage(new Gtk::VBox);
	vbox->show();

	m_combo = manage(new Gtk::ComboBoxText);
	m_combo->set_size_request(-1, 32);
	m_combo->show();
	m_combo->signal_changed().connect(sigc::mem_fun(*this, &WindowLevelToolButton::on_changed));

	vbox->pack_start(*m_combo, true, false);

	hbox->pack_start(*vbox, Gtk::PACK_SHRINK);

	vbox = manage(new Gtk::VBox);
	vbox->show();

	image = manage(new Gtk::Image(Gtk::Stock::ADD, Gtk::ICON_SIZE_SMALL_TOOLBAR));
	image->show();

	Gtk::ToolButton* btn = manage(new Gtk::ToolButton(*image));
	btn->set_size_request(32, 32);
	btn->set_tooltip(m_tooltips, gettext("Add new windowlevel preset"));
	btn->show();
	btn->signal_clicked().connect(sigc::mem_fun(*this, &WindowLevelToolButton::on_add));

	vbox->pack_start(*btn, true, false);
	hbox->pack_start(*vbox, true,false);
	
	hbox->show();

	add(*hbox);

	update();
	m_widgetlist.insert(this);
}

WindowLevelToolButton::~WindowLevelToolButton() {
	m_widgetlist.erase(this);
}

void WindowLevelToolButton::set_modality(Glib::ustring modality) {
	if(m_modality == modality) {
		return;
	}
	
	m_modality = modality;
	update();
}

const Glib::ustring& WindowLevelToolButton::get_modality() {
	return m_modality;
}

void WindowLevelToolButton::update() {
	m_configuration.get_windowlevel_list(m_modality, m_list);

	Glib::ustring a = m_combo->get_active_text();

	m_combo->clear();
	Aeskulap::WindowLevelList::iterator i;

	for(i = m_list.begin(); i != m_list.end(); i++) {
		m_combo->append_text(i->first);
	}

	m_combo->append_text(gettext("Default"));
	m_combo->append_text(gettext("Custom"));

	if(a.empty()) {
		m_combo->set_active_text(gettext("Default"));
	}
	else {
		m_combo->set_active_text(a);
	}
}

Aeskulap::WindowLevelList::iterator WindowLevelToolButton::find_windowlevel(const Glib::ustring& desc) {
	Aeskulap::WindowLevelList::iterator i;
	for(i = m_list.begin(); i != m_list.end(); i++) {
		if(i->first == desc) {
			break;
		}
	}
	
	return i;
}

void WindowLevelToolButton::set_windowlevel(const Aeskulap::WindowLevel& level) {
	if(m_last_level == level) {
		return;
	}

	Aeskulap::WindowLevelList::iterator i;
	for(i = m_list.begin(); i != m_list.end(); i++) {
		if(i->second == level) {
			break;
		}
	}

	if(i != m_list.end()) {
		m_combo->set_active_text(i->first);
	}
	else {
		m_combo->set_active_text(gettext("Custom"));
	}

	m_last_level = level;
}

void WindowLevelToolButton::on_changed() {
	if(m_combo->get_active_text() == Glib::ustring(gettext("Default"))) {
		signal_windowlevel_default();
		return;
	}

	Aeskulap::WindowLevelList::iterator i = find_windowlevel(m_combo->get_active_text());
	if(i == m_list.end()) {
		return;
	}

	m_last_level = i->second;
	signal_windowlevel_changed(i->second);
}

void WindowLevelToolButton::on_add() {
	signal_windowlevel_add(this);
}

void WindowLevelToolButton::update_all() {
	std::set<WindowLevelToolButton*>::iterator i;
	for(i = m_widgetlist.begin(); i != m_widgetlist.end(); i++) {
		(*i)->update();
	}
}

} // namespace Aeskulap
