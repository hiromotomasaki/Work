/**
 * GeographicCoordinate.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "GeographicCoordinate.hpp"

#include <iostream>

using namespace hiro;

GeographicCoordinate::GeographicCoordinate() {
  this -> phi_ = 0;
  this -> lambda_ = 0;
}
GeographicCoordinate::GeographicCoordinate( const double phi_, const double lambda_ ) {
  this -> phi_ = phi_;
  this -> lambda_ = lambda_;
}
GeographicCoordinate::~GeographicCoordinate() {

}
double GeographicCoordinate::getPhi() const {
  return phi_;
}
void GeographicCoordinate::setPhi( const double phi_ ) {
  this -> phi_ = phi_;
}
double GeographicCoordinate::getLambda() const {
  return lambda_;
}
void GeographicCoordinate::setLambda( const double lambda_ ) {
  this -> lambda_ = lambda_;
}
void GeographicCoordinate::set( const double phi_, const double lambda_ ) {
  this -> phi_ = phi_;
  this -> lambda_ = lambda_;
}
void GeographicCoordinate::print() const {
  std::cout << "( latitude, longitude ) = (" << this -> phi_ << ", " << this -> lambda_ << ")" <<std::endl;
}

