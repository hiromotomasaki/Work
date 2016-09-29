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

using namespace hiro;


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
CartesianCoordinate convertGCoorIntoCCoor( const GeographicCoordinate &gCoor, const GeographicCoordinate &gCoorCri ) {
  double phi = gCoor.getPhi() * M_PI / 180.0; // ラジアンに単位を変換した、これから変形する地理座標系の緯度
  double lambda = gCoor.getLambda() * M_PI / 180.0; // ラジアンに単位を変換した、これから変形する地理座標系の経度
  double phiCri = gCoorCri.getPhi() * M_PI / 180.0; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の緯度
  double lambdaCri = gCoorCri.getLambda() * M_PI / 180.0; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の経度
  double a = 6378137;         // 楕円体の長半径
  double F = 298.257222101;   // 楕円体の逆扁平率
  double m_0 = 0.9999;        // 平面直角座標系のX軸上における縮尺係数
  double n = 1.0 / ( 2 * F - 1 );
  //--------------------------------//
  double vec_n[6];              // vec_n[i] == n^i (1, n, n^2, n^3, n^4, n^5)
  vec_n[0] = 1;                 // 1
  vec_n[1] = vec_n[0] * n;      // n
  vec_n[2] = vec_n[1] * n;      // n^2
  vec_n[3] = vec_n[2] * n;      // n^3
  vec_n[4] = vec_n[3] * n;      // n^4
  vec_n[5] = vec_n[4] * n;      // n^5

  double vec_alpha[5];          // [0] <-> \alpha_1, ..., [4] <-> \alpha_5
  vec_alpha[0] = 0.5 * vec_n[1] - 2.0 / 3.0 * vec_n[2] + 0.3125 * vec_n[3] + 41.0 / 180.0 * vec_n[4] - 127.0 / 288.0 * vec_n[5]; // \alpha_1
  vec_alpha[1] = 13.0 / 48.0 * vec_n[2] - 0.6 * vec_n[3] + 557.0 / 1440.0 * vec_n[4] + 281.0 / 630.0 * vec_n[5]; // alpha_2
  vec_alpha[2] = 61.0 / 240.0 * vec_n[3] - 103.0 / 140.0 * vec_n[4] + 15061.0 / 26880.0 * vec_n[5]; // alpha_3
  vec_alpha[3] = 49561.0 / 161280.0 * vec_n[4] - 179.0 / 168.0 * vec_n[5]; // alpha_4
  vec_alpha[4] = 34729.0 / 80640.0 * vec_n[5]; // alpha_5

  double vec_A[6];              // vec_A[i] == A_i (A_0, ..., A_5)
  vec_A[0] = vec_n[0] + 0.25 * vec_n[2] + 0.015625 * vec_n[4]; // A_0
  vec_A[1] = - 1.5 * ( vec_n[1] - 0.125 * vec_n[3] - 0.015625 * vec_n[5] ); // A_1
  vec_A[2] = 0.9375 * ( vec_n[2] - 0.25 * vec_n[4] ); // A_2
  vec_A[3] = - 35.0 / 48.0 * ( vec_n[3] - 0.3125 * vec_n[5] ); // A_3
  vec_A[4] = 315.0 / 512.0 * vec_n[4]; // A_4
  vec_A[5] = - 693.0 / 1280.0 * vec_n[5]; // A_5
  //--------------------------------//
  double n_plus_1 = n + 1;
  double A_prime = m_0 * a / n_plus_1;
  double A_bar = A_prime * vec_A[0];

  double s = 0;              // 内積を格納する変数
  for( int j = 1; j <= 5; j++ ) {
    s += vec_A[j] * sin( 2 * j * phiCri );
  }
  double S_bar = A_bar * phiCri + A_prime * s;
  s = 0;

  double sin_phi = sin( phi );
  double sqrt_n_times_2 = 2 * sqrt( n );
  double diff_lambda = lambda - lambdaCri;
  double sqrt_n_times_2_division_n_plus_1 = sqrt_n_times_2 / n_plus_1;
  double t = sinh( atanh( sin_phi ) - sqrt_n_times_2_division_n_plus_1 * atanh( sqrt_n_times_2_division_n_plus_1 * sin_phi ) );
  double t_bar = sqrt( 1 + pow( t, 2 ) );
  double lambda_c = cos( diff_lambda );
  double lambda_s = sin( diff_lambda );
  double xi_prime = atan( t / lambda_c );
  double ita_prime = atanh( lambda_s / t_bar );

  s = 0;
  for( int j = 1; j <= 5; j++ ) {
    s += vec_alpha[j-1] * sin( 2 * j * xi_prime ) * cosh( 2 * j * ita_prime );
  }
  double x = A_bar * ( xi_prime + s ) - S_bar;

  s = 0;
  for( int j = 1; j <= 5; j++ ) {
    s += vec_alpha[j-1] * cos( 2 * j * xi_prime ) * sinh( 2 * j * ita_prime );
  }
  double y = A_bar * ( ita_prime + s );

  CartesianCoordinate ret( x, y );  // 返り値
  return ret;
}
GeographicCoordinate convertCCoorIntoGCoor( const CartesianCoordinate &cCoor, const GeographicCoordinate &gCoorCri ) {
  double x = cCoor.getX();   // これから変形するデカルト座標系のx座標
  double y = cCoor.getY();   // これから変形するデカルト座標系のy座標
  double phiCri = gCoorCri.getPhi() * M_PI / 180.0; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の緯度
  double lambdaCri = gCoorCri.getLambda() * M_PI / 180.0; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の経度
  
  double a = 6378137;         // 楕円体の長半径
  double F = 298.257222101;   // 楕円体の逆扁平率
  double m_0 = 0.9999;        // 平面直角座標系のX軸上における縮尺係数
  double n = 1.0 / ( 2 * F - 1 );

  double vec_n[7];              // vec_n[i] == n^i (1, n, n^2, n^3, n^4, n^5, n^6)
  vec_n[0] = 1;                 // 1
  vec_n[1] = vec_n[0] * n;      // n
  vec_n[2] = vec_n[1] * n;      // n^2
  vec_n[3] = vec_n[2] * n;      // n^3
  vec_n[4] = vec_n[3] * n;      // n^4
  vec_n[5] = vec_n[4] * n;      // n^5
  vec_n[6] = vec_n[5] * n;      // n^6

  double vec_beta[5];          // [0] <-> \beta_1, ..., [4] <-> \beta_5
  vec_beta[0] = 0.5 * vec_n[1] - 2.0 / 3.0 * vec_n[2] + 37.0 / 96.0 * vec_n[3] - 1.0 / 360.0 * vec_n[4] - 81.0 / 512.0 * vec_n[5]; // \beta_1

  vec_beta[1] = 1.0 / 48.0 * vec_n[2] + 1.0 / 15.0 * vec_n[3] - 437.0 / 1440.0 * vec_n[4] + 46.0 / 105.0 * vec_n[5]; // beta_2
  vec_beta[2] = 17.0 / 480.0 * vec_n[3] - 37.0 / 840.0 * vec_n[4] - 209.0 / 4480.0 * vec_n[5]; // beta_3
  vec_beta[3] = 4397.0 / 161280.0 * vec_n[4] - 11.0 / 504.0 * vec_n[5]; // beta_4
  vec_beta[4] = 4583.0 / 161280.0 * vec_n[5]; // beta_5

  double vec_delta[6];          // [0] <-> \delta_1, ..., [5] <-> \delta_6
  vec_delta[0] = 2.0 * vec_n[1] - 2.0 / 3.0 * vec_n[2] - 2.0 * vec_n[3] + 116.0 / 45.0 * vec_n[4] + 26.0 / 45.0 * vec_n[5] - 2854.0 / 675.0 * vec_n[6]; // \delta_1
  vec_delta[1] = 7.0 / 3.0 * vec_n[2] - 1.6 * vec_n[3] - 227.0 / 45.0 * vec_n[4] + 2704.0 / 315.0 * vec_n[5] - 2323.0 / 945.0 * vec_n[6]; // \delta_2
  vec_delta[2] = 56.0 / 15.0 * vec_n[3] - 136.0 / 35.0 * vec_n[4] - 1262.0 / 105.0 * vec_n[5] + 73814.0 / 2835.0 * vec_n[6]; // \delta_3
  vec_delta[3] = 4279.0 / 630.0 * vec_n[4] - 332.0 / 35.0 * vec_n[5] - 399572.0 / 14175.0 * vec_n[6]; // \delta_4
  vec_delta[4] = 4174.0 / 315.0 * vec_n[5] - 144838.0 / 6237.0 * vec_n[6]; // \delta_5
  vec_delta[5] = 601676.0 / 22275.0 * vec_n[6]; // \delta_6

  double vec_A[6];              // vec_A[i] == A_i (A_0, ..., A_5)
  vec_A[0] = vec_n[0] + 0.25 * vec_n[2] + 0.015625 * vec_n[4]; // A_0
  vec_A[1] = - 1.5 * ( vec_n[1] - 0.125 * vec_n[3] - 0.015625 * vec_n[5] ); // A_1
  vec_A[2] = 0.9375 * ( vec_n[2] - 0.25 * vec_n[4] ); // A_2
  vec_A[3] = - 35.0 / 48.0 * ( vec_n[3] - 0.3125 * vec_n[5] ); // A_3
  vec_A[4] = 315.0 / 512.0 * vec_n[4]; // A_4
  vec_A[5] = - 693.0 / 1280.0 * vec_n[5]; // A_5

  double A_prime = m_0 * a / ( n + 1.0 );
  double A_bar = A_prime * vec_A[0];

  double s = 0;              // 内積を格納する変数
  for( int j = 1; j <= 5; j++ ){
    s += vec_A[j] * sin( 2 * j * phiCri );
  }
  double S_bar = A_bar * phiCri + A_prime * s;
  s = 0;

  double xi, ita, xi_prime, ita_prime;

  xi = ( x + S_bar ) / A_bar;
  ita = y / A_bar;

  s = 0;
  for( int j = 1; j <= 5; j++ ) {
    s += vec_beta[j-1] * sin(2 * j * xi) * cosh(2 * j * ita);
  }
  xi_prime = xi - s;
  s = 0;

  s = 0;
  for( int j = 1; j <= 5; j++ ) {
    s += vec_beta[j-1] * cos( 2 * j * xi ) * sinh( 2 * j * ita );
  }
  ita_prime = ita - s;
  s = 0;

  double kai = asin( sin( xi_prime ) / cosh( ita_prime ) );

  // ラジアン形式で求めてら度へ変換する
  double phi, lambda;
  s = 0;
  for( int j = 1; j <= 6; j++ ) {
    s += vec_delta[j-1] * sin( 2 * j * kai );
  }
  phi = ( kai + s ) * 180.0 / M_PI;

  lambda = ( lambdaCri + atan( sinh( ita_prime ) / cos( xi_prime ) ) ) * 180.0 / M_PI;

  GeographicCoordinate ret( phi, lambda );  // 返り値
  return ret;
}
TotalStation calculateTStationFromGCoor( const GeographicCoordinate &gCoorStart, const GeographicCoordinate &gCoorGoal ) {
  if( gCoorStart.getPhi() == gCoorGoal.getPhi() && gCoorStart.getLambda() == gCoorGoal.getLambda() ) {
    // ここの部分は国土地理院の計算式と違う
    // 同じ時に除算で0でわる操作があるため測地線長も方位角も-nanになってしまう。
    // ゾーンは1でsita_0は0でitaは0でxiは0.82...であるので,gが0hが1になる。
    // J=2ghでgamma=y*sin(sita_n)/Jのためgammaがnanになる（yは正）.しかし-nanとなっていた.
    // そこで、例外処理をすることにした。
    TotalStation ret(0, 0 );
    return ret;
  }
  double phi_1 = gCoorStart.getPhi() * M_PI / 180.0; // ラジアンに単位を変換した、出発点の地理座標系の緯度
  double L_1 = gCoorStart.getLambda() * M_PI / 180.0; // ラジアンに単位を変換した、出発点の地理座標系の経度
  double phi_2 = gCoorGoal.getPhi() * M_PI / 180.0; // ラジアンに単位を変換した、到着点の地理座標系の緯度
  double L_2 = gCoorGoal.getLambda() * M_PI / 180.0; // ラジアンに単位を変換した、到着点の地理座標系の経度


  double a = 6378137;             // 楕円体の長半径
  double f = 1.0 / 298.257222101; // 楕円体の扁平率
  double l = L_2 - L_1;
  double l_prime;
  if(l > M_PI){
    l_prime = l - 2 * M_PI;
  }else if(l < - M_PI){
    l_prime = l + 2 * M_PI;
  }else{
    l_prime = l;
  }
  double L = fabs(l_prime);
  double L_prime = M_PI - L;
  double Delta;
  if(l_prime >= 0){
    Delta = phi_2 - phi_1;
  }else{
    Delta = phi_1 - phi_2;
  }
  double Sigma = phi_1 + phi_2;
  double u_1, u_2;
  if(l_prime >= 0){
    u_1 = atan((1 - f) * tan(phi_1));
  }else{
    u_1 = atan((1 - f) * tan(phi_2));
  }
  if(l_prime >= 0){
    u_2 = atan((1 - f) * tan(phi_2));
  }else{
    u_2 = atan((1 - f) * tan(phi_1));
  }
  double Sigma_prime = u_1 + u_2;
  double Delta_prime = u_2 - u_1;
  double xi = cos(Sigma_prime / 2.0);
  double xi_prime = sin(Sigma_prime / 2.0);
  double ita = sin(Delta_prime / 2.0);
  double ita_prime = cos(Delta_prime / 2.0);
  double x = sin(u_1) * sin(u_2);
  double y = cos(u_1) * cos(u_2);
  double c = y * cos(L) + x;
  double epsilon = f * (2 - f) / (pow((1 - f), 2));

  // ゾーンの判定. ゾーンは1, 2, 3の3種類
  int flag_zone = 0;
  double sita_0;
  if(c >= 0){
    flag_zone = 1;
    sita_0 = L * (1 + f * y);
  }else if(c < 0 && c >= - cos(3.0 / 180.0 * M_PI * cos(u_1))){
    flag_zone = 2;
    sita_0 = L_prime;
  }else{
    flag_zone = 3;
    // sita_0は次のところで決める
  }
  // flag_zone == 3 の場合、ある条件で例外処理がある
  // int flag_3a = 0, flag_3b1 = 0, flag_3b2 = 0, flag_3b3 = 0;
  if(flag_zone == 3){
    double R = f * M_PI * pow(cos(u_1), 2) * (1 - 0.25 * f * (1 + f) * pow(sin(u_1), 2) + 3.0 / 16.0 * pow(f, 2) * pow(sin(u_1), 4));
    double d_1 = L_prime * cos(u_1) - R;
    double d_2 = fabs(Sigma_prime) + R;
    double q = L_prime / (f * M_PI);
    double f_1 = 0.25 * f * (1+ 0.5 * f);
    double gamma_0 = q + f_1 * q - f_1 * pow(q, 3);
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
        TotalStation ret(s, ( alpha_1 / M_PI * 180.0 ) );           // alpha_1(rad) を度に変換する
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
  TotalStation ret(s, ( alpha_1 / M_PI * 180.0 ) );           // alpha_1(rad) を度に変換する
  return ret;
}
TotalStation calculateTStationFromCCoor( const CartesianCoordinate &cCoorStart, const CartesianCoordinate &cCoorGoal, const GeographicCoordinate &gCoorCri ) {
  double phi_0 = gCoorCri.getPhi() * M_PI / 180.0; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の緯度
  // double lambda_0 = gCoorCri.getLambda() * M_PI / 180.0; // ラジアンに単位を変換した、デカルト座標系の原点に対応する地理座標系の経度
  double y_1 = cCoorStart.getY(); // 出発点のデカルト座標系のy座標
  double x_1 = cCoorStart.getX(); // 出発点のデカルト座標系のx座標
  double y_2 = cCoorGoal.getY(); // 到着点のデカルト座標系のy座標
  double x_2 = cCoorGoal.getX(); // 到着点のデカルト座標系のx座標
  double m_0 = 0.9999;
  double a = 6378137;
  double F = 298.257222101;
  double f = 1.0 / F;
  double e = sqrt(f * (2 - f));
  double R_0 = a * sqrt(1 - pow(e, 2)) / (1 - pow(e, 2) * pow(sin(phi_0), 2));
  double S_over_s = m_0 * (1 + (pow(y_1, 2) + y_1 * y_2 + pow(y_2, 2)) / (6 * pow(R_0, 2) * pow(m_0, 2)));
  double S = sqrt(pow(x_2 - x_1, 2) + pow(y_2 - y_1, 2)) / S_over_s;
  double t_1 = atan((y_2 - y_1) / (x_2 - x_1));
  if(t_1 < 0){
    t_1 += M_PI;
  }
  double t_1_muinus_T_1 = -((y_2 + y_1) * (x_2 - x_1)) / (4 * pow(m_0, 2) * pow(R_0, 2)) + ((x_2 - x_1) * (y_2 - y_1)) / (12 * pow(m_0, 2) * pow(R_0, 2));
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
  TotalStation ret( S, ( T_1 / M_PI * 180.0 ) );              // 返り値, alpha_1(rad) を度に変換する
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
