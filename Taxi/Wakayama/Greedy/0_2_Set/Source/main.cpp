/**
 * main.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "main.hpp"

using namespace hiro;

// 【この関数の目的】
// Greedy関係の設定値のxmlファイルを基に1_Cron, 2_ForEachRequestで使用する値を事前に作成する.

// 辞書式ソートができるように構造体を宣言し、ソートの規則をクラスで書く
struct PairIndexTSTation {
    int index;
    TotalStation tStation;
};
class MyLessDefinition {
public:
    bool operator()(const PairIndexTSTation& a, const PairIndexTSTation& b)
    {
		if( a.tStation.getLength() == b.tStation.getLength() ) {
			if( a.tStation.getAzimuth() == b.tStation.getAzimuth() ) {
				return a.index < b.index;
			}else{
				return a.tStation.getAzimuth() < b.tStation.getAzimuth();
			}
		}else{
			return a.tStation.getLength() < b.tStation.getLength();
		}
		return a.tStation.getLength() < b.tStation.getLength();
    }
};

// 補間関数
double myHokann(const double x, const double minX, const double maxX, const double yOfMinX, const double yOfMaxX);
double myHokann(const double x, const double minX, const double maxX, const double yOfMinX, const double yOfMaxX) {
	double alpha = (x - minX) / (double)(maxX - minX);
	double ret = (1-alpha) * yOfMinX + alpha * yOfMaxX;
	return ret;
}

// オブジェクトの底辺の端点を計算する関数
int myCalculateVertex(GeographicCoordinate &gCoor_1, GeographicCoordinate &gCoor_2, const GeographicCoordinate &gCoorTarget, const double squareLength, const double deg_10);
int myCalculateVertex(GeographicCoordinate &gCoor_1, GeographicCoordinate &gCoor_2, const GeographicCoordinate &gCoorTarget, const double squareLength, const double deg_10) {
	// オブジェクトの底辺のX軸(北)からの時計回りのずれ
	double theta = deg_10 + 90;
	if (theta >= 360) {
		theta -= 360;
	}
	// この時点で 0° <= theta < 360°
	// 図形の対称性を用いて計算する(thataとthata+180°が表す底辺は同じ)
	if (theta >= 180) {
		theta -= 180;
	}
	// この時点で 0° <= thea < 180°
	double l = squareLength / 2.0;
	// 北[m]
	double x = 0;
	// 東[m]
	double y = 0;
	{
		if ( (theta >= 0) && (theta < 45) ) {
			x = l;
			y = l * tan( ( theta ) * M_PI / 180.0 );
		}else if ( (theta >= 45) && (theta < 90) ) {
			x = l * tan( ( 90 - theta ) * M_PI / 180.0 );
			y = l;
		}else if ( (theta >= 90) && (theta < 135) ) {
			x = - l * tan( ( theta - 90 ) * M_PI / 180.0 );
			y = l;
		}else if ( (theta >= 135) && (theta < 180) ) {
			x = - l;
			y = l * tan( ( 180 - theta ) * M_PI / 180.0 );
		}
	}
	CartesianCoordinate cCoor_1(x, y);
	CartesianCoordinate cCoor_2(-x, -y);
	// 変換
	gCoor_1 = convertCCoorIntoGCoor( cCoor_1, gCoorTarget );
	gCoor_2 = convertCCoorIntoGCoor( cCoor_2, gCoorTarget );
	return EXIT_SUCCESS;
}

// 扇型の探索範囲内に入る角度のものか確認する関数
bool myCheckDegreeRange(const double theta, const double thetaCri, const double Theta);
bool myCheckDegreeRange(const double theta, const double thetaCri, const double Theta) {
	// theta : 範囲内かどうか判定する角度(0 <= theta < 360)
	// thetaCri : dirの方向角度．そこを中心に扇型が展開される．
	// Theta : 扇型の中心角度
	bool ret = false;
	if(Theta >= 360) {
		ret = true;
	}else{
		// 0 < Theta < 360
		double theta_s = thetaCri - Theta / 2.0;
		double theta_g = thetaCri + Theta / 2.0;
		// 区間が一つの場合
		// 0 <= theta_s < theta_g < 360
		// 区間が二つの場合
		// theta_s < 0
		// theta_g > 360
		if (theta_s < 0) {
			// theta \in [theta_s1,theta_g1)かチェック
			double theta_s1 = 360 - theta_s;
			double theta_g1 = 360;
			// theta \in [theta_s2,theta_g2]かチェック
			double theta_s2 = 0;
			double theta_g2 = theta_g;
			bool condition = ((theta >= theta_s1) && (theta < theta_g1)) || ((theta >= theta_s2) && (theta <= theta_g2));
			if (condition) {
				ret = true;
			}
		}else if (theta_g > 360) {
			// theta \in [theta_s1,theta_g1)かチェック
			double theta_s1 = theta_s;
			double theta_g1 = 360;
			// theta \in [theta_s2,theta_g2]かチェック
			double theta_s2 = 0;
			double theta_g2 = 360 - theta_g;
			bool condition = ((theta >= theta_s1) && (theta < theta_g1)) || ((theta >= theta_s2) && (theta <= theta_g2));
			if (condition) {
				ret = true;
			}
		}else{
			bool condition = ((theta >= theta_s) && (theta <= theta_g));
			if (condition) {
				ret = true;
			}
		}
	}
	return ret;
}

int main()
{
	// ------------- fileNameから取得するもの  ------------- //
	// 営業領域の外枠の最北西
	GeographicCoordinate gCoorNW;
	// 営業領域の外枠の最南東
	GeographicCoordinate gCoorSE;
	// 追加する営業領域の最北西
	std::vector<GeographicCoordinate> gCoorAddNW;
	// 追加する営業領域の最南東
	std::vector<GeographicCoordinate> gCoorAddSE;
	// 追加する営業領域の最北西
	std::vector<GeographicCoordinate> gCoorRemoveNW;
	// 追加する営業領域の最南東
	std::vector<GeographicCoordinate> gCoorRemoveSE;
	// セルの一辺の長さ[m]
	double cellSizeMeter;
	// 表示するピンの日にちに関する抽出条件
	std::vector<int> displayDate;
	// 表示するピンの時間幅の始点は現在時刻から何分前なのか
	int displayTimeFrom;
	// 表示するピンの時間幅の始点は現在時刻から何分後なのか
	int displayTimeTo;
	// セルから描画するオブジェクトの有効範囲の円の半径[m]
	double searchRange;
	// タクシーの進行方向を考慮した時に描画するオブジェクトの有効範囲の扇型（円に含まれる）の中心角度[deg](45度から180度まで)
	double searchDegree;
	// オブジェクト生成の基準となるboxの一辺の長さの最小値
	double squareLengthMin;
	// オブジェクト生成の基準となるboxの一辺の長さの最大値
	double squareLengthMax;
	// タクシーの進行方向を計算するときに使用する二点間の座標間の距離の最大値．これを越すとオブジェクトは円の中から探索して選ばれる(dir1に相当)．越さなければ進行方向(dir2からdir9)の扇型の中から探索して選ばれる
	double maxDistanceFromPreposition;
	// 需要が集中していると判定するためのしきい値．このしきい値以上であれば需要が集中しているとみなす．
	double threshold;
	// 離散時間幅
	double discreteTimeWidth;
	// ----------------------------------------------------- //
	// ----------------------- その他 ---------------------- //
	// 設定値の確認表示をするかどうか
	bool display = true;
	// bool display = false;
	// ----------------------------------------------------- //

	// 処理
	{
		// 0_1_Presetで設定したファイルの読込
		{
			std::cout << "0_1_Presetで設定したファイルの読込中..." << "\n";
			// 設定値読み込みファイル名
			const std::string fileName = "base.xml";
			// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/0_1_Preset/Other";
			std::string fileRela = fileDire + "/" + fileName;
			{
				// create an empty property tree
				boost::property_tree::ptree pt;
				// read the xml file
				boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
				{
					gCoorNW.setPhi( pt.get<double>( "table.gCoorNW.phi" ) );
					gCoorNW.setLambda( pt.get<double>( "table.gCoorNW.lambda" ) );
				}
				{
					gCoorSE.setPhi( pt.get<double>( "table.gCoorSE.phi" ) );
					gCoorSE.setLambda( pt.get<double>( "table.gCoorSE.lambda" ) );
				}
				{
					// 数を調べる
					int N = 0;
					{
						boost::property_tree::ptree::iterator itr_first, itr_last, it;
						itr_first = pt.get_child( "table.gCoorAdd" ).begin();
						itr_last = pt.get_child( "table.gCoorAdd" ).end();
						N = std::distance(itr_first, itr_last);
					}
					// 要素の追加
					gCoorAddNW.reserve(N);
					gCoorAddSE.reserve(N);
					{
						boost::property_tree::ptree::iterator itr_first, itr_last, it;
						itr_first = pt.get_child( "table.gCoorAdd" ).begin();
						itr_last = pt.get_child( "table.gCoorAdd" ).end();
						for(it = itr_first; it != itr_last; ++it) {
							gCoorAddNW.push_back(GeographicCoordinate(it->second.get<double>("phiNW"), it->second.get<double>("lambdaNW")));
							gCoorAddSE.push_back(GeographicCoordinate(it->second.get<double>("phiSE"), it->second.get<double>("lambdaSE")));
						}
					}
				}
				{
					// 数を調べる
					int N = 0;
					{
						boost::property_tree::ptree::iterator itr_first, itr_last, it;
						itr_first = pt.get_child( "table.gCoorRemove" ).begin();
						itr_last = pt.get_child( "table.gCoorRemove" ).end();
						N = std::distance(itr_first, itr_last);
					}
					// 要素の追加
					gCoorRemoveNW.reserve(N);
					gCoorRemoveSE.reserve(N);
					{
						boost::property_tree::ptree::iterator itr_first, itr_last, it;
						itr_first = pt.get_child( "table.gCoorRemove" ).begin();
						itr_last = pt.get_child( "table.gCoorRemove" ).end();
						for(it = itr_first; it != itr_last; ++it) {
							gCoorRemoveNW.push_back(GeographicCoordinate(it->second.get<double>("phiNW"), it->second.get<double>("lambdaNW")));
							gCoorRemoveSE.push_back(GeographicCoordinate(it->second.get<double>("phiSE"), it->second.get<double>("lambdaSE")));
						}
					}
				}
				{
					cellSizeMeter = pt.get<double>("table.cellSizeMeter.value");
				}
				{
					// 数を調べる
					int N = 0;
					{
						boost::property_tree::ptree::iterator itr_first, itr_last, it;
						itr_first = pt.get_child( "table.displayDate" ).begin();
						itr_last = pt.get_child( "table.displayDate" ).end();
						N = std::distance(itr_first, itr_last);
					}
					// 要素の追加
					displayDate.reserve(N);
					{
						boost::property_tree::ptree::iterator itr_first, itr_last, it;
						itr_first = pt.get_child( "table.displayDate" ).begin();
						itr_last = pt.get_child( "table.displayDate" ).end();
						for(it = itr_first; it != itr_last; ++it) {
							displayDate.push_back(it->second.get<int>("value"));
						}
					}
				}
				{
					displayTimeFrom = pt.get<double>("table.displayTimeFrom.value");
					displayTimeTo = pt.get<double>("table.displayTimeTo.value");
					searchRange = pt.get<double>("table.searchRange.value");
					searchDegree = pt.get<double>("table.searchDegree.value");
					squareLengthMin = pt.get<double>("table.squareLengthMin.value");
					squareLengthMax = pt.get<double>("table.squareLengthMax.value");
					maxDistanceFromPreposition = pt.get<double>("table.maxDistanceFromPreposition.value");
					threshold = pt.get<double>("table.threshold.value");
					discreteTimeWidth = pt.get<double>("table.discreteTimeWidth.value");
				}
			}
		}
		// 基本的な営業領域の情報の作成(内部でgCoorSEに変更あり)
		// 営業領域の外枠の行数
		int numRow;
		// 営業領域の外枠の列数
		int numCol;
		// 営業領域の外枠内にあるセル数
		int numCell;
		// セルの南北の緯度差
		double cellSizePhi;
		// セルの東西の経度差
		double cellSizeLambda;
		{
			std::cout << "基本的な営業領域の情報の作成中..." << "\n";
			// gCoorNWをデカルト座標系の原点にした時のgCoorSEのデカルト座標系での位置を求める。
			CartesianCoordinate cCoorSE = convertGCoorIntoCCoor( gCoorSE, gCoorNW );
			// デカルト座標系で定義された営業領域に対して北から南まで一辺cellSizeMeterの格子を敷き詰めていくと、何枚で営業領域の南北を埋めることができるのか計算する。
			numRow = ceil( fabs( cCoorSE.getX() ) / cellSizeMeter ); // 絶対値の小数切り上げ
			// デカルト座標系で定義された営業領域に対して西から東まで一辺cellSizeMeterの格子を敷き詰めていくと、何枚で営業領域の東西を埋めることができるのか計算する。
			numCol = ceil( fabs( cCoorSE.getY() ) / cellSizeMeter ); // 絶対値の小数切り上げ
			numCell = numRow * numCol;
			// 営業領域の新しい最南東位置に置き換える(デカルト座標)
			cCoorSE.set( (const double)( - numRow * cellSizeMeter ), (const double)( numCol * cellSizeMeter ) );
			// 営業領域の新しい最南東位置に置き換える(地理座標)
			gCoorSE = convertCCoorIntoGCoor( cCoorSE, gCoorNW );
			// 緯度(latitude)方向, 経度(longitude)方向のcellSizeを計算
			cellSizePhi = (gCoorNW.getPhi() - gCoorSE.getPhi()) / (double)numRow;
			cellSizeLambda = (gCoorSE.getLambda() - gCoorNW.getLambda()) / (double)numCol;
		}
		// 各セルの代表点や有効かどうかなどの情報を計算
		std::vector<GeographicCoordinate> vRepresentativePoint(numCell);
		std::vector<int> vIndex(numCell);
		std::vector<int> vRow(numCell);
		std::vector<int> vCol(numCell);
		std::vector<bool> vValid(numCell, false);
		int numValidCell;
		{
			std::cout << "各セルの代表点や有効かどうかなどの情報を計算中..." << "\n";
			// vRepresentativePoint, vIndex, vRow, vColの作成
			{
				std::vector<GeographicCoordinate>::iterator it, itr_first, itr_last;
				itr_first = vRepresentativePoint.begin();
				itr_last = vRepresentativePoint.end();
				for(it = itr_first; it != itr_last; it++) {
					std::size_t i = std::distance(itr_first, it);
					vIndex[i] = i + 1;
					int row = calculateRowFromIndex( vIndex[i], numCol, numCell );
					vRow[i] = row;
					int col = calculateColFromIndex( vIndex[i], numCol, numCell );
					vCol[i] = col;
					vRepresentativePoint[i].setPhi( gCoorNW.getPhi() - row * cellSizePhi + cellSizePhi / 2.0 );
					vRepresentativePoint[i].setLambda( gCoorNW.getLambda() + col * cellSizeLambda - cellSizeLambda / 2.0 );
				}
			}
			// vValid, numValidCellの作成
			{
				// gCoorAddに基づいてvValidを編集
				{
					int N = gCoorAddNW.size();
					for (int i = 0; i < N; i++) {
						// gCoorに対してgCoorAddが重なりを持つか確認
						bool isOver = ( gCoorAddSE[i].getPhi() >= gCoorNW.getPhi() );
						bool isUnder = ( gCoorAddNW[i].getPhi() <= gCoorSE.getPhi() );
						bool isOnTheRight = ( gCoorAddNW[i].getLambda() >= gCoorSE.getLambda() );
						bool isOnTheLeft = ( gCoorAddSE[i].getLambda() <= gCoorNW.getLambda() );
						bool overlap = !( isOver || isUnder || isOnTheRight || isOnTheLeft );
						if (overlap) {
							int RowN = calculateRowFromLatitudes( gCoorAddNW[i].getPhi(), gCoorNW.getPhi(), gCoorSE.getPhi(), cellSizePhi );
							int RowS = calculateRowFromLatitudes( gCoorAddSE[i].getPhi(), gCoorNW.getPhi(), gCoorSE.getPhi(), cellSizePhi );
							int ColE = calculateColFromLongitudes( gCoorAddSE[i].getLambda(), gCoorNW.getLambda(), gCoorSE.getLambda(), cellSizeLambda );
							int ColW = calculateColFromLongitudes( gCoorAddNW[i].getLambda(), gCoorNW.getLambda(), gCoorSE.getLambda(), cellSizeLambda );
							{
								if (RowN == 0) {
									RowN = 1;
								}
								if (RowS == 0) {
									RowS = numRow;
								}
								if (ColE == 0) {
									ColE = numCol;
								}
								if (ColW == 0) {
									ColW = 1;
								}
							}
							// (RowN, RowS, ColE, ColW)の領域のvValidをtrueにする
							for (int j = RowN; j <= RowS; j++) {
								for (int k = ColW; k <= ColE; k++) {
									int indexHoge = calculateIndexFromRowCol( j, k, numRow, numCol );
									vValid[indexHoge - 1] = true;
								}
							}
							// std::cout <<"[Row] " << RowN << ", " << RowS << "[Col] " << ColW << ", " << ColE << "\n";
						}
					}
				}
				// gCoorRemoveに基づいてvValidを編集
				{
					{
						// 配列の1行目はダミー
						int N = gCoorRemoveNW.size();
						if (N >= 2) {
							for (int i = 1; i < N; i++) {
								// gCoorに対してgCoorAddが重なりを持つか確認
								bool isOver = ( gCoorRemoveSE[i].getPhi() >= gCoorNW.getPhi() );
								bool isUnder = ( gCoorRemoveNW[i].getPhi() <= gCoorSE.getPhi() );
								bool isOnTheRight = ( gCoorRemoveNW[i].getLambda() >= gCoorSE.getLambda() );
								bool isOnTheLeft = ( gCoorRemoveSE[i].getLambda() <= gCoorNW.getLambda() );
								bool overlap = !( isOver || isUnder || isOnTheRight || isOnTheLeft );
								if (overlap) {
									int RowN = calculateRowFromLatitudes( gCoorRemoveNW[i].getPhi(), gCoorNW.getPhi(), gCoorSE.getPhi(), cellSizePhi );
									int RowS = calculateRowFromLatitudes( gCoorRemoveSE[i].getPhi(), gCoorNW.getPhi(), gCoorSE.getPhi(), cellSizePhi );
									int ColE = calculateColFromLongitudes( gCoorRemoveSE[i].getLambda(), gCoorNW.getLambda(), gCoorSE.getLambda(), cellSizeLambda );
									int ColW = calculateColFromLongitudes( gCoorRemoveNW[i].getLambda(), gCoorNW.getLambda(), gCoorSE.getLambda(), cellSizeLambda );
									{
										if (RowN == 0) {
											RowN = 1;
										}else{
											RowN++;
										}
										if (RowS == 0) {
											RowS = numRow;
										}else{
											RowS--;
										}
										if (ColE == 0) {
											ColE = numCol;
										}else{
											ColE--;
										}
										if (ColW == 0) {
											ColW = 1;
										}else{
											ColW++;
										}
									}
									bool checkRow = ( RowN >= 1 ) && ( RowN <= numRow ) && ( RowS >= 1 ) && ( RowS <= numRow ) && ( RowN <= RowS );
									bool checkCol = ( ColE >= 1 ) && ( ColE <= numCol ) && ( ColW >= 1 ) && ( ColW <= numCol ) && ( ColW <= ColE );
									bool check = checkRow && checkCol;
									if (check) {
										// (RowN, RowS, ColE, ColW)の領域のvValidをfalseにする
										for (int j = RowN; j <= RowS; j++) {
											for (int k = ColW; k <= ColE; k++) {
												int indexHoge = calculateIndexFromRowCol( j, k, numRow, numCol );
												vValid[indexHoge - 1] = false;
											}
										}
									}
								}
							}
						}
					}
				}
				// vValidのtrueの数を数えてnumValidCellを作成
				{
					numValidCell = 0;
					std::vector<bool>::iterator it, itr_first, itr_last;
					itr_first = vValid.begin();
					itr_last = vValid.end();
					for(it = itr_first; it != itr_last; it++) {
						if (*it) {
							numValidCell++;
						}
					}
				}
			}
		}
		// ここまでの営業領域に関する情報を保存
		{
			std::cout << "ここまでの営業領域に関する情報を保存" << "\n";
			// baseMap.xml
			{
				// 設定値保存ファイル名
				const std::string fileName = "baseMap.xml";
				// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
				const std::string fileDire = "./../Data/0_2_Set/Other";
				std::cout << fileName << "を保存中..." << std::endl;
				// ディレクトリの作成
				{
					boost::filesystem::path path(fileDire);
					boost::system::error_code error;
					const bool result = boost::filesystem::create_directories(path, error);
					if (!result || error) {
						// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
					}
				}
				// 保存
				{
					// 保存path
					std::string fileRela = fileDire + "/" + fileName;
					// create an empty property tree
					boost::property_tree::ptree pt;

					// create the root element
					boost::property_tree::ptree& root = pt.put("table", "");

					// add child elements
					{
						{
							boost::property_tree::ptree& child = root.add("gCoorNW", "");
							child.put("phi", gCoorNW.getPhi());
							child.put("lambda", gCoorNW.getLambda());
						}
						{
							boost::property_tree::ptree& child = root.add("gCoorSE", "");
							child.put("phi", gCoorSE.getPhi());
							child.put("lambda", gCoorSE.getLambda());
						}
						{
							boost::property_tree::ptree& child = root.add("cellSizeMeter", "");
							child.put("value", cellSizeMeter);
						}
						{
							boost::property_tree::ptree& child = root.add("numRow", "");
							child.put("value", numRow);
						}
						{
							boost::property_tree::ptree& child = root.add("numCol", "");
							child.put("value", numCol);
						}
						{
							boost::property_tree::ptree& child = root.add("numCell", "");
							child.put("value", numCell);
						}
						{
							boost::property_tree::ptree& child = root.add("numValidCell", "");
							child.put("value", numValidCell);
						}
						{
							boost::property_tree::ptree& child = root.add("cellSizePhi", "");
							child.put("value", cellSizePhi);
						}
						{
							boost::property_tree::ptree& child = root.add("cellSizeLambda", "");
							child.put("value", cellSizeLambda);
						}
					}

					// output
					boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
				}

			}
			// representativePoints.xml
			{
				// 設定値保存ファイル名
				const std::string fileName = "representativePoints.xml";
				// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
				const std::string fileDire = "./../Data/0_2_Set/Other";
				std::cout << fileName << "を保存中..." << std::endl;
				// ディレクトリの作成
				{
					boost::filesystem::path path(fileDire);
					boost::system::error_code error;
					const bool result = boost::filesystem::create_directories(path, error);
					if (!result || error) {
						// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
					}
				}
				// 保存
				{
					// 保存path
					std::string fileRela = fileDire + "/" + fileName;
					// create an empty property tree
					boost::property_tree::ptree pt;

					// create the root element
					boost::property_tree::ptree& root = pt.put("table", "");

					// add child elements
					{
						for (int i = 0; i < numCell; i++) {
							boost::property_tree::ptree& child = root.add("cell", "");
							child.put("index", vIndex[i]);
							child.put("isValid", vValid[i]);
							child.put("row", vRow[i]);
							child.put("col", vCol[i]);
							child.put("position.phi", vRepresentativePoint[i].getPhi());
							child.put("position.lambda", vRepresentativePoint[i].getLambda());
						}
					}

					// output
					boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
				}
			}
			// IsValid/(i+1).xml
			{
				// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
				const std::string fileDire = "./../Data/0_2_Set/Other/IsValid";
				// ディレクトリの削除 : isValid/
				{
					boost::filesystem::path path(fileDire);
					boost::filesystem::remove_all(path);
				}
				// ディレクトリの作成
				{
					boost::filesystem::path path(fileDire);
					boost::system::error_code error;
					const bool result = boost::filesystem::create_directories(path, error);
					if (!result || error) {
						// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
					}
				}
				std::cout << fileDire << "内のファイルを保存中..." << std::endl;
				// 設定値保存ファイル名
				for (int i = 0; i < numCell; i++) {
					std::string fileName = boost::lexical_cast<std::string>( i+1 ) + ".xml";
					// 保存path
					std::string fileRela = fileDire + "/" + fileName;
					// create an empty property tree
					boost::property_tree::ptree pt;

					// create the root element
					boost::property_tree::ptree& root = pt.put("table", "");

					// add child elements
					{
						{
							boost::property_tree::ptree& child = root.add("isValid", "");
							child.put("value", vValid[i]);
						}
					}
					// output
					boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
				}
			}
		}
		// vValidがtrueのセルに対してのみ, 円状の探索範囲や描画するオブジェクト, 扇型の探索範囲を定めて保存
		// indexTaxiにおける円状の探索範囲の詳細(1_Cron, 2_ForEachRequestでは使わない．ただの保存)
		// : InfoAboutValidCells/(indexTaxi)/searchRange.xml
		// indexTaxiから見てindexTarget(!=indexTaxi)を指すときに使用するオブジェクト情報
		// : InfoAboutValidCells/(indexTaxi)/Object/(indexTarget).xml
		// indexTaxiから見てindexTarget(==indexTaxi)を指すときに使用するオブジェクト情報．ただし，走行方向はdir(k)
		// : InfoAboutValidCells/(indexTaxi)/Object/(indexTarget)/dir(k).xml
		// dir(k)の方向に進んでいるタクシーがindexTaxiにいる場合の探索範囲(dir1は円，dir2からdir9は扇型)
		// : InfoAboutValidCells/(indexTaxi)/SearchRange/dir(k).xml
		{
			std::cout << "InfoAboutValidCells内のファイルを作成していく" << "\n";
			// ディレクトリの削除 : InfoAboutValidCells/
			{
				// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
				const std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells";
				boost::filesystem::path path(fileDire);
				boost::filesystem::remove_all(path);
			}
			// ディレクトリの作成 : InfoAboutValidCells/
			{
				const std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells";
				boost::filesystem::path path(fileDire);
				boost::system::error_code error;
				const bool result = boost::filesystem::create_directories(path, error);
				if (!result || error) {
					// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
				}
			}
			std::cout << "vValidがtrueのセルに対してのみ, 円状の探索範囲や描画するオブジェクト, 扇型の探索範囲を定める" << "\n";
			for (int i = 0; i < numCell; i++) {
				int indexTaxi = i + 1;
				// 進行状況の描画
				if (display) {
					if (vValid[i]) {
						std::cout << "[" << indexTaxi << ", " << numCell << "]" << "\n";
					}else{
						std::cout << "[" << indexTaxi << ", " << numCell << "]" << " -> Passed" << "\n";
					}
				}
				// 有効な各セルでの処理
				if (vValid[i]) {
					// InfoAboutValidCells/(indexTaxi)以下の全てのディレクトリの作成
					{
						{
							std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi );
							boost::filesystem::path path(fileDire);
							boost::system::error_code error;
							const bool result = boost::filesystem::create_directories(path, error);
							if (!result || error) {
								// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
							}
						}
						{
							std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object";
							boost::filesystem::path path(fileDire);
							boost::system::error_code error;
							const bool result = boost::filesystem::create_directories(path, error);
							if (!result || error) {
								// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
							}
						}
						{
							std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object/" + boost::lexical_cast<std::string>( indexTaxi );
							boost::filesystem::path path(fileDire);
							boost::system::error_code error;
							const bool result = boost::filesystem::create_directories(path, error);
							if (!result || error) {
								// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
							}
						}
						{
							std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi ) + "/SearchRange";
							boost::filesystem::path path(fileDire);
							boost::system::error_code error;
							const bool result = boost::filesystem::create_directories(path, error);
							if (!result || error) {
								// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
							}
						}
					}
					// indexTaxiに対するindexTarget(詳細情報付き)の集合を求めて保存
					std::vector<PairIndexTSTation> vPairIndexTSTation;
					{
						{
							for (int j = 0; j < numCell; j++) {
								int indexTarget = j + 1;
								PairIndexTSTation pairHoge;
								// indexTaxiからindexTargetへの測地線長と方位角を取得する
								pairHoge.tStation = calculateTStationFromGCoor( vRepresentativePoint[i], vRepresentativePoint[j] );
								bool isWithinNeighborhood = (pairHoge.tStation.getLength() <= searchRange);
								bool isValidCell = vValid[j];
								bool isValid = isWithinNeighborhood && isValidCell;
								if (isValid) {
									// indexTargetを登録
									pairHoge.index = indexTarget;
									// 有効セルの情報ベクトルへプッシュ
									vPairIndexTSTation.push_back(pairHoge);
								}
							}
						}
						// 第一辞書を測地線長, 第二辞書を方位角, 第三辞書をindexとしてvPairIndexTSTationをソート
						{
							std::vector<PairIndexTSTation>::iterator it, itr_first, itr_last;
							itr_first = vPairIndexTSTation.begin();
							itr_last = vPairIndexTSTation.end();
							sort(itr_first, itr_last, MyLessDefinition());
						}
						// vPairIndexTSTationの保存
						{
							// 保存ファイル名
							const std::string fileName = "searchRange.xml";
							// 保存ファイル先のディレクトリのmakefileからの相対位置
							std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi );
							// 保存path
							std::string fileRela = fileDire + "/" + fileName;
							// create an empty property tree
							boost::property_tree::ptree pt;

							// create the root element
							boost::property_tree::ptree& root = pt.put("table", "");
							// add child elements
							{
								int N = vPairIndexTSTation.size();
								for (int j = 0; j < N; j++) {
									boost::property_tree::ptree& child = root.add("cell", "");
									child.put("index", vPairIndexTSTation[j].index);
									child.put("length", vPairIndexTSTation[j].tStation.getLength());
									child.put("azimuth", vPairIndexTSTation[j].tStation.getAzimuth());
								}
							}

							// output
							boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
						}
					}
					// indexTaxiから見てvPairIndexTSTationの要素を指すときのオブジェクトを求めて保存
					{
						int N = vPairIndexTSTation.size();
						for (int j = 0; j < N; j++) {
							int indexTarget = vPairIndexTSTation[j].index;
							bool isSameCell = (indexTaxi == indexTarget);
							// ------ 必要なもの ------ //
							// ・indexTargetのセルの代表座標
							// : gCoorTargetHoge
							// ・オブジェクト生成の基準となるboxの一辺の長さ[m]
							// : squareLengthHoge
							// ・indexTaxiのセルからindexTargetのセルを見た時の方位角[deg]([0, 360))
							// : deg_10_Hoge
							// ・計算結果を格納するためにgCoorを２つ
							// : gCoor_1, gCoor_2
							// ------------------------ //
							GeographicCoordinate gCoorTargetHoge = vRepresentativePoint[indexTarget-1];
							double squareLengthHoge = 0;
							{
								double x = vPairIndexTSTation[j].tStation.getLength();
								double minX = 0;
								double maxX = searchRange;
								double yOfMinX = squareLengthMin;
								double yOfMaxX = squareLengthMax;
								squareLengthHoge = myHokann(x, minX, maxX, yOfMinX, yOfMaxX);
							}
							double deg_10_Hoge = vPairIndexTSTation[j].tStation.getAzimuth();
							if (isSameCell) {
								// infoAboutValidCells/(indexTaxi)/object/(indexTarget)/dir(k).xml
								for (int k = 1; k <= 9; k++) {
									// k = 1 : 真横
									// k = 2 : 真横
									// k = 3 : 下り斜め
									// k = 4 : 縦
									// k = 5 : 上り斜め
									// k = 6 : 真横
									// k = 7 : 下り斜め
									// k = 8 : 縦
									// k = 9 : 上り斜め
									// (1, 2, 6) -> 真横 -> 0[deg], (3, 7) -> 下り斜め -> 45[deg], (5, 9) -> 上り斜め -> 135[deg], (4, 8) -> 縦 -> 90[deg] と見なせば良い
									// dir(k)に合わせたdeg_10_Hogeの更新
									{
										if (k== 1 || k == 2 || k == 6) {
											// 真横
											deg_10_Hoge = 0;
										}else if (k == 4 || k == 8) {
											// 縦
											deg_10_Hoge = 90;
										}else if (k == 5 || k == 9) {
											// 上り斜め
											deg_10_Hoge = 135;
										}else{
											// 下り斜め
											deg_10_Hoge = 45;
										}
									}
									GeographicCoordinate gCoor_1, gCoor_2;
									myCalculateVertex(gCoor_1, gCoor_2, gCoorTargetHoge, squareLengthHoge, deg_10_Hoge);
									// 保存
									{
										// 保存ファイル名
										std::string fileName = "dir" + boost::lexical_cast<std::string>( k ) + ".xml";
										// 保存ファイル先のディレクトリのmakefileからの相対位置
										std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object/" + boost::lexical_cast<std::string>( indexTarget );
										// 保存path
										std::string fileRela = fileDire + "/" + fileName;
										// create an empty property tree
										boost::property_tree::ptree pt;
										// create the root element
										// boost::property_tree::ptree& root = pt.put("table", "");
										{
											pt.put("table.latitude1", gCoor_1.getPhi());
											pt.put("table.longitude1", gCoor_1.getLambda());
											pt.put("table.latitude2", gCoor_2.getPhi());
											pt.put("table.longitude2", gCoor_2.getLambda());
										}
										// output
										boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
									}
								}
							}else{
								// infoAboutValidCells/(indexTaxi)/object/(indexTarget).xml
								GeographicCoordinate gCoor_1, gCoor_2;
								myCalculateVertex(gCoor_1, gCoor_2, gCoorTargetHoge, squareLengthHoge, deg_10_Hoge);
								// 保存
								{
									// 保存ファイル名
									std::string fileName = boost::lexical_cast<std::string>( indexTarget ) + ".xml";
									// 保存ファイル先のディレクトリのmakefileからの相対位置
									std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object/";
									// 保存path
									std::string fileRela = fileDire + "/" + fileName;
									// create an empty property tree
									boost::property_tree::ptree pt;
									// create the root element
									// boost::property_tree::ptree& root = pt.put("table", "");
									{
										pt.put("table.latitude1", gCoor_1.getPhi());
										pt.put("table.longitude1", gCoor_1.getLambda());
										pt.put("table.latitude2", gCoor_2.getPhi());
										pt.put("table.longitude2", gCoor_2.getLambda());
									}
									// output
									boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
								}
							}
						}
					}
					// dir(k)の方向に進んでいるタクシーがindexTaxiにいる場合の探索範囲(dir1は円，dir2からdir9は扇型)を求めて保存
					{
						double dirTheta[9] = {0, 0, 45, 90, 135, 180, 225, 270, 315};
						for (int k = 1; k <= 9; k++) {
							std::vector<PairIndexTSTation> vPairIndexTSTationHoge = vPairIndexTSTation;
							// vPairIndexTSTationHogeから範囲外の角度を持つ要素を取り除いていく.
							{
								if (k != 1) {
									auto itr = vPairIndexTSTationHoge.begin();
									while (itr != vPairIndexTSTationHoge.end()) {
										bool isWithinValidDegree = false;
										{
											// indexTaxiのセルからitrが指すindexTargetのセルを見た時の方位角[deg]([0, 360))
											double theta_Hoge = itr -> tStation.getAzimuth();
											// 進行方向の方位角
											double thetaCri_Hoge = dirTheta[k-1];
											// タクシーの進行方向を考慮した時に描画するオブジェクトの有効範囲の扇型（円に含まれる）の中心角度[deg](45度から180度まで)．しかし，dir1もあるので中心角度は360度までにしても大丈夫にする
											double Theta_Hoge = searchDegree;
											int indexTarget = itr -> index;
											bool condition = (indexTaxi == indexTarget);
											if (condition) {
												isWithinValidDegree = true;
											}else{
												isWithinValidDegree = myCheckDegreeRange(theta_Hoge, thetaCri_Hoge, Theta_Hoge);
											}
										}
										if(!isWithinValidDegree) {
											itr = vPairIndexTSTationHoge.erase(itr);
										}else{
											itr++;
										}
									}
								}
							}
							// indexTargetから見たdir(k)の方向に対する探索範囲がvPairIndexTSTationHogeに決定したので保存する
							{
								// 保存ファイル名
								std::string fileName = "dir" + boost::lexical_cast<std::string>( k ) + ".xml";
								// 保存ファイル先のディレクトリのmakefileからの相対位置
								std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi ) + "/SearchRange";
								// 保存path
								std::string fileRela = fileDire + "/" + fileName;
								// create an empty property tree
								boost::property_tree::ptree pt;

								// create the root element
								boost::property_tree::ptree& root = pt.put("table", "");
								// add child elements
								{
									int N = vPairIndexTSTationHoge.size();
									for (int j = 0; j < N; j++) {
										boost::property_tree::ptree& child = root.add("cell", "");
										child.put("index", vPairIndexTSTationHoge[j].index);
									}
								}

								// output
								boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
							}
						}
					}
				}
			}
		}
		// 空のオブジェクト情報の作成・保存
		{
			// 保存ファイル名
			const std::string fileName = "emptyObject.xml";
			// 保存ファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/0_2_Set/Other";
			// 保存path
			std::string fileRela = fileDire + "/" + fileName;
			// create an empty property tree
			boost::property_tree::ptree pt;
			// create the root element
			// boost::property_tree::ptree& root = pt.put("table", "");
			{
				pt.put("table.latitude1", -1);
				pt.put("table.longitude1", -1);
				pt.put("table.latitude2", -1);
				pt.put("table.longitude2", -1);
			}
			// output
			boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
		}
		// 1_Cronの初期取り込みデータの作成・保存
		{
			// 保存ファイル名
			const std::string fileName = "InputDataFor1_Cron.xml";
			// 保存ファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/0_2_Set/Other";
			// 保存path
			std::string fileRela = fileDire + "/" + fileName;
			// create an empty property tree
			boost::property_tree::ptree pt;
			// create the root element
			boost::property_tree::ptree& root = pt.put("table", "");
			// add child elements
			{
				// ----- 加えるもの ----- //
				// gCoorNW
				// gCoorSE
				// numCell
				// numRow
				// numCol
				// cellSizePhi
				// cellSizeLambda
				// 
				// threshold
				// 
				// displayDate
				// displayTimeFrom
				// displayTimeTo
				// discreteTimeWidth
				// 
				// vValid
				// ---------------------- //
				// gCoorNW
				{
					boost::property_tree::ptree& child = root.add("gCoorNW", "");
					child.put("phi", gCoorNW.getPhi());
					child.put("lambda", gCoorNW.getLambda());
				}
				// gCoorSE
				{
					boost::property_tree::ptree& child = root.add("gCoorSE", "");
					child.put("phi", gCoorSE.getPhi());
					child.put("lambda", gCoorSE.getLambda());
				}
				// numCell
				{
					boost::property_tree::ptree& child = root.add("numCell", "");
					child.put("value", numCell);
				}
				// numRow
				{
					boost::property_tree::ptree& child = root.add("numRow", "");
					child.put("value", numRow);
				}
				// numCol
				{
					boost::property_tree::ptree& child = root.add("numCol", "");
					child.put("value", numCol);
				}
				// cellSizePhi
				{
					boost::property_tree::ptree& child = root.add("cellSizePhi", "");
					child.put("value", cellSizePhi);
				}
				// cellSizeLambda
				{
					boost::property_tree::ptree& child = root.add("cellSizeLambda", "");
					child.put("value", cellSizeLambda);
				}
				// threshold
				{
					boost::property_tree::ptree& child = root.add("threshold", "");
					child.put("value", threshold);
				}
				// displayDate
				{
					boost::property_tree::ptree& child = root.add("displayDate", "");
					int N = displayDate.size();
					for (int i = 0; i < N; i++) {
						boost::property_tree::ptree& cchild = child.add("date", "");
						cchild.put("value", displayDate[i]);
					}
				}
                // displayTimeFrom
				{
					boost::property_tree::ptree& child = root.add("displayTimeFrom", "");
					child.put("value", displayTimeFrom);
				}
				// displayTimeTo
				{
					boost::property_tree::ptree& child = root.add("displayTimeTo", "");
					child.put("value", displayTimeTo);
				}
				// discreteTimeWidth
				{
					boost::property_tree::ptree& child = root.add("discreteTimeWidth", "");
					child.put("value", discreteTimeWidth);
				}
				// vValid
				{
					boost::property_tree::ptree& child = root.add("cellIsValid", "");
					for (int i = 0; i < numCell; i++) {
						boost::property_tree::ptree& cchild = child.add("cell", "");
						cchild.put("value", vValid[i]);
					}
				}
			}
			// output
			boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
		}
		// 2_ForEachRequestの初期取り込みデータの作成・保存
		{
			// 保存ファイル名
			const std::string fileName = "InputDataFor2_ForEachRequest.xml";
			// 保存ファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/0_2_Set/Other";
			// 保存path
			std::string fileRela = fileDire + "/" + fileName;
			// create an empty property tree
			boost::property_tree::ptree pt;
			// create the root element
			boost::property_tree::ptree& root = pt.put("table", "");
			// add child elements
			{
				// ----- 加えるもの ----- //
				// gCoorNW
				// gCoorSE
				// numCell
				// numRow
				// numCol
				// cellSizePhi
				// cellSizeLambda
				// 
				// maxDistanceFromPreposition
				// 
				// ---------------------- //
				// gCoorNW
				{
					boost::property_tree::ptree& child = root.add("gCoorNW", "");
					child.put("phi", gCoorNW.getPhi());
					child.put("lambda", gCoorNW.getLambda());
				}
				// gCoorSE
				{
					boost::property_tree::ptree& child = root.add("gCoorSE", "");
					child.put("phi", gCoorSE.getPhi());
					child.put("lambda", gCoorSE.getLambda());
				}
				// numCell
				{
					boost::property_tree::ptree& child = root.add("numCell", "");
					child.put("value", numCell);
				}
				// numRow
				{
					boost::property_tree::ptree& child = root.add("numRow", "");
					child.put("value", numRow);
				}
				// numCol
				{
					boost::property_tree::ptree& child = root.add("numCol", "");
					child.put("value", numCol);
				}
				// cellSizePhi
				{
					boost::property_tree::ptree& child = root.add("cellSizePhi", "");
					child.put("value", cellSizePhi);
				}
				// cellSizeLambda
				{
					boost::property_tree::ptree& child = root.add("cellSizeLambda", "");
					child.put("value", cellSizeLambda);
				}
				// maxDistanceFromPreposition
				{
					boost::property_tree::ptree& child = root.add("maxDistanceFromPreposition", "");
					child.put("value", maxDistanceFromPreposition);
				}
			}
			// output
			boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
		}
	}
    return EXIT_SUCCESS;
}

