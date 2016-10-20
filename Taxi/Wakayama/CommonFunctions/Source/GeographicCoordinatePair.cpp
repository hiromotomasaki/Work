/**
 * GeographicCoordinatePair.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "GeographicCoordinatePair.hpp"

#include <iostream>

using namespace hiro;

GeographicCoordinatePair::GeographicCoordinatePair() {
	GeographicCoordinate gCoorFirst_(0, 0);
	GeographicCoordinate gCoorSecond_(0, 0);
	this -> gCoorFirst_ = gCoorFirst_;
	this -> gCoorSecond_ = gCoorSecond_;
}

GeographicCoordinatePair::GeographicCoordinatePair( const GeographicCoordinate &gCoorFirst_, const GeographicCoordinate &gCoorSecond_ ) {
	this -> gCoorFirst_ = gCoorFirst_;
	this -> gCoorSecond_ = gCoorSecond_;
}

GeographicCoordinatePair::GeographicCoordinatePair(const double gCoorFirstPhi, const double gCoorFirstLambda, const double gCoorSecondPhi, const double gCoorSecondLambda) {
	GeographicCoordinate gCoorFirst_(gCoorFirstPhi, gCoorFirstLambda);
	GeographicCoordinate gCoorSecond_(gCoorSecondPhi, gCoorSecondLambda);
	this -> gCoorFirst_ = gCoorFirst_;
	this -> gCoorSecond_ = gCoorSecond_;
}
GeographicCoordinatePair::~GeographicCoordinatePair() {

}

GeographicCoordinate GeographicCoordinatePair::getFirst() const {
	return gCoorFirst_;
}
void GeographicCoordinatePair::setFirst( const GeographicCoordinate &gCoorFirst_ ) {
	this -> gCoorFirst_ = gCoorFirst_;
}

void GeographicCoordinatePair::setFirst( const double gCoorFirstPhi, const double gCoorFirstLambda ) {
	GeographicCoordinate gCoorFirst_(gCoorFirstPhi, gCoorFirstLambda);
	this -> gCoorFirst_ = gCoorFirst_;
}

GeographicCoordinate GeographicCoordinatePair::getSecond() const {
	return gCoorSecond_;
}
void GeographicCoordinatePair::setSecond( const GeographicCoordinate &gCoorSecond_ ) {
	this -> gCoorSecond_ = gCoorSecond_;
}

void GeographicCoordinatePair::setSecond( const double gCoorSecondPhi, const double gCoorSecondLambda ) {
	GeographicCoordinate gCoorSecond_(gCoorSecondPhi, gCoorSecondLambda);
	this -> gCoorSecond_ = gCoorSecond_;
}

void GeographicCoordinatePair::set(const double gCoorFirstPhi, const double gCoorFirstLambda, const double gCoorSecondPhi, const double gCoorSecondLambda) {
	GeographicCoordinate gCoorFirst_(gCoorFirstPhi, gCoorFirstLambda);
	GeographicCoordinate gCoorSecond_(gCoorSecondPhi, gCoorSecondLambda);
	this -> gCoorFirst_ = gCoorFirst_;
	this -> gCoorSecond_ = gCoorSecond_;
}

void GeographicCoordinatePair::set(const GeographicCoordinate &gCoorFirst_, const GeographicCoordinate &gCoorSecond_) {
	this -> gCoorFirst_ = gCoorFirst_;
	this -> gCoorSecond_ = gCoorSecond_;
}

double GeographicCoordinatePair::getFirstPhi() const {
	return gCoorFirst_.getPhi();
}
double GeographicCoordinatePair::getFirstLambda() const {
	return gCoorFirst_.getLambda();
}
double GeographicCoordinatePair::getSecondPhi() const {
	return gCoorSecond_.getPhi();
}
double GeographicCoordinatePair::getSecondLambda() const {
	return gCoorSecond_.getLambda();
}

void GeographicCoordinatePair::print() const {
	std::cout << "[ First, Second ] = [ (longitude, latitude), (longitude, latitude) ] = " << "[ (" << this -> gCoorFirst_.getPhi() << ", " << this -> gCoorFirst_.getLambda() << "), (" << this -> gCoorSecond_.getPhi() << ", " << this -> gCoorSecond_.getLambda() << ") ]" << "\n";
}

void GeographicCoordinatePair::printStr() const {
	std::cout << this -> gCoorFirst_.getPhi() << ", " << this -> gCoorFirst_.getLambda() << ", " << this -> gCoorSecond_.getPhi() << ", " << this -> gCoorSecond_.getLambda();
}
