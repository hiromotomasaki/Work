/**
 * GeographicCoordinate.hpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 *
 */

#ifndef __HIRO_GEOGRAPHICCOORDINATE__
#define __HIRO_GEOGRAPHICCOORDINATE__

namespace hiro {
// クラス:GeographicCoordinate
// 定義名:gCoor
// 説明:地理座標系による位置クラス
	class GeographicCoordinate
	{
	public:
		GeographicCoordinate();       // コンストラクタ
		GeographicCoordinate( const double phi_, const double lambda_ ); // コンストラクタ

		~GeographicCoordinate();         // デストラクタ

		double getPhi() const;           // phi_を取得
		void setPhi( const double phi_ );       // phi_を設定

		double getLambda() const;        // lambda_を取得
		void setLambda( const double lambda_ ); // lambda_を設定

		void set( const double phi_, const double lambda_ ); // phi_, lambda_を設定

		void print() const;                    // メンバ変数の表示
	private:
		double phi_;                  // 緯度(latitude)
		double lambda_;               // 経度(longitude)
	};

} // namespace hiro

#endif // __HIRO_GEOGRAPHICCOORDINATE__
