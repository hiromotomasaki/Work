/**
 * CartesianCoordinate.hpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 *
 */

#ifndef __HIRO_CARTESIANCOORDINATE__
#define __HIRO_CARTESIANCOORDINATE__

namespace hiro {
// クラス:CartesianCoordinate
// 定義名:cCoor
// 説明:デカルト座標系による位置クラス
	class CartesianCoordinate
	{
	public:
		CartesianCoordinate();	// コンストラクタ
		CartesianCoordinate( const double x_, const double y_ ); // コンストラクタ
		~CartesianCoordinate();						  // デストラクタ
		double getX() const;						  // x_を取得
		void setX( const double x_ );				  // x_を設定
		double getY() const;						  // y_を取得
		void setY( const double y_ );				  // y_を設定
		void set( const double x_, const double y_ ); // x_, y_を設定
		void print() const;		// メンバ変数の表示
	private:
		double x_;		   // 地理座標系の原点から北を正、南を負とする
		double y_;		   // 地理座標系の原点から東を正、西を負とする
	};

} // namespace hiro

#endif // __HIRO_CARTESIANCOORDINATE__
