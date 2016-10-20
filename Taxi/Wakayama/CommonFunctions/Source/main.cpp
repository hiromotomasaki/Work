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

int test_5_coordinate_degree(void);
int test_5_coordinate_degree(void)
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
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int test_6_coordinate_convertGCoorIntoCCoor(void);
int test_6_coordinate_convertGCoorIntoCCoor(void)
{
	std::cout << "=========================" << "\n";
	{
		std::cout << "******* gCoor -> cCoor *******" << "\n";
		{
			GeographicCoordinate gCoor( 36, 136 );
			GeographicCoordinate gCoorCri( 36, 136 );
			CartesianCoordinate cCoor = convertGCoorIntoCCoor( gCoor, gCoorCri );
			gCoor.print();
			gCoorCri.print();
			cCoor.print();
		}
		{
			GeographicCoordinate gCoor( 36.23, 136.1 );
			GeographicCoordinate gCoorCri( 36, 136 );
			CartesianCoordinate cCoor = convertGCoorIntoCCoor( gCoor, gCoorCri );
			gCoor.print();
			gCoorCri.print();
			cCoor.print();
		}
		{
			GeographicCoordinate gCoor( 35.99, 135.67 );
			GeographicCoordinate gCoorCri( 36, 136 );
			CartesianCoordinate cCoor = convertGCoorIntoCCoor( gCoor, gCoorCri );
			gCoor.print();
			gCoorCri.print();
			cCoor.print();
		}
		std::cout << "**************" << "\n";
	}
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int test_7_coordinate_convertCCoorIntoGCoor(void);
int test_7_coordinate_convertCCoorIntoGCoor(void)
{
	std::cout << "=========================" << "\n";
	{
		std::cout << "******* cCoor -> gCoor *******" << "\n";
		{
			CartesianCoordinate cCoor(0, 0);
			GeographicCoordinate gCoorCri( 36, 136 );
			GeographicCoordinate gCoor = convertCCoorIntoGCoor( cCoor, gCoorCri );
			cCoor.print();
			gCoorCri.print();
			gCoor.print();
			{
				Degree hoge = convertDoubleIntoDegree(gCoor.getPhi());
				hoge.print();
			}
			{
				Degree hoge = convertDoubleIntoDegree(gCoor.getLambda());
				hoge.print();
			}
		}
		{
			CartesianCoordinate cCoor(100, 100);
			GeographicCoordinate gCoorCri( 36, 136 );
			GeographicCoordinate gCoor = convertCCoorIntoGCoor( cCoor, gCoorCri );
			cCoor.print();
			gCoorCri.print();
			gCoor.print();
			{
				Degree hoge = convertDoubleIntoDegree(gCoor.getPhi());
				hoge.print();
			}
			{
				Degree hoge = convertDoubleIntoDegree(gCoor.getLambda());
				hoge.print();
			}
		}
		{
			CartesianCoordinate cCoor(100, -100);
			GeographicCoordinate gCoorCri( 36, 136 );
			GeographicCoordinate gCoor = convertCCoorIntoGCoor( cCoor, gCoorCri );
			cCoor.print();
			gCoorCri.print();
			gCoor.print();
			{
				Degree hoge = convertDoubleIntoDegree(gCoor.getPhi());
				hoge.print();
			}
			{
				Degree hoge = convertDoubleIntoDegree(gCoor.getLambda());
				hoge.print();
			}
		}
		std::cout << "**************" << "\n";
	}
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int test_8_coordinate_calculateTStationFromGCoor(void);
int test_8_coordinate_calculateTStationFromGCoor(void)
{
	std::cout << "=========================" << "\n";
	{
		std::cout << "******* (gCoor, gCoor) -> tStation *******" << "\n";
		{
			GeographicCoordinate gCoorStart( 36, 136 );
			GeographicCoordinate gCoorGoal( 36, 136 );
			TotalStation tStation = calculateTStationFromGCoor( gCoorStart, gCoorGoal );
			gCoorStart.print();
			gCoorGoal.print();
			tStation.print();
		}
		{
			GeographicCoordinate gCoorStart( 36, 136 );
			GeographicCoordinate gCoorGoal( 36.1, 136 );
			TotalStation tStation = calculateTStationFromGCoor( gCoorStart, gCoorGoal );
			gCoorStart.print();
			gCoorGoal.print();
			tStation.print();
		}
		{
			GeographicCoordinate gCoorStart( 36, 136 );
			GeographicCoordinate gCoorGoal( 36, 136.1 );
			TotalStation tStation = calculateTStationFromGCoor( gCoorStart, gCoorGoal );
			gCoorStart.print();
			gCoorGoal.print();
			tStation.print();
		}
		{
			GeographicCoordinate gCoorStart( 36, 136 );
			GeographicCoordinate gCoorGoal( 35.9, 136 );
			TotalStation tStation = calculateTStationFromGCoor( gCoorStart, gCoorGoal );
			gCoorStart.print();
			gCoorGoal.print();
			tStation.print();
		}
		{
			GeographicCoordinate gCoorStart( 36, 136 );
			GeographicCoordinate gCoorGoal( 36, 135.9 );
			TotalStation tStation = calculateTStationFromGCoor( gCoorStart, gCoorGoal );
			gCoorStart.print();
			gCoorGoal.print();
			tStation.print();
		}
		{
			GeographicCoordinate gCoorStart( 36, 136 );
			GeographicCoordinate gCoorGoal( 36.12321, 135.9234 );
			TotalStation tStation = calculateTStationFromGCoor( gCoorStart, gCoorGoal );
			gCoorStart.print();
			gCoorGoal.print();
			tStation.print();
		}
		std::cout << "**************" << "\n";
	}
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int test_9_coordinate_calculateTStationFromCCoor(void);
int test_9_coordinate_calculateTStationFromCCoor(void)
{
	std::cout << "=========================" << "\n";
	{
		std::cout << "******* (cCoor, cCoor) -> tStation *******" << "\n";
		{
			{
				CartesianCoordinate cCoorStart( 0, 0 );
				CartesianCoordinate cCoorGoal( 0, 0 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
			{
				CartesianCoordinate cCoorStart( 10, 0 );
				CartesianCoordinate cCoorGoal( 10, 0 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
			{
				CartesianCoordinate cCoorStart( 10, 10 );
				CartesianCoordinate cCoorGoal( 10, 10 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
			{
				CartesianCoordinate cCoorStart( 0, 10 );
				CartesianCoordinate cCoorGoal( 0, 10 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
			{
				CartesianCoordinate cCoorStart( 0, 0 );
				CartesianCoordinate cCoorGoal( 10, 0 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
			{
				CartesianCoordinate cCoorStart( 0, 0 );
				CartesianCoordinate cCoorGoal( 0, 10 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
			{
				CartesianCoordinate cCoorStart( 0, 0 );
				CartesianCoordinate cCoorGoal( -10, 0 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
			{
				CartesianCoordinate cCoorStart( 0, 0 );
				CartesianCoordinate cCoorGoal( 0, -10 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
			{
				CartesianCoordinate cCoorStart( 123, 23.2 );
				CartesianCoordinate cCoorGoal( 1002, 11 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
			{
				CartesianCoordinate cCoorStart( 0, 0 );
				CartesianCoordinate cCoorGoal( 10, 10 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
			{
				CartesianCoordinate cCoorStart( 0, 0 );
				CartesianCoordinate cCoorGoal( 10, -10 );
				GeographicCoordinate gCoorCri(36, 136);
				TotalStation tStation = calculateTStationFromCCoor( cCoorStart, cCoorGoal, gCoorCri );
				tStation.print();
				{
					Degree hoge = convertDoubleIntoDegree(tStation.getAzimuth());
					hoge.print();
				}
			}
		}
		std::cout << "**************" << "\n";
	}
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int test_10_GeographicCoordinatePair(void);
int test_10_GeographicCoordinatePair(void)
{
	std::cout << "=========================" << "\n";
	{
		{
			GeographicCoordinatePair gCoorPair;
			gCoorPair.print();
		}
		{
			GeographicCoordinatePair gCoorPair(1,2,3,4);
			gCoorPair.print();
		}
		{
			GeographicCoordinatePair gCoorPair;
			gCoorPair.setFirst(1,2);
			gCoorPair.print();
		}
		{
			GeographicCoordinatePair gCoorPair;
			GeographicCoordinate gCoorFirst(1,2);
			gCoorPair.setFirst(gCoorFirst);
			gCoorPair.print();
		}
		{
			GeographicCoordinatePair gCoorPair;
			GeographicCoordinate gCoorFirst(1,2);
			GeographicCoordinate gCoorSecond(3,4);
			gCoorPair.setFirst(gCoorFirst);
			gCoorPair.setSecond(gCoorSecond);
			gCoorPair.print();
		}
	}
	std::cout << "=========================" << "\n";
	return EXIT_SUCCESS;
}

int main()
{
	// test_1_Degree();
	// test_2_TotalStation();
	// test_3_CartesianCoordinate();
	// test_4_GeographicCoordinate();
	// test_5_coordinate_degree();
	// test_6_coordinate_convertGCoorIntoCCoor();
	// test_7_coordinate_convertCCoorIntoGCoor();
	// test_8_coordinate_calculateTStationFromGCoor();
	// test_9_coordinate_calculateTStationFromCCoor();
	test_10_GeographicCoordinatePair();
    return EXIT_SUCCESS;
}
