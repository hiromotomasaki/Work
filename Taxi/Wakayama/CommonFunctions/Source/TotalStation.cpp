/**
 * TotalStation.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "TotalStation.hpp"

#include <iostream>

using namespace hiro;

TotalStation::TotalStation() {
	this -> length_ = 0;
	this -> azimuth_ = 0;
}
TotalStation::TotalStation( const double length_, const double azimuth_ ) {
	this -> length_ = length_;
	this -> azimuth_ = azimuth_;
}

TotalStation::~TotalStation() {

}

double TotalStation::getLength() const {
	return length_;
}
void TotalStation::setLength( const double length_ ) {
	this -> length_ = length_;
}

double TotalStation::getAzimuth() const {
	return azimuth_;
}
void TotalStation::setAzimuth( const double azimuth_ ) {
	this -> azimuth_ = azimuth_;
}

void TotalStation::set( const double length_, const double azimuth_ ) {
	this -> length_ = length_;
	this -> azimuth_ = azimuth_;
}
void TotalStation::print() const {
	std::cout << "(length of geodesics, azimuth ) = (" << this -> length_ << ", " << this -> azimuth_ << ")" << std::endl;
}

bool LessTotalStation::operator()(const TotalStation& tStation_a, const TotalStation& tStation_b) {
	if( tStation_a.getLength() == tStation_b.getLength() ) {
		return tStation_a.getAzimuth() < tStation_b.getAzimuth();
	}else{
		return tStation_a.getLength() < tStation_b.getLength();
	}
}
