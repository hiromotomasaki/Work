/**
 * main.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "main.hpp"

using namespace hiro;

// 【この関数の目的】
// Requestごとに実行するGreedyのプログラム

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
		}
	}
	return ret;
}

void myDisplayMyData(MyData &myData);
void myDisplayMyData(MyData &myData)
{
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

void myDisplayCommandlineData(double maxDistanceFromPreposition, int idTaxi, GeographicCoordinatePair &gCoorPairTaxi);
void myDisplayCommandlineData(double maxDistanceFromPreposition, int idTaxi, GeographicCoordinatePair &gCoorPairTaxi)
{
	std::cout << "maxDistanceFromPreposition : " << "[" << maxDistanceFromPreposition << "]" << "\n";
	std::cout << "idTaxi : " << "[" << idTaxi << "]" << "\n";
	std::cout << "gCoorPairTaxi : ";
	std::cout << "[";
    gCoorPairTaxi.printStr();
	std::cout << "]" << "\n";
}

MyData2 myReadMyData2( const std::string &fileName, const std::string &fileDire );
MyData2 myReadMyData2( const std::string &fileName, const std::string &fileDire )
{
	MyData2 ret;
	{
		std::string fileRela = fileDire + "/" + fileName;
		// create an empty property tree
		boost::property_tree::ptree pt;
		// read the xml file
		boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
		{
			{
				int N = 0;
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "table.vIndex" ).begin();
					itr_last = pt.get_child( "table.vIndex" ).end();
					N = std::distance(itr_first, itr_last);
				}
				// 要素の追加する領域を確保
				ret.vIndex.reserve(N);
				ret.vDemand.reserve(N);
				// 値の追加
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "table.vIndex" ).begin();
					itr_last = pt.get_child( "table.vIndex" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						ret.vIndex.push_back( it->second.get<int>("value") );
						ret.vDemand.push_back( it->second.get<int>("demand") );
					}
				}
			}
		}
	}
	return ret;
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

int myCalculateDir(double theta);
int myCalculateDir(double theta)
{
	int dir = 2;
	{
		if((theta >= 0) && (theta < 22.5)) {
			dir = 2;
		}else if((theta >= 22.5) && (theta < 67.5 )) {
			dir = 3;
		}else if((theta >= 67.5) && (theta < 112.5 )) {
			dir = 4;
		}else if((theta >= 112.5) && (theta < 157.5 )) {
			dir = 5;
		}else if((theta >= 157.5) && (theta < 202.5 )) {
			dir = 6;
		}else if((theta >= 202.5) && (theta <247.5 )) {
			dir = 7;
		}else if((theta >= 247.5) && (theta < 292.5 )) {
			dir = 8;
		}else if((theta >= 292.5) && (theta < 337.5 )) {
			dir = 9;
		}else{
			dir = 2;
		}
	}
	// {
	// 	if((theta >= 0) && (theta < 22.5)) {
	// 		dir = 2;
	// 	}else if((theta >= 45 - 22.5) && (theta < 45 + 22.5 )) {
	// 		dir = 3;
	// 	}else if((theta >= 90 - 22.5) && (theta < 90 + 22.5 )) {
	// 		dir = 4;
	// 	}else if((theta >= 135 - 22.5) && (theta < 135 + 22.5 )) {
	// 		dir = 5;
	// 	}else if((theta >= 180 - 22.5) && (theta < 180 + 22.5 )) {
	// 		dir = 6;
	// 	}else if((theta >= 225 - 22.5) && (theta < 225 + 22.5 )) {
	// 		dir = 7;
	// 	}else if((theta >= 270 - 22.5) && (theta < 270 + 22.5 )) {
	// 		dir = 8;
	// 	}else if((theta >= 315 - 22.5) && (theta < 315 + 22.5 )) {
	// 		dir = 9;
	// 	}else{
	// 		dir = 2;
	// 	}
	// }
	return dir;
}

std::vector<int> myReadVIndexSearch(std::string &fileName, std::string &fileDire);
std::vector<int> myReadVIndexSearch(std::string &fileName, std::string &fileDire)
{
	std::vector<int> ret;
	{
		std::string fileRela = fileDire + "/" + fileName;
		{
			// create an empty property tree
			boost::property_tree::ptree pt;
			// read the xml file
			boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
			{
				// 数を調べる
				int N = 0;
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "table" ).begin();
					itr_last = pt.get_child( "table" ).end();
					N = std::distance(itr_first, itr_last);
				}
				ret.reserve(N);
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "table" ).begin();
					itr_last = pt.get_child( "table" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						ret.push_back(it->second.get<int>(""));
					}
				}
			}
		}
	}
	return ret;
}

void mySearchIndexTarget(std::vector<int> &v1, std::vector<int> &v2, std::vector<int> &v3, int &indexTarget, int &valueTarget);
void mySearchIndexTarget(std::vector<int> &v1, std::vector<int> &v2, std::vector<int> &v3, int &indexTarget, int &valueTarget)
{
	// v2に含まれる要素の中でv1の先頭により近い値をindexTargetに格納する．
	// v1が探索範囲．v2が需要数のあるセル．
	// もし，v2の中にv1の要素がひとつもなければ0をindexTargetに格納する．
	{
		std::vector<int>::iterator it;
		it = find_first_of(v1.begin(), v1.end(), v2.begin(), v2.end());
		if (it != v1.end()) {
			indexTarget = *it;
			std::vector<int> vHoge(1, indexTarget);
			std::vector<int>::iterator itHoge;
			itHoge = find_first_of(v2.begin(), v2.end(), vHoge.begin(), vHoge.end());
			std::size_t i = std::distance(v2.begin(), itHoge);
			valueTarget = v3[i];
		}else{
			indexTarget = 0;
			valueTarget = -1;
		}
	}
}

void myCreateValue(const std::string &fileName, const std::string &fileDire, const int value);
void myCreateValue(const std::string &fileName, const std::string &fileDire, const int value)
{
	// 保存path
	std::string fileRela = fileDire + "/" + fileName;
	// create an empty property tree
	boost::property_tree::ptree pt;

	// create the root element
	boost::property_tree::ptree& root = pt.put("table", "");

	// add child elements
	{
		root.put("value", value);
	}

	// output
	boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
}

int main(int argc, char *argv[])
{
    // argv[0] : ./Bin/main.out
	// タクシーの進行方向を計算するときに使用する二点間の座標間の距離の最大値．これを越すとオブジェクトは円の中から探索して選ばれる(dir1に相当)．越さなければ進行方向(dir2からdir9)の扇型の中から探索して選ばれる
	// argv[1] : 3000 (double 型のmaxDistanceFromPreposition)
	// argv[2] : 1 (taxiのid番号)
	// argv[3] : 34.222080 (latitude of prePosition)
	// argv[4] : 135.20 (longitude of prePosition)
	// argv[5] : 34.222080 (latitude of curPosition)
	// argv[6] : 135.20 (longitude of curPosition)

	// example of input at commandline : ./Bin/main.out 3000 1 34.222080 135.20 34.222080 135.20

	if (argc != 7) {
		std::cout << "argc must be 7. failure!" << "\n";
		return EXIT_FAILURE;
	}

    double maxDistanceFromPreposition = 0;
	{
		try
        {
			maxDistanceFromPreposition = boost::lexical_cast<double>(argv[1]);
        }
        catch(boost::bad_lexical_cast &)
        {
			std::cout << "argv[1](maxDistanceFromPreposition) must be a type of double. failure!" << "\n";
			return EXIT_FAILURE;
        }
	}

	int idTaxi = 0;
	std::string idTaxiStr = "0";
	// idの取得
	{
		try
        {
			idTaxi = boost::lexical_cast<int>( argv[2] );
        }
        catch(boost::bad_lexical_cast &)
        {
			std::cout << "argv[2](idTaxi) must be a type of int. failure!" << "\n";
			return EXIT_FAILURE;
        }
		idTaxiStr = boost::lexical_cast<std::string>( idTaxi );
	}

	// タクシーの時間前後の位置情報の取得
	GeographicCoordinatePair gCoorPairTaxi;
	{
		double preLat = 0;
		double preLon = 0;
		double curLat = 0;
		double curLon = 0;
		{
			try
			{
				preLat = boost::lexical_cast<double>( argv[3] );
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[3](preLat) must be a type of double. failure!" << "\n";
				return EXIT_FAILURE;
			}
			try
			{
				preLon = boost::lexical_cast<double>( argv[4] );
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[4](preLon) must be a type of double. failure!" << "\n";
				return EXIT_FAILURE;
			}
			try
			{
				curLat = boost::lexical_cast<double>( argv[5] );
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[5](curLat) must be a type of double. failure!" << "\n";
				return EXIT_FAILURE;
			}
			try
			{
				curLon = boost::lexical_cast<double>( argv[6] );
			}
			catch(boost::bad_lexical_cast &)
			{
				std::cout << "argv[6](curLon) must be a type of double. failure!" << "\n";
				return EXIT_FAILURE;
			}
		}

		gCoorPairTaxi.set( preLat, preLon, curLat, curLon );
	}

	// // 入力情報に基づく表示
    // myDisplayCommandlineData(maxDistanceFromPreposition, idTaxi, gCoorPairTaxi);

	// 0_2_Setからパラメータを受け取る
	MyData myData;
	{
		// ファイル名
		const std::string fileName = "inputDataFor2_ForEachRequest.xml";
		// ディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other";
		myData = myReadMyData( fileName, fileDire );
	}

	// // 表示
	// myDisplayMyData(myData);

	// gCoorPairTaxiをindexTaxiに変換
	int indexTaxi = 0;
	{
		int row = calculateRowFromLatitudes( gCoorPairTaxi.getSecondPhi(), myData.gCoorPair.getFirstPhi(), myData.gCoorPair.getSecondPhi(), myData.cellSizePhi );
		int col = calculateColFromLongitudes( gCoorPairTaxi.getSecondLambda(), myData.gCoorPair.getFirstLambda(), myData.gCoorPair.getSecondLambda(), myData.cellSizeLambda );
		indexTaxi = calculateIndexFromRowCol( row, col, myData.numRow, myData.numCol );
	}
	if (indexTaxi == 0) {
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyValue.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + "_value.xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
			return EXIT_SUCCESS;
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
	}

	// // 表示
	// std::cout << "indexTaxi : " << indexTaxi << "\n";

	// indexTaxiが有効かどうか確認
	bool isValid = false;
	{
		// 読み込みファイル名
		std::string fileName = "isValid.xml";
		// 読み込みファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi );
		std::string fileRela = fileDire + "/" + fileName;
		{
			// create an empty property tree
			boost::property_tree::ptree pt;
			// read the xml file
			boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
			{
				isValid = pt.get<bool>("table.isValid");
			}
		}
	}

	// // 表示
	// std::cout << "isValid : " << isValid << "\n";

	if (!isValid) {
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyValue.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + "_value.xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
			return EXIT_SUCCESS;
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
	}

	// indexTaxiがサークルの上にいるかどうか確認
	bool isOnCircle = false;
	{
		// 読み込みファイル名
		std::string fileName = boost::lexical_cast<std::string>( indexTaxi ) + ".xml";
		// 読み込みファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/1_Cron/Other";
		std::string fileRela = fileDire + "/" + fileName;
		{
			// create an empty property tree
			boost::property_tree::ptree pt;
			// read the xml file
			boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
			{
				isOnCircle = pt.get<bool>("table.vDemandCircleFlag");
			}
		}
	}

	if (isOnCircle) {
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyValue.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + "_value.xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
			return EXIT_SUCCESS;
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
	}

	// 以下，idTaxiがいるindexTaxiは営業領域内の有効なセルである．

	// indexNotLessThanThreshold.xmlとindexUnderThreshold.xmlの取得
	MyData2 myData2NotLessThanThreshold;
	MyData2 myData2UnderThreshold;
	{
		{
			const std::string fileName = "indexNotLessThanThreshold.xml";
			const std::string fileDire = "./../Data/1_Cron/Other";
			myData2NotLessThanThreshold = myReadMyData2( fileName, fileDire );
		}
		{
			const std::string fileName = "indexUnderThreshold.xml";
			const std::string fileDire = "./../Data/1_Cron/Other";
			myData2UnderThreshold = myReadMyData2( fileName, fileDire );
		}
	}

	// 表示
	// myDisplayVIndexNotLessThanThreshold(myData2NotLessThanThreshold.vIndex);

    // // 表示
	// myDisplayVIndexUnderThreshold(myData2UnderThreshold.vIndex);

	// しきい値以上の需要があるセルがあるか
	bool checkNLTT = false;
	// しきい値未満の需要があるセルがあるか
	bool checkUT = false;
	// Targetとなりうるセルが存在するか
	bool checkDemand = false;
	{
		{
			int N = myData2NotLessThanThreshold.vIndex.size();
			if (N != 1) {
				checkNLTT = true;
			}
		}
		{
			int N = myData2UnderThreshold.vIndex.size();
			if (N != 1) {
				checkUT = true;
			}
		}
		checkDemand = checkNLTT || checkUT;
	}

	if (!checkDemand) {
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyValue.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + "_value.xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
			return EXIT_SUCCESS;
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
	}

	// // 表示
	// std::cout << "checkDemand : " << checkDemand << "\n";

	// 以下，営業領域内にオブジェクトで刺される可能性のあるターゲットとなっているセルが少なくともひとつ存在している

	// 探索範囲の方向を取得
	int dir = 1;
	{
		TotalStation tStationHoge = calculateTStationFromGCoor( gCoorPairTaxi.getFirst(), gCoorPairTaxi.getSecond() );
		if (tStationHoge.getLength() > maxDistanceFromPreposition || tStationHoge.getLength() == 0) {
			dir = 1;
		}else{
			dir = myCalculateDir(tStationHoge.getAzimuth());
		}
	}

	// // 表示
	// std::cout << "dir : " << dir << "\n";

	// indexTaxiに対するindexTargetを求める
	int indexTarget = 0;
	int valueTarget = -1;
	bool gotIndexTarget = false;
	{
		// 探索範囲を取得
		std::vector<int> vIndexSearch;
		{
			// ファイル名
			std::string fileName = "dir" + boost::lexical_cast<std::string>( dir ) + ".xml";
			// ディレクトリのmakefileからの相対位置
			std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi ) + "/SearchRange";
			vIndexSearch = myReadVIndexSearch(fileName, fileDire);
		}

		// {
		// 	int N = vIndexSearch.size();
		// 	for (int i = 0; i < N; i++) {
		// 		std::cout << "[" << vIndexSearch[i] << "]";
		// 	}
		// 	std::cout << "\n";
		// }

		// {
		// 	int N = myData2NotLessThanThreshold.vIndex.size();
		// 	for (int i = 0; i < N; i++) {
		// 		std::cout << "[" << myData2NotLessThanThreshold.vIndex[i] << "]";
		// 	}
		// 	std::cout << "\n";
		// }

		// {
		// 	int N = myData2NotLessThanThreshold.vIndex.size();
		// 	for (int i = 0; i < N; i++) {
		// 		std::cout << "[" << myData2NotLessThanThreshold.vDemand[i] << "]";
		// 	}
		// 	std::cout << "\n";
		// }

		// {
		// 	int N = myData2UnderThreshold.vIndex.size();
		// 	for (int i = 0; i < N; i++) {
		// 		std::cout << "[" << myData2UnderThreshold.vIndex[i] << "]";
		// 	}
		// 	std::cout << "\n";
		// }

		// {
		// 	int N = myData2UnderThreshold.vIndex.size();
		// 	for (int i = 0; i < N; i++) {
		// 		std::cout << "[" << myData2UnderThreshold.vDemand[i] << "]";
		// 	}
		// 	std::cout << "\n";
		// }

		mySearchIndexTarget(vIndexSearch, myData2NotLessThanThreshold.vIndex, myData2NotLessThanThreshold.vDemand, indexTarget, valueTarget);
		if (indexTarget == 0) {
			mySearchIndexTarget(vIndexSearch, myData2UnderThreshold.vIndex, myData2UnderThreshold.vDemand, indexTarget, valueTarget);
			if (indexTarget != 0) {
				gotIndexTarget = true;
			}
		}else{
			gotIndexTarget = true;
		}
	}

	if (!gotIndexTarget) {
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyValue.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + "_value.xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
			return EXIT_SUCCESS;
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
	}

	// indexTaxiに対してindexTargetを指すオブジェクトを返せば良いことがわかった．
	// もし，indexTaxi == indexTargetなら，dirの情報を使って描画するオブジェクトを返す．

	// // 表示
	// std::cout << "indexTaxi : " << indexTaxi << "\n";

	// // 表示
	// std::cout << "indexTarget : " << indexTarget << "\n";

	// // 表示
	// std::cout << "valueTarget : " << valueTarget << "\n";

	// 保存
	{
		if(indexTaxi != indexTarget) {
			// indexTaxiとindexTargetに対応するオブジェクト情報をコピーする
			try {
				boost::filesystem::path src("./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object/" + boost::lexical_cast<std::string>( indexTarget ) + ".xml" );
				boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
				boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
			} catch (std::exception& e) {
				std::cout << "failure!" << "\n";
				return EXIT_FAILURE;
			}
		}else{
			// indexTaxiとindexTargetとdirに対応するオブジェクト情報をコピーする
			try {
				boost::filesystem::path src("./../Data/0_2_Set/Other/InfoAboutCell/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object/" + boost::lexical_cast<std::string>( indexTarget ) + "/dir" + boost::lexical_cast<std::string>( dir ) + ".xml" );
				boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
				boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
			} catch (std::exception& e) {
				std::cout << "failure!" << "\n";
				return EXIT_FAILURE;
			}
		}

		// valueTargetの保存
		{
			// 保存ファイル名
			const std::string fileName = "id_" + idTaxiStr + "_value.xml";
			// ディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/2_ForEachRequest/Other";
			myCreateValue(fileName, fileDire, valueTarget);
		}
	}
    return EXIT_SUCCESS;
}
