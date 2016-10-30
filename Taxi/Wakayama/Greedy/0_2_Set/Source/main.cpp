/**
 * main.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "main.hpp"

using namespace hiro;

// 【この関数の目的】
// Greedy関係の設定値のxmlファイルを0_1_Presetで作成したxmlファイルから作成する．

struct MyData
{
	// 外枠や領域を決めるboxは以下の2要素で定義される．
	// ・boxの最北西(north east)
	// ・boxの最南東(south west)

	// 営業領域の外枠
	GeographicCoordinatePair gCoorPair;
	// 追加する営業領域
	std::vector<GeographicCoordinatePair> gCoorPairAdd;
	// 削除する営業領域
	std::vector<GeographicCoordinatePair> gCoorPairRemove;

	// 営業領域は采の目上に分割される．分割された領域の最小単位をセルと呼ぶ．

	// セルの一辺の長さ[m]
	double cellSizeMeter;

	// アプリ上で描画される三角形の図形をオブジェクトと呼ぶ．

    // オブジェクトが指すセルとタクシーがいるセルの代表座標の最大距離．つまり，描画オブジェクトを決定するときの円状の探索範囲の半径[m]
	double searchRange;
	// 探索範囲を更に狭めるためのパラメータ．タクシーの進行方向(dir1, ..., dir9)に対して扇型の探索範囲を考えた時の中心角度[deg]．ただし，0[deg] < x <= 360[deg]
	double searchDegree;
	// オブジェクトの底辺の長さの最小値
	double baseLengthMin;
	// オブジェクトの底辺の長さの最大値
	double baseLengthMax;
};

struct MyData2
{
	// 営業領域の外枠
	GeographicCoordinatePair gCoorPair;
	// セルの一辺の長さ[m]
	double cellSizeMeter;
	// 営業領域の外枠の行数
	int numRow;
	// 営業領域の外枠の列数
	int numCol;
	// 営業領域の外枠内にあるセル数
	int numCell;
	// 営業領域の外枠内にある有効セル数
	int numValidCell;
	// セルの南北の緯度差
	double cellSizePhi;
	// セルの東西の経度差
	double cellSizeLambda;
};

// 指定した文字を、指定した文字数だけ出力するクラス
class CPut
{
// グローバルな演算子オーバーロードをフレンド指定
	friend std::ostream& operator<<(std::ostream& os, const CPut& put);

public:
	CPut(char c, int num) : m_char(c), m_num(num){}

private:
	char m_char;  // 出力する文字
	int m_num;    // 出力する文字数
};

// 出力用の<<演算子定義
std::ostream& operator<<(std::ostream& os, const CPut& put)
{
	for( int i = 0; i < put.m_num; ++i )
	{
		os << put.m_char;
	}
	return os;
}

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

MyData myReadMyData( const std::string &fileName, const std::string &fileDire );
MyData myReadMyData( const std::string &fileName, const std::string &fileDire )
{
	MyData ret;
	{
		std::string fileRela = fileDire + "/" + fileName;
		// create an empty property tree
		boost::property_tree::ptree pt;
		// read the xml file
		boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
		{
			ret.gCoorPair.set( pt.get<double>( "myData.gCoorPair.gCoorFirst_.phi_" ), pt.get<double>( "myData.gCoorPair.gCoorFirst_.lambda_" ), pt.get<double>( "myData.gCoorPair.gCoorSecond_.phi_" ), pt.get<double>( "myData.gCoorPair.gCoorSecond_.lambda_" ) );
			{
				// 数を調べる
				int N = 0;
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData.gCoorPairAdd" ).begin();
					itr_last = pt.get_child( "myData.gCoorPairAdd" ).end();
					N = std::distance(itr_first, itr_last);
				}
				// 要素の追加する領域を確保
				ret.gCoorPairAdd.reserve(N);
				// 値の追加
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData.gCoorPairAdd" ).begin();
					itr_last = pt.get_child( "myData.gCoorPairAdd" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						ret.gCoorPairAdd.push_back( GeographicCoordinatePair( it->second.get<double>("gCoorFirst_.phi_"), it->second.get<double>("gCoorFirst_.lambda_"), it->second.get<double>("gCoorSecond_.phi_"), it->second.get<double>("gCoorSecond_.lambda_") ) );
					}
				}
			}
			{
				// 数を調べる
				int N = 0;
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData.gCoorPairRemove" ).begin();
					itr_last = pt.get_child( "myData.gCoorPairRemove" ).end();
					N = std::distance(itr_first, itr_last);
				}
				// 要素の追加する領域を確保
				ret.gCoorPairRemove.reserve(N);
				// 値の追加
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData.gCoorPairRemove" ).begin();
					itr_last = pt.get_child( "myData.gCoorPairRemove" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						ret.gCoorPairRemove.push_back( GeographicCoordinatePair( it->second.get<double>("gCoorFirst_.phi_"), it->second.get<double>("gCoorFirst_.lambda_"), it->second.get<double>("gCoorSecond_.phi_"), it->second.get<double>("gCoorSecond_.lambda_") ) );
					}
				}
			}
			ret.cellSizeMeter = pt.get<double>( "myData.cellSizeMeter" );
			ret.searchRange = pt.get<double>( "myData.searchRange" );
			ret.searchDegree = pt.get<double>( "myData.searchDegree" );
			ret.baseLengthMin = pt.get<double>( "myData.baseLengthMin" );
			ret.baseLengthMax = pt.get<double>( "myData.baseLengthMax" );
		}
	}
	return ret;
}

void myDisplayMyData(const MyData &myData);
void myDisplayMyData(const MyData &myData)
{
	CPut cPutEq( '=', 30 );
	CPut cPutSp( ' ', 2 );
	CPut cPutSp2( ' ', 4 );
	std::cout << cPutEq << "\n";
	std::cout << cPutSp << "設定パラメータの表示" << "\n";
	std::cout << cPutEq << "\n";
	std::cout << cPutSp << "* gCoorPair" << "\n";
	{
		std::cout << cPutSp2 << "- ";
		std::cout << "latitude(NW), longitude(NW), latitude(SE), longitude(SE)";
		std::cout << "\n";
		{
			std::cout << cPutSp2 << "# ";
			myData.gCoorPair.printStr();
			std::cout << "\n";
		}
	}
	std::cout << cPutSp << "* gCoorPairAdd" << "\n";
	{
		int N = myData.gCoorPairAdd.size();
		if (N >= 1) {
			std::cout << cPutSp2 << "- ";
			std::cout << "追加領域の個数 : " << N;
			std::cout << "\n";
			std::cout << cPutSp2 << "- ";
			std::cout << "latitude(NW), longitude(NW), latitude(SE), longitude(SE)";
			std::cout << "\n";
			{
				for (int i = 0; i < N; i++) {
					std::cout << cPutSp2 << "# ";
					myData.gCoorPairAdd[i].printStr();
					std::cout << "\n";
				}
			}
		}else{
			std::cout << cPutSp2 << "- ";
			std::cout << "追加する領域が0個です．設定値の変更をしてください．";
			std::cout << "\n";
		}
	}
	std::cout << cPutSp << "* gCoorPairRemove" << "\n";
	{
		int N = myData.gCoorPairRemove.size();
		if (N >= 2) {
			std::cout << cPutSp2 << "- ";
			std::cout << "削除領域の個数 : " << N-1;
			std::cout << "\n";
			std::cout << cPutSp2 << "- ";
			std::cout << "latitude(NW), longitude(NW), latitude(SE), longitude(SE)";
			std::cout << "\n";
			{
				for (int i = 1; i < N; i++) {
					std::cout << cPutSp2 << "# ";
					myData.gCoorPairRemove[i].printStr();
					std::cout << "\n";
				}
			}
		}else{
			std::cout << cPutSp2 << "- ";
			std::cout << "削除する領域は0個です．";
			std::cout << "\n";
		}
	}
	std::cout << cPutSp << "* cellSizeMeter" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData.cellSizeMeter;
		std::cout << "\n";
	}
	std::cout << cPutSp << "* searchRange" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData.searchRange;
		std::cout << "\n";
	}
	std::cout << cPutSp << "* searchDegree" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData.searchDegree;
		std::cout << "\n";
	}
	std::cout << cPutSp << "* baseLengthMin" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData.baseLengthMin;
		std::cout << "\n";
	}
	std::cout << cPutSp << "* baseLengthMax" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData.baseLengthMax;
		std::cout << "\n";
	}
}

void myDisplayMyData2(const MyData2 &myData2);
void myDisplayMyData2(const MyData2 &myData2)
{
	CPut cPutEq( '=', 30 );
	CPut cPutSp( ' ', 2 );
	CPut cPutSp2( ' ', 4 );
	std::cout << cPutEq << "\n";
	std::cout << cPutSp << "作成した設定パラメータの表示" << "\n";
	std::cout << cPutEq << "\n";
	std::cout << cPutSp << "* gCoorPair" << "\n";
	{
		std::cout << cPutSp2 << "- ";
		std::cout << "latitude(NW), longitude(NW), latitude(SE), longitude(SE)";
		std::cout << "\n";
		{
			std::cout << cPutSp2 << "# ";
			myData2.gCoorPair.printStr();
			std::cout << "\n";
		}
	}
	std::cout << cPutSp << "* cellSizeMeter" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData2.cellSizeMeter;
		std::cout << "\n";
	}
	std::cout << cPutSp << "* numRow" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData2.numRow;
		std::cout << "\n";
	}
	std::cout << cPutSp << "* numCol" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData2.numCol;
		std::cout << "\n";
	}
	std::cout << cPutSp << "* numCell" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData2.numCell;
		std::cout << "\n";
	}
	std::cout << cPutSp << "* numValidCell" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData2.numValidCell;
		std::cout << "\n";
	}
	std::cout << cPutSp << "* cellSizePhi" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData2.cellSizePhi;
		std::cout << "\n";
	}
	std::cout << cPutSp << "* cellSizeLambda" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData2.cellSizeLambda;
		std::cout << "\n";
	}
}

bool checkOverlap(const GeographicCoordinatePair &gCoorPairTarget, const GeographicCoordinatePair &gCoorPairCri);
bool checkOverlap(const GeographicCoordinatePair &gCoorPairTarget, const GeographicCoordinatePair &gCoorPairCri)
{
	bool ret = false;
	{
		bool isOver = ( gCoorPairTarget.getSecondPhi() > gCoorPairCri.getFirstPhi() );
		bool isUnder = ( gCoorPairTarget.getFirstPhi() < gCoorPairCri.getSecondPhi() );
		bool isOnTheRight = ( gCoorPairTarget.getFirstLambda() > gCoorPairCri.getSecondLambda() );
		bool isOnTheLeft = ( gCoorPairTarget.getSecondLambda() < gCoorPairCri.getFirstLambda() );
		ret = !( isOver || isUnder || isOnTheRight || isOnTheLeft );
	}
	return ret;
}

void mySaveMyData2(const MyData2 &myData2, const std::string &fileName, const std::string &fileDire);
void mySaveMyData2(const MyData2 &myData2, const std::string &fileName, const std::string &fileDire)
{
	// 保存path
	std::string fileRela = fileDire + "/" + fileName;
	// create an empty property tree
	boost::property_tree::ptree pt;

	// create the root element
	boost::property_tree::ptree& root = pt.put("myData2", "");

	// add child elements
	{
		{
			boost::property_tree::ptree& child = root.put("gCoorPair", "");
			child.put("gCoorFirst_.phi_", myData2.gCoorPair.getFirstPhi());
			child.put("gCoorFirst_.lambda_", myData2.gCoorPair.getFirstLambda());
			child.put("gCoorSecond_.phi_", myData2.gCoorPair.getSecondPhi());
			child.put("gCoorSecond_.lambda_", myData2.gCoorPair.getSecondLambda());
		}
		{
			boost::property_tree::ptree& child = root.put("cellSizeMeter", "");
			child.put("", myData2.cellSizeMeter);
		}
		{
			boost::property_tree::ptree& child = root.put("numRow", "");
			child.put("", myData2.numRow);
		}
		{
			boost::property_tree::ptree& child = root.put("numCol", "");
			child.put("", myData2.numCol);
		}
		{
			boost::property_tree::ptree& child = root.put("numCell", "");
			child.put("", myData2.numCell);
		}
		{
			boost::property_tree::ptree& child = root.put("numValidCell", "");
			child.put("", myData2.numValidCell);
		}
		{
			boost::property_tree::ptree& child = root.put("cellSizePhi", "");
			child.put("", myData2.cellSizePhi);
		}
		{
			boost::property_tree::ptree& child = root.put("cellSizeLambda", "");
			child.put("", myData2.cellSizeLambda);
		}
	}

	// output
	boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
}

void mySaveRepresentativePoints(std::vector<GeographicCoordinate> &vRepresentativePoint, std::vector<int> &vIndex, std::vector<int> &vRow, std::vector<int> &vCol, std::vector<bool> &vIsValid, const std::string &fileName, const std::string &fileDire);
void mySaveRepresentativePoints(std::vector<GeographicCoordinate> &vRepresentativePoint, std::vector<int> &vIndex, std::vector<int> &vRow, std::vector<int> &vCol, std::vector<bool> &vIsValid, const std::string &fileName, const std::string &fileDire)
{
	// 保存path
	std::string fileRela = fileDire + "/" + fileName;
	// create an empty property tree
	boost::property_tree::ptree pt;

	// create the root element
	boost::property_tree::ptree& root = pt.put("table", "");

	// add child elements
	{
		int N = vIndex.size();
		for (int i = 0; i < N; i++) {
			boost::property_tree::ptree& child = root.add("element", "");
			child.put("index", vIndex[i]);
			child.put("isValid", vIsValid[i]);
			child.put("gCoorPhi", vRepresentativePoint[i].getPhi());
			child.put("gCoorLambda", vRepresentativePoint[i].getLambda());
			child.put("row", vRow[i]);
			child.put("col", vCol[i]);
		}
	}

	// output
	boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
}

void myCreateInfoAboutCellDirectory(const std::vector<bool> &vIsValid);
void myCreateInfoAboutCellDirectory(const std::vector<bool> &vIsValid)
{
	int N = vIsValid.size();
	for (int i = 0; i < N; i++) {
		int indexTaxi = i + 1;
		{
			// makefileからの相対位置
			const std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi );
			boost::filesystem::path path(fileDire);
			boost::system::error_code error;
			const bool result = boost::filesystem::create_directories(path, error);
			if (!result || error) {
				// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
			}
		}
		if (vIsValid[i]) {
			{
				// makefileからの相対位置
				const std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi ) + "/SearchRange";
				boost::filesystem::path path(fileDire);
				boost::system::error_code error;
				const bool result = boost::filesystem::create_directories(path, error);
				if (!result || error) {
					// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
				}
			}
			{
				// makefileからの相対位置
				const std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object";
				boost::filesystem::path path(fileDire);
				boost::system::error_code error;
				const bool result = boost::filesystem::create_directories(path, error);
				if (!result || error) {
					// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
				}
			}
			{
				int indexTarget = indexTaxi;
				// makefileからの相対位置
				const std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object/" + boost::lexical_cast<std::string>( indexTarget );
				boost::filesystem::path path(fileDire);
				boost::system::error_code error;
				const bool result = boost::filesystem::create_directories(path, error);
				if (!result || error) {
					// std::cout << "ディレクトリの作成に失敗したか、すでにあります。" << std::endl;
				}
			}
		}
	}
}

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

// 補間関数
double myHokann(const double x, const double minX, const double maxX, const double yOfMinX, const double yOfMaxX);
double myHokann(const double x, const double minX, const double maxX, const double yOfMinX, const double yOfMaxX) {
	double alpha = (x - minX) / (double)(maxX - minX);

	double hoge1 = 1 / 3.0;
	double hoge2 = 2 / 3.0;

	double ret = 0;

	if (alpha >= 0 && alpha < hoge1) {
		ret = 200;
	}else if (alpha >= hoge1 && alpha < hoge2) {
		ret = 500;
	}else{
		ret = 1000;
	}
	// double ret = (1-alpha) * yOfMinX + alpha * yOfMaxX;
	return ret;
}

GeographicCoordinatePair myCalculateBaseInfo(double azimuth, const GeographicCoordinate &gCoorTarget, double baseLength);
GeographicCoordinatePair myCalculateBaseInfo(double azimuth, const GeographicCoordinate &gCoorTarget, double baseLength)
{
	// : 垂線が引かれる直線の情報
	//   ・デカルト座標系での直線の傾き[deg](0 <= azimuth < 360)
	//   ・直線とこれから引く垂線が交わる地理座標平面上の点(gCoorTarget)
	// : 垂線の線分の長さ[m](baseLength．垂線の線分は直線によって2等分される)
	GeographicCoordinatePair ret;
	{
		constexpr double degToRad = M_PI / 180.0;

		// オブジェクトの底辺のX軸(北)からの時計回りのずれ
		double theta = azimuth + 90;
		if (theta >= 360) {
			theta -= 360;
		}
		// この時点で 0° <= theta < 360°

        // メモ
		// 線分の端点retの情報を計算する．地理座標系における線分の中点はgCoorTarget．線分の長さはbaseLength．線分のX軸(北)からの時計回りのずれはtheta( 0[deg] <= theta < 360[deg])

		// 図形の対称性を用いて計算する(thataとthata+180°が表す底辺は同じ)
		if (theta >= 180) {
			theta -= 180;
		}
		// この時点で 0° <= teta < 180°
		double l = baseLength / 2.0;
		// indexTargetの代表座標から北へx[m]
		double x = 0;
		// indexTargetの代表座標から東へy[m]
		double y = 0;
		{
			double thetaRad = theta * degToRad;
			x = l * cos(thetaRad);
			y = l * sin(thetaRad);
		}
		CartesianCoordinate cCoor_1( x, y );
		CartesianCoordinate cCoor_2( -x, -y );
		// 変換
		GeographicCoordinate gCoor_1 = convertCCoorIntoGCoor( cCoor_1, gCoorTarget );
		GeographicCoordinate gCoor_2 = convertCCoorIntoGCoor( cCoor_2, gCoorTarget );
		ret.set( gCoor_1, gCoor_2 );
	}
	return ret;
}

void myCreateInfoAboutCell(int indexTaxi, const MyData &myData, const MyData2 &myData2, const std::vector<GeographicCoordinate> &vRepresentativePoint, const std::vector<bool> &vIsValid);
void myCreateInfoAboutCell(int indexTaxi, const MyData &myData, const MyData2 &myData2, const std::vector<GeographicCoordinate> &vRepresentativePoint, const std::vector<bool> &vIsValid)
{
	// 作成するもの
    // ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/isValid.xml
	// もしisValid.xmlがtrueを示していれば
    // ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/searchRange.xml
	// ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/SearchRange/dir(k).xml
	// ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/Object/(indexTarget).xml
	// もしindexTaxi == indexTargetなら
	// ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/Object/(indexTarget)/dir(k).xml

	// 注意
	// フォルダは事前に作成済
	int i = indexTaxi - 1;
	// ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/isValid.xmlのデータの作成
	{
		std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi );
		std::string fileName = "isValid.xml";
		// 保存path
		std::string fileRela = fileDire + "/" + fileName;
		// create an empty property tree
		boost::property_tree::ptree pt;

		// create the root element
		boost::property_tree::ptree& root = pt.put("table", "");

		// add child elements
		{
			root.put("isValid", vIsValid[i]);
		}
		// output
		boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
	}
	if (vIsValid[i]) {
		std::vector<PairIndexTSTation> vPairIndexTSTation;
		{
			for (int j = 0; j < myData2.numCell; j++) {
				int indexTarget = j + 1;
				PairIndexTSTation pairIndexTSTationHoge;
				// indexTaxi(i)からindexTarget(j)への測地線長と方位角を取得する
				pairIndexTSTationHoge.tStation = calculateTStationFromGCoor( vRepresentativePoint[i], vRepresentativePoint[j] );
				bool jIsWithinNeighborhood = (pairIndexTSTationHoge.tStation.getLength() <= myData.searchRange);
				bool jIsValidCell = vIsValid[j];
				bool isValid = jIsWithinNeighborhood && jIsValidCell;
				if (isValid) {
					// indexTargetを登録
					pairIndexTSTationHoge.index = indexTarget;
					// 有効セルの情報ベクトルへプッシュ
					vPairIndexTSTation.push_back(pairIndexTSTationHoge);
				}
			}
			// 第一辞書を測地線長, 第二辞書を方位角, 第三辞書をindexとしてvPairIndexTSTationをソート
			sort(vPairIndexTSTation.begin(), vPairIndexTSTation.end(), MyLessDefinition());
		}
		// indexTaxiに対するindexTarget(詳細情報付き)の集合を求めて保存
		// ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/searchRange.xmlにvPairIndexTSTationを保存
		{
			// 保存ファイル名
			const std::string fileName = "searchRange.xml";
			// 保存ファイル先のディレクトリのmakefileからの相対位置
			std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi );
			// 保存path
			std::string fileRela = fileDire + "/" + fileName;
			// create an empty property tree
			boost::property_tree::ptree pt;

			// create the root element
			boost::property_tree::ptree& root = pt.put("vPairIndexTSTation", "");
			// add child elements
			{
				int N = vPairIndexTSTation.size();
				for (int j = 0; j < N; j++) {
					boost::property_tree::ptree& child = root.add("element", "");
					child.put("index", vPairIndexTSTation[j].index);
					child.put("tStation.length_", vPairIndexTSTation[j].tStation.getLength());
					child.put("tStation.azimuth_", vPairIndexTSTation[j].tStation.getAzimuth());
				}
			}

			// output
			boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
		}
		// dir(k)の方向に進んでいるタクシーがindexTaxiにいる場合の探索範囲(dir1は円，dir2からdir9は扇型)を求めて保存
		// ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/SearchRange/dir(k).xml
		{
			constexpr double dirTheta[9] = {0, 0, 45, 90, 135, 180, 225, 270, 315};
			for (int k = 1; k <= 9; k++) {
				std::vector<PairIndexTSTation> vPairIndexTSTationK = vPairIndexTSTation;
				// vPairIndexTSTationKから範囲外の角度を持つ要素を取り除いていく.
				{
					if (k != 1) {
						auto itr = vPairIndexTSTationK.begin();
						while (itr != vPairIndexTSTationK.end()) {
							bool isWithinValidDegree = false;
							{
								// ターゲットのセル
								int indexTarget = itr -> index;
								// indexTaxiのセルからitrが指すindexTargetのセルを見た時の方位角[deg]([0, 360))
								double thetaHoge = itr -> tStation.getAzimuth();
								// 進行方向の方位角
								double thetaCriHoge = dirTheta[k-1];
								// タクシーの進行方向を考慮した時に描画するオブジェクトの有効範囲の扇型（円に含まれる）の中心角度[deg](45度から180度まで)．しかし，dir1もあるので中心角度は360度までにしても大丈夫にする
								double ThetaHoge = myData.searchDegree;
								bool condition = (indexTaxi == indexTarget);
								if (condition) {
									isWithinValidDegree = true;
								}else{
									isWithinValidDegree = myCheckDegreeRange(thetaHoge, thetaCriHoge, ThetaHoge);
								}
							}
							if(!isWithinValidDegree) {
								itr = vPairIndexTSTationK.erase(itr);
							}else{
								itr++;
							}
						}
					}
				}
				// indexTargetから見たdir(k)の方向に対する探索範囲がvPairIndexTSTationKに決定したので保存する
				{
					// 保存ファイル名
					std::string fileName = "dir" + boost::lexical_cast<std::string>( k ) + ".xml";
					// 保存ファイル先のディレクトリのmakefileからの相対位置
					std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi ) + "/SearchRange";
					// 保存path
					std::string fileRela = fileDire + "/" + fileName;
					// create an empty property tree
					boost::property_tree::ptree pt;

					// create the root element
					boost::property_tree::ptree& root = pt.put("table", "");
					// add child elements
					{
						int N = vPairIndexTSTationK.size();
						for (int j = 0; j < N; j++) {
							root.add("element", vPairIndexTSTationK[j].index);
						}
					}

					// output
					boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
				}
			}
		}
		// indexTaxiから見てvPairIndexTSTationの要素を指すときのオブジェクトを求めて保存
		// ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/Object/(indexTarget).xml
		// もしindexTaxi == indexTargetなら
		// ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/Object/(indexTarget)/dir(k).xml
		{
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
			constexpr double dirAzimuth[9] = {0, 0, 45, 90, 135, 0, 45, 90, 135};
			int N = vPairIndexTSTation.size();
			for (int j = 0; j < N; j++) {
				int indexTarget = vPairIndexTSTation[j].index;
				// ---------- 必要なもの ----------
				// : 垂線が引かれる直線の情報
				//   ・デカルト座標系での直線の傾き[deg](0 <= x < 360)
				//   ・直線とこれから引く垂線が交わる地理座標平面上の点
				// : 垂線の線分の長さ[m](垂線の線分は直線によって2等分される)
				// --------------------------------
				bool areSameCells = (indexTaxi == indexTarget);
				double lengthHoge = 0;
				{
					double x = vPairIndexTSTation[j].tStation.getLength();
					double minX = 0;
					double maxX = myData.searchRange;
					double yOfMinX = myData.baseLengthMin;
					double yOfMaxX = myData.baseLengthMax;
					lengthHoge = myHokann(x, minX, maxX, yOfMinX, yOfMaxX);
				}
				if (areSameCells) {
					for (int k = 1; k <= 9; k++) {
						// ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/Object/(indexTarget)/dir(k).xml
						GeographicCoordinatePair gCoorPairHoge = myCalculateBaseInfo(dirAzimuth[k - 1], vRepresentativePoint[indexTarget - 1], lengthHoge);
						// オブジェクト情報gCoorPairHogeを保存する
						{
							// 保存ファイル名
							std::string fileName = "dir" + boost::lexical_cast<std::string>( k ) + ".xml";
							// 保存ファイル先のディレクトリのmakefileからの相対位置
							std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object/" + boost::lexical_cast<std::string>( indexTarget );
							// 保存path
							std::string fileRela = fileDire + "/" + fileName;
							// create an empty property tree
							boost::property_tree::ptree pt;
							// create the root element
							// boost::property_tree::ptree& root = pt.put("table", "");
							{
								pt.put("table.latitude1", gCoorPairHoge.getFirstPhi());
								pt.put("table.longitude1", gCoorPairHoge.getFirstLambda());
								pt.put("table.latitude2", gCoorPairHoge.getSecondPhi());
								pt.put("table.longitude2", gCoorPairHoge.getSecondLambda());
							}
							// output
							boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
						}
					}
				}else{
					// ./../Data/0_2_Set/Other/InfoAboutCell/(indexTaxi)/Object/(indexTarget).xml
					GeographicCoordinatePair gCoorPairHoge = myCalculateBaseInfo(vPairIndexTSTation[j].tStation.getAzimuth(), vRepresentativePoint[indexTarget - 1], lengthHoge);
					// オブジェクト情報gCoorPairHogeを保存する
					{
						// 保存ファイル名
						std::string fileName = boost::lexical_cast<std::string>( indexTarget ) + ".xml";
						// 保存ファイル先のディレクトリのmakefileからの相対位置
						std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object";
						// 保存path
						std::string fileRela = fileDire + "/" + fileName;
						// create an empty property tree
						boost::property_tree::ptree pt;
						// create the root element
						// boost::property_tree::ptree& root = pt.put("table", "");
						{
							pt.put("table.latitude1", gCoorPairHoge.getFirstPhi());
							pt.put("table.longitude1", gCoorPairHoge.getFirstLambda());
							pt.put("table.latitude2", gCoorPairHoge.getSecondPhi());
							pt.put("table.longitude2", gCoorPairHoge.getSecondLambda());
						}
						// output
						boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
					}
				}
			}
		}
	}
}

void myCreateEmptyObject(const std::string &fileName, const std::string &fileDire);
void myCreateEmptyObject(const std::string &fileName, const std::string &fileDire)
{
	// 保存path
	std::string fileRela = fileDire + "/" + fileName;
	// create an empty property tree
	boost::property_tree::ptree pt;

	// create the root element
	boost::property_tree::ptree& root = pt.put("table", "");

	// add child elements
	{
		root.put("latitude1", -1);
		root.put("longitude1", -1);
		root.put("latitude2", -1);
		root.put("longitude2", -1);
	}

	// output
	boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
}

void myCreateEmptyValue(const std::string &fileName, const std::string &fileDire);
void myCreateEmptyValue(const std::string &fileName, const std::string &fileDire)
{
	// 保存path
	std::string fileRela = fileDire + "/" + fileName;
	// create an empty property tree
	boost::property_tree::ptree pt;

	// create the root element
	boost::property_tree::ptree& root = pt.put("table", "");

	// add child elements
	{
		root.put("value", -1);
	}

	// output
	boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
}

void myCreateInputDataFor1_Cron(MyData2 &myData2, std::vector<bool> &vIsValid, const std::string &fileName, const std::string &fileDire);
void myCreateInputDataFor1_Cron(MyData2 &myData2, std::vector<bool> &vIsValid, const std::string &fileName, const std::string &fileDire)
{
	// 保存path
	std::string fileRela = fileDire + "/" + fileName;
	// create an empty property tree
	boost::property_tree::ptree pt;

	// create the root element
	boost::property_tree::ptree& root = pt.put("myData", "");

	// add child elements
	{
		{
			boost::property_tree::ptree& child = root.put("vIsValid", "");
			int N = vIsValid.size();
			for (int i = 0; i < N; i++) {
				boost::property_tree::ptree& cchild = child.add("element", "");
				cchild.put("", vIsValid[i]);
			}
		}
		{
			boost::property_tree::ptree& child = root.put("gCoorPair", "");
			child.put("gCoorFirst_.phi_", myData2.gCoorPair.getFirstPhi());
			child.put("gCoorFirst_.lambda_", myData2.gCoorPair.getFirstLambda());
			child.put("gCoorSecond_.phi_", myData2.gCoorPair.getSecondPhi());
			child.put("gCoorSecond_.lambda_", myData2.gCoorPair.getSecondLambda());
		}
		{
			boost::property_tree::ptree& child = root.put("numRow", "");
			child.put("", myData2.numRow);
		}
		{
			boost::property_tree::ptree& child = root.put("numCol", "");
			child.put("", myData2.numCol);
		}
		{
			boost::property_tree::ptree& child = root.put("numCell", "");
			child.put("", myData2.numCell);
		}
		{
			boost::property_tree::ptree& child = root.put("cellSizePhi", "");
			child.put("", myData2.cellSizePhi);
		}
		{
			boost::property_tree::ptree& child = root.put("cellSizeLambda", "");
			child.put("", myData2.cellSizeLambda);
		}
	}

	// output
	boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
}

void myCreateInputDataFor2_ForEachRequest(MyData2 &myData2, const std::string &fileName, const std::string &fileDire);
void myCreateInputDataFor2_ForEachRequest(MyData2 &myData2, const std::string &fileName, const std::string &fileDire)
{
	// 保存path
	std::string fileRela = fileDire + "/" + fileName;
	// create an empty property tree
	boost::property_tree::ptree pt;

	// create the root element
	boost::property_tree::ptree& root = pt.put("myData", "");

	// add child elements
	{
		{
			boost::property_tree::ptree& child = root.put("gCoorPair", "");
			child.put("gCoorFirst_.phi_", myData2.gCoorPair.getFirstPhi());
			child.put("gCoorFirst_.lambda_", myData2.gCoorPair.getFirstLambda());
			child.put("gCoorSecond_.phi_", myData2.gCoorPair.getSecondPhi());
			child.put("gCoorSecond_.lambda_", myData2.gCoorPair.getSecondLambda());
		}
		{
			boost::property_tree::ptree& child = root.put("numRow", "");
			child.put("", myData2.numRow);
		}
		{
			boost::property_tree::ptree& child = root.put("numCol", "");
			child.put("", myData2.numCol);
		}
		{
			boost::property_tree::ptree& child = root.put("numCell", "");
			child.put("", myData2.numCell);
		}
		{
			boost::property_tree::ptree& child = root.put("cellSizePhi", "");
			child.put("", myData2.cellSizePhi);
		}
		{
			boost::property_tree::ptree& child = root.put("cellSizeLambda", "");
			child.put("", myData2.cellSizeLambda);
		}
	}

	// output
	boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
}
int main()
{
	// --- ユーザー入力値  --- //
	bool displayMyData = false;
	bool displayMyData2 = false;
	bool deleteOrCreateDirectory = true;
	bool createMain = true;
	// ----------------------- //

	// この関数で作成するデータを全削除．./../Data/0_2_Set/Otherのフォルダを作成．
	if (deleteOrCreateDirectory) {
		std::cout << "ディレクトリの削除・再作成中..." << "\n";
		// ディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other";
		// ディレクトリの削除
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
	}

	// 設定値の取得
	MyData myData;
	{
		// 設定値保存ファイル名
		const std::string fileName = "base.xml";
		// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_1_Preset/Other";
		myData = myReadMyData( fileName, fileDire );
	}

	// 設定値の確認
	if (displayMyData) {
		myDisplayMyData(myData);
	}

	// 現時点で計算可能なmyData2の要素(myData2.numValidCell以外)を計算して格納する
	MyData2 myData2;
	{
		myData2.gCoorPair = myData.gCoorPair;
		// myData.cellSizeMeterに基づいてmyData2.gCoorPair.gCoorSecond_の値を変更する
		CartesianCoordinate cCoorSE = convertGCoorIntoCCoor( myData2.gCoorPair.getSecond(), myData2.gCoorPair.getFirst() );
		myData2.cellSizeMeter = myData.cellSizeMeter;
		myData2.numRow = ceil( fabs( cCoorSE.getX() ) / (double)myData.cellSizeMeter );
		myData2.numCol = ceil( fabs( cCoorSE.getY() ) / (double)myData.cellSizeMeter );
		myData2.numCell = myData2.numRow * myData2.numCol;
		myData2.numValidCell = 0;
		cCoorSE.set( (const double)( - myData2.numRow * myData2.cellSizeMeter ), (const double)( myData2.numCol * myData2.cellSizeMeter ) );
		GeographicCoordinate gCoorSE = convertCCoorIntoGCoor( cCoorSE, myData2.gCoorPair.getFirst() );
		myData2.gCoorPair.setSecond(gCoorSE);
		// 緯度(latitude)方向, 経度(longitude)方向のcellSizeを計算
		myData2.cellSizePhi = (myData2.gCoorPair.getFirstPhi() - myData2.gCoorPair.getSecondPhi()) / (double)myData2.numRow;
		myData2.cellSizeLambda = (myData2.gCoorPair.getSecondLambda() - myData2.gCoorPair.getFirstLambda()) / (double)myData2.numCol;
	}

	// 各セルの代表点を計算
	std::vector<GeographicCoordinate> vRepresentativePoint(myData2.numCell);
	std::vector<int> vIndex(myData2.numCell);
	std::vector<int> vRow(myData2.numCell);
	std::vector<int> vCol(myData2.numCell);
	{
		std::vector<GeographicCoordinate>::iterator it, itr_first, itr_last;
		itr_first = vRepresentativePoint.begin();
		itr_last = vRepresentativePoint.end();
		for(it = itr_first; it != itr_last; it++) {
			std::size_t i = std::distance(itr_first, it);
			vIndex[i] = (int)i + 1;
			vRow[i] = calculateRowFromIndex( vIndex[i], myData2.numCol, myData2.numCell );
			vCol[i] = calculateColFromIndex( vIndex[i], myData2.numCol, myData2.numCell );
			vRepresentativePoint[i].setPhi( myData2.gCoorPair.getFirstPhi() - vRow[i] * myData2.cellSizePhi + myData2.cellSizePhi / 2.0 );
			vRepresentativePoint[i].setLambda( myData2.gCoorPair.getFirstLambda() + vCol[i] * myData2.cellSizeLambda - myData2.cellSizeLambda / 2.0 );
		}
	}
	// 各セルの代表点が有効かどうか計算する
	std::vector<bool> vIsValid(myData2.numCell, false);
	{
		// 計算にはmyData.gCoorPairAddとmyData.gCoorPairRemoveを使う

		// myData.gCoorPairAddを使ってvIsValidの該当要素をtrueにしていく
		{
			int N = myData.gCoorPairAdd.size();
			for (int i = 0; i < N; i++) {
				// myData.gCoorPairAdd[i]がmyData2.gCoorPairと重なりを持つか確認する
				bool overlap = checkOverlap( myData.gCoorPairAdd[i], myData2.gCoorPair );
				if (overlap) {
					// vIsValidをtrueにするセルの行列のポイントを決定する
					int rowFrom = 0;
					int rowTo = 0;
					int colFrom = 0;
					int colTo = 0;
					{
						rowFrom = calculateRowFromLatitudes( myData.gCoorPairAdd[i].getFirstPhi(), myData2.gCoorPair.getFirstPhi(), myData2.gCoorPair.getSecondPhi(), myData2.cellSizePhi );
						if (rowFrom == 0) {
							rowFrom = 1;
						}
						rowTo = calculateRowFromLatitudes( myData.gCoorPairAdd[i].getSecondPhi(), myData2.gCoorPair.getFirstPhi(), myData2.gCoorPair.getSecondPhi(), myData2.cellSizePhi );
						if (rowTo == 0) {
							rowTo = myData2.numRow;
						}
						colFrom = calculateColFromLongitudes( myData.gCoorPairAdd[i].getFirstLambda(), myData2.gCoorPair.getFirstLambda(), myData2.gCoorPair.getSecondLambda(), myData2.cellSizeLambda );
						if (colFrom == 0) {
							colFrom = 1;
						}
						colTo = calculateColFromLongitudes( myData.gCoorPairAdd[i].getSecondLambda(), myData2.gCoorPair.getFirstLambda(), myData2.gCoorPair.getSecondLambda(), myData2.cellSizeLambda );
						if (colTo == 0) {
							colTo = myData2.numCol;
						}
					}
					for (int i1 = rowFrom; i1 <= rowTo; i1++) {
						for (int i2 = colFrom; i2 <= colTo; i2++) {
							int indexHoge = calculateIndexFromRowCol( i1, i2, myData2.numRow, myData2.numCol );
							vIsValid[indexHoge - 1] = true;
						}
					}
				}
			}
		}
		// myData.gCoorPairRemoveを使ってvIsValidの該当要素をfalseにしていく
		{
			int N = myData.gCoorPairRemove.size();
			for (int i = 0; i < N; i++) {
				// myData.gCoorPairRemove[i]がmyData2.gCoorPairと重なりを持つか確認する
				bool overlap = checkOverlap( myData.gCoorPairRemove[i], myData2.gCoorPair );
				if (overlap) {
					// vIsValidをtrueにするセルの行列のポイントを決定する
					int rowFrom = 0;
					int rowTo = 0;
					int colFrom = 0;
					int colTo = 0;
					{
						rowFrom = calculateRowFromLatitudes( myData.gCoorPairRemove[i].getFirstPhi(), myData2.gCoorPair.getFirstPhi(), myData2.gCoorPair.getSecondPhi(), myData2.cellSizePhi );
						if (rowFrom == 0) {
							rowFrom = 1;
						}else{
							rowFrom++;
						}
						rowTo = calculateRowFromLatitudes( myData.gCoorPairRemove[i].getSecondPhi(), myData2.gCoorPair.getFirstPhi(), myData2.gCoorPair.getSecondPhi(), myData2.cellSizePhi );
						if (rowTo == 0) {
							rowTo = myData2.numRow;
						}else{
							rowTo--;
						}
						colFrom = calculateColFromLongitudes( myData.gCoorPairRemove[i].getFirstLambda(), myData2.gCoorPair.getFirstLambda(), myData2.gCoorPair.getSecondLambda(), myData2.cellSizeLambda );
						if (colFrom == 0) {
							colFrom = 1;
						}else{
							colFrom++;
						}
						colTo = calculateColFromLongitudes( myData.gCoorPairRemove[i].getSecondLambda(), myData2.gCoorPair.getFirstLambda(), myData2.gCoorPair.getSecondLambda(), myData2.cellSizeLambda );
						if (colTo == 0) {
							colTo = myData2.numCol;
						}else{
							colTo--;
						}
					}
					bool check = false;
					{
						bool checkRow = ( rowFrom >= 1 ) && ( rowFrom <= myData2.numRow ) && ( rowTo >= 1 ) && ( rowTo <= myData2.numRow ) && ( rowFrom <= rowTo );
						bool checkCol = ( colFrom >= 1 ) && ( colFrom <= myData2.numCol ) && ( colTo >= 1 ) && ( colTo <= myData2.numCol ) && ( colFrom <= colTo );
						check = checkRow && checkCol;
					}
					if (check) {
						for (int i1 = rowFrom; i1 <= rowTo; i1++) {
							for (int i2 = colFrom; i2 <= colTo; i2++) {
								int indexHoge = calculateIndexFromRowCol( i1, i2, myData2.numRow, myData2.numCol );
								vIsValid[indexHoge - 1] = false;
							}
						}
					}
				}
			}
		}
	}
	// vIsValidを使ってmyData2.numValidCellの値を決定する
	{
		bool targetHoge = true;
		myData2.numValidCell = std::count(vIsValid.begin(), vIsValid.end(), targetHoge);
	}

	// 作成した設定値の確認
	if (displayMyData2) {
		myDisplayMyData2(myData2);
	}

	// ここまでの営業領域に関する情報を保存
	{
		// ./../Data/0_2_Set/Other/base.xml
		{
			// 保存ファイル名
			const std::string fileName = "base.xml";
			// ディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/0_2_Set/Other";
			mySaveMyData2(myData2, fileName, fileDire);
		}
		// ./../Data/0_2_Set/Other/representativePoints.xml
		{
			// 保存ファイル名
			const std::string fileName = "representativePoints.xml";
			// ディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/0_2_Set/Other";
			mySaveRepresentativePoints(vRepresentativePoint, vIndex, vRow, vCol, vIsValid, fileName, fileDire);
		}
	}

	// ディレクトリの作成
	if (deleteOrCreateDirectory) {
		std::cout << "ディレクトリの削除・再作成中..." << "\n";
		myCreateInfoAboutCellDirectory( vIsValid );
	}

	// ファイルの作成
	if (createMain) {
		std::cout << "各セルの情報の計算と保存を実行中..." << "\n";
		int N = vIndex.size();
        boost::progress_display show_progress( N );
		for (int i = 0; i < N; i++) {
			int indexTaxi = i + 1;
			// 処理
			myCreateInfoAboutCell(indexTaxi, myData, myData2, vRepresentativePoint, vIsValid);
			// boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			// プログレス表示
			++show_progress;
		}
	}


	// ./../Data/0_2_Set/Other/emptyObject.xml
	{
		// 保存ファイル名
		const std::string fileName = "emptyObject.xml";
		// ディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other";
		myCreateEmptyObject(fileName, fileDire);
	}
	// ./../Data/0_2_Set/Other/emptyValue.xml
	{
		// 保存ファイル名
		const std::string fileName = "emptyValue.xml";
		// ディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other";
		myCreateEmptyValue(fileName, fileDire);
	}
	// ./../Data/0_2_Set/Other/inputDataFor1_Cron.xml
	{
		// 保存ファイル名
		const std::string fileName = "inputDataFor1_Cron.xml";
		// ディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other";
		myCreateInputDataFor1_Cron(myData2, vIsValid, fileName, fileDire);
	}
	// ./../Data/0_2_Set/Other/inputDataFor2_ForEachRequest.xml
	{
		// 保存ファイル名
		const std::string fileName = "inputDataFor2_ForEachRequest.xml";
		// ディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other";
		myCreateInputDataFor2_ForEachRequest(myData2, fileName, fileDire);
	}
    return EXIT_SUCCESS;
}
