/**
 * TotalStation.hpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 *
 */

#ifndef __HIRO_TOTALSTATION__
#define __HIRO_TOTALSTATION__

namespace hiro {
// クラス:TotalStation
// 変数宣言:tStation
// 説明:測地線長(メートル)と方位角(ラジアン)のペアのクラス
	//=============測地線長と方位角のペアクラス(定義名:tStation)=============//
	class TotalStation
	{
	public:
		TotalStation();			// コンストラクタ
		TotalStation( const double length_, const double azimuth_ ); // コンストラクタ

		~TotalStation();                               // デストラクタ

		double getLength() const;                        // length_を取得
		void setLength( const double length_ );                 // length_を設定

		double getAzimuth() const;         // azimuth_を取得
		void setAzimuth( const double azimuth_ ); // azimuth_を設定

		void set( const double length_, const double azimuth_ ); // length_, azimuth_を設定

		void print() const;                // メンバ変数の表示
	private:
		double length_;    // 測地線長
		double azimuth_;   // 方位角(真北=0, 真東=90, 真南=180, 真西=270)
	};

	class LessTotalStation {
	public:
		bool operator()(const TotalStation& tStation_a, const TotalStation& tStation_b);
	};

} // namespace hiro

#endif // __HIRO_TOTALSTATION__
