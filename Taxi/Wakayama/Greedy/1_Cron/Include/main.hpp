/**
 * main.hpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 *
 */

#ifndef __HIRO_MAIN__
#define __HIRO_MAIN__

#include <iostream>				// std::cout, std::endl
// http://cpprefjp.github.io/reference/iostream.html
#include <cstdlib>				// EXIT_SUCCESS, EXIT_FAILURE
// http://cpprefjp.github.io/reference/cstdlib.html

#include "Degree.hpp"
#include "TotalStation.hpp"
#include "CartesianCoordinate.hpp"
#include "GeographicCoordinate.hpp"
#include "coordinate.hpp"

#include "GeographicCoordinatePair.hpp"

#include <boost/property_tree/xml_parser.hpp> /* boost::property_tree::xml_parser::write_xml, boost::property_tree::xml_parser::read_xml */
/* http://www.boost.org/doc/libs/1_61_0/doc/html/boost/property_tree/xml_parser/write_xml_idp197174048.html */
#include <boost/filesystem.hpp> /* boost::filesystem::create_directories, -lboost_system -lboost_filesystemが必要 */
/* http://boostjp.github.io/tips/filesystem.html */
#include <boost/lexical_cast.hpp> /* boost::lexical_cast<型> */
/* http://www.kmonos.net/alang/boost/classes/lexical_cast.html */

// プログレス表示に必要
#include <boost/progress.hpp>
// 待ち時間
#include <boost/date_time/posix_time/posix_time.hpp>
// プログレス表示に必要
#include <boost/thread/thread.hpp> 

// serverとhiromotoのライブラリのバージョンが違うため
#include <vector>
#include <numeric>
#include <string>
#include <functional>

namespace hiro {

} // namespace hiro


#endif // __HIRO_MAIN__
