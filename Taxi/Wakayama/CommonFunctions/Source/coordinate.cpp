/**
 * coordinate.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "coordinate.hpp"

#include "GeographicCoordinate.hpp" // 地理座標のクラス
#include "CartesianCoordinate.hpp" // デカルト座標のクラス
#include "Degree.hpp"             // 角度のクラス
#include "TotalStation.hpp"       // 測地線長と方位角のクラス
#include <cmath>

#include <iostream>

using namespace hiro;

namespace hito
{
    constexpr double sqrt_impl( double s, double x, double last )
    {
        return x != last ? sqrt_impl( s, (x + s / x) / 2.0, x ) : x ;
    }

    constexpr double sqrt( double s )
    {
        return sqrt_impl( s, s / 2.0, 0.0 ) ;
    }
}


double convertDegreeIntoDouble( const Degree &deg ) {
	double ret = deg.getDegrees() + deg.getMinutes() / 60.0 + deg.getSeconds() / 3600.0;
	return ret;
}
Degree convertDoubleIntoDegree( const double &deg10 ) {
	double hoge = deg10;
	double degrees = floor(hoge);    // 小数切り捨て
	hoge -= degrees;
	hoge *= 60;
	double minutes = floor(hoge); // 小数切り捨て
	hoge -= minutes;
	hoge *= 60;
	double seconds = hoge;
	Degree ret(degrees, minutes, seconds);
	return ret;
}

int calculateRowFromLatitudes( const double phi, const double northPhi, const double southPhi, const double cellSizePhi ) {
	if( phi > northPhi || phi < southPhi ) {
		return 0;                   // 営業領域外
	}else if( phi == northPhi ) {
		return 1;
	}
	int ret = ceil( ( northPhi - phi ) / cellSizePhi ); // ceilは小数切り上げ
	return ret;
}
int calculateColFromLongitudes( const double lambda, const double westLambda, const double eastLambda, const double cellSizeLambda ) {
	if( lambda > eastLambda || lambda < westLambda ) {
		return 0;                   // 営業領域外
	}else if( lambda == westLambda ) {
		return 1;
	}
	int ret = ceil( ( lambda - westLambda ) / cellSizeLambda ); // ceilは小数切り上げ
	return ret;
}

int calculateRowFromIndex( const int index, const int numCol, const int numCell ) {
	if( index > numCell || index < 1 ) {
		return 0;
	}
	if(index == 1) {
		return 1;
	}
	int ret = ( index - 1 ) / numCol + 1;
	return ret;
}
int calculateColFromIndex( const int index, const int numCol, const int numCell ) {
	if( index > numCell || index < 1 ) {
		return 0;
	}
	int ret = index % numCol;
	if( ret == 0 ) {
		ret = numCol;
	}
	return ret;
}

int calculateIndexFromRowCol( const int row, const int col, const int numRow, const int numCol ) {
	if( row < 1 || row > numRow || col < 1 || col > numCol ) {
		return 0;
	}
	int ret = ( row - 1 ) * numCol + col;
	return ret;
}

CartesianCoordinate convertGCoorIntoCCoor( const GeographicCoordinate &gCoor, const GeographicCoordinate &gCoorCri ) {
	if (gCoor.getPhi() == gCoorCri.getPhi() && gCoor.getLambda() == gCoorCri.getLambda()) {
		CartesianCoordinate ret(0, 0);
		return ret;
	}
	constexpr double degToRad = M_PI / 180.0;
	double phi = gCoor.getPhi() * degToRad; // ラジアンに単位を変換した、これから変形する地理座標系の緯度
	double lambda = gCoor.getLambda() * degToRad; // ラジアンに単位を変換した、これから変形する地理座標系の経度
	double phiCri = gCoorCri.getPhi() * degToRad; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の緯度
	double lambdaCri = gCoorCri.getLambda() * degToRad; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の経度
	constexpr double a = 6378137;         // 楕円体の長半径
	constexpr double F = 298.257222101;   // 楕円体の逆扁平率
	constexpr double m_0 = 0.9999;        // 平面直角座標系のX軸上における縮尺係数
	constexpr double n = 1.0 / ( 2.0 * F - 1.0 );
	//--------------------------------//
	constexpr double n2 = n * n;
	constexpr double n3 = n * n2;
	constexpr double n4 = n * n3;
	constexpr double n5 = n * n4;

	constexpr double alpha_1 = (1 / 2.0) * n - (2 / 3.0) * n2 + (5 / 16.0) * n3 + (41 / 180.0) * n4 - (127 / 288.0) * n5;
	constexpr double alpha_2 = (13 / 48.0) * n2 - (3 / 5.0) * n3 + (557 / 1440.0) * n4 + (281 / 630.0) * n5;
	constexpr double alpha_3 = (61 / 240.0) * n3 - (103 / 140.0) * n4 + (15061 / 26880.0) * n5;
	constexpr double alpha_4 = (49561 / 161280.0) * n4 - (179 / 168.0) * n5;
	constexpr double alpha_5 = (34729 / 80640.0) * n5;

	constexpr double A_0 = 1 + (1 / 4.0) * n2 + (1 / 64.0) * n4;
	constexpr double A_1 = - (3 / 2.0) * (n - (1 / 8.0) * n3 - (1 / 64.0) * n5);
	constexpr double A_2 = (15 / 16.0) * (n2 - (1 / 4.0) * n4);
	constexpr double A_3 = - (35 / 48.0) * (n3 - (5 / 16.0) * n5);
	constexpr double A_4 = (315 / 512.0) * n4;
	constexpr double A_5 = - (693 / 1280.0) * n5;

	//--------------------------------//
	constexpr double n_plus_1 = n + 1;
	constexpr double A_prime = m_0 * a / n_plus_1;
	constexpr double A_bar = A_prime * A_0;

	double S_bar = 0;
	{
		double s = 0;              // 内積を格納する変数
		{
			s += A_1 * sin(2 * phiCri);
			s += A_2 * sin(4 * phiCri);
			s += A_3 * sin(6 * phiCri);
			s += A_4 * sin(8 * phiCri);
			s += A_5 * sin(10 * phiCri);
		}
		S_bar = A_bar * phiCri + A_prime * s;
	}

	double sin_phi = sin( phi );
	constexpr double paraHoge = 2 * hito::sqrt(n) / n_plus_1;
	double t = sinh( atanh(sin_phi) - paraHoge * atanh(paraHoge * sin_phi) );
	double t_bar = sqrt(1 + t * t);
	double diff_lambda = lambda - lambdaCri;
	double lambda_c = cos( diff_lambda );
	double lambda_s = sin( diff_lambda );
	double xi_prime = atan( t / lambda_c );
	double ita_prime = atanh( lambda_s / t_bar );

	double x = 0;
	{
		double s = 0;              // 内積を格納する変数
		{
			s += alpha_1 * sin(2 * xi_prime) * cosh(2 * ita_prime);
			s += alpha_2 * sin(4 * xi_prime) * cosh(4 * ita_prime);
			s += alpha_3 * sin(6 * xi_prime) * cosh(6 * ita_prime);
			s += alpha_4 * sin(8 * xi_prime) * cosh(8 * ita_prime);
			s += alpha_5 * sin(10 * xi_prime) * cosh(10 * ita_prime);
		}
		x = A_bar * (xi_prime + s) - S_bar;
	}

	double y = 0;
	{
		double s = 0;              // 内積を格納する変数
		{
			s += alpha_1 * cos(2 * xi_prime) * sinh(2 * ita_prime);
			s += alpha_2 * cos(4 * xi_prime) * sinh(4 * ita_prime);
			s += alpha_3 * cos(6 * xi_prime) * sinh(6 * ita_prime);
			s += alpha_4 * cos(8 * xi_prime) * sinh(8 * ita_prime);
			s += alpha_5 * cos(10 * xi_prime) * sinh(10 * ita_prime);
		}
		y = A_bar * (ita_prime + s);
	}

	CartesianCoordinate ret( x, y );  // 返り値
	return ret;
}
GeographicCoordinate convertCCoorIntoGCoor( const CartesianCoordinate &cCoor, const GeographicCoordinate &gCoorCri ) {
	if (cCoor.getX() == 0 && cCoor.getY() == 0) {
		GeographicCoordinate ret(gCoorCri.getPhi(), gCoorCri.getLambda());
		return ret;
	}
	constexpr double degToRad = M_PI / 180.0;
	double x = cCoor.getX();   // これから変形するデカルト座標系のx座標
	double y = cCoor.getY();   // これから変形するデカルト座標系のy座標
	double phiCri = gCoorCri.getPhi() * degToRad; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の緯度
	double lambdaCri = gCoorCri.getLambda() * degToRad; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の経度

	constexpr double a = 6378137;         // 楕円体の長半径
	constexpr double F = 298.257222101;   // 楕円体の逆扁平率
	constexpr double m_0 = 0.9999;        // 平面直角座標系のX軸上における縮尺係数
	constexpr double n = 1.0 / ( 2 * F - 1 );

	constexpr double n2 = n * n;
	constexpr double n3 = n * n2;
	constexpr double n4 = n * n3;
	constexpr double n5 = n * n4;
	constexpr double n6 = n * n5;

	constexpr double beta_1 = (1 / 2.0) * n - (2 / 3.0) * n2 + (37 / 96.0) * n3 - (1 / 360.0) * n4 - (81 / 512.0) * n5;
	constexpr double beta_2 = (1 / 48.0) * n2 + (1 / 15.0) * n3 - (437 / 1440.0) * n4 + (46 / 105.0) * n5;
	constexpr double beta_3 = (17 / 480.0) * n3 - (37 / 840.0) * n4 - (209 / 4480.0) * n5;
	constexpr double beta_4 = (4397 / 161280.0) * n4 - (11 / 504.0) * n5;
	constexpr double beta_5 = (4583 / 161280.0) * n5;

	constexpr double delta_1 = (2) * n - (2 / 3.0) * n2 - (2) * n3 + (116 / 45.0) * n4 + (26 / 45.0) * n5 - (2854 / 675.0) * n6;
	constexpr double delta_2 = (7 / 3.0) * n2 - (8 / 5.0) * n3 - (227 / 45.0) * n4 + (2704 / 315.0) * n5 + (2323 / 945.0) * n6;
	constexpr double delta_3 = (56 / 15.0) * n3 - (136 / 35.0) * n4 - (1262 / 105.0) * n5 + (73814 / 2835.0) * n6;
	constexpr double delta_4 = (4279 / 630.0) * n4 - (332 / 35.0) * n5 - (399572 / 14175.0) * n6;
	constexpr double delta_5 = (4174 / 315.0) * n5 - (144838 / 6237.0) * n6;
	constexpr double delta_6 = (601676 / 22275.0) * n6;

	constexpr double A_0 = 1 + (1 / 4.0) * n2 + (1 / 64.0) * n4;
	constexpr double A_1 = - (3 / 2.0) * (n - (1 / 8.0) * n3 - (1 / 64.0) * n5);
	constexpr double A_2 = (15 / 16.0) * (n2 - (1 / 4.0) * n4);
	constexpr double A_3 = - (35 / 48.0) * (n3 - (5 / 16.0) * n5);
	constexpr double A_4 = (315 / 512.0) * n4;
	constexpr double A_5 = - (693 / 1280.0) * n5;

	//--------------------------------//
	constexpr double n_plus_1 = n + 1;
	constexpr double A_prime = m_0 * a / n_plus_1;
	constexpr double A_bar = A_prime * A_0;
	constexpr double A_bar_inv = 1 / A_bar;

	double S_bar = 0;
	{
		double s = 0;              // 内積を格納する変数
		{
			s += A_1 * sin(2 * phiCri);
			s += A_2 * sin(4 * phiCri);
			s += A_3 * sin(6 * phiCri);
			s += A_4 * sin(8 * phiCri);
			s += A_5 * sin(10 * phiCri);
		}
		S_bar = A_bar * phiCri + A_prime * s;
	}

	double xi = (x + S_bar) * A_bar_inv;
	double ita = y * A_bar_inv;
	double xi_prime = 0;
	{
		double s = 0;
		{
			s += beta_1 * sin(2 * xi) * cosh(2 * ita);
			s += beta_2 * sin(4 * xi) * cosh(4 * ita);
			s += beta_3 * sin(6 * xi) * cosh(6 * ita);
			s += beta_4 * sin(8 * xi) * cosh(8 * ita);
			s += beta_5 * sin(10 * xi) * cosh(10 * ita);
		}
		xi_prime = xi - s;
	}
	double ita_prime = 0;
	{
		double s = 0;
		{
			s += beta_1 * cos(2 * xi) * sinh(2 * ita);
			s += beta_2 * cos(4 * xi) * sinh(4 * ita);
			s += beta_3 * cos(6 * xi) * sinh(6 * ita);
			s += beta_4 * cos(8 * xi) * sinh(8 * ita);
			s += beta_5 * cos(10 * xi) * sinh(10 * ita);
		}
		ita_prime = ita - s;
	}

	double kai = asin( sin( xi_prime ) / cosh( ita_prime ) );

	// ラジアン形式で求めてら度へ変換する
	constexpr double radToDeg = 180.0 / M_PI;
	double phi = 0;
	{
		double s = 0;
		{
			s += delta_1 * sin(2 * kai);
			s += delta_2 * sin(4 * kai);
			s += delta_3 * sin(6 * kai);
			s += delta_4 * sin(8 * kai);
			s += delta_5 * sin(10 * kai);
			s += delta_6 * sin(12 * kai);
		}
		phi = (kai + s) * radToDeg;
	}
	double lambda = 0;
	{
		lambda = ( lambdaCri + atan( sinh( ita_prime ) / cos( xi_prime ) ) ) * radToDeg;
	}

	GeographicCoordinate ret( phi, lambda );  // 返り値
	return ret;
}

//////////////////////////////////////////////////////////////////////////////

TotalStation calculateTStationFromCCoor( const CartesianCoordinate &cCoorStart, const CartesianCoordinate &cCoorGoal, const GeographicCoordinate &gCoorCri ) {
	if (cCoorStart.getX() == cCoorGoal.getX() && cCoorStart.getY() == cCoorGoal.getY()) {
		TotalStation ret(0, 0);
		return ret;
	}
	double length = 0;
	double azimuth = 0;
	// 処理
	{
		constexpr double  degToRad = M_PI / 180.0;
		double phi_0 = gCoorCri.getPhi() * degToRad; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の緯度
		// double lambda_0 = gCoorCri.getLambda() * M_PI / 180.0; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の経度
		double y_1 = cCoorStart.getY(); // 出発点のデカルト座標系のy座標
		double x_1 = cCoorStart.getX(); // 出発点のデカルト座標系のx座標
		double y_2 = cCoorGoal.getY(); // 到着点のデカルト座標系のy座標
		double x_2 = cCoorGoal.getX(); // 到着点のデカルト座標系のx座標
		constexpr double m_0 = 0.9999;
		constexpr double a = 6378137;
		constexpr double F = 298.257222101;
		constexpr double f = 1.0 / F;
		constexpr double e = hito::sqrt(f * (2 - f));
		constexpr double e2 = e * e;
		constexpr double R_0_ue = a * hito::sqrt(1 - e2);
		double sin_phi = sin(phi_0);
		double R_0 = R_0_ue / (1 - e2 * sin_phi * sin_phi);
		double R2_0 = R_0 * R_0;

		double x_2_minus_x_1 = x_2 - x_1;
		double y_2_minus_y_1 = y_2 - y_1;
		double y_1_plus_y_2 = y_1 + y_2;
		double y_1_times_y_2 = y_1 * y_2;
		constexpr double m2_0 = m_0 * m_0;
		constexpr double m3_0 = m_0 * m_0 * m_0;

		double S_over_s_inv = 0;
		{
			constexpr double paraHoge1 = 6 * m2_0;
			constexpr double paraHoge2 = 6 * m3_0;
			S_over_s_inv = (paraHoge1 * R2_0) / (paraHoge2 * R2_0 + y_1_plus_y_2 * y_1_plus_y_2 - y_1_times_y_2);
		}

		double S = sqrt(x_2_minus_x_1 * x_2_minus_x_1 + y_2_minus_y_1 * y_2_minus_y_1) * S_over_s_inv;

		length = S;

		if (x_2_minus_x_1 > 0 && y_2_minus_y_1 == 0) {
			azimuth = 0;
		}else if (x_2_minus_x_1 == 0 && y_2_minus_y_1 > 0) {
			azimuth = 90;
		}else if (x_2_minus_x_1 < 0 && y_2_minus_y_1 == 0) {
			azimuth = 180;
		}else if (x_2_minus_x_1 == 0 && y_2_minus_y_1 < 0) {
			azimuth = 270;
		}else{
			double t_1 = atan(y_2_minus_y_1 / x_2_minus_x_1);
			double t_1_muinus_T_1 = 0;
			{
				constexpr double paraHoge1 = 1 / (4.0 * m2_0);
				constexpr double paraHoge2 = - (4 / 3.0);
				constexpr double paraHoge3 = - (2 / 3.0);
				t_1_muinus_T_1 = paraHoge1 * (paraHoge2 * y_1 + paraHoge3 * y_2) * x_2_minus_x_1 / R2_0;
			}
			double T_1 = t_1 - t_1_muinus_T_1;
			if(T_1 < 0){
				while(T_1 < 0){
					T_1 += 2 * M_PI;
				}
			}else if(T_1 >= 2 * M_PI){
				while(T_1 >= 2 * M_PI){
					T_1 -= 2 * M_PI;
				}
			}
			constexpr double radToDeg = 180.0 / M_PI;
			azimuth = T_1 * radToDeg;
		}
	}
	TotalStation ret(length, azimuth);
	return ret;
}

TotalStation calculateTStationFromGCoor( const GeographicCoordinate &gCoorStart, const GeographicCoordinate &gCoorGoal ) {
	constexpr double degToRad = M_PI / 180.0;
	constexpr double radToDeg = 180.0 / M_PI;
	constexpr double M_PI_times_2 = 2 * M_PI;
	if( gCoorStart.getPhi() == gCoorGoal.getPhi() && gCoorStart.getLambda() == gCoorGoal.getLambda() ) {
		// ここの部分は国土地理院の計算式と違う
		// 同じ時に除算で0でわる操作があるため測地線長も方位角も-nanになってしまう。
		// ゾーンは1でsita_0は0でitaは0でxiは0.82...であるので,gが0hが1になる。
		// J=2ghでgamma=y*sin(sita_n)/Jのためgammaがnanになる（yは正）.しかし-nanとなっていた.
		// そこで、例外処理をすることにした。
		TotalStation ret(0, 0 );
		return ret;
	}
	double phi_1 = gCoorStart.getPhi() * degToRad; // ラジアンに単位を変換した、出発点の地理座標系の緯度
	double L_1 = gCoorStart.getLambda() * degToRad; // ラジアンに単位を変換した、出発点の地理座標系の経度
	double phi_2 = gCoorGoal.getPhi() * degToRad; // ラジアンに単位を変換した、到着点の地理座標系の緯度
	double L_2 = gCoorGoal.getLambda() * degToRad; // ラジアンに単位を変換した、到着点の地理座標系の経度

	constexpr double a = 6378137;             // 楕円体の長半径
	constexpr double f = 1.0 / 298.257222101; // 楕円体の扁平率
	double l = L_2 - L_1;
	double l_prime = 0;
	{
		if(l > M_PI){
			l_prime = l - M_PI_times_2;
		}else if(l < - M_PI){
			l_prime = l + M_PI_times_2;
		}else{
			l_prime = l;
		}
	}
	double L = fabs(l_prime);
	double L_prime = M_PI - L;
	double Delta = 0;
	double u_1 = 0;
	double u_2 = 0;
	constexpr double minus_f_plus_1 = - f + 1;
	{
		if(l_prime >= 0){
			Delta = phi_2 - phi_1;
			u_1 = atan(minus_f_plus_1 * tan(phi_1));
			u_2 = atan(minus_f_plus_1 * tan(phi_2));
		}else{
			Delta = phi_1 - phi_2;
			u_1 = atan(minus_f_plus_1 * tan(phi_2));
			u_2 = atan(minus_f_plus_1 * tan(phi_1));
		}
	}
	double Sigma = phi_1 + phi_2;

	double Sigma_prime = u_1 + u_2;
	double Delta_prime = u_2 - u_1;
	double xi = cos(Sigma_prime / 2.0);
	double xi_prime = sin(Sigma_prime / 2.0);
	double ita = sin(Delta_prime / 2.0);
	double ita_prime = cos(Delta_prime / 2.0);

	double sin_u_1 = sin(u_1);
	double sin_u_2 = sin(u_2);
	double cos_u_1 = cos(u_1);
	double cos_u_2 = cos(u_2);
	double x = sin_u_1 * sin_u_2;
	double y = cos_u_1 * cos_u_2;
	double c = y * cos(L) + x;
	constexpr double epsilon = f * (2 - f) / (minus_f_plus_1 * minus_f_plus_1);

    // ゾーンの判定. ゾーンは1, 2, 3の3種類
	int flag_zone = 0;
	double sita_0;
	if(c >= 0){
		flag_zone = 1;
		sita_0 = L * (1 + f * y);
	}else if(c < 0 && c >= - cos(3.0 *degToRad * cos_u_1)){
		flag_zone = 2;
		sita_0 = L_prime;
	}else{
		flag_zone = 3;
		// sita_0は次のところで決める
	}
	// flag_zone == 3 の場合、ある条件で例外処理がある
	// int flag_3a = 0, flag_3b1 = 0, flag_3b2 = 0, flag_3b3 = 0;
	if(flag_zone == 3){
		double R = 0;
		{
			constexpr double paraHoge1 = f * M_PI;
			double cos2_u_1 = cos_u_1 * cos_u_1;
			constexpr double paraHoge2 = - (1 / 4.0) * f * (1 + f);
			double sin2_u_1 = sin_u_1 * sin_u_1;
			constexpr double paraHoge3 = (3 / 16.0) * f * f;
			double sin4_u_1 = sin2_u_1 * sin2_u_1;
			R = paraHoge1 * cos2_u_1 * (1 + paraHoge2 * sin2_u_1 + paraHoge3 * sin4_u_1);
		}
		double d_1 = L_prime * cos_u_1 - R;
		double d_2 = fabs(Sigma_prime) + R;
		double q = 0;
		{
			constexpr double paraHoge1 = 1 / (f * M_PI);
			q = L_prime * paraHoge1;
		}
		constexpr double f_1 = (1 / 4.0) * f * (1 + (1 / 2.0) * f);
		constexpr double f_1_plus_1 = f_1 + 1;
		double gamma_0 = f_1_plus_1 * q - f_1 * pow(q, 3);
		if(Sigma != 0){
			// \sita_0を求める
			// flag_3a = 1;
			double A_0 = atan(d_1 / d_2);
			double B_0 = asin(R / sqrt(pow(d_1, 2) + pow(d_2, 2)));
			double psy = A_0 + B_0;
			double j = gamma_0 / cos(u_1);
			double k = (1 + f_1) * fabs(Sigma_prime) * (1 - f * y) / (f * M_PI * y);
			double j_1 = j / (1 + k / cos(psy));
			double psy_prime = asin(j_1);
			double psy_prime_prime = asin(cos(u_1) * j_1 / cos(u_2));
			sita_0 = 2 * atan(tan((psy_prime + psy_prime_prime) / 2.0) * sin(fabs(Sigma_prime) / 2.0) / cos(Delta_prime / 2.0));
		}else{
			if(d_1 > 0){
				// \sita_0を求める
				// flag_3b1 = 1;
				sita_0 = L_prime;
			}else if(d_1 == 0){
				// \sita_0を求めずに例外処理でTsを求める。そしてこのリージョンでreturnする。
				// flag_3b2 = 1;
				// double alpha_1 = M_PI_2; // 出発点における到着点の方位角(これが求める方位角(rad))
				// double alpha_2 = M_PI_2; // ?
				// double alpha_21 = M_PI + M_PI_2; // 到着点における出発点の方位角
				double Gamma = pow(u_1, 2);
				double n_0 = epsilon * Gamma / pow(sqrt(1 + epsilon * Gamma) + 1, 2);
				double A = (1 + n_0) * (1 + 1.25 * pow(n_0, 2));
				double s = (1 - f) * a * A * M_PI; // 測地線長
				TotalStation ret(s, 90);           // alpha_1(rad) = M_PI_2を度に変換すると90度になる
				return ret;
			}else{
				// \sita_0を求めずに例外処理でTsを求める。そしてこのリージョンでreturnする。
				// flag_3b3 = 1;
				double gamma_n = gamma_0;
				double gamma_n_plus_1 = gamma_0;
				double Gamma = 0;
				double D = 0;
				double diff = 1;
				while(fabs(diff) >= pow(10, -15)){
					Gamma = 1 - pow(gamma_n, 2);
					D = 0.25 * f * (1 + f) - 3.0 / 16.0 * pow(f, 2) * Gamma;
					gamma_n_plus_1 = q / (1 - D * Gamma);
					diff = gamma_n_plus_1 - gamma_n;
					gamma_n = gamma_n_plus_1;
				}
				double m = 1 - q / cos(u_1);
				double n = D * Gamma / (1 - D * Gamma);
				double w = m - n + m * n;
				double alpha_1 = 0;
				if(w <= 0){
					alpha_1 = M_PI_2; // 出発点における到着点の方位角(これが求める方位角(rad))
				}else{
					alpha_1 = M_PI_2 - 2 * asin(sqrt(w / 2.0)); // 出発点における到着点の方位角(これが求める方位角(rad))
				}
				// double alpha_2 = M_PI - alpha_1; // ?
				// double alpha_21 = M_PI + alpha_2; // 到着点における出発点の方位角
				double n_0 = epsilon * Gamma / pow(sqrt(1 + epsilon * Gamma) + 1, 2);
				double A = (1 + n_0) * (1 + 1.25 * pow(n_0, 2));
				double s = (1 - f) * a * A * M_PI; // 測地線長
				// alpha_1を0から2 * M_PIの範囲内に収める
				if(alpha_1 < 0){
					while(alpha_1 < 0){
						alpha_1 += 2 * M_PI;
					}
				}else if(alpha_1 >= 2 * M_PI){
					while(alpha_1 >= 2 * M_PI){
						alpha_1 -= 2 * M_PI;
					}
				}
				TotalStation ret(s, ( alpha_1 *radToDeg ) );           // alpha_1(rad) を度に変換する
				return ret;
			}
		}
	}
	// ここまでで、例外処理が終了し、フラグもつけることができ、sita_0を求められた。
	// sitaを求める
	double sita_n = sita_0;
	double sita_n_plus_1 = sita_0;
	double g = 0;
	double h = 0;
	double sigma = 0;
	double J = 0;
	double K = 0;
	double gamma = 0;
	double Gamma = 0;
	double zeta = 0;
	double zeta_prime = 0;
	double D = 0;
	double E = 0;
	double F = 1;
	double G = 0;
	if(flag_zone == 1){
		while(fabs(F) >= pow(10, -15)){
			g = sqrt(pow(ita, 2) * pow(cos(sita_n / 2.0), 2) + pow(xi, 2) * pow(sin(sita_n / 2.0), 2));
			h = sqrt(pow(ita_prime, 2) * pow(cos(sita_n / 2.0), 2) + pow(xi_prime, 2) * pow(sin(sita_n / 2.0), 2));
			sigma = 2 * atan(g / h);
			J = 2 * g * h;
			K = pow(h, 2) - pow(g, 2);
			gamma = y * sin(sita_n) / J;
			Gamma = 1 - pow(gamma, 2);
			zeta = Gamma * K - 2 * x;
			zeta_prime = zeta + x;
			D = 0.25 * f * (1 + f) - 3.0 / 16.0 * pow(f, 2) * Gamma;
			E = (1 - D * Gamma) * f * gamma * (sigma + D * J * (zeta + D * K * (2 * pow(zeta, 2) - pow(Gamma, 2))));
			F = sita_n - L - E;
			G = f * pow(gamma, 2) * (1 - 2 * D * Gamma) + f * zeta_prime * sigma / J * (1 - D * Gamma + 0.5 * f * pow(gamma, 2)) + 0.25 * pow(f, 2) * zeta * zeta_prime;
			sita_n_plus_1 = sita_n - F / (1 - G);
			sita_n = sita_n_plus_1;
		}
	}else{
		while(fabs(F) >= pow(10, -15)){
			g = sqrt(pow(ita, 2) * pow(sin(sita_n / 2.0), 2) + pow(xi, 2) * pow(cos(sita_n / 2.0), 2));
			h = sqrt(pow(ita_prime, 2) * pow(sin(sita_n / 2.0), 2) + pow(xi_prime, 2) * pow(cos(sita_n / 2.0), 2));
			sigma = 2 * atan(g / h);
			J = 2 * g * h;
			K = pow(h, 2) - pow(g, 2);
			gamma = y * sin(sita_n) / J;
			Gamma = 1 - pow(gamma, 2);
			zeta = Gamma * K - 2 * x;
			zeta_prime = zeta + x;
			D = 0.25 * f * (1 + f) - 3.0 / 16.0 * pow(f, 2) * Gamma;
			E = (1 - D * Gamma) * f * gamma * (sigma + D * J * (zeta + D * K * (2 * pow(zeta, 2) - pow(Gamma, 2))));
			F = sita_n - L_prime - E;
			G = f * pow(gamma, 2) * (1 - 2 * D * Gamma) + f * zeta_prime * sigma / J * (1 - D * Gamma + 0.5 * f * pow(gamma, 2)) + 0.25 * pow(f, 2) * zeta * zeta_prime;
			sita_n_plus_1 = sita_n - F / (1 - G);
			sita_n = sita_n_plus_1;
		}
	}
	double sita = sita_n;
	// sitaが求まった
	double alpha = 0;
	double alpha_Delta_2 = 0;
	if(flag_zone == 1){
		alpha = atan(xi * tan(sita / 2.0) / ita);
		alpha_Delta_2 = atan(xi_prime * tan(sita / 2.0) / ita_prime);
	}else{
		alpha = atan(ita_prime * tan(sita / 2.0) / xi_prime);
		alpha_Delta_2 = atan(ita * tan(sita / 2.0) / xi);
	}
	double alpha_prime = 0;
	if(alpha < 0 && L > 0){
		alpha_prime = alpha + M_PI;
	}else{
		alpha_prime = alpha;
	}
	if((alpha == 0 && L == 0 && Delta > 0) || (alpha == 0 && fabs(L) == M_PI && sigma > 0)){
		alpha_prime = 0;
	}else if((alpha == 0 && L == 0 && Delta < 0) || (alpha == 0 && fabs(L) == M_PI && sigma < 0)){
		alpha_prime = M_PI;
	}
	double alpha_1_prime = alpha_prime - alpha_Delta_2;
	double alpha_2 = 0;
	if(flag_zone == 1){
		alpha_2 = alpha_prime + alpha_Delta_2;
	}else{
		alpha_2 = M_PI - alpha_prime - alpha_Delta_2;
	}
	double alpha_21_prime = M_PI + alpha_2;
	double alpha_1 = 0;
	if(l_prime >= 0){
		alpha_1 = alpha_1_prime; // 出発点における到着点の方位角(これが求める方位角(rad))
	}else{
		alpha_1 = alpha_21_prime; // 出発点における到着点の方位角(これが求める方位角(rad))
	}
	// double alpha_21 = 0;
	// if(l_prime >= 0){
	//   alpha_21 = alpha_21_prime; // 到着点における出発点の方位角
	// }else{
	//   alpha_21 = alpha_1_prime; // 到着点における出発点の方位角
	// }

	double n_0 = epsilon * Gamma / pow(sqrt(1 + epsilon * Gamma) + 1, 2);
	double A = (1 + n_0) * (1 + 1.25 * pow(n_0, 2));
	double B = epsilon * (1 - 0.375 * pow(n_0, 2)) / pow(sqrt(1 + epsilon * Gamma) + 1, 2);
	double s = (1 - f) * a * A * (sigma - B * J * (zeta - 0.25 * B * (K * (pow(Gamma, 2) - 2 * pow(zeta, 2)) - 1.0 / 6.0 * B * zeta * (1 - 4 * pow(K, 2)) * (3 * pow(Gamma, 2) - 4 * pow(zeta, 2)))));

	// alpha_1を0から2 * M_PIの範囲内に収める
	if(alpha_1 < 0){
		while(alpha_1 < 0){
			alpha_1 += 2 * M_PI;
		}
	}else if(alpha_1 >= 2 * M_PI){
		while(alpha_1 >= 2 * M_PI){
			alpha_1 -= 2 * M_PI;
		}
	}
	TotalStation ret(s, ( alpha_1 * radToDeg ) );           // alpha_1(rad) を度に変換する
	return ret;
}
