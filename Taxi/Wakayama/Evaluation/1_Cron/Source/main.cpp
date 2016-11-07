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

struct MyCommandData
{
	int year;
	int month;
	int date;
	int preTime;
	int postTime;
	int threshold;
	int preWeek;
	int postWeek;
	int discreteTimeWidth;
};

// 構造体の宣言
struct DataFactor{
	boost::posix_time::ptime pTime;
	int id;
	int index;
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
			ret.gCoorPair.set( pt.get<double>( "myData2.gCoorPair.gCoorFirst_.phi_" ), pt.get<double>( "myData2.gCoorPair.gCoorFirst_.lambda_" ), pt.get<double>( "myData2.gCoorPair.gCoorSecond_.phi_" ), pt.get<double>( "myData2.gCoorPair.gCoorSecond_.lambda_" ) );
			ret.numRow = pt.get<int>( "myData2.numRow" );
			ret.numCol = pt.get<int>( "myData2.numCol" );
			ret.numCell = pt.get<int>( "myData2.numCell" );
			ret.cellSizePhi = pt.get<double>( "myData2.cellSizePhi" );
			ret.cellSizeLambda = pt.get<double>( "myData2.cellSizeLambda" );
			{
				int N = ret.numCell;
				// 要素の追加する領域を確保
				ret.vIsValid.reserve(N);
				// 値の追加
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData2.vIsValid" ).begin();
					itr_last = pt.get_child( "myData2.vIsValid" ).end();
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

int getStep(const boost::posix_time::ptime &pTime);
int getStep(const boost::posix_time::ptime &pTime)
{
	int ret = 0;
	int hours = pTime.time_of_day().hours();
	int minutes = pTime.time_of_day().minutes();
	int sumMinutes = 60 * hours + minutes;
	ret = sumMinutes;
	return ret;
}

void mySaveVData( std::vector<int> &v, const std::string &fileName, const std::string &fileDire );
void mySaveVData( std::vector<int> &v, const std::string &fileName, const std::string &fileDire )
{
	// 保存path
	std::string fileRela = fileDire + "/" + fileName;
	// create an empty property tree
	boost::property_tree::ptree pt;

	// create the root element
	boost::property_tree::ptree& root = pt.put("table", "");

	// add child elements
	{
		int N = v.size();
		{
			for (int i = 0; i < N; i++) {
				boost::property_tree::ptree& child = root.add("data", "");
				child.put("value", v[i]);
			}
			root.add("total", std::accumulate(v.begin(), v.end(), 0.0));
		}
	}

	// output
	boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
}

int main(int argc, char *argv[])
{

    // argv[0] : ./Bin/main.out
	// argv[1] : 2016 (year)
	// argv[2] : 9 (month)
	// argv[3] : 1 (date)
	// argv[4] : 10 (preTime)
	// argv[5] : 15 (postTime)
	// argv[6] : 1 (threshold)
	// argv[7] : 8 (preWeek)
	// argv[8] : 4 (postWeek)
	// argv[9] : 2 (discreteTimeWidth)

	// example of input at commandline : ./Bin/main.out 2016 9 1 10 15 1 8 4 2

	if (argc != 10) {
		std::cout << "argc must be 10. failure!" << "\n";
		return EXIT_FAILURE;
	}

	// コマンドライン引数の構造体への登録
	MyCommandData myCommandData;
	{
		{
			int hoge = 0;
			try{
				hoge = boost::lexical_cast<int>(argv[1]);
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[1](year) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			if (hoge < 0) {
				std::cout << "argv[1](year) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			myCommandData.year = hoge;
		}
		{
			int hoge = 0;
			try{
				hoge = boost::lexical_cast<int>(argv[2]);
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[2](month) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			if (hoge < 0) {
				std::cout << "argv[2](month) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			myCommandData.month = hoge;
		}
		{
			int hoge = 0;
			try{
				hoge = boost::lexical_cast<int>(argv[3]);
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[3](date) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			if (hoge < 0) {
				std::cout << "argv[3](date) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			myCommandData.date = hoge;
		}
		{
			int hoge = 0;
			try{
				hoge = boost::lexical_cast<int>(argv[4]);
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[4](preTime) must be a type of int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			myCommandData.preTime = hoge;
		}
		{
			int hoge = 0;
			try{
				hoge = boost::lexical_cast<int>(argv[5]);
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[5](postTime) must be a type of int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			myCommandData.postTime = hoge;
		}
		{
			int hoge = 0;
			try{
				hoge = boost::lexical_cast<int>(argv[6]);
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[6](threshold) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			if (hoge < 0) {
				std::cout << "argv[6](threshold) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			myCommandData.threshold = hoge;
		}
		{
			int hoge = 0;
			try{
				hoge = boost::lexical_cast<int>(argv[7]);
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[7](preWeek) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			if (hoge < 0) {
				std::cout << "argv[7](preWeek) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			myCommandData.preWeek = hoge;
		}
		{
			int hoge = 0;
			try{
				hoge = boost::lexical_cast<int>(argv[8]);
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[8](postWeek) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			if (hoge < 0) {
				std::cout << "argv[8](postWeek) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			myCommandData.postWeek = hoge;
		}
		{
			int hoge = 0;
			try{
				hoge = boost::lexical_cast<int>(argv[9]);
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[9](discreteTimeWidth) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			if (hoge < 0) {
				std::cout << "argv[9](discreteTimeWidth) must be a type of unsigned int. failure!" << "\n";
				return EXIT_FAILURE;
			}
			myCommandData.discreteTimeWidth = hoge;
		}

		// pre, postのチェック
		{
			if (myCommandData.preTime > myCommandData.postTime) {
				std::cout << "check preTime and postTime. failure!" << "\n";
				return EXIT_FAILURE;
			}
			if (myCommandData.preWeek < myCommandData.postWeek) {
				std::cout << "check preWeek and postWeek. failure!" << "\n";
				return EXIT_FAILURE;
			}
		}
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

	// // 表示
	// myDisplayMyData(myData);

	// 一日は1440分
	constexpr int numT = 60*24;

	boost::posix_time::ptime pTimeCri;
	{
		std::string strHoge = boost::lexical_cast<std::string>( myCommandData.year ) + "-" + boost::lexical_cast<std::string>( myCommandData.month ) + "-" + boost::lexical_cast<std::string>( myCommandData.date ) + " 00:00:00";
		// boost::posix_time::ptime 型に変換
		pTimeCri = boost::posix_time::time_from_string(strHoge);
	}

	// 表示
	// std::cout << pTimeCri << "\n";

	// ピン付近で
	std::vector<int> vNumGotCustomerNearPins(numT, 0);
	// ピン付近以外で
	std::vector<int> vNumGotCustomerFarFromPins(numT, 0);
	{
		// evalData.xmlの読込
		std::vector<DataFactor> dataFactorEval;
		{
			// create an empty property tree
			boost::property_tree::ptree pt;
			{
				// 設定値読み込みファイル名
				const std::string fileName = "evalData.xml";
				// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
				const std::string fileDire = "./../Data/1_Cron/FromServer";
				std::string fileRela = fileDire + "/" + fileName;
				// read the xml file
				boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
			}
			{
				boost::property_tree::ptree::iterator itr_first, itr_last, it;
				itr_first = pt.get_child( "table" ).begin();
				itr_last = pt.get_child( "table" ).end();
				for(it = itr_first; it != itr_last; it++) {
					// startPointを取得
					GeographicCoordinate gCoorHoge;
					gCoorHoge.setPhi( it->second.get<double>("point.latitude") );
					gCoorHoge.setLambda( it->second.get<double>("point.longitude") );
					int row = calculateRowFromLatitudes( gCoorHoge.getPhi(), myData.gCoorPair.getFirstPhi(), myData.gCoorPair.getSecondPhi(), myData.cellSizePhi );
					int col = calculateColFromLongitudes( gCoorHoge.getLambda(), myData.gCoorPair.getFirstLambda(), myData.gCoorPair.getSecondLambda(), myData.cellSizeLambda );
					int index = calculateIndexFromRowCol( row, col, myData.numRow, myData.numCol );
					// 範囲外だとindex = 0になる
					if( index != 0 ) {
						// 有効なセルかどうか
						bool isValidCell = myData.vIsValid[index - 1];
						if (isValidCell) {
							// dateを取得する
							std::string sTimeStr = it->second.get<std::string>("date");
							// 空文字の除去
							boost::trim (sTimeStr);
							// boost::posix_time::ptime 型に変換
							boost::posix_time::ptime pTime = boost::posix_time::time_from_string(sTimeStr);
							DataFactor dataFactorHoge;
							dataFactorHoge.pTime = pTime;
							dataFactorHoge.id = it->second.get<int>("id");
							dataFactorHoge.index = index;
							dataFactorEval.push_back(dataFactorHoge);
						}
					}
				}
			}
		}
		// step(時間)の時にindex(ただし，有効なセル)で描画されていたピンの数
		std::vector<std::vector<int>> vNumPin(numT);
		{
			// 領域確保
			for (int i = 0; i < numT; i++) {
				vNumPin[i].resize(myData.numCell, 0);
			}
			// pinsData.xmlの読込
			std::vector<DataFactor> dataFactorPins;
			{
				// create an empty property tree
				boost::property_tree::ptree pt;
				{
					// 設定値読み込みファイル名
					const std::string fileName = "pinsData.xml";
					// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
					const std::string fileDire = "./../Data/1_Cron/FromServer";
					std::string fileRela = fileDire + "/" + fileName;
					// read the xml file
					boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
				}
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "table" ).begin();
					itr_last = pt.get_child( "table" ).end();
					for(it = itr_first; it != itr_last; it++) {
						// startPointを取得
						GeographicCoordinate gCoorHoge;
						gCoorHoge.setPhi( it->second.get<double>("point.latitude") );
						gCoorHoge.setLambda( it->second.get<double>("point.longitude") );
						int row = calculateRowFromLatitudes( gCoorHoge.getPhi(), myData.gCoorPair.getFirstPhi(), myData.gCoorPair.getSecondPhi(), myData.cellSizePhi );
						int col = calculateColFromLongitudes( gCoorHoge.getLambda(), myData.gCoorPair.getFirstLambda(), myData.gCoorPair.getSecondLambda(), myData.cellSizeLambda );
						int index = calculateIndexFromRowCol( row, col, myData.numRow, myData.numCol );
						// 範囲外だとindex = 0になる
						if( index != 0 ) {
							// 有効なセルかどうか
							bool isValidCell = myData.vIsValid[index - 1];
							if (isValidCell) {
								// dateを取得する
								std::string sTimeStr = it->second.get<std::string>("date");
								// 空文字の除去
								boost::trim (sTimeStr);
								// boost::posix_time::ptime 型に変換
								boost::posix_time::ptime pTime = boost::posix_time::time_from_string(sTimeStr);
								DataFactor dataFactorHoge;
								dataFactorHoge.pTime = pTime;
								dataFactorHoge.id = it->second.get<int>("id");
								dataFactorHoge.index = index;
								dataFactorPins.push_back(dataFactorHoge);
							}
						}
					}
				}
			}

			int numWeeks = myCommandData.preWeek - myCommandData.postWeek + 1;
			// discreteTimeWidthの倍数の時刻のみ計算して値を格納する
			{
				for (int i = 0; i < numT; i++) {
					if (i % myCommandData.discreteTimeWidth == 0) {
						// 判別の時間を取得
						std::vector<boost::posix_time::ptime> vPTimeFrom(numWeeks, pTimeCri);
						std::vector<boost::posix_time::ptime> vPTimeTo(numWeeks, pTimeCri);
						{
							for (int j = 0; j < numWeeks; j++) {
								int k = myCommandData.postWeek + j;
								vPTimeFrom[j] = vPTimeFrom[j] - boost::gregorian::days(7 * k);
								vPTimeTo[j] = vPTimeTo[j] - boost::gregorian::days(7 * k);
								// if (i == 2) {
								// 	std::cout << vPTimeFrom[j] << "\n";
								// }
							}
							for (int j = 0; j < numWeeks; j++) {
								vPTimeFrom[j] = vPTimeFrom[j] + boost::posix_time::minutes(i);
								vPTimeTo[j] = vPTimeTo[j] + boost::posix_time::minutes(i);
								// if (i == 2) {
								// 	std::cout << vPTimeFrom[j] << "\n";
								// }
							}
							for (int j = 0; j < numWeeks; j++) {
								vPTimeFrom[j] = vPTimeFrom[j] + boost::posix_time::minutes(myCommandData.preTime);
								vPTimeTo[j] = vPTimeTo[j] + boost::posix_time::minutes(myCommandData.postTime);
								// if (i == 2) {
								// 	std::cout << vPTimeFrom[j] << "\n";
								// }
							}
						}
						// dataFactorPinsの中でこの判別時間内のものを探す．一致したら対応するindexをインクリメントする
						for (int j = 0; j < (int)dataFactorPins.size(); j++) {
							bool check = false;
							{
								for (int k = 0; k < numWeeks; k++) {
									if (dataFactorPins[j].pTime >= vPTimeFrom[k] && dataFactorPins[j].pTime < vPTimeTo[k]) {
										check = true;
										break;
									}
								}
							}
							if (check) {
								vNumPin[i][dataFactorPins[j].index - 1]++;
							}
						}
					}
				}
			}
			// discreteTimeWidthの倍数の時刻以外のところを定める
			{
				for (int i = 0; i < numT; i++) {
					if (i % myCommandData.discreteTimeWidth != 0) {
						int iBase = (i / (int)myCommandData.discreteTimeWidth) * myCommandData.discreteTimeWidth;
						for (int j = 0; j < (int)myData.numCell; j++) {
							vNumPin[i][j] = vNumPin[iBase][j];
						}
					}
				}
			}
		}

		// dataFactorEvalを一つづつ比較していく
		{
			for (int i = 0; i < (int)dataFactorEval.size(); i++) {
				int j = getStep(dataFactorEval[i].pTime);
				int k = dataFactorEval[i].index - 1;
				if (vNumPin[j][k] >= myCommandData.threshold) {
					vNumGotCustomerNearPins[j]++;
				}else{
					vNumGotCustomerFarFromPins[j]++;
				}
			}
		}
	}

	std::cout <<  "ピンの近くの総計 : " << std::accumulate(vNumGotCustomerNearPins.begin(), vNumGotCustomerNearPins.end(), 0.0) << "\n";
	std::cout <<  "ピンの近く以外の総計 : " << std::accumulate(vNumGotCustomerFarFromPins.begin(), vNumGotCustomerFarFromPins.end(), 0.0) << "\n";
	// 保存
	{
		{
			// 設定値保存ファイル名
			const std::string fileName = "vNumGotCustomerNearPins.xml";
			// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/1_Cron/Other";
			mySaveVData(vNumGotCustomerNearPins, fileName, fileDire);
		}
		{
			// 設定値保存ファイル名
			const std::string fileName = "vNumGotCustomerFarFromPins.xml";
			// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/1_Cron/Other";
			mySaveVData(vNumGotCustomerFarFromPins, fileName, fileDire);
		}
	}
    return EXIT_SUCCESS;
}
