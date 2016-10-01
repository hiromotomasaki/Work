/**
 * main.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "main.hpp"

using namespace hiro;

// 【この関数の目的】
// Cronごとにある抽出条件にしたがって得られたData.xmlから，有効な各セルに対してサークル内の需要数を求める．有効なセルの中で，しきい値以上の需要数を持つセルの代表点を間引いて保存する．描画を行うサークルの需要数から対応するサークルの空車数を引いた仮需要数を計算する．仮しきい値以上の仮需要数を持つセルのindex番号を取得，1以上仮しきい値未満の仮需要数を持つセルのindex番号を取得，保存する．

// 以下のファイルが作成される
// *** circleFlag.xml
// サークルが描画されたときに被覆されるセルの判別情報
// *** indexUnderThresholdKari.xml
// 需要数がしきい値未満0より大きいindexを有する
// *** indexNotLessThanThresholdKari.xml
// 需要数がしきい値以上indexを有する
// *** circlePoints.xml
// サークルが描画される位置情報

// 時間を丸める関数
boost::posix_time::ptime roundPTime(const boost::posix_time::ptime &pTime, const int delta);
boost::posix_time::ptime roundPTime(const boost::posix_time::ptime &pTime, const int delta) {
	boost::posix_time::ptime ret(boost::gregorian::date(pTime.date().year(),pTime.date().month().as_number(),pTime.date().day()), boost::posix_time::time_duration(pTime.time_of_day().hours(),0,0));
	// f(秒) = 60(秒/分)*x(分)+y(秒)
	// 1cycle = z(秒) = 60(秒/分)*Δt(分)
	// f ≡ γ(mod z)
	// if γ >= z/2 then 繰り上げ
	// if γ <  z/2 then 切り捨て
	// y(秒)=0として
	// あとはx(分)だけ見れば良い
	// 繰り上げると x+Δt-x%Δt
	// 切り捨てると x    -x%Δt
	// (=X(分))になる.
	// この値を保持しておき, まずx=0としてその後pTime+Xをすればよい
	int z = 60 * delta;           // 時間幅
	int z_2 = 30 * delta;           // 時間幅の半分
	int x = pTime.time_of_day().minutes(); // 分
	int y = pTime.time_of_day().seconds(); // 秒
	int f = 60 * x + y;
	int gamma = f % z;
	int X = 0;                    // 丸め後の分
	if( gamma >= z_2) {
		X = x + delta - x % delta;
	}else{
		X = x - x % delta;
	}
	ret = ret + boost::posix_time::minutes(X);
	return ret;
}

int main()
{
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

    // 需要が集中していると判定するためのしきい値．このしきい値以上であれば需要が集中しているとみなす．
	double threshold;

    // 表示するピンの日にちに関する抽出条件
	std::vector<int> displayDate;
	// 表示するピンの時間幅の始点は現在時刻から何分前なのか
	int displayTimeFrom;
	// 表示するピンの時間幅の始点は現在時刻から何分後なのか
	int displayTimeTo;
	// 離散時間幅
	int discreteTimeWidth;
	// セルが有効かどうか
	std::vector<bool> vValid;
	// 需要数をカウントするサークル情報
	std::vector<std::vector<int>> demandCircleRange;
	// 空車数をカウントするサークル情報
	std::vector<std::vector<int>> vacantCircleRange;
	// ----------------------------------------------------- //

	// Data/0_2_Set/InputDataFor1_Cron.xmlから読み込む
	{
		// 設定値読み込みファイル名
		const std::string fileName = "InputDataFor1_Cron.xml";
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
				threshold = pt.get<double>("table.threshold.value");
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
				displayTimeFrom = pt.get<int>("table.displayTimeFrom.value");
				displayTimeTo = pt.get<int>("table.displayTimeTo.value");
				discreteTimeWidth = pt.get<int>("table.discreteTimeWidth.value");
			}
			{
				// 領域の確保
				vValid.resize(numCell);
				boost::property_tree::ptree::iterator itr_first, itr_last, it;
				itr_first = pt.get_child( "table.cellIsValid" ).begin();
				itr_last = pt.get_child( "table.cellIsValid" ).end();
				for(it = itr_first; it != itr_last; ++it) {
					std::size_t i = std::distance(itr_first, it);
					vValid[i] = it->second.get<bool>("value");
				}
			}
			{
				demandCircleRange.resize(numCell);
				boost::property_tree::ptree::iterator itr_first, itr_last, it;
				itr_first = pt.get_child( "table.demandCircleRange" ).begin();
				itr_last = pt.get_child( "table.demandCircleRange" ).end();
				for(it = itr_first; it != itr_last; ++it) {
					// itはptree::value_typeのイテレータ
					boost::property_tree::ptree::iterator itr_first_c, itr_last_c, it_c;
					itr_first_c = it->second.get_child( "" ).begin();
					itr_last_c = it->second.get_child( "" ).end();
					std::size_t i = std::distance(itr_first, it);
					std::size_t N = std::distance(itr_first_c, itr_last_c);
					demandCircleRange[i].resize(N);
					for(it_c = itr_first_c; it_c != itr_last_c; ++it_c) {
						std::size_t j = std::distance(itr_first_c, it_c);
						demandCircleRange[i][j] = it_c->second.get<int>("value");
					}
				}
			}
			{
				vacantCircleRange.resize(numCell);
				boost::property_tree::ptree::iterator itr_first, itr_last, it;
				itr_first = pt.get_child( "table.vacantCircleRange" ).begin();
				itr_last = pt.get_child( "table.vacantCircleRange" ).end();
				for(it = itr_first; it != itr_last; ++it) {
					// itはptree::value_typeのイテレータ
					boost::property_tree::ptree::iterator itr_first_c, itr_last_c, it_c;
					itr_first_c = it->second.get_child( "" ).begin();
					itr_last_c = it->second.get_child( "" ).end();
					std::size_t i = std::distance(itr_first, it);
					std::size_t N = std::distance(itr_first_c, itr_last_c);
					demandCircleRange[i].resize(N);
					for(it_c = itr_first_c; it_c != itr_last_c; ++it_c) {
						std::size_t j = std::distance(itr_first_c, it_c);
						demandCircleRange[i][j] = it_c->second.get<int>("value");
					}
				}
			}
		}
	}

	// demandCircleRangeの確認
	// {
	// 	for (int i = 0; i < (int)demandCircleRange.size(); i++) {
	// 		int N = demandCircleRange[i].size();
	// 		if (N >= 2) {
	// 			std::cout << "[" << i+1 << "] : ";
	// 			for (int j = 0; j < N; j++) {
	// 				std::cout << demandCircleRange[i][j] << ", " ;
	// 			}
	// 			std::cout << "\n";
	// 		}
	// 	}
	// }




























// // 需要数を取得する
	// std::vector<int> vDemand(numCell, 0);
	// {
	// 	// currentTime.xmlを取得
	// 	boost::posix_time::ptime cTime;
	// 	{
	// 		boost::property_tree::ptree pt;
	// 		// 設定値読み込みファイル名
	// 		const std::string fileName = "currentTime.xml";
	// 		// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
	// 		const std::string fileDire = "./../Data/1_Cron/FromServer";
	// 		std::string fileRela = fileDire + "/" + fileName;
	// 		// read the xml file
	// 		boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
	// 		{
	// 			// 文字列の取得
	// 			std::string timeStr = pt.get<std::string>("table.data.currentTime");
	// 			// 空文字の削除
	// 			boost::trim (timeStr);
	// 			// boost::posix_time::ptime 型に変換
	// 			cTime = boost::posix_time::time_from_string(timeStr);
	// 		}
	// 	}
	// 	// Data.xmlを取得
	// 	// create an empty property tree
	// 	boost::property_tree::ptree pt;
	// 	{
	// 		// 設定値読み込みファイル名
	// 		const std::string fileName = "data.xml";
	// 		// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
	// 		const std::string fileDire = "./../Data/1_Cron/FromServer";
	// 		std::string fileRela = fileDire + "/" + fileName;
	// 		// read the xml file
	// 		boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
	// 	}
	// 	// CurrentTimeを丸める
	// 	{
	// 		cTime = roundPTime(cTime, discreteTimeWidth);
	// 	}
	// 	// データ抽出条件の計算
	// 	std::vector<boost::posix_time::ptime> vCTimeStart;
	// 	std::vector<boost::posix_time::ptime> vCTimeEnd;
	// 	{
	// 		int N = displayDate.size();
	// 		// ベクトルの要素の追加
	// 		{
	// 			vCTimeStart.reserve(N);
	// 			vCTimeEnd.reserve(N);
	// 		}
	// 		// 値の登録
	// 		{
	// 			// displayTimeFrom(=-2)はdiscreteTimeWidth(=2)何個分(=-1)か
	// 			int numPre = round(displayTimeFrom / (double)discreteTimeWidth);
	// 			// displayTimeTo(=4)はdiscreteTimeWidth(=2)何個分(=2)か
	// 			int numPost = round(displayTimeTo / (double)discreteTimeWidth);
	// 			for (int i = 0; i < N; i++) {
	// 				vCTimeStart[i] = cTime + ( - boost::gregorian::days(displayDate[i]) );
	// 				vCTimeStart[i] += boost::posix_time::seconds( numPre * discreteTimeWidth * 60 - discreteTimeWidth * 30 );
	// 				vCTimeEnd[i] = cTime + ( - boost::gregorian::days(displayDate[i]) );
	// 				vCTimeEnd[i] += boost::posix_time::seconds( numPost * discreteTimeWidth * 60 + discreteTimeWidth * 30 );
	// 				// std::cout << "[" << vCTimeStart[i] << "]" << "[" << vCTimeEnd[i] << "]" << "\n";
	// 			}
	// 		}
	// 	}

	// 	// メモ
	// 	// table-date-startDate
	// 	//           -startPoint-latitude
	// 	//                      -longitude

	// 	// ptを解析していき需要数をカウントしていく
	// 	{
	// 		boost::property_tree::ptree::iterator itr_first, itr_last, it;
	// 		itr_first = pt.get_child( "table" ).begin();
	// 		itr_last = pt.get_child( "table" ).end();
	// 		for(it = itr_first; it != itr_last; it++) {
	// 			// startPointを取得
	// 			GeographicCoordinate gCoorHoge;
	// 			gCoorHoge.setPhi( it->second.get<double>("startPoint.latitude") );
	// 			gCoorHoge.setLambda( it->second.get<double>("startPoint.longitude") );
	// 			int row = calculateRowFromLatitudes( gCoorHoge.getPhi(), gCoorNW.getPhi(), gCoorSE.getPhi(), cellSizePhi );
	// 			int col = calculateColFromLongitudes( gCoorHoge.getLambda(), gCoorNW.getLambda(), gCoorSE.getLambda(), cellSizeLambda );
	// 			int index = calculateIndexFromRowCol( row, col, numRow, numCol );
	// 			// 範囲外だとindex = 0になる
	// 			if( index != 0 ) {
	// 				// 有効なセルかどうか
	// 				bool isValid = vValid[index-1];
	// 				if (isValid) {
	// 					// startDateを取得する
	// 					std::string sTimeStr = it->second.get<std::string>("startDate");
	// 					// 空文字の除去
	// 					boost::trim (sTimeStr);
	// 					// boost::posix_time::ptime 型に変換
	// 					boost::posix_time::ptime sTime = boost::posix_time::time_from_string(sTimeStr);
	// 					// Does there exist i s.t. vTimeStart[i] <= sTime < vTimeEnd[i]?
	// 					bool exists = false;
	// 					{
	// 						int N = displayDate.size();
	// 						for (int i = 0; i < N; i++) {
	// 							exists = ( ( vCTimeStart[i] <= sTime ) && ( sTime < vCTimeEnd[i] ) );
	// 							if (exists) {
	// 								break;
	// 							}
	// 						}
	// 					}
	// 					if (exists) {
	// 						// 需要数のカウントアップ
	// 						vDemand[index - 1]++;
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// }
	// // std::cout << "総需要数 : " << std::accumulate(vDemand.begin(), vDemand.end(), 0.0) << std::endl;
	// // std::cout << "需要数の確認" << "\n";
	// // for (int i = 0; i < numCell; i++) {
	// // 	if (vDemand[i] >= 1) {
	// // 		std::cout << "[index]" << i+1 << ", [demand]" << vDemand[i] << "\n";
	// // 	}
	// // }

	// std::vector<int> vDemandIndex1;
	// vDemandIndex1.reserve(numCell+1);
    // vDemandIndex1.push_back(-1);
	// std::vector<int> vDemandIndex2;
	// vDemandIndex2.reserve(numCell+1);
	// vDemandIndex2.push_back(-1);
	// // thereshold以上の需要を持つセルとthreshold未満0より大きい需要を持つセルのindexの保存
	// {
	// 	for (int i = 0; i < numCell; i++) {
	// 		if (vDemand[i] >= threshold) {
	// 			vDemandIndex1.push_back(i + 1);
	// 		}else if((vDemand[i] < threshold) && (vDemand[i] > 0)) {
	// 			vDemandIndex2.push_back(i + 1);
	// 		}
	// 	}

	// 	// indexUpperThreshold.xmlに保存
	// 	{
	// 		// 設定値保存ファイル名
	// 		const std::string fileName = "indexNotLessThanThreshold.xml";
	// 		// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
	// 		const std::string fileDire = "./../Data/1_Cron/Other";
	// 		// 保存path
	// 		std::string fileRela = fileDire + "/" + fileName;
	// 		// create an empty property tree
	// 		boost::property_tree::ptree pt;

	// 		// create the root element
	// 		boost::property_tree::ptree& root = pt.put("table", "");

	// 		// add child elements
	// 		{
	// 			int N = vDemandIndex1.size();
	// 			for (int i = 0; i < N; i++) {
	// 				boost::property_tree::ptree& child = root.add("index", "");
	// 				child.put("value", vDemandIndex1[i]);
	// 			}
	// 		}

	// 		// output
	// 		boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
	// 	}

	// 	// indexUpperThreshold.xmlに保存
	// 	{
	// 		// 設定値保存ファイル名
	// 		const std::string fileName = "indexUnderThreshold.xml";
	// 		// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
	// 		const std::string fileDire = "./../Data/1_Cron/Other";
	// 		// 保存path
	// 		std::string fileRela = fileDire + "/" + fileName;
	// 		// create an empty property tree
	// 		boost::property_tree::ptree pt;

	// 		// create the root element
	// 		boost::property_tree::ptree& root = pt.put("table", "");

	// 		// add child elements
	// 		{
	// 			int N = vDemandIndex2.size();
	// 			for (int i = 0; i < N; i++) {
	// 				boost::property_tree::ptree& child = root.add("index", "");
	// 				child.put("value", vDemandIndex2[i]);
	// 			}
	// 		}

	// 		// output
	// 		boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
	// 	}
	// }

    return EXIT_SUCCESS;
}
