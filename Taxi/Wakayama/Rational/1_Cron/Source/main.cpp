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
	// 需要数カウントのための情報
	std::vector<std::vector<int>> vDemandCountCircleIndex;
	// 空車数カウントのための情報
	std::vector<std::vector<int>> vVacantCountCircleIndex;
	// 円が重ならないようにするための情報
	std::vector<std::vector<int>> vFlagSetCircleIndex;
};

struct MyData2
{
	std::vector<int> vIndex;
	std::vector<int> vDemand;
};

struct PairIndexDemand {
    int index;
    int demand;
};
class MyNotLessDefinition {
public:
    bool operator()(const PairIndexDemand& a, const PairIndexDemand& b)
    {
		if( a.demand == b.demand ) {
			return a.index < b.index;
		}
		return a.demand > b.demand;
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
			{
				int N = ret.numCell;
				// 要素の追加する領域を確保
				ret.vDemandCountCircleIndex.reserve(N);
				// 値の追加
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData.vDemandCountCircleIndex" ).begin();
					itr_last = pt.get_child( "myData.vDemandCountCircleIndex" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						std::vector<int> vHoge;
						{
							boost::property_tree::ptree::iterator itr_first_c, itr_last_c, it_c;
							itr_first_c = it->second.get_child( "" ).begin();
							itr_last_c = it->second.get_child( "" ).end();
							std::size_t NHoge = std::distance(itr_first_c, itr_last_c);
							vHoge.reserve(NHoge);
							for(it_c = itr_first_c; it_c != itr_last_c; ++it_c) {
								vHoge.push_back(it_c->second.get<int>("j"));
							}
						}
						ret.vDemandCountCircleIndex.push_back(vHoge);
					}
				}
			}
			{
				int N = ret.numCell;
				// 要素の追加する領域を確保
				ret.vVacantCountCircleIndex.reserve(N);
				// 値の追加
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData.vVacantCountCircleIndex" ).begin();
					itr_last = pt.get_child( "myData.vVacantCountCircleIndex" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						std::vector<int> vHoge;
						{
							boost::property_tree::ptree::iterator itr_first_c, itr_last_c, it_c;
							itr_first_c = it->second.get_child( "" ).begin();
							itr_last_c = it->second.get_child( "" ).end();
							std::size_t NHoge = std::distance(itr_first_c, itr_last_c);
							vHoge.reserve(NHoge);
							for(it_c = itr_first_c; it_c != itr_last_c; ++it_c) {
								vHoge.push_back(it_c->second.get<int>("j"));
							}
						}
						ret.vVacantCountCircleIndex.push_back(vHoge);
					}
				}
			}
			{
				int N = ret.numCell;
				// 要素の追加する領域を確保
				ret.vFlagSetCircleIndex.reserve(N);
				// 値の追加
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData.vFlagSetCircleIndex" ).begin();
					itr_last = pt.get_child( "myData.vFlagSetCircleIndex" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						std::vector<int> vHoge;
						{
							boost::property_tree::ptree::iterator itr_first_c, itr_last_c, it_c;
							itr_first_c = it->second.get_child( "" ).begin();
							itr_last_c = it->second.get_child( "" ).end();
							std::size_t NHoge = std::distance(itr_first_c, itr_last_c);
							vHoge.reserve(NHoge);
							for(it_c = itr_first_c; it_c != itr_last_c; ++it_c) {
								vHoge.push_back(it_c->second.get<int>("j"));
							}
						}
						ret.vFlagSetCircleIndex.push_back(vHoge);
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

void myDisplayCircleIndex(std::vector<std::vector<int>> &V);
void myDisplayCircleIndex(std::vector<std::vector<int>> &V)
{
	for (int i = 0; i < (int)V.size(); i++) {
		int N = V[i].size();
		if (N >= 2) {
			std::cout << "[" << i+1 << ", num = " << N-1 << "] : ";
			for (int j = 0; j < N; j++) {
				std::cout << V[i][j] << ", " ;
			}
			std::cout << "\n";
		}
	}
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

void myDisplayVVacantTaxi(std::vector<int> &vVacantTaxi);
void myDisplayVVacantTaxi(std::vector<int> &vVacantTaxi)
{
	int sumVacantTaxi = std::accumulate(vVacantTaxi.begin(), vVacantTaxi.end(), 0);
	std::cout << "総空車数 : " << sumVacantTaxi << "\n";
	if (sumVacantTaxi > 0) {
		std::cout << "空車数の確認" << "\n";
		int N = vVacantTaxi.size();
		for (int i = 0; i < N; i++) {
			if (vVacantTaxi[i] >= 1) {
				std::cout << "[index]" << i+1 << ", [numVacant]" << vVacantTaxi[i] << "\n";
			}
		}
	}
}

void myDisplayVDemandCircle(std::vector<int> &vDemandCircle);
void myDisplayVDemandCircle(std::vector<int> &vDemandCircle)
{
	std::cout << "サークルでカウントした需要数の確認" << "\n";
	int N = vDemandCircle.size();
	for (int i = 0; i < N; i++) {
		if (vDemandCircle[i] >= 1) {
			std::cout << "[index]" << i+1 << ", [numDemand]" << vDemandCircle[i] << "\n";
		}
	}
}

void myDisplayVVacantTaxiCircle(std::vector<int> &vVacantTaxiCircle);
void myDisplayVVacantTaxiCircle(std::vector<int> &vVacantTaxiCircle)
{
	std::cout << "サークルでカウントした空車数の確認" << "\n";
	int N = vVacantTaxiCircle.size();
	for (int i = 0; i < N; i++) {
		if (vVacantTaxiCircle[i] >= 1) {
			std::cout << "[index]" << i+1 << ", [numVacant]" << vVacantTaxiCircle[i] << "\n";
		}
	}
}

void myDisplayVDemandIndexNotLessThanCircleDisplayThreshold(std::vector<PairIndexDemand> &vDemandIndexNotLessThanCircleDisplayThreshold, MyData &myData);
void myDisplayVDemandIndexNotLessThanCircleDisplayThreshold(std::vector<PairIndexDemand> &vDemandIndexNotLessThanCircleDisplayThreshold, MyData &myData)
{
	std::cout << "サークルでカウントした需要数の確認" << "\n";
	int N = vDemandIndexNotLessThanCircleDisplayThreshold.size();
	for (int i = 0; i < N; i++) {
		int index = vDemandIndexNotLessThanCircleDisplayThreshold[i].index;
		int demand = vDemandIndexNotLessThanCircleDisplayThreshold[i].demand;
		// indexを位置情報に変換する
		int row = calculateRowFromIndex( index, myData.numCol, myData.numCell );
		int col = calculateColFromIndex( index, myData.numCol, myData.numCell );
		GeographicCoordinate gCoorHoge;
		gCoorHoge.setPhi( myData.gCoorPair.getFirstPhi() - row * myData.cellSizePhi + myData.cellSizePhi / 2.0 );
		gCoorHoge.setLambda( myData.gCoorPair.getFirstLambda() + col * myData.cellSizeLambda - myData.cellSizeLambda / 2.0 );
		std::cout << "[index]" << index << ", [numDemand]" << demand << ", [latitude]" << gCoorHoge.getPhi() << ", [longitude]" << gCoorHoge.getLambda() << "\n";
	}
}

void myUpdateVFlags(std::vector<bool> &vFlag, std::vector<int> &vIndex);
void myUpdateVFlags(std::vector<bool> &vFlag, std::vector<int> &vIndex)
{
	int N = vIndex.size();
	for (int i = 1; i < N; i++) {
		int indexMark = vIndex[i];
		vFlag[indexMark - 1] = true;
	}
}

void myDisplayCirclePairIndexDemand(std::vector<PairIndexDemand> &v, std::vector<bool> &vFlag);
void myDisplayCirclePairIndexDemand(std::vector<PairIndexDemand> &v, std::vector<bool> &vFlag)
{
	int N = vFlag.size();
	for (int i = 0; i < N; i++) {
		if (vFlag[i]) {
			std::cout << "index : " << "[" << v[i].index << "], " << "demand : " << v[i].demand << "\n";
		}
	}
}

void mySaveVDemandCircleFlag( std::vector<bool> &v, std::vector<bool> &vIsValid );
void mySaveVDemandCircleFlag( std::vector<bool> &v, std::vector<bool> &vIsValid )
{
	int N = v.size();
	for (int i = 0; i < N; i++) {
		if (vIsValid[i]) {
			// 保存ファイル名
			const std::string fileName = boost::lexical_cast<std::string>( i + 1 ) + ".xml";
			// 保存ファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/1_Cron/Other";
			// 保存path
			std::string fileRela = fileDire + "/" + fileName;
			// create an empty property tree
			boost::property_tree::ptree pt;

			// create the root element
			boost::property_tree::ptree& root = pt.put("table", "");

			// add child elements
			{
				{
					root.put("vDemandCircleFlag", v[i]);
				}
			}

			// output
			boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
		}
	}
}

void mySaveDisplayCircleInfo(std::vector<PairIndexDemand> &v, std::vector<bool> &vFlag, MyData &myData, int numCircleDisplay);
void mySaveDisplayCircleInfo(std::vector<PairIndexDemand> &v, std::vector<bool> &vFlag, MyData &myData, int numCircleDisplay)
{
	if (numCircleDisplay != 0) {
		std::vector<GeographicCoordinate> vRepresentativePoint(numCircleDisplay);
		int N = vFlag.size();
		int j = 0;
		for (int i = 0; i < N; i++) {
			if (vFlag[i]) {
				int indexCircle = v[i].index;
				int row = calculateRowFromIndex( indexCircle, myData.numCol, myData.numCell );
				int col = calculateColFromIndex( indexCircle, myData.numCol, myData.numCell );
				vRepresentativePoint[j].setPhi( myData.gCoorPair.getFirstPhi() - row * myData.cellSizePhi + myData.cellSizePhi / 2.0 );
				vRepresentativePoint[j].setLambda( myData.gCoorPair.getFirstLambda() + col * myData.cellSizeLambda - myData.cellSizeLambda / 2.0 );
				j++;
			}
		}
		// 保存
		{
			// 保存ファイル名
			const std::string fileName = "circlePoints.xml";
			// 保存ファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/1_Cron/Other";
			// 保存path
			std::string fileRela = fileDire + "/" + fileName;
			// create an empty property tree
			boost::property_tree::ptree pt;

			// create the root element
			boost::property_tree::ptree& root = pt.put("table", "");

			// add child elements
			{
				for (int i = 0; i < numCircleDisplay; i++) {
					boost::property_tree::ptree& child = root.add("data", "");
					child.put("latitude", vRepresentativePoint[i].getPhi());
					child.put("longitude", vRepresentativePoint[i].getLambda());
				}
			}
			// output
			boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
		}
	}else{
		// 空のオブジェクトファイルの生成
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyCircle.xml");
			boost::filesystem::path dst("./../Data/1_Cron/Other/circlePoints.xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			// return EXIT_FAILURE;
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
	// argv[1] : 4 (unsigned int 型のしきい値．circleDisplayThreshold)
	// argv[2] : 1 (unsigned int 型のしきい値．minCircleKariDemand)
	// argv[3] : 8 (unsigned int 型のしきい値．circleKariDemandThreshold)

	// example of input at commandline : ./Bin/main.out 2 1 4

	if (argc != 4) {
		std::cout << "argc must be 4. failure!" << "\n";
		return EXIT_FAILURE;
	}

	unsigned int circleDisplayThreshold = 0;
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
		circleDisplayThreshold = thresholdHoge;
	}

	unsigned int minCircleKariDemand = 0;
	{
		int thresholdHoge = 0;
		try
        {
			thresholdHoge = boost::lexical_cast<int>(argv[2]);
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
		minCircleKariDemand = thresholdHoge;
	}

	unsigned int circleKariDemandThreshold = 0;
	{
		int thresholdHoge = 0;
		try
        {
			thresholdHoge = boost::lexical_cast<int>(argv[3]);
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
		circleKariDemandThreshold = thresholdHoge;
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

	// // 表示
	// myDisplayCircleIndex(myData.vDemandCountCircleIndex);

    // // 表示
	// myDisplayCircleIndex(myData.vVacantCountCircleIndex);

	// // 表示
	// myDisplayCircleIndex(myData.vFlagSetCircleIndex);

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

	// 空車数を取得する
	std::vector<int> vVacantTaxi(myData.numCell, 0);
	{
		// taxiPositions.xmlを取得
		// create an empty property tree
		boost::property_tree::ptree pt;
		{
			// 設定値読み込みファイル名
			const std::string fileName = "taxiPositions.xml";
			// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/1_Cron/FromServer";
			std::string fileRela = fileDire + "/" + fileName;
			// read the xml file
			boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
		}

		// メモ
		// table-date-id
		//           -status
		//           -position-latitude
		//                    -longitude

		// ptを解析していき空車数をカウントしていく
		{
			boost::property_tree::ptree::iterator itr_first, itr_last, it;
			itr_first = pt.get_child( "table" ).begin();
			itr_last = pt.get_child( "table" ).end();
			for(it = itr_first; it != itr_last; it++) {
				// statusを取得
				int statusHoge = it->second.get<int>("status");
				if (statusHoge == 0) {
					// 空車であれば
					// positionを取得
					GeographicCoordinate gCoorHoge;
					gCoorHoge.setPhi( it->second.get<double>("position.latitude") );
					gCoorHoge.setLambda( it->second.get<double>("position.longitude") );
					int row = calculateRowFromLatitudes( gCoorHoge.getPhi(), myData.gCoorPair.getFirstPhi(), myData.gCoorPair.getSecondPhi(), myData.cellSizePhi );
					int col = calculateColFromLongitudes( gCoorHoge.getLambda(), myData.gCoorPair.getFirstLambda(), myData.gCoorPair.getSecondLambda(), myData.cellSizeLambda );
					int index = calculateIndexFromRowCol( row, col, myData.numRow, myData.numCol );
					// 範囲外だとindex = 0になる
					if( index != 0 ) {
						int i = index - 1;
						// 有効なセルかどうか
						bool isValid = myData.vIsValid[i];
						if (isValid) {
							vVacantTaxi[i]++;
						}
					}
				}
			}
		}
	}

	// // 表示
	// myDisplayVVacantTaxi(vVacantTaxi);

	// 有効な各セルでサークル内の需要数と空車数を取得
	std::vector<int> vDemandCircle(myData.numCell, 0);
	std::vector<int> vVacantTaxiCircle(myData.numCell, 0);
	{
		for (int i = 0; i < myData.numCell; i++) {
			if (myData.vIsValid[i]) {
				{
					int M = myData.vDemandCountCircleIndex[i].size();
					for (int j = 1; j < M; j++) {
						int indexCount = myData.vDemandCountCircleIndex[i][j];
						vDemandCircle[i] += vDemand[indexCount - 1];
					}
				}
				{
					int M = myData.vVacantCountCircleIndex[i].size();
					for (int j = 1; j < M; j++) {
						int indexCount = myData.vVacantCountCircleIndex[i][j];
						vVacantTaxiCircle[i] += vVacantTaxi[indexCount - 1];
					}
				}
			}
		}
	}

	// // 表示
	// myDisplayVDemandCircle(vDemandCircle);

	// // 表示
	// myDisplayVVacantTaxiCircle(vVacantTaxiCircle);

	// vDemandCircleから需要数がcircleDisplayThreshold以上あるものを取り出してきて大きい順に並べ替える．
	std::vector<PairIndexDemand> vDemandIndexNotLessThanCircleDisplayThreshold;
	{
		for (int i = 0; i < myData.numCell; i++) {
			if (myData.vIsValid[i]) {
				if (vDemandCircle[i] >= (int)circleDisplayThreshold) {
					PairIndexDemand pairHoge;
					pairHoge.index = i + 1;
					pairHoge.demand = vDemandCircle[i];
					vDemandIndexNotLessThanCircleDisplayThreshold.push_back(pairHoge);
				}
			}
		}
		// 需要に関してソート
		{
			std::vector<PairIndexDemand>::iterator it, itr_first, itr_last;
			itr_first = vDemandIndexNotLessThanCircleDisplayThreshold.begin();
			itr_last = vDemandIndexNotLessThanCircleDisplayThreshold.end();
			sort(itr_first, itr_last, MyNotLessDefinition());
		}
	}

	// // サークルの需要数の確認
    // myDisplayVDemandIndexNotLessThanCircleDisplayThreshold(vDemandIndexNotLessThanCircleDisplayThreshold, myData);

	// サークルを描画するかどうか決めるフラグ配列の準備
	std::vector<bool> vCircleDisplayFlag((int)vDemandIndexNotLessThanCircleDisplayThreshold.size(), false);

	// 需要サークルのフラグ
	std::vector<bool> vDemandCircleFlag(myData.numCell, false);
	// サークルの重なりを防ぐためのフラグ
	std::vector<bool> vFlagSetCircleIndexFlag(myData.numCell, false);

	// vDemandIndexNotLessThanCircleDisplayThresholdの中で描画する円を決定する．
	int numCircleDisplay = 0;
	{
		int N = vDemandIndexNotLessThanCircleDisplayThreshold.size();
		for (int i = 0; i < N; i++) {
			int indexCircleCenter = vDemandIndexNotLessThanCircleDisplayThreshold[i].index;
			bool hasMarked = vFlagSetCircleIndexFlag[indexCircleCenter - 1];
			if (!hasMarked) {
				// 描画をする(vCircleDisplayFlag[i])
				vCircleDisplayFlag[i] = true;
				numCircleDisplay++;
				// 需要サークル内の複数のindexのフラグをtrueにする(vDemandCircleFlag)
				myUpdateVFlags(vDemandCircleFlag, myData.vDemandCountCircleIndex[indexCircleCenter - 1]);
				// 円重複描画を防ぐためのサークル内の複数のindexのフラグをtrueにする(vFlagSetCircleIndexFlag)
				myUpdateVFlags(vFlagSetCircleIndexFlag, myData.vFlagSetCircleIndex[indexCircleCenter - 1]);
			}
		}
	}

	// // 表示
	// myDisplayCirclePairIndexDemand(vDemandIndexNotLessThanCircleDisplayThreshold, vCircleDisplayFlag);

	// vDemandCircleFlagの保存
	mySaveVDemandCircleFlag(vDemandCircleFlag, myData.vIsValid);

	// 描画するサークルの保存(numCircleDisplayがゼロのときは例外処理)
	mySaveDisplayCircleInfo(vDemandIndexNotLessThanCircleDisplayThreshold, vCircleDisplayFlag, myData, numCircleDisplay);

	// 需要数-空車数を仮需要数と呼ぶ．circleKariDemandThreshold以上の値をもつセルとcircleKariDemandThreshold未満のminCircleKariDemand以上の値を持つセルのindexの集合を求める
	MyData2 myData2NotLessThanThreshold;
    myData2NotLessThanThreshold.vIndex.reserve(numCircleDisplay+1);
    myData2NotLessThanThreshold.vIndex.push_back(-1);
    myData2NotLessThanThreshold.vDemand.reserve(numCircleDisplay+1);
    myData2NotLessThanThreshold.vDemand.push_back(-1);
	MyData2 myData2UnderThreshold;
	myData2UnderThreshold.vIndex.reserve(numCircleDisplay+1);
    myData2UnderThreshold.vIndex.push_back(-1);
	myData2UnderThreshold.vDemand.reserve(numCircleDisplay+1);
    myData2UnderThreshold.vDemand.push_back(-1);
	{
		int N = vDemandIndexNotLessThanCircleDisplayThreshold.size();
		for (int i = 0; i < N; i++) {
			if (vCircleDisplayFlag[i]) {
				int indexHoge = vDemandIndexNotLessThanCircleDisplayThreshold[i].index;
				int demandHoge = vDemandIndexNotLessThanCircleDisplayThreshold[i].demand;
				int vacantHoge = vVacantTaxiCircle[indexHoge - 1];
				int kariDemand = demandHoge - vacantHoge;
				if (kariDemand >= (int)circleKariDemandThreshold) {
					myData2NotLessThanThreshold.vIndex.push_back(indexHoge);
					myData2NotLessThanThreshold.vDemand.push_back(kariDemand);
				}else if((kariDemand < (int)circleKariDemandThreshold) && (kariDemand > (int)minCircleKariDemand)) {
					myData2UnderThreshold.vIndex.push_back(indexHoge);
					myData2UnderThreshold.vDemand.push_back(kariDemand);
				}
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
