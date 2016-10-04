/**
 * main.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "main.hpp"

using namespace hiro;

// 【この関数の目的】
// Rational関係の設定値のxmlファイルを作成する．

// サークルの重なりの解決方法
// まず，threshold以上の需要数を持つ全サークルの中心セル番号を取得
// 大きい順に並べる
// 上から順番に描画を決定
// サークルに被覆されるセル(フラグ1)とその周りのもし描画するとサークルが重なる位置となるセル(フラグ2)にフラグを立てる(つまり，サークルが書かれたセルからサークルの直径分の半径の円の領域にフラグが立つ)
// 描画を決定するときにそのフラグのチェックを行い，立っていれば描画はしないことにする．
// 描画されるサークルの各需要数からある大きさのサークル内にある空車の数を引き，それをそのセルの仮需要数とする．

// 保存するのは(描画するサークルの位置情報)と(そのサークルのindexと仮需要数のペアとフラグ1情報)
// 前者は1_Cronの結果として描画に使用され，後者は2_ForEachRequestでオブジェクト生成に使用される．(** に続く)

// **
// もし，indexTaxiがフラグ1の場所である場合
// : 空のオブジェクト情報を返す．
// そうでない場合
// : まず探索範囲内で仮需要数がしきい値以上ある最も近いサークルの中心セルをindexTargetとしてオブジェクト情報を返す．
// : もしなければ，仮需要数がしきい値未満，0より大きい最も近いサークルの中心セルをindexTargetとしてオブジェクト情報を返す．
// : それでもなければ，空のオブジェクト情報を返す．

int myTestFunc();
int myCreateTestData();
int myCreateData();

int myTestFunc()
{
	std::cout << "読み込みテスト中..." << "\n";
	const std::string fileName = "base.xml";
	const std::string fileDire = "./../Data/0_1_Preset/Other";
	std::string fileRela = fileDire + "/" + fileName;
	{
		// create an empty property tree
		boost::property_tree::ptree pt;
		// read the xml file
		boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
		// ループなし
		{
			GeographicCoordinate gCoorNW;
			gCoorNW.setPhi( pt.get<double>( "table.gCoorNW.phi" ) );
			gCoorNW.setLambda( pt.get<double>( "table.gCoorNW.lambda" ) );
			gCoorNW.print();
		}
		// ループあり
		{
			boost::property_tree::ptree::iterator itr_first, itr_last, it;
			itr_first = pt.get_child( "table.gCoorRemove" ).begin();
			itr_last = pt.get_child( "table.gCoorRemove" ).end();
			int N = std::distance(itr_first, itr_last);
			std::cout << "要素数 ： " << N << "\n";
			for(it = itr_first; it != itr_last; ++it) {
				std::cout << "[" << std::distance(itr_first, it) << "]" << "\n";
				std::cout << it->second.get<double>("phiNW") << "\n";
			}
		}
	}
	std::cout << fileName << "の読込終了" << std::endl;
    return EXIT_SUCCESS;
}

int myCreateTestData()
{
	// ------------- ユーザー入力値  ------------- //
	// 営業領域の外枠の最北西
	GeographicCoordinate gCoorNW( 34.275080, 135.138074 );
	// 営業領域の外枠の最南東
	GeographicCoordinate gCoorSE( 34.178566, 135.242616 );
	// 追加する営業領域の最北西
	std::vector<GeographicCoordinate> gCoorAddNW;
	gCoorAddNW.push_back( GeographicCoordinate( 34.276499, 135.177899 ) );
	gCoorAddNW.push_back( GeographicCoordinate( 34.271675, 135.146657 ) );
	// 追加する営業領域の最南東
	std::vector<GeographicCoordinate> gCoorAddSE;
	gCoorAddSE.push_back( GeographicCoordinate( 34.184814, 135.236264 ) );
	gCoorAddSE.push_back( GeographicCoordinate( 34.182542, 135.190602 ) );
	// 削除する営業領域の最北西
	std::vector<GeographicCoordinate> gCoorRemoveNW;
	gCoorRemoveNW.push_back( GeographicCoordinate( -1, -1 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.276782, 135.129233 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.200859, 135.193864 ) );
	// 削除する営業領域の最南東
	std::vector<GeographicCoordinate> gCoorRemoveSE;
	gCoorRemoveSE.push_back( GeographicCoordinate( -1, -1 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.242164, 135.162535 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.174944, 135.221072 ) );
	// セルの一辺の長さ[m]
	double cellSizeMeter = 200;
	// 表示するピンの日にちに関する抽出条件
	std::vector<int> displayDate;
	displayDate.push_back( 7*4 );
	displayDate.push_back( 7*5 );
	displayDate.push_back( 7*6 );
	displayDate.push_back( 7*7 );
	displayDate.push_back( 7*8 );
	displayDate.push_back( 7*9 );
	displayDate.push_back( 7*10 );
	displayDate.push_back( 7*11 );
	// 表示するピンの時間幅の始点は現在時刻から何分前なのか
	int displayTimeFrom = -2;
	// 表示するピンの時間幅の始点は現在時刻から何分後なのか
	int displayTimeTo = 4;
	// セルから描画するオブジェクトの有効範囲の円の半径[m]
	double searchRange = 3000;
	// double searchRange = 1000;	// test
	// タクシーの進行方向を考慮した時に描画するオブジェクトの有効範囲の扇型（円に含まれる）の中心角度[deg](45度から180度まで)
	double searchDegree = 180;
	// オブジェクト生成の基準となるboxの一辺の長さの最小値
	double squareLengthMin = 200;
	// オブジェクト生成の基準となるboxの一辺の長さの最大値
	double squareLengthMax = 1000;
	// タクシーの進行方向を計算するときに使用する二点間の座標間の距離の最大値．これを越すとオブジェクトは円の中から探索して選ばれる(dir1に相当)．越さなければ進行方向(dir2からdir9)の扇型の中から探索して選ばれる
	double maxDistanceFromPreposition = 3000;
	// 需要が集中していると判定するためのしきい値．このしきい値以上であれば需要が集中しているとみなす．
	double threshold = 2;
	// 離散時間幅
	int discreteTimeWidth = 2;
	// あるセルに対する需要数カウントのサークルの半径[m]
	double demandCountCircleRadius = 500;
	// あるセルに対する空車数カウントのサークルの半径[m]
	double vacantCountCircleRadius = 1000;
	// 仮需要のためのしきい値．
	double thresholdKari = 1;
	// ------------------------------------------- //
	// ------------------ その他 ----------------- //
	// 設定値の確認表示をするかどうか
	bool display = true;
	// 保存ファイルの一部読み込みをしてチェックするか
	bool check = true;
	// ------------------------------------------- //

	// 表示
	if (display) {
		std::cout << "============== 確認 ==============" << "\n";
		std::cout << "--------- 営業領域の外枠 ---------" << "\n";
		{
			std::cout << "北西 ： " << "\n";
			gCoorNW.print();
			std::cout << "南東 ： " << "\n";
			gCoorSE.print();
		}
		std::cout << "--------- 追加される営業領域 ---------" << "\n";
		{
			int N = gCoorAddNW.size();
			std::cout << "追加領域の数 ： " << N << "\n";
			for (int i = 0; i < N; i++) {
				std::cout << "No." << i+1 << " : 北西 = (" << gCoorAddNW[i].getPhi() << ", " << gCoorAddNW[i].getLambda() << "), 南東 = (" << gCoorAddSE[i].getPhi() << ", " << gCoorAddSE[i].getLambda() << ")" << "\n";
			}
		}
		std::cout << "--------- 削除される営業領域 ---------" << "\n";
		{
			int N = gCoorRemoveNW.size();
			if (N == 1) {
				std::cout << "削除領域はありません．" << "\n";
			}else{
				std::cout << "削除領域の数 ： " << N-1 << "\n";
				for (int i = 1; i < N; i++) {
					std::cout << "No." << i << " : 北西 = (" << gCoorRemoveNW[i].getPhi() << ", " << gCoorRemoveNW[i].getLambda() << "), 南東 = (" << gCoorRemoveSE[i].getPhi() << ", " << gCoorRemoveSE[i].getLambda() << ")" << "\n";
				}
			}
		}
		std::cout << "--------- セルの一辺の長さ[m] ---------" << "\n";
		std::cout << cellSizeMeter << "\n";
		std::cout << "--------- 表示するピンは何日前のものか ---------" << "\n";
		{
			int N = displayDate.size();
			for (int i = 0; i < N; i++) {
				std::cout << displayDate[i];
				if (i != N-1) {
					std::cout << ", ";
				}
			}
			std::cout << "\n";
		}
		std::cout << "---------  表示するピンの時間幅の始点は現在時刻から何分前なのか ---------" << "\n";
		std::cout << displayTimeFrom << "\n";
		std::cout << "---------  表示するピンの時間幅の始点は現在時刻から何分後なのか ---------" << "\n";
		std::cout << displayTimeTo << "\n";
		std::cout << "--------- セルから描画するオブジェクトの有効範囲の円の半径[m]  ---------" << "\n";
		std::cout << searchRange << "\n";
		std::cout << "--------- タクシーの進行方向を考慮した時に描画するオブジェクトの有効範囲の扇型（円に含まれる）の中心角度[deg](45度から180度まで)  ---------" << "\n";
		std::cout << searchDegree << "\n";
		std::cout << "--------- オブジェクトの底辺の長さの最小値  ---------" << "\n";
		std::cout << squareLengthMin << "\n";
		std::cout << "--------- オブジェクトの底辺の長さの最大値  ---------" << "\n";
		std::cout << squareLengthMax << "\n";
		std::cout << "--------- タクシーの進行方向を計算するときに使用する二点間の座標間の距離の最大値．これを越すとオブジェクトは円の中から探索して選ばれる(dir1に相当)．越さなければ進行方向(dir2からdir9)の扇型の中から探索して選ばれる  ---------" << "\n";
		std::cout << maxDistanceFromPreposition << "\n";
		std::cout << "--------- 需要が集中していると判定するためのしきい値．このしきい値以上であれば需要が集中しているとみなす．  ---------" << "\n";
		std::cout << threshold << "\n";
		std::cout << "--------- 離散時間幅．  ---------" << "\n";
		std::cout << discreteTimeWidth << "\n";
		std::cout << "--------- あるセルに対する需要数カウントのサークルの半径[m]  ---------" << "\n";
		std::cout << demandCountCircleRadius << "\n";
		std::cout << "--------- あるセルに対する空車数カウントのサークルの半径[m]  ---------" << "\n";
		std::cout << vacantCountCircleRadius << "\n";
		std::cout << "--------- 仮需要のためのしきい値  ---------" << "\n";
		std::cout << thresholdKari << "\n";
		std::cout << "==================================" << "\n";
	}

	// 設定値の保存
	{
		// 設定値保存ファイル名
		const std::string fileName = "base.xml";
		// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_1_Preset/Other";
		std::cout << fileName << "を保存中..." << std::endl;
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
					boost::property_tree::ptree& child = root.add("gCoorAdd", "");
					int N = gCoorAddNW.size();
					for (int i = 0; i < N; i++) {
						boost::property_tree::ptree& cchild = child.add("tuples", "");
						cchild.put("phiNW", gCoorAddNW[i].getPhi());
						cchild.put("lambdaNW", gCoorAddNW[i].getLambda());
						cchild.put("phiSE", gCoorAddSE[i].getPhi());
						cchild.put("lambdaSE", gCoorAddSE[i].getLambda());
					}
				}
				{
					boost::property_tree::ptree& child = root.add("gCoorRemove", "");
					int N = gCoorRemoveNW.size();
					for (int i = 0; i < N; i++) {
						boost::property_tree::ptree& cchild = child.add("tuples", "");
						cchild.put("phiNW", gCoorRemoveNW[i].getPhi());
						cchild.put("lambdaNW", gCoorRemoveNW[i].getLambda());
						cchild.put("phiSE", gCoorRemoveSE[i].getPhi());
						cchild.put("lambdaSE", gCoorRemoveSE[i].getLambda());
					}
				}
				{
					boost::property_tree::ptree& child = root.add("cellSizeMeter", "");
					child.put("value", cellSizeMeter);
				}
				{
					boost::property_tree::ptree& child = root.add("displayDate", "");
					int N = displayDate.size();
					for (int i = 0; i < N; i++) {
						boost::property_tree::ptree& cchild = child.add("date", "");
						cchild.put("value", displayDate[i]);
					}
				}
				{
					boost::property_tree::ptree& child = root.add("displayTimeFrom", "");
					child.put("value", displayTimeFrom);
				}
				{
					boost::property_tree::ptree& child = root.add("displayTimeTo", "");
					child.put("value", displayTimeTo);
				}
				{
					boost::property_tree::ptree& child = root.add("searchRange", "");
					child.put("value", searchRange);
				}
				{
					boost::property_tree::ptree& child = root.add("searchDegree", "");
					child.put("value", searchDegree);
				}
				{
					boost::property_tree::ptree& child = root.add("squareLengthMin", "");
					child.put("value", squareLengthMin);
				}
				{
					boost::property_tree::ptree& child = root.add("squareLengthMax", "");
					child.put("value", squareLengthMax);
				}
				{
					boost::property_tree::ptree& child = root.add("maxDistanceFromPreposition", "");
					child.put("value", maxDistanceFromPreposition);
				}
				{
					boost::property_tree::ptree& child = root.add("threshold", "");
					child.put("value", threshold);
				}
				{
					boost::property_tree::ptree& child = root.add("discreteTimeWidth", "");
					child.put("value", discreteTimeWidth);
				}
				{
					boost::property_tree::ptree& child = root.add("demandCountCircleRadius", "");
					child.put("value", demandCountCircleRadius);
				}
				{
					boost::property_tree::ptree& child = root.add("vacantCountCircleRadius", "");
					child.put("value", vacantCountCircleRadius);
				}
				{
					boost::property_tree::ptree& child = root.add("thresholdKari", "");
					child.put("value", thresholdKari);
				}
			}

			// output
			boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
			}
		}

	// テスト
	if (check) {
		myTestFunc();
	}

    return EXIT_SUCCESS;
}

int myCreateData()
{
	// ------------- ユーザー入力値  ------------- //
	// 営業領域の外枠の最北西
	GeographicCoordinate gCoorNW( 34.296332, 135.061091 );
	// 営業領域の外枠の最南東
	GeographicCoordinate gCoorSE( 34.143423, 135.339895 );
	// 追加する営業領域の最北西
	std::vector<GeographicCoordinate> gCoorAddNW;
	gCoorAddNW.push_back( GeographicCoordinate( 34.284311, 135.183871 ) );
	gCoorAddNW.push_back( GeographicCoordinate( 34.217739, 135.265546 ) );
	gCoorAddNW.push_back( GeographicCoordinate( 34.297741, 135.241681 ) );
	gCoorAddNW.push_back( GeographicCoordinate( 34.223232, 135.339878 ) );
	gCoorAddNW.push_back( GeographicCoordinate( 34.296332, 135.061091 ) );
	// 追加する営業領域の最南東
	std::vector<GeographicCoordinate> gCoorAddSE;
	gCoorAddSE.push_back( GeographicCoordinate( 34.140970, 135.278189 ) );
	gCoorAddSE.push_back( GeographicCoordinate( 34.141232, 135.311170 ) );
	gCoorAddSE.push_back( GeographicCoordinate( 34.223232, 135.339878 ) );
	gCoorAddSE.push_back( GeographicCoordinate( 34.150343, 135.192901 ) );
	gCoorAddSE.push_back( GeographicCoordinate( 34.224480, 135.140605 ) );
	// 削除する営業領域の最北西
	std::vector<GeographicCoordinate> gCoorRemoveNW;
	gCoorRemoveNW.push_back( GeographicCoordinate( -1, -1 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.224480, 135.140605 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.251027, 135.085241 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.303788, 135.083578 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.301980, 135.236779 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.235600, 135.304050 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.225734, 135.283667 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.163086, 135.279536 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.180746, 135.296754 ) );
	gCoorRemoveNW.push_back( GeographicCoordinate( 34.180980, 135.109406 ) );
	// 削除する営業領域の最南東
	std::vector<GeographicCoordinate> gCoorRemoveSE;
	gCoorRemoveSE.push_back( GeographicCoordinate( -1, -1 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.216726, 135.090817 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.217182, 135.098910 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.278084, 135.142001 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.287886, 135.294815 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.217238, 135.348713 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.203705, 135.320705 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.132188, 135.327012 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.157178, 135.329913 ) );
	gCoorRemoveSE.push_back( GeographicCoordinate( 34.143055, 135.169831 ) );
	// セルの一辺の長さ[m]
	double cellSizeMeter = 200;
	// 表示するピンの日にちに関する抽出条件
	std::vector<int> displayDate;
	displayDate.push_back( 7*4 );
	displayDate.push_back( 7*5 );
	displayDate.push_back( 7*6 );
	displayDate.push_back( 7*7 );
	displayDate.push_back( 7*8 );
	displayDate.push_back( 7*9 );
	displayDate.push_back( 7*10 );
	displayDate.push_back( 7*11 );
	// 表示するピンの時間幅の始点は現在時刻から何分前なのか
	int displayTimeFrom = -2;
	// 表示するピンの時間幅の始点は現在時刻から何分後なのか
	int displayTimeTo = 4;
	// セルから描画するオブジェクトの有効範囲の円の半径[m]
	double searchRange = 3000;
	// double searchRange = 1000;	// test
	// タクシーの進行方向を考慮した時に描画するオブジェクトの有効範囲の扇型（円に含まれる）の中心角度[deg](45度から180度まで)
	double searchDegree = 180;
	// オブジェクト生成の基準となるboxの一辺の長さの最小値
	double squareLengthMin = 200;
	// オブジェクト生成の基準となるboxの一辺の長さの最大値
	double squareLengthMax = 1000;
	// タクシーの進行方向を計算するときに使用する二点間の座標間の距離の最大値．これを越すとオブジェクトは円の中から探索して選ばれる(dir1に相当)．越さなければ進行方向(dir2からdir9)の扇型の中から探索して選ばれる
	double maxDistanceFromPreposition = 3000;
	// 需要が集中していると判定するためのしきい値．このしきい値以上であれば需要が集中しているとみなす．
	double threshold = 2;
	// 離散時間幅
	int discreteTimeWidth = 2;
	// あるセルに対する需要数カウントのサークルの半径[m]
	double demandCountCircleRadius = 500;
	// あるセルに対する空車数カウントのサークルの半径[m]
	double vacantCountCircleRadius = 1000;
	// 仮需要のためのしきい値．
	double thresholdKari = 1;
	// ------------------------------------------- //
	// ------------------ その他 ----------------- //
	// 設定値の確認表示をするかどうか
	bool display = true;
	// 保存ファイルの一部読み込みをしてチェックするか
	bool check = true;
	// ------------------------------------------- //

	// 表示
	if (display) {
		std::cout << "============== 確認 ==============" << "\n";
		std::cout << "--------- 営業領域の外枠 ---------" << "\n";
		{
			std::cout << "北西 ： " << "\n";
			gCoorNW.print();
			std::cout << "南東 ： " << "\n";
			gCoorSE.print();
		}
		std::cout << "--------- 追加される営業領域 ---------" << "\n";
		{
			int N = gCoorAddNW.size();
			std::cout << "追加領域の数 ： " << N << "\n";
			for (int i = 0; i < N; i++) {
				std::cout << "No." << i+1 << " : 北西 = (" << gCoorAddNW[i].getPhi() << ", " << gCoorAddNW[i].getLambda() << "), 南東 = (" << gCoorAddSE[i].getPhi() << ", " << gCoorAddSE[i].getLambda() << ")" << "\n";
			}
		}
		std::cout << "--------- 削除される営業領域 ---------" << "\n";
		{
			int N = gCoorRemoveNW.size();
			if (N == 1) {
				std::cout << "削除領域はありません．" << "\n";
			}else{
				std::cout << "削除領域の数 ： " << N-1 << "\n";
				for (int i = 1; i < N; i++) {
					std::cout << "No." << i << " : 北西 = (" << gCoorRemoveNW[i].getPhi() << ", " << gCoorRemoveNW[i].getLambda() << "), 南東 = (" << gCoorRemoveSE[i].getPhi() << ", " << gCoorRemoveSE[i].getLambda() << ")" << "\n";
				}
			}
		}
		std::cout << "--------- セルの一辺の長さ[m] ---------" << "\n";
		std::cout << cellSizeMeter << "\n";
		std::cout << "--------- 表示するピンは何日前のものか ---------" << "\n";
		{
			int N = displayDate.size();
			for (int i = 0; i < N; i++) {
				std::cout << displayDate[i];
				if (i != N-1) {
					std::cout << ", ";
				}
			}
			std::cout << "\n";
		}
		std::cout << "---------  表示するピンの時間幅の始点は現在時刻から何分前なのか ---------" << "\n";
		std::cout << displayTimeFrom << "\n";
		std::cout << "---------  表示するピンの時間幅の始点は現在時刻から何分後なのか ---------" << "\n";
		std::cout << displayTimeTo << "\n";
		std::cout << "--------- セルから描画するオブジェクトの有効範囲の円の半径[m]  ---------" << "\n";
		std::cout << searchRange << "\n";
		std::cout << "--------- タクシーの進行方向を考慮した時に描画するオブジェクトの有効範囲の扇型（円に含まれる）の中心角度[deg](45度から180度まで)  ---------" << "\n";
		std::cout << searchDegree << "\n";
		std::cout << "--------- オブジェクトの底辺の長さの最小値  ---------" << "\n";
		std::cout << squareLengthMin << "\n";
		std::cout << "--------- オブジェクトの底辺の長さの最大値  ---------" << "\n";
		std::cout << squareLengthMax << "\n";
		std::cout << "--------- タクシーの進行方向を計算するときに使用する二点間の座標間の距離の最大値．これを越すとオブジェクトは円の中から探索して選ばれる(dir1に相当)．越さなければ進行方向(dir2からdir9)の扇型の中から探索して選ばれる  ---------" << "\n";
		std::cout << maxDistanceFromPreposition << "\n";
		std::cout << "--------- 需要が集中していると判定するためのしきい値．このしきい値以上であれば需要が集中しているとみなす．  ---------" << "\n";
		std::cout << threshold << "\n";
		std::cout << "--------- 離散時間幅．  ---------" << "\n";
		std::cout << discreteTimeWidth << "\n";
		std::cout << "--------- あるセルに対する需要数カウントのサークルの半径[m]  ---------" << "\n";
		std::cout << demandCountCircleRadius << "\n";
		std::cout << "--------- あるセルに対する空車数カウントのサークルの半径[m]  ---------" << "\n";
		std::cout << vacantCountCircleRadius << "\n";
		std::cout << "--------- 仮需要のためのしきい値  ---------" << "\n";
		std::cout << thresholdKari << "\n";
		std::cout << "==================================" << "\n";
	}

	// 設定値の保存
	{
		// 設定値保存ファイル名
		const std::string fileName = "base.xml";
		// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_1_Preset/Other";
		std::cout << fileName << "を保存中..." << std::endl;
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
					boost::property_tree::ptree& child = root.add("gCoorAdd", "");
					int N = gCoorAddNW.size();
					for (int i = 0; i < N; i++) {
						boost::property_tree::ptree& cchild = child.add("tuples", "");
						cchild.put("phiNW", gCoorAddNW[i].getPhi());
						cchild.put("lambdaNW", gCoorAddNW[i].getLambda());
						cchild.put("phiSE", gCoorAddSE[i].getPhi());
						cchild.put("lambdaSE", gCoorAddSE[i].getLambda());
					}
				}
				{
					boost::property_tree::ptree& child = root.add("gCoorRemove", "");
					int N = gCoorRemoveNW.size();
					for (int i = 0; i < N; i++) {
						boost::property_tree::ptree& cchild = child.add("tuples", "");
						cchild.put("phiNW", gCoorRemoveNW[i].getPhi());
						cchild.put("lambdaNW", gCoorRemoveNW[i].getLambda());
						cchild.put("phiSE", gCoorRemoveSE[i].getPhi());
						cchild.put("lambdaSE", gCoorRemoveSE[i].getLambda());
					}
				}
				{
					boost::property_tree::ptree& child = root.add("cellSizeMeter", "");
					child.put("value", cellSizeMeter);
				}
				{
					boost::property_tree::ptree& child = root.add("displayDate", "");
					int N = displayDate.size();
					for (int i = 0; i < N; i++) {
						boost::property_tree::ptree& cchild = child.add("date", "");
						cchild.put("value", displayDate[i]);
					}
				}
				{
					boost::property_tree::ptree& child = root.add("displayTimeFrom", "");
					child.put("value", displayTimeFrom);
				}
				{
					boost::property_tree::ptree& child = root.add("displayTimeTo", "");
					child.put("value", displayTimeTo);
				}
				{
					boost::property_tree::ptree& child = root.add("searchRange", "");
					child.put("value", searchRange);
				}
				{
					boost::property_tree::ptree& child = root.add("searchDegree", "");
					child.put("value", searchDegree);
				}
				{
					boost::property_tree::ptree& child = root.add("squareLengthMin", "");
					child.put("value", squareLengthMin);
				}
				{
					boost::property_tree::ptree& child = root.add("squareLengthMax", "");
					child.put("value", squareLengthMax);
				}
				{
					boost::property_tree::ptree& child = root.add("maxDistanceFromPreposition", "");
					child.put("value", maxDistanceFromPreposition);
				}
				{
					boost::property_tree::ptree& child = root.add("threshold", "");
					child.put("value", threshold);
				}
				{
					boost::property_tree::ptree& child = root.add("discreteTimeWidth", "");
					child.put("value", discreteTimeWidth);
				}
				{
					boost::property_tree::ptree& child = root.add("demandCountCircleRadius", "");
					child.put("value", demandCountCircleRadius);
				}
				{
					boost::property_tree::ptree& child = root.add("vacantCountCircleRadius", "");
					child.put("value", vacantCountCircleRadius);
				}
				{
					boost::property_tree::ptree& child = root.add("thresholdKari", "");
					child.put("value", thresholdKari);
				}
			}

			// output
			boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
			}
		}

	// テスト
	if (check) {
		myTestFunc();
	}

    return EXIT_SUCCESS;
}

int main()
{
	// --- ユーザー入力値  --- //
	bool isTest = true;
	// ----------------------- //
	if (isTest) {
		myCreateTestData();
	}else{
		myCreateData();
	}
    return EXIT_SUCCESS;
}
