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

// // 辞書式ソートができるように構造体を宣言し、ソートの規則をクラスで書く
// struct PairIndexTSTation {
//     int index;
//     TotalStation tStation;
// };
// class MyLessDefinition {
// public:
//     bool operator()(const PairIndexTSTation& a, const PairIndexTSTation& b)
//     {
// 		if( a.tStation.getLength() == b.tStation.getLength() ) {
// 			if( a.tStation.getAzimuth() == b.tStation.getAzimuth() ) {
// 				return a.index < b.index;
// 			}else{
// 				return a.tStation.getAzimuth() < b.tStation.getAzimuth();
// 			}
// 		}else{
// 			return a.tStation.getLength() < b.tStation.getLength();
// 		}
// 		return a.tStation.getLength() < b.tStation.getLength();
//     }
// };

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

void myCreateEmptyCircle(const std::string &fileName, const std::string &fileDire);
void myCreateEmptyCircle(const std::string &fileName, const std::string &fileDire)
{
	// 保存path
	std::string fileRela = fileDire + "/" + fileName;
	// create an empty property tree
	boost::property_tree::ptree pt;

	// create the root element
	boost::property_tree::ptree& root = pt.put("table", "");

	// add child elements
	{
		root.put("data.latitude", -1);
		root.put("data.longitude", -1);
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
	boost::property_tree::ptree& root = pt.put("myData2", "");

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

int main()
{
	// --- ユーザー入力値  --- //
	bool displayMyData = true;
	bool displayMyData2 = true;
	// ----------------------- //

	{
		const std::string fileDire = "./../Data/0_2_Set/Other";
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

	// ./../Data/0_2_Set/Other/inputDataFor1_Cron.xml
	{
		// 保存ファイル名
		const std::string fileName = "inputDataFor1_Cron.xml";
		// ディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other";
		myCreateInputDataFor1_Cron(myData2, vIsValid, fileName, fileDire);
	}
    return EXIT_SUCCESS;
}
