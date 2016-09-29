/**
 * coordinate.hpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 *
 */

#ifndef __HIRO_COORDINATE__
#define __HIRO_COORDINATE__

#include "GeographicCoordinate.hpp" // 地理座標のクラス
#include "CartesianCoordinate.hpp"	// デカルト座標のクラス
#include "Degree.hpp"				// 角度のクラス
#include "TotalStation.hpp"			// 測地線長と方位角のクラス

extern double convertDegreeIntoDouble( const hiro::Degree &deg ); // 60進数の角度を10進数に変換する
extern hiro::Degree convertDoubleIntoDegree( const double &deg10 ); // 10進数を60進数の角度に変換する
// 参考URL : http://vldb.gsi.go.jp/sokuchi/surveycalc/surveycalc/algorithm/bl2xy/bl2xy.htm
// 結果確認URL : http://vldb.gsi.go.jp/sokuchi/surveycalc/surveycalc/bl2xyf.html
// 6系(36, 136)を基準にチェックした
extern hiro::CartesianCoordinate convertGCoorIntoCCoor( const hiro::GeographicCoordinate &gCoor, const hiro::GeographicCoordinate &gCoorCri ); // 地理座標系をデカルト座標系に変換する
// 参考URL : http://vldb.gsi.go.jp/sokuchi/surveycalc/surveycalc/algorithm/xy2bl/xy2bl.htm
// 結果確認URL : http://vldb.gsi.go.jp/sokuchi/surveycalc/surveycalc/xy2blf.html
// 6系(36, 136)を基準にチェックした
extern hiro::GeographicCoordinate convertCCoorIntoGCoor( const hiro::CartesianCoordinate &cCoor, const hiro::GeographicCoordinate &gCoorCri ); // デカルト座標系を地理座標系に変換する
// 参考URL : http://vldb.gsi.go.jp/sokuchi/surveycalc/surveycalc/algorithm/bl2st/bl2st.html
// 結果確認URL : http://vldb.gsi.go.jp/sokuchi/surveycalc/surveycalc/bl2stf.html
extern hiro::TotalStation calculateTStationFromGCoor( const hiro::GeographicCoordinate &gCoorStart, const hiro::GeographicCoordinate &gCoorGoal ); // 地理座標系の二点を入力すると測地線長と方位角を返す
// 参考URL : http://vldb.gsi.go.jp/sokuchi/surveycalc/surveycalc/algorithm/xy2st/xy2st.htm
// 結果確認URL : http://vldb.gsi.go.jp/sokuchi/surveycalc/surveycalc/xy2stf.html
// 6系(36, 136)を基準にチェックした
extern hiro::TotalStation calculateTStationFromCCoor( const hiro::CartesianCoordinate &cCoorStart, const hiro::CartesianCoordinate &cCoorGoal, const hiro::GeographicCoordinate &gCoorCri ); // デカルト座標系の二点と地理座標系で与えられた基準点を入力すると測地線長と方位角を返す

extern int calculateRowFromLatitudes( const double phi, const double northPhi, const double southPhi, const double cellSizePhi ); // 緯度を対応する行に変換する。ただし、営業領域外だと0を返す
extern int calculateColFromLongitudes( const double lambda, const double westLambda, const double eastLambda, const double cellSizeLambda ); // 経度を対応する列に変換する。ただし、営業領域外だと0を返す
extern int calculateRowFromIndex( const int index, const int numCol, const int numCell );
extern int calculateColFromIndex( const int index, const int numCol, const int numCell );
extern int calculateIndexFromRowCol( const int row, const int col, const int numRow, const int numCol );

#endif // __HIRO_COORDINATE__
