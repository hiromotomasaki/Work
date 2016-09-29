/**
 * Degree.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "Degree.hpp"

#include <iostream>

using namespace hiro;

Degree::Degree() {
  this -> degrees_ = 0;
  this -> minutes_ = 0;
  this -> seconds_ = 0;
}
Degree::Degree( const int degrees_, const int minutes_, const double seconds_ ) {
  this -> degrees_ = degrees_;
  this -> minutes_ = minutes_;
  this -> seconds_ = seconds_;
}
Degree::~Degree() {

}

int Degree::getDegrees() const {
  return degrees_;
}
void Degree::setDegrees( const int degrees_ ) {
  this -> degrees_ = degrees_;
}

int Degree::getMinutes() const {
  return minutes_;
}
void Degree::setMinutes( const int minutes_ ) {
  this -> minutes_ = minutes_;
}

double Degree::getSeconds() const {
  return seconds_;
}
void Degree::setSeconds( const double seconds_ ) {
  this -> seconds_ = seconds_;
}

void Degree::set( const int degrees_, const int minutes_, const double seconds_ ) {
  this -> degrees_ = degrees_;
  this -> minutes_ = minutes_;
  this -> seconds_ = seconds_;
}
void Degree::print() const {
  std::cout << "( deg, min, sec ) = (" << this -> degrees_ << ", " << this -> minutes_ << ", " << this -> seconds_ << ")" <<std::endl;
}

