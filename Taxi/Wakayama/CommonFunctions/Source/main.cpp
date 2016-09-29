/**
 * main.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "main.hpp"

using namespace hiro;

int test_1_Degree(void);
int test_1_Degree(void)
{

	std::cout << "=========================" << "\n";
	{
		Degree deg( 10, 20, 30.1 );

		deg.print();
		{
			int degHoge = deg.getDegrees();
			std::cout << "( " << degHoge << " )" << "\n";
		}
		deg.setDegrees( 20 );
		deg.print();
	}
	{
		Degree deg;
		deg.print();
	}
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int test_2_TotalStation(void);
int test_2_TotalStation(void)
{
	std::cout << "=========================" << "\n";
	{
		TotalStation tStation( 1071.1234, 20.123 );

		tStation.print();
		{
			double tStationHoge = tStation.getLength();
			std::cout << "( " << tStationHoge << " )" << "\n";
		}
		{
			double tStationHoge = tStation.getAzimuth();
			std::cout << "( " << tStationHoge << " )" << "\n";
		}
		tStation.setLength( 20 );
		tStation.print();
	}
	{
		TotalStation tStation;
		tStation.print();
	}
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int test_3_CartesianCoordinate(void);
int test_3_CartesianCoordinate(void)
{
	std::cout << "=========================" << "\n";
	{
		CartesianCoordinate cCoor( 1071.1234, 20.123 );

		cCoor.print();
		{
			double cCoorHoge = cCoor.getX();
			std::cout << "( " << cCoorHoge << " )" << "\n";
		}
		{
			double cCoorHoge = cCoor.getY();
			std::cout << "( " << cCoorHoge << " )" << "\n";
		}
		cCoor.setX( 20 );
		cCoor.print();
	}
	{
		CartesianCoordinate cCoor;
		cCoor.print();
	}
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int test_4_GeographicCoordinate(void);
int test_4_GeographicCoordinate(void)
{
	std::cout << "=========================" << "\n";
	{
		GeographicCoordinate gCoor( 45.1234, 135.123 );

		gCoor.print();
		{
			double gCoorHoge = gCoor.getPhi();
			std::cout << "( " << gCoorHoge << " )" << "\n";
		}
		{
			double gCoorHoge = gCoor.getLambda();
			std::cout << "( " << gCoorHoge << " )" << "\n";
		}
		gCoor.setPhi( 20 );
		gCoor.print();
	}
	{
		GeographicCoordinate gCoor;
		gCoor.print();
	}
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int test_5_coordinate(void);
int test_5_coordinate(void)
{
	std::cout << "=========================" << "\n";
	{
		std::cout << "**************" << "\n";
		Degree deg( 23, 34, 123.1 );
		double hoge = convertDegreeIntoDouble( deg );
		std::cout << hoge << "\n";
		std::cout << "**************" << "\n";
	}
	{
		std::cout << "**************" << "\n";
		Degree deg = convertDoubleIntoDegree( 123.324 );
		deg.print();
		std::cout << "**************" << "\n";
	}
	{
		std::cout << "**************" << "\n";
		GeographicCoordinate gCoor( 36.23, 136.1 );
		GeographicCoordinate gCoorCri( 36, 136 );
		CartesianCoordinate cCoor = convertGCoorIntoCCoor( gCoor, gCoorCri );
		cCoor.print();
		std::cout << "**************" << "\n";
	}
	{
		// std::cout << "**************" << "\n";
		// CartesianCoordinate cCoor( 36.23, 136.1 );
		// GeographicCoordinate gCoorCri( 36, 136 );
		// GeographicCoordinate gCoor = convertCCoorIntoGCoor( cCoor, gCoorCri );
		// gCoor.print();
		// Degree degLat = convertDoubleIntoDegree( gCoor.getPhi() );
		// Degree degLon = convertDoubleIntoDegree( gCoor.getLambda() );
		// degLat.print();
		// degLon.print();
		// std::cout << "**************" << "\n";
		std::cout << "**************" << "\n";
		CartesianCoordinate cCoor( -2134, 6432 );
		GeographicCoordinate gCoorCri( 36, 136 );
		GeographicCoordinate gCoor = convertCCoorIntoGCoor( cCoor, gCoorCri );
		gCoor.print();
		Degree degLat = convertDoubleIntoDegree( gCoor.getPhi() );
		Degree degLon = convertDoubleIntoDegree( gCoor.getLambda() );
		degLat.print();
		degLon.print();
		std::cout << "**************" << "\n";
	}
	{
		// std::cout << "**************" << "\n";
		// GeographicCoordinate gCoorStart( 36, 136 );
		// GeographicCoordinate gCoorGoal( 36.1, 136 );
		// TotalStation tStation = calculateTStationFromGCoor( gCoorStart, gCoorGoal );
		// tStation.print();
		// std::cout << "**************" << "\n";
		std::cout << "**************" << "\n";
		GeographicCoordinate gCoorStart( 36, 136 );
		GeographicCoordinate gCoorGoal( 36.123, 135.876 );
		TotalStation tStation = calculateTStationFromGCoor( gCoorStart, gCoorGoal );
		tStation.print();
		std::cout << "**************" << "\n";
	}
	{
		std::cout << "**************" << "\n";
		GeographicCoordinate gCoorCri( 36, 136 );
		CartesianCoordinate cCoorStart( 36.23, 136.1 );
		CartesianCoordinate cCoorGoal( 100, 1312.1 );
		TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
		tStation.print();
		std::cout << "**************" << "\n";
	}
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	test_1_Degree();
	test_2_TotalStation();
	test_3_CartesianCoordinate();
	test_4_GeographicCoordinate();
	test_5_coordinate();
    return EXIT_SUCCESS;
}
