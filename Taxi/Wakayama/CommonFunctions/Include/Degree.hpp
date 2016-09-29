/**
 * Degree.hpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 *
 */

#ifndef __HIRO_DEGREE__
#define __HIRO_DEGREE__

namespace hiro {
// クラス:Degree
// 定義名:deg
// 説明:角度(ラジアン)のクラス
	class Degree
	{
	public:
		Degree();				// コンストラクタ
		Degree( const int degrees_, const int minutes_, const double seconds_ ); // コンストラクタ

		~Degree();								  // デストラクタ

		int getDegrees() const;					  // degrees_を取得
		void setDegrees( const int degrees_ );    // degrees_を設定

		int getMinutes() const;					  // minutes_を取得
		void setMinutes( const int minutes_ );    // minutes_を設定

		double getSeconds() const;				  // seconds_を取得
		void setSeconds( const double seconds_ ); // seconds_を設定

		void set( const int degrees_, const int minutes_, const double seconds_ ); // degrees_, minutes_, seconds_を設定

		void print() const;		// メンバ変数の表示
	private:
		int degrees_;			// 度
		int minutes_;			// 分
		double seconds_;		// 秒
	};

} // namespace hiro

#endif // __HIRO_DEGREE__
