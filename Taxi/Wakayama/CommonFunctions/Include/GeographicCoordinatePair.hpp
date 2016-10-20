/**
 * GeographicCoordinatePair.hpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 *
 */

#ifndef __HIRO_GEOGRAPHICCOORDINATEPAIR__
#define __HIRO_GEOGRAPHICCOORDINATEPAIR__

#include "GeographicCoordinate.hpp"

namespace hiro {
// クラス:GeographicCoordinatePair
// 定義名:gCoorPair
// 説明:地理座標系による位置クラス
	class GeographicCoordinatePair
	{
	public:
		GeographicCoordinatePair();	// コンストラクタ
		GeographicCoordinatePair(const GeographicCoordinate &gCoorFirst_, const GeographicCoordinate &gCoorSecond_);	// コンストラクタ
		GeographicCoordinatePair(const double gCoorFirstPhi, const double gCoorFirstLambda, const double gCoorSecondPhi, const double gCoorSecondLambda);	// コンストラクタ
		~GeographicCoordinatePair(); // デストラクタ

		GeographicCoordinate getFirst() const;
		void setFirst(const GeographicCoordinate &gCoorFirst_);
		void setFirst(const double gCoorFirstPhi, const double gCoorFirstLambda);

		GeographicCoordinate getSecond() const;
		void setSecond(const GeographicCoordinate &gCoorSecond_);
		void setSecond(const double gCoorSecondPhi, const double gCoorSecondLambda);

		void set(const double gCoorFirstPhi, const double gCoorFirstLambda, const double gCoorSecondPhi, const double gCoorSecondLambda);
		void set(const GeographicCoordinate &gCoorFirst_, const GeographicCoordinate &gCoorSecond_);

		double getFirstPhi() const;
		double getFirstLambda() const;
		double getSecondPhi() const;
		double getSecondLambda() const;

		void print() const;
		void printStr() const;
	private:
		GeographicCoordinate gCoorFirst_; // 地図上のboxの最北西，または，始点
		GeographicCoordinate gCoorSecond_; // 地図上のboxの最南東，または，終点
	};

} // namespace hiro

#endif // __HIRO_GEOGRAPHICCOORDINATEPAIR__
