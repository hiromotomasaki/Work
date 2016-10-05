/**
 * main.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "main.hpp"

using namespace hiro;

// 【この関数の目的】
// neural networkの作成

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

	// 有効なセルの確認に必要
	std::vector<int> indexToTrueIndex;
	// ----------------------------------------------------- //

	// Data/0_2_Set/InputDataFor0_3_Learn.xmlから読み込む
	{
		// 設定値読み込みファイル名
		const std::string fileName = "InputDataFor0_3_Learn.xml";
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
			}
			{
				// 数を調べる
				int N = 0;
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "table.indexToTrueIndex" ).begin();
					itr_last = pt.get_child( "table.indexToTrueIndex" ).end();
					N = std::distance(itr_first, itr_last);
				}
				// 要素の追加
				indexToTrueIndex.reserve(N);
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "table.indexToTrueIndex" ).begin();
					itr_last = pt.get_child( "table.indexToTrueIndex" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						indexToTrueIndex.push_back(it->second.get<int>("value"));
					}
				}
			}
		}
	}

    return EXIT_SUCCESS;
}
