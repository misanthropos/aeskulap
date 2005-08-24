/*
    Aeskulap ImagePool - DICOM abstraction library
    Copyright (C) 2005  Alexander Pipelka

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    Alexander Pipelka
    pipelka@teleweb.at

    Last Update:      $Author: braindead $
    Update Date:      $Date: 2005/08/23 19:31:54 $
    Source File:      $Source: /cvsroot/aeskulap/aeskulap/imagepool/poolstudy.cpp,v $
    CVS/RCS Revision: $Revision: 1.1 $
    Status:           $State: Exp $
*/

#include "poolstudy.h"
#include "imagepool.h"

namespace ImagePool {

Study::~Study() {
	for(iterator i = begin(); i != end(); i++) {
		i->second.clear();
	}
	m_list.clear();
}

const std::string& Study::studyinstanceuid() {
	return m_studyinstanceuid;
}

const std::string& Study::patientsname() {
	return m_patientsname;
}

const std::string& Study::patientsbirthdate() {
	return m_patientsbirthdate;
}

const std::string& Study::studydescription() {
	return m_studydescription;
}

const std::string& Study::studydate() {
	return m_studydate;
}

const std::string& Study::studytime() {
	return m_studytime;
}

const std::string& Study::patientssex() {
	return m_patientssex;
}

}