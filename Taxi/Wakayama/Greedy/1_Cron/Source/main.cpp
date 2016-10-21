/**
 * main.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "main.hpp"

using namespace hiro;

// 【この関数の目的】
// Cronごとに実行するGreedyのプログラム

struct MyData
{
	// 営業領域の外枠
	GeographicCoordinatePair gCoorPair;
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
	// セルが有効かどうか判別するフラグ
	std::vector<bool> vIsValid;
};

struct MyData2
{
	std::vector<int> vIndex;
	std::vector<int> vDemand;
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
			ret.numRow = pt.get<int>( "myData.numRow" );
			ret.numCol = pt.get<int>( "myData.numCol" );
			ret.numCell = pt.get<int>( "myData.numCell" );
			ret.cellSizePhi = pt.get<double>( "myData.cellSizePhi" );
			ret.cellSizeLambda = pt.get<double>( "myData.cellSizeLambda" );
			{
				int N = ret.numCell;
				// 要素の追加する領域を確保
				ret.vIsValid.reserve(N);
				// 値の追加
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData.vIsValid" ).begin();
					itr_last = pt.get_child( "myData.vIsValid" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						ret.vIsValid.push_back( it->second.get<bool>("") );
					}
				}
			}
		}
	}
	return ret;
}

void myDisplayMyData(MyData &myData);
void myDisplayMyData(MyData &myData)
{
	{
		std::cout << "myData.vIsValid" << "\n";
		int N = myData.numCell;
		for (int i = 0; i < N; i++) {
			std::cout << "[" << myData.vIsValid[i] << "]";
		}
		std::cout << "\n";
	}
	std::cout << "myData.gCoorPair : ";
	std::cout << "[";
	myData.gCoorPair.printStr();
	std::cout << "]" << "\n";
	std::cout << "myData.numRow : " << "[" << myData.numRow << "]" << "\n";
	std::cout << "myData.numCol : " << "[" << myData.numCol << "]" << "\n";
	std::cout << "myData.numCell : " << "[" << myData.numCell << "]" << "\n";
	std::cout << "myData.cellSizePhi : " << "[" << myData.cellSizePhi << "]" << "\n";
	std::cout << "myData.cellSizeLambda : " << "[" << myData.cellSizeLambda << "]" << "\n";
}

void myDisplayVDemand(std::vector<int> &vDemand);
void myDisplayVDemand(std::vector<int> &vDemand)
{
	int sumDemand = std::accumulate(vDemand.begin(), vDemand.end(), 0);
	std::cout << "総需要数 : " << sumDemand << "\n";
	if (sumDemand > 0) {
		std::cout << "需要数の確認" << "\n";
		int N = vDemand.size();
		for (int i = 0; i < N; i++) {
			if (vDemand[i] >= 1) {
				std::cout << "[index]" << i+1 << ", [demand]" << vDemand[i] << "\n";
			}
		}
	}
}

void myDisplayVIndexNotLessThanThreshold(std::vector<int> &vIndexNotLessThanThreshold);
void myDisplayVIndexNotLessThanThreshold(std::vector<int> &vIndexNotLessThanThreshold)
{
		std::cout << "需要数がしきい値以上あるセルの確認" << "\n";
		int N = vIndexNotLessThanThreshold.size();
		for (int i = 0; i < N; i++) {
			std::cout << "[index]" << vIndexNotLessThanThreshold[i];
		}
		std::cout << "\n";
}

void myDisplayVIndexUnderThreshold(std::vector<int> &vIndexUnderThreshold);
void myDisplayVIndexUnderThreshold(std::vector<int> &vIndexUnderThreshold)
{
		std::cout << "需要数がしきい値未満のセルの確認" << "\n";
		int N = vIndexUnderThreshold.size();
		for (int i = 0; i < N; i++) {
			std::cout << "[index]" << vIndexUnderThreshold[i];
		}
		std::cout << "\n";
}

void mySaveMyData2( MyData2 &myData2, const std::string &fileName, const std::string &fileDire );
void mySaveMyData2( MyData2 &myData2, const std::string &fileName, const std::string &fileDire )
{
	// 保存path
	std::string fileRela = fileDire + "/" + fileName;
	// create an empty property tree
	boost::property_tree::ptree pt;

	// create the root element
	boost::property_tree::ptree& root = pt.put("table", "");

	// add child elements
	{
		int N = myData2.vIndex.size();
		{
			boost::property_tree::ptree& child = root.put("vIndex", "");
			for (int i = 0; i < N; i++) {
				boost::property_tree::ptree& cchild = child.add("element", "");
				cchild.put("value", myData2.vIndex[i]);
				cchild.put("demand", myData2.vDemand[i]);
			}
		}
	}

	// output
	boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
}

int main(int argc, char *argv[])
{
    // argv[0] : ./Bin/main.out
	// argv[1] : 2 (unsigned int 型のthreshold)

	// example of input at commandline : ./Bin/main.out 2

	if (argc != 2) {
		std::cout << "argc must be 2. failure!" << "\n";
		return EXIT_FAILURE;
	}

	unsigned int threshold = 0;
	{
		int thresholdHoge = 0;
		try
        {
			thresholdHoge = boost::lexical_cast<int>(argv[1]);
        }
        catch(boost::bad_lexical_cast &)
        {
			std::cout << "argv[1](threshold) must be a type of unsigned int. failure!" << "\n";
			return EXIT_FAILURE;
        }
		if (thresholdHoge < 0) {
			std::cout << "argv[1](threshold) must be a type of unsigned int. failure!" << "\n";
			return EXIT_FAILURE;
		}
		threshold = thresholdHoge;
	}

	// 0_2_Setからパラメータを受け取る
	MyData myData;
	{
		// ファイル名
		const std::string fileName = "inputDataFor1_Cron.xml";
		// ディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other";
		myData = myReadMyData( fileName, fileDire );
	}

	// 表示
	// myDisplayMyData(myData);

	// 各セルの需要数を取得する
	std::vector<int> vDemand(myData.numCell, 0);
	{
		// Data.xmlを取得
		// create an empty property tree
		boost::property_tree::ptree pt;
		{
			// 設定値読み込みファイル名
			const std::string fileName = "data.xml";
			// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/1_Cron/FromServer";
			std::string fileRela = fileDire + "/" + fileName;
			// read the xml file
			boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
		}
		// ptを解析していき需要数をカウントしていく
		{
			boost::property_tree::ptree::iterator itr_first, itr_last, it;
			itr_first = pt.get_child( "table" ).begin();
			itr_last = pt.get_child( "table" ).end();
			for(it = itr_first; it != itr_last; it++) {
				// startPointを取得
				GeographicCoordinate gCoorHoge;
				gCoorHoge.setPhi( it->second.get<double>("startPoint.latitude") );
				gCoorHoge.setLambda( it->second.get<double>("startPoint.longitude") );
				int row = calculateRowFromLatitudes( gCoorHoge.getPhi(), myData.gCoorPair.getFirstPhi(), myData.gCoorPair.getSecondPhi(), myData.cellSizePhi );
				int col = calculateColFromLongitudes( gCoorHoge.getLambda(), myData.gCoorPair.getFirstLambda(), myData.gCoorPair.getSecondLambda(), myData.cellSizeLambda );
				int index = calculateIndexFromRowCol( row, col, myData.numRow, myData.numCol );
				// 範囲外だとindex = 0になる
				if( index != 0 ) {
					int i = index-1;
					// 有効なセルかどうか
					bool isValid = myData.vIsValid[i];
					if (isValid) {
						// 需要数のカウントアップ
						vDemand[i]++;
					}
				}
			}
		}
	}

	// // 表示
    // myDisplayVDemand(vDemand);


	// thresholdの値に基づき需要があるセルのうち，しきい値以上の値をもつセルとしきい値未満の値を持つセルのindexの集合を求める
	MyData2 myData2NotLessThanThreshold;
    myData2NotLessThanThreshold.vIndex.reserve(myData.numCell+1);
    myData2NotLessThanThreshold.vIndex.push_back(-1);
    myData2NotLessThanThreshold.vDemand.reserve(myData.numCell+1);
    myData2NotLessThanThreshold.vDemand.push_back(-1);
	MyData2 myData2UnderThreshold;
	myData2UnderThreshold.vIndex.reserve(myData.numCell+1);
    myData2UnderThreshold.vIndex.push_back(-1);
	myData2UnderThreshold.vDemand.reserve(myData.numCell+1);
    myData2UnderThreshold.vDemand.push_back(-1);
	{
		for (int i = 0; i < myData.numCell; i++) {
			if (vDemand[i] >= (int)threshold) {
				myData2NotLessThanThreshold.vIndex.push_back(i + 1);
				myData2NotLessThanThreshold.vDemand.push_back(vDemand[i]);
			}else if((vDemand[i] < (int)threshold) && (vDemand[i] > 0)) {
				myData2UnderThreshold.vIndex.push_back(i + 1);
				myData2UnderThreshold.vDemand.push_back(vDemand[i]);
			}
		}
	}

	// // 表示
	// myDisplayVIndexNotLessThanThreshold(myData2NotLessThanThreshold.vIndex);

    // // 表示
	// myDisplayVIndexUnderThreshold(myData2UnderThreshold.vIndex);

	// indexNotLessThanThreshold.xmlに保存
	{
		// 設定値保存ファイル名
		const std::string fileName = "indexNotLessThanThreshold.xml";
		// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/1_Cron/Other";
		mySaveMyData2( myData2NotLessThanThreshold, fileName, fileDire );
	}

	// indexUnderThreshold.xmlに保存
	{
		// 設定値保存ファイル名
		const std::string fileName = "indexUnderThreshold.xml";
		// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/1_Cron/Other";
		mySaveMyData2( myData2UnderThreshold, fileName, fileDire );
	}
    return EXIT_SUCCESS;
}
