/**
 * main.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "main.hpp"

using namespace hiro;

// 【この関数の目的】
// タクシーからの描画要求ごとに実行する．1_Cronでの結果を参照して描画オブジェクトを返す．

// メモ

// id取得
// okだったら
// そのidのファイルを読み込む
// ファイルがなければemptyObjectをコピーしてそのidでファイルを作成して終了
// 読み込んでindexTaxiを計算
// 営業領域の外枠外ならemptyObjectをコピーしてそのidでファイルを作成して終了
// indexTaxiが有効でなければemptyObjectをコピーしてそのidでファイルを作成して終了
// 位置情報のズレからdirを決定
// そのdirとindexTaxiに対応する探索範囲を取得
// 1_Cronの結果を取得
// まずNotLessThanの結果と比較してマッチするindexTargetを取得
// 取得できればそれに対応するオブジェクトをコピーして終了
// 取得できなければUnderの結果と比較してマッチするindexTargetを取得
// 取得できればそれに対応するオブジェクトをコピーして終了
// 取得できなければemptyObjectをコピーしてそのidでファイルを作成して終了

int myCalculateDir(double theta);
int myCalculateDir(double theta) {
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


int main(int argc, char *argv[])
{
	int idTaxi = 0;
	std::string idTaxiStr = "0";
	// idの取得
	{
		// 第一引数は実行ファイル名
		// 第二引数はタクシーのID番号の文字列
		if( argc != 2 ) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
		// 入力された番号の文字列を数字やstringに変換
		idTaxi = boost::lexical_cast<int>( argv[1] );
		idTaxiStr = boost::lexical_cast<std::string>( idTaxi );
	}

	// idTaxiに対応する位置情報ファイルを読み込む
	GeographicCoordinate gCoorCur;
	GeographicCoordinate gCoorPre;
	{
		// 設定値読み込みファイル名
		std::string fileName = "id_" + idTaxiStr + ".xml";
		// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/2_ForEachRequest/FromServer/TaxiPosition";
		std::string fileRela = fileDire + "/" + fileName;
		// create an empty property tree
		boost::property_tree::ptree pt;
		// read the xml file
		try {
			boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
		} catch (std::exception& e) {
			// 空のオブジェクトファイルの生成
			try {
				const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
				boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
				boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
			} catch (std::exception& e) {
				std::cout << "failure!" << "\n";
				return EXIT_FAILURE;
			}
			std::cout << "failure! copied an emtptyObject.xml" << "\n";
			return EXIT_FAILURE;
		}
		gCoorCur.set( pt.get<double>( "table.curPosition.latitude" ), pt.get<double>( "table.curPosition.longitude" ) );
		gCoorPre.set( pt.get<double>( "table.prePosition.latitude" ), pt.get<double>( "table.prePosition.longitude" ) );
	}

	// InputDataFor2_ForEachRequest.xmlからデータを読み込む
	// ------------- 取得するもの  ------------- //
	// 営業領域の外枠の最北西
	GeographicCoordinate gCoorNW;
	// 営業領域の外枠の最南東
	GeographicCoordinate gCoorSE;
	// 営業領域の外枠内にあるセル数
	int numCell;
	// 営業領域の外枠の行数
	int numRow;
	// 営業領域の外枠の列数
	int numCol;
	// セルの南北の緯度差
	double cellSizePhi;
	// セルの東西の経度差
	double cellSizeLambda;
	// タクシーの進行方向を計算するときに使用する二点間の座標間の距離の最大値．これを越すとオブジェクトは円の中から探索して選ばれる(dir1に相当)．越さなければ進行方向(dir2からdir9)の扇型の中から探索して選ばれる
	double maxDistanceFromPreposition;
	// ----------------------------------------------------- //
	{
		// 設定値読み込みファイル名
		const std::string fileName = "InputDataFor2_ForEachRequest.xml";
		// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other";
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
				numCell = pt.get<int>("table.numCell.value");
				numRow = pt.get<int>("table.numRow.value");
				numCol = pt.get<int>("table.numCol.value");
				cellSizePhi = pt.get<double>("table.cellSizePhi.value");
				cellSizeLambda = pt.get<double>("table.cellSizeLambda.value");
				maxDistanceFromPreposition = pt.get<double>("table.maxDistanceFromPreposition.value");
			}
		}
	}
	int indexTaxi = 0;
	{
		int row = calculateRowFromLatitudes( gCoorCur.getPhi(), gCoorNW.getPhi(), gCoorSE.getPhi(), cellSizePhi );
		int col = calculateColFromLongitudes( gCoorCur.getLambda(), gCoorNW.getLambda(), gCoorSE.getLambda(), cellSizeLambda );
		indexTaxi = calculateIndexFromRowCol( row, col, numRow, numCol );
		// indexTaxiが営業領域の外枠外ならemptyObjectをコピーしてそのidでファイルを作成して終了
		if (indexTaxi == 0) {
			try {
				const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
				boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
				boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
				return EXIT_SUCCESS;
			} catch (std::exception& e) {
				std::cout << "failure!" << "\n";
				return EXIT_FAILURE;
			}
		}
	}

	// indexTaxiが有効なセルなのか確認して，有効でなければemptyObject.xmlをコピーして終了
	{
		bool check = false;
		{
			// 読み込みファイル名
			std::string fileName = boost::lexical_cast<std::string>( indexTaxi ) + ".xml";
			// 読み込みファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/0_2_Set/Other/IsValid";
			std::string fileRela = fileDire + "/" + fileName;
			{
				// create an empty property tree
				boost::property_tree::ptree pt;
				// read the xml file
				boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
				{
					check = pt.get<bool>("table.isValid.value");
				}
			}
		}
		if (!check) {
			try {
				const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
				boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
				boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
				return EXIT_SUCCESS;
			} catch (std::exception& e) {
				std::cout << "failure!" << "\n";
				return EXIT_FAILURE;
			}
		}
	}
	// indexTaxiがサークル上のセルなのか確認して，サークル上であればemptyObject.xmlをコピーして終了
	{
		bool check = false;
		{
			// 読み込みファイル名
			std::string fileName = boost::lexical_cast<std::string>( indexTaxi ) + ".xml";
			// 読み込みファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/1_Cron/Other/IsOnCircle";
			std::string fileRela = fileDire + "/" + fileName;
			{
				// create an empty property tree
				boost::property_tree::ptree pt;
				// read the xml file
				boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
				{
					check = pt.get<bool>("table.isOnCircle.value");
				}
			}
		}
		if (check) {
			try {
				const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
				boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
				boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
				return EXIT_SUCCESS;
			} catch (std::exception& e) {
				std::cout << "failure!" << "\n";
				return EXIT_FAILURE;
			}
		}
	}
	// 位置情報のズレからdirを決定
	int dir = 1;
	{
		TotalStation tStation = calculateTStationFromGCoor( gCoorPre, gCoorCur );
		if (tStation.getLength() > maxDistanceFromPreposition || tStation.getLength() == 0) {
			dir = 1;
		}else{
			dir = myCalculateDir(tStation.getAzimuth());
		}
	}

    // dirとindexTaxiに対応する探索範囲を取得
	std::vector<int> searchIndex;
	{
		// 読み込みファイル名
		std::string fileName = "dir" + boost::lexical_cast<std::string>( dir ) + ".xml";
		// 読み込みファイル先のディレクトリのmakefileからの相対位置
		std::string fileDire = "./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi ) + "/SearchRange";
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
				searchIndex.reserve(N);
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "table" ).begin();
					itr_last = pt.get_child( "table" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						searchIndex.push_back(it->second.get<int>("index"));
					}
				}
			}
		}
	}
	// 1_Cronの結果を取得

	// 0 : 需要はなし
	bool check_0 = false;
	// 1 : 需要が集中しているところがある
	bool check_1 = false;
	// 2 : 少し需要があるところがある
	bool check_2 = false;

	std::vector<int> valueOfIndex(numCell, 0);
	std::vector<int> notLessThanThresholdIndex;
	std::vector<int> underThresholdIndex;
	{
		{
			// 読み込みファイル名
			const std::string fileName = "indexNotLessThanThresholdKari.xml";
			// 読み込みファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/1_Cron/Other";
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
					if (N != 1) {
						check_1 = true;
					}
					notLessThanThresholdIndex.reserve(N);
					{
						boost::property_tree::ptree::iterator itr_first, itr_last, it;
						itr_first = pt.get_child( "table" ).begin();
						itr_last = pt.get_child( "table" ).end();
						for(it = itr_first; it != itr_last; ++it) {
							notLessThanThresholdIndex.push_back(it->second.get<int>("value"));
							if (it != itr_first) {
								valueOfIndex[it->second.get<int>("value") - 1] = 1;
							}
						}
					}
				}
			}
		}
		{
			// 読み込みファイル名
			const std::string fileName = "indexUnderThresholdKari.xml";
			// 読み込みファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/1_Cron/Other";
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
					if (N != 1) {
						check_2 = true;
					}
					underThresholdIndex.reserve(N);
					{
						boost::property_tree::ptree::iterator itr_first, itr_last, it;
						itr_first = pt.get_child( "table" ).begin();
						itr_last = pt.get_child( "table" ).end();
						for(it = itr_first; it != itr_last; ++it) {
							underThresholdIndex.push_back(it->second.get<int>("value"));
							if (it != itr_first) {
								valueOfIndex[it->second.get<int>("value") - 1] = 2;
							}
						}
					}
				}
			}
		}
		check_0 = !(check_1 || check_2);
	}

	// for (int i = 0; i < numCell; i++) {
	// 	std::cout << valueOfIndex[i] ;
	// }
	// std::cout << "\n";

	if(check_0) {
		// 営業領域に需要が全く無い
		try {
			const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
			boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
			boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
			return EXIT_SUCCESS;
		} catch (std::exception& e) {
			std::cout << "failure!" << "\n";
			return EXIT_FAILURE;
		}
	}else{
		// 指すべきオブジェクトが営業領域内にある
		// オブジェクトが指すindexTargetを求める
		int indexTarget = 0;
		bool existsTarget = false;
		{
			int N = searchIndex.size();
			// bool checkGet_1 = false;
			bool checkGet_2 = false;
			for (int i = 0; i < N; i++) {
				int indexTargetCandidate = searchIndex[i];
				int valueHoge = valueOfIndex[indexTargetCandidate - 1];
				if (valueHoge == 1) {
					indexTarget = indexTargetCandidate;
					existsTarget = true;
					// checkGet_1 = true;
					break;
				}else if(valueHoge == 2) {
					if (!checkGet_2) {
						indexTarget = indexTargetCandidate;
						existsTarget = true;
						checkGet_2 = true;
					}
				}
			}
		}
		if (existsTarget) {
			if(indexTaxi != indexTarget) {
				// indexTaxiとindexTargetに対応するオブジェクト情報をコピーする
				try {
					boost::filesystem::path src("./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object/" + boost::lexical_cast<std::string>( indexTarget ) + ".xml" );
					boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
					boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
					return EXIT_SUCCESS;
				} catch (std::exception& e) {
					std::cout << "failure!" << "\n";
					return EXIT_FAILURE;
				}
			}else{
				// indexTaxiとindexTargetとdirに対応するオブジェクト情報をコピーする
				try {
					boost::filesystem::path src("./../Data/0_2_Set/Other/InfoAboutValidCells/" + boost::lexical_cast<std::string>( indexTaxi ) + "/Object/" + boost::lexical_cast<std::string>( indexTarget ) + "/dir" + boost::lexical_cast<std::string>( dir ) + ".xml" );
					boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
					boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
					return EXIT_SUCCESS;
				} catch (std::exception& e) {
					std::cout << "failure!" << "\n";
					return EXIT_FAILURE;
				}
			}
		}else{
			// emptyObjectをコピーする
			try {
				const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyObject.xml");
				boost::filesystem::path dst("./../Data/2_ForEachRequest/Other/id_" + idTaxiStr + ".xml");
				boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
				return EXIT_SUCCESS;
			} catch (std::exception& e) {
				std::cout << "failure!" << "\n";
				return EXIT_FAILURE;
			}
		}
	}

    return EXIT_SUCCESS;
}
