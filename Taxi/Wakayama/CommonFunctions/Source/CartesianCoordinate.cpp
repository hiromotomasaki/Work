/**
 * CartesianCoordinate.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "CartesianCoordinate.hpp"

#include <iostream>

using namespace hiro;

CartesianCoordinate::CartesianCoordinate() {
  this -> x_ = 0;
  this -> y_ = 0;
}
CartesianCoordinate::CartesianCoordinate( const double x_, const double y_ ) {
  this -> x_ = x_;
  this -> y_ = y_;
}
CartesianCoordinate::~CartesianCoordinate() {

}
double CartesianCoordinate::getX() const {
  return x_;
}
void CartesianCoordinate::setX( const double x_ ) {
  this -> x_ = x_;
}
double CartesianCoordinate::getY() const {
  return y_;
}
void CartesianCoordinate::setY( const double y_ ) {
  this -> y_ = y_;
}
void CartesianCoordinate::set( const double x_, const double y_ ) {
  this -> x_ = x_;
  this -> y_ = y_;
}
void CartesianCoordinate::print() const {
  std::cout << "( X, Y ) = (" << this -> x_ << ", " << this -> y_ << ")" <<std::endl;
}

