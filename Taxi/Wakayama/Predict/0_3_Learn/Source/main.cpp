/**
 * main.cpp
 *
 * Copyright (c) 2016 HIROMOTO,Masaki
 *
 */
#include "main.hpp"

#define MAX_TEMP_MAX 40
#define MAX_TEMP_MIN 0
#define MIN_TEMP_MAX 35
#define MIN_TEMP_MIN -5

using namespace hiro;

// 【この関数の目的】
// neural networkの作成

// 時間幅を最小に見積もって1分なので, 1時間で60, 24時間で1440, 1年で525600, 2年で1051200個となり、学習データ数の最大値は1051200である。

// std::vector<double> step;			// 1日のうち何ステップ目なのか(もし時間幅が2分なら1日は1440分なので720ステップ, 1ステップ目は前日の23:59から0:00:59, 720ステップ目は23:57から23:58:59)
// double day_5_10_20_lastDay;	// 5, 10, 20日, 月末であれば1, そうでなければ0
// double holiday;			// 土日や祝日なら1, そうでなければ0
// double month12to2;		// 12月から2月なら1, そうでなければ0
// double month3to5;		// 3月から5月なら1, そうでなければ0
// double month6to8;		// 6月から8月なら1, そうでなければ0
// double month9to11;		// 9月から11月なら1, そうでなければ0

// double dayW;			// (6:00から18:00までの天候, 雨 : 1, 雨ではない : 0)
// double nightW;			// (18:00から翌日の6:00までの天候, 雨 : 1, 雨ではない : 0)

// double maxTemp;	     // その日の最高気温
// double minTemp;	     // その日の最低気温

// pairTrueIndexAndDemand
struct PTIaD
{
	int trueIndex;
	double demand;
};

// 構造体の宣言
struct DataFactor{
	boost::posix_time::ptime pTimeRound;
	int step;
	int day_5_10_20_lastDay;
	int holiday;
	int month12to2;
	int month3to5;
	int month6to8;
	int month9to11;
	double maxTemp;
	double minTemp;
	double dayW;
	double nightW;
	std::vector<PTIaD> pTIaD;
};


boost::posix_time::ptime roundPTime(const boost::posix_time::ptime &pTime, const int delta);
bool IsLeapYear(int year);
int GetLastDay(int year, int month);
int getStep(const boost::posix_time::ptime &pTime, const int delta);
int evaluateDay_5_10_20_lastDay(const boost::posix_time::ptime &pTime, const int delta);
int evaluateMonth12to2(const boost::posix_time::ptime &pTime, const int delta);
int evaluateMonth3to5(const boost::posix_time::ptime &pTime, const int delta);
int evaluateMonth6to8(const boost::posix_time::ptime &pTime, const int delta);
int evaluateMonth9to11(const boost::posix_time::ptime &pTime, const int delta);
int Holiday(const time_t t);
int Syunbun(int yy);
int Syubun(int yy);
time_t my_to_time_t(boost::posix_time::ptime t);
int evaluateHoliday(const boost::posix_time::ptime &pTime, const int delta);
////////////////////////////////////////////////////////

// 時間を丸める関数
boost::posix_time::ptime roundPTime(const boost::posix_time::ptime &pTime, const int delta)
{
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
bool IsLeapYear(int year)
{
    if (((year % 4 == 0) && (year % 100 != 0)) || year % 400 == 0) {
        return true;
    } else {
        return false;
    }
}
int GetLastDay(int year, int month)
{
    bool leap;
    int lmdays[] = {31,29,31,30,31,30,31,31,30,31,30,31};
    int mdays[]  = {31,28,31,30,31,30,31,31,30,31,30,31};

    leap = IsLeapYear(year);
    // leap year
    if (leap == true) {
        return lmdays[month - 1];
		// no_leap year
    } else {
        return mdays[month - 1];
    }
}
int getStep(const boost::posix_time::ptime &pTime, const int delta)
{
	boost::posix_time::ptime pTimeHoge = roundPTime(pTime, delta);
	int ret = 0;
	int hours = pTimeHoge.time_of_day().hours();
	int minutes = pTimeHoge.time_of_day().minutes();
	int sumMinutes = 60 * hours + minutes;
	ret = sumMinutes / delta + 1;
	return ret;
}
int evaluateDay_5_10_20_lastDay(const boost::posix_time::ptime &pTime, const int delta)
{
	boost::posix_time::ptime pTimeHoge = roundPTime(pTime, delta);
	int ret = 0;
	int days = pTimeHoge.date().day();
	if( days == 5 || days == 10 || days == 20 ) {
		ret = 1;
	}else{
		int years = pTimeHoge.date().year();
		int months = pTimeHoge.date().month().as_number();
		bool isMonthEnd = ( days == GetLastDay( years, months) );
		if( isMonthEnd == true ) {
			ret = 1;
		}
	}
	return ret;
}
int evaluateMonth12to2(const boost::posix_time::ptime &pTime, const int delta)
{
	boost::posix_time::ptime pTimeHoge = roundPTime(pTime, delta);
	int ret = 0;
	int months = pTimeHoge.date().month();
	if (months == 12 || months == 1 || months == 2) {
		ret = 1;
	}
	return ret;
}
int evaluateMonth3to5(const boost::posix_time::ptime &pTime, const int delta)
{
	boost::posix_time::ptime pTimeHoge = roundPTime(pTime, delta);
	int ret = 0;
	int months = pTimeHoge.date().month();
	if (months == 3 || months == 4 || months == 5) {
		ret = 1;
	}
	return ret;
}
int evaluateMonth6to8(const boost::posix_time::ptime &pTime, const int delta)
{
	boost::posix_time::ptime pTimeHoge = roundPTime(pTime, delta);
	int ret = 0;
	int months = pTimeHoge.date().month();
	if (months == 6 || months == 7 || months == 8) {
		ret = 1;
	}
	return ret;
}
int evaluateMonth9to11(const boost::posix_time::ptime &pTime, const int delta)
{
	boost::posix_time::ptime pTimeHoge = roundPTime(pTime, delta);
	int ret = 0;
	int months = pTimeHoge.date().month();
	if (months == 9 || months == 10 || months == 11) {
		ret = 1;
	}
	return ret;
}
int Holiday(const time_t t)
{
	int yy;
	int mm;
	int dd;
	int ww;
	int r;
	struct tm *Hizuke;
	const time_t SYUKUJITSU=-676976400; /*1948年7月20日*/
	const time_t FURIKAE=103388400; /*1973年04月12日*/
	Hizuke=localtime(&t);
	yy=Hizuke->tm_year+1900;
	mm=Hizuke->tm_mon+1;
	dd=Hizuke->tm_mday;
	ww=Hizuke->tm_wday;

	r=0;
	if (ww==6){
		r=1;
	} else if (ww==0){
		r=2;
	}

	if (t<SYUKUJITSU){
		return r;
	}

	switch (mm) {
	case 1:
		if (dd==1){
			r=5; /*元日*/
		} else {
			if (yy>=2000){
				if (((int)((dd-1)/7)==1)&&(ww==1)){
					r=5; /*成人の日*/
				}
			} else {
				if (dd==15){
					r=5; /*成人の日*/
				}
			}
		}
		break;
	case 2:
		if (dd==11){
			if (yy>=1967){
				r=5; /*建国記念の日*/
			}
		} else if ((yy==1989)&&(dd==24)){
			r=5; /*昭和天皇の大喪の礼*/
		}
		break;
	case 3:
		if (dd==Syunbun(yy)){
			r=5; /*春分の日*/
		}
		break;
	case 4:
		if (dd==29){
			if (yy>=2007){
				r=5; /*昭和の日*/
			} else if (yy>=1989){
				r=5; /*みどりの日*/
			} else {
				r=5; /*天皇誕生日*/
			}
		} else if ((yy==1959)&&(dd==10)){
			r=5; /*皇太子明仁親王の結婚の儀*/
		}
		break;
	case 5:
		if (dd==3){
			r=5; /*憲法記念日*/
		} else if (dd==4){
			if (yy>=2007) {
				r=5; /*みどりの日*/
			} else if (yy>=1986) {
				/* 5/4が日曜日は『只の日曜』､月曜日は『憲法記念日の振替休日』(～2006年)*/
				if (ww>1) {
					r=3; /*国民の休日*/
				}
			}
		} else if (dd==5) {
			r=5; /*こどもの日*/
		} else if (dd==6) {
			/* [5/3,5/4が日曜]ケースのみ、ここで判定 */
			if ((yy>=2007)&&((ww==2)||(ww==3))){
				r=4; /*振替休日*/
			}
		}
		break;
	case 6:
		if ((yy==1993)&&(dd==9)){
			r=5; /*皇太子徳仁親王の結婚の儀*/
		}
		break;
	case 7:
		if (yy>=2003){
			if (((int)((dd-1)/7)==2)&&(ww==1)){
				r=5; /*海の日*/
			}
		} else if (yy>=1996){
			if (dd==20) {
				r=5; /*海の日*/
			}
		}
		break;
	case 8:
		if (dd==11){
			if (yy>=2016){
				r=5; /*山の日*/
			}
		}
		break;
	case 9:
		if (dd==Syubun(yy)){
			r=5; /*秋分の日*/
		} else {
			if (yy>=2003) {
				if (((int)((dd-1)/7)==2)&&(ww==1)){
					r=5; /*敬老の日*/
				} else if (ww==2){
					if (dd==Syubun(yy)-1){
						r=3; /*国民の休日*/
					}
				}
			} else if (yy>=1966){
				if (dd==15) {
					r=5; /*敬老の日*/
				}
			}
		}
		break;
	case 10:
		if (yy>=2000){
			if (((int)((dd-1)/7)==1)&&(ww==1)){
				r=5; /*体育の日*/
			}
		} else if (yy>=1966){
			if (dd==10){
				r=5; /*体育の日*/
			}
		}
		break;
	case 11:
		if (dd==3){
			r=5; /*文化の日*/
		} else if (dd==23) {
			r=5; /*勤労感謝の日*/
		} else if ((yy==1990)&&(dd==12)){
			r=5; /*即位礼正殿の儀*/
		}
		break;
	case 12:
		if (dd==23){
			if (yy>=1989){
				r=5; /*天皇誕生日*/
			}
		}
	}

	if ((r<=3)&&(ww==1)){
		/*月曜以外は振替休日判定不要
		  5/6(火,水)の判定は上記ステップで処理済
		  5/6(月)はここで判定する  */
		if (t>=FURIKAE) {
			if (Holiday(t-86400)==5){    /* 再帰呼出 */
				r=4;
			}
		}
	}
	return r;
}

/*  春分/秋分日の略算式は
    『海上保安庁水路部 暦計算研究会編 新こよみ便利帳』
	で紹介されている式です。 */

/*春分の日を返す関数*/
int Syunbun(int yy)
{
	int dd;
	if (yy<=1947){
		dd=99;
	} else if (yy<=1979){
		dd=(int)(20.8357+(0.242194*(yy-1980))-(int)((yy-1983)/4));
	} else if (yy<=2099){
		dd=(int)(20.8431+(0.242194*(yy-1980))-(int)((yy-1980)/4));
	} else if (yy<=2150){
		dd=(int)(21.851+(0.242194*(yy-1980))-(int)((yy-1980)/4));
	} else {
		dd=99;
	}
	return dd;
}

/*秋分の日を返す関数*/
int Syubun(int yy)
{
	int dd;
	if (yy<=1947){
		dd=99;
	} else if (yy<=1979){
		dd=(int)(23.2588+(0.242194*(yy-1980))-(int)((yy-1983)/4));
	} else if (yy<=2099){
		dd=(int)(23.2488+(0.242194*(yy-1980))-(int)((yy-1980)/4));
	} else if (yy<=2150){
		dd=(int)(24.2488+(0.242194*(yy-1980))-(int)((yy-1980)/4));
	} else {
		dd=99;
	}
	return dd;
}

time_t my_to_time_t(boost::posix_time::ptime t)
{
    using namespace boost::posix_time;
    ptime epoch(boost::gregorian::date(1970,1,1));
    time_duration::sec_type x = (t - epoch).total_seconds();

    // ... check overflow here ...

    return time_t(x);
}

int evaluateHoliday(const boost::posix_time::ptime &pTime, const int delta)
{
	boost::posix_time::ptime pTimeHoge = roundPTime(pTime, delta);
	int ret = 0;
	{
		int dayOfWeek = pTimeHoge.date().day_of_week().as_number(); // Sunday == 0, Monday == 1, Saturday == 6
		if( dayOfWeek == 0 || dayOfWeek == 6 ) {
			ret = 1;
		}else {
			// 祝日かどうかの確認
			bool isHoliday = false;
			if( 5 == Holiday( my_to_time_t( pTimeHoge ) ) ) {
				isHoliday = true;
			}
			if( isHoliday == true ) {
				ret = 1;
			}
		}
	}
	return ret;
}

int main()
{
	bool displayDataFactor = false;
	bool makeNeural = true;
	// ------------- 取得するもの  ------------- //
	// 営業領域の外枠の最北西
	GeographicCoordinate gCoorNW;
	// 営業領域の外枠の最南東
	GeographicCoordinate gCoorSE;
	// 営業領域の外枠内にあるセル数
	int numCell;
	// 営業領域の外枠内にある有効セル数
	int numValidCell;
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
		const std::string fileName = "inputDataFor0_3_Learn.xml";
		// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_2_Set/Other";
		std::string fileRela = fileDire + "/" + fileName;
		{
			// create an empty property tree
			boost::property_tree::ptree pt;
			// read the xml file
			boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
			{
				gCoorNW.setPhi( pt.get<double>( "myData2.gCoorPair.gCoorFirst_.phi_" ) );
				gCoorNW.setLambda( pt.get<double>( "myData2.gCoorPair.gCoorFirst_.lambda_" ) );
			}
			{
				gCoorSE.setPhi( pt.get<double>( "myData2.gCoorPair.gCoorSecond_.phi_" ) );
				gCoorSE.setLambda( pt.get<double>( "myData2.gCoorPair.gCoorSecond_.lambda_" ) );
			}
			{
				numCell = pt.get<int>("myData2.numCell");
				numValidCell = pt.get<int>("myData2.numValidCell");
				numRow = pt.get<int>("myData2.numRow");
				numCol = pt.get<int>("myData2.numCol");
				cellSizePhi = pt.get<double>("myData2.cellSizePhi");
				cellSizeLambda = pt.get<double>("myData2.cellSizeLambda");
			}
			{
				// 数を調べる
				int N = 0;
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData2.indexToTrueIndex" ).begin();
					itr_last = pt.get_child( "myData2.indexToTrueIndex" ).end();
					N = std::distance(itr_first, itr_last);
				}
				// 要素の追加
				indexToTrueIndex.reserve(N);
				{
					boost::property_tree::ptree::iterator itr_first, itr_last, it;
					itr_first = pt.get_child( "myData2.indexToTrueIndex" ).begin();
					itr_last = pt.get_child( "myData2.indexToTrueIndex" ).end();
					for(it = itr_first; it != itr_last; ++it) {
						indexToTrueIndex.push_back(it->second.get<int>("value"));
					}
				}
			}
		}
	}

	// fullData.xmlの読込
	std::vector<DataFactor> dataFactor;
	dataFactor.reserve(1051200);
	{
		// create an empty property tree
		boost::property_tree::ptree pt;
		{
			// 設定値読み込みファイル名
			const std::string fileName = "fullData.xml";
			// 設定値読み込みファイル先のディレクトリのmakefileからの相対位置
			const std::string fileDire = "./../Data/0_3_Learn/FromServer";
			std::string fileRela = fileDire + "/" + fileName;
			// read the xml file
			boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
		}
		// データ数
		std::size_t numTreeNode = 0;
		{
			boost::property_tree::ptree::iterator itr_first, itr_last, it;
			itr_first = pt.get_child( "table" ).begin();
			itr_last = pt.get_child( "table" ).end();
			numTreeNode = std::distance(itr_first, itr_last);
		}
		std::cout << "[ノードの数] : " << numTreeNode << std::endl;
		if( numTreeNode <= 1 ) {
			std::cout << "データ数が少なすぎます" << std::endl;
			return EXIT_FAILURE;
		}
		{
			boost::property_tree::ptree::iterator itr_first, itr_last, it;
			itr_first = pt.get_child( "table" ).begin();
			itr_last = pt.get_child( "table" ).end();
			for(it = itr_first; it != itr_last; it++) {
				// startPointを取得
				GeographicCoordinate gCoorHoge;
				gCoorHoge.setPhi( it->second.get<double>("startPoint.latitude") );
				gCoorHoge.setLambda( it->second.get<double>("startPoint.longitude") );
				int row = calculateRowFromLatitudes( gCoorHoge.getPhi(), gCoorNW.getPhi(), gCoorSE.getPhi(), cellSizePhi );
				int col = calculateColFromLongitudes( gCoorHoge.getLambda(), gCoorNW.getLambda(), gCoorSE.getLambda(), cellSizeLambda );
				int index = calculateIndexFromRowCol( row, col, numRow, numCol );
				// 範囲外だとindex = 0になる
				if( index != 0 ) {
					// 有効なセルかどうか
					int trueIndex = indexToTrueIndex[index - 1];
					if (trueIndex != -1) {
						// startDateを取得する
						std::string sTimeStr = it->second.get<std::string>("startDate");
						// 空文字の除去
						boost::trim (sTimeStr);
						// boost::posix_time::ptime 型に変換
						boost::posix_time::ptime pTime = boost::posix_time::time_from_string(sTimeStr);
						// 丸めを行う
						pTime = roundPTime(pTime, 1);
						// 末尾に新規登録または登録内容変更
						{
							if (dataFactor.size() == 0) {
								DataFactor dataFactorHoge;
								dataFactorHoge.pTimeRound = pTime;
								dataFactorHoge.step = getStep(pTime, 1);
								dataFactorHoge.day_5_10_20_lastDay = evaluateDay_5_10_20_lastDay(pTime, 1);
								dataFactorHoge.holiday = evaluateHoliday(pTime, 1);
								dataFactorHoge.month12to2 = evaluateMonth12to2(pTime, 1);
								dataFactorHoge.month3to5 = evaluateMonth3to5(pTime, 1);
								dataFactorHoge.month6to8 = evaluateMonth6to8(pTime, 1);
								dataFactorHoge.month9to11 = evaluateMonth9to11(pTime, 1);
								PTIaD pTIaDHoge;
								pTIaDHoge.trueIndex = trueIndex;
								pTIaDHoge.demand = 1;
								dataFactorHoge.pTIaD.push_back(pTIaDHoge);
								dataFactorHoge.maxTemp = 0;
								{
									double maxTempHoge = it->second.get<double>("maxTemp");
									if (maxTempHoge > MAX_TEMP_MAX) {
										maxTempHoge == MAX_TEMP_MAX;
									}else if (maxTempHoge < MAX_TEMP_MIN) {
										maxTempHoge >= MAX_TEMP_MIN;
									}
									maxTempHoge = (maxTempHoge - MAX_TEMP_MIN) / (double)(MAX_TEMP_MAX - MAX_TEMP_MIN);
									dataFactorHoge.maxTemp = maxTempHoge;
								}
								dataFactorHoge.minTemp = 0;
								{
									double minTempHoge = it->second.get<double>("minTemp");
									if (minTempHoge > MIN_TEMP_MAX) {
										minTempHoge == MIN_TEMP_MAX;
									}else if (minTempHoge < MIN_TEMP_MIN) {
										minTempHoge >= MIN_TEMP_MIN;
									}
									minTempHoge = (minTempHoge - MIN_TEMP_MIN) / (double)(MIN_TEMP_MAX - MIN_TEMP_MIN);
									dataFactorHoge.minTemp = minTempHoge;
								}
								dataFactorHoge.dayW = it->second.get<double>("dayW");
								dataFactorHoge.nightW = it->second.get<double>("nightW");
								dataFactor.push_back(dataFactorHoge);
							}else{
								// dataFactorの中で時刻が一致するものがあるか調べる．
								std::vector<DataFactor>::iterator itr_DataFactor_first, itr_DataFactor_last, it_DataFactor;
								itr_DataFactor_first = dataFactor.begin();
								itr_DataFactor_last = dataFactor.end();
								bool existsDataFactor = false;
								for(it_DataFactor = itr_DataFactor_first; it_DataFactor != itr_DataFactor_last; it_DataFactor++) {
									if( it_DataFactor -> pTimeRound == pTime ) {
										existsDataFactor = true;
										// 既存の要素の内容を変更する
										{
											// it_DataFactor -> pTIaD 内でtrueIndexと一致するものがあるか確認
											std::vector<PTIaD>::iterator itr_PTIaD_first, itr_PTIaD_last, it_PTIaD;
											itr_PTIaD_first = it_DataFactor -> pTIaD.begin();
											itr_PTIaD_last = it_DataFactor -> pTIaD.end();
											bool existsTrueIndex = false;
											for(it_PTIaD = itr_PTIaD_first; it_PTIaD != itr_PTIaD_last; it_PTIaD++) {
												if (it_PTIaD -> trueIndex == trueIndex) {
													existsTrueIndex = true;
													(it_PTIaD -> demand)++;
													break;
												}
											}
											if (!existsTrueIndex) {
												// it_DataFactor -> pTIaDの末尾に新規trueIndexと需要1を追加
												PTIaD pTIaDHoge;
												pTIaDHoge.trueIndex = trueIndex;
												pTIaDHoge.demand = 1;
												it_DataFactor -> pTIaD.push_back(pTIaDHoge);
											}
										}
										break;
									}
								}
								if (!existsDataFactor) {
									// dataFactorの末尾に新規追加
									DataFactor dataFactorHoge;
									dataFactorHoge.pTimeRound = pTime;
									dataFactorHoge.step = getStep(pTime, 1);
									dataFactorHoge.day_5_10_20_lastDay = evaluateDay_5_10_20_lastDay(pTime, 1);
									dataFactorHoge.holiday = evaluateHoliday(pTime, 1);
									dataFactorHoge.month12to2 = evaluateMonth12to2(pTime, 1);
									dataFactorHoge.month3to5 = evaluateMonth3to5(pTime, 1);
									dataFactorHoge.month6to8 = evaluateMonth6to8(pTime, 1);
									dataFactorHoge.month9to11 = evaluateMonth9to11(pTime, 1);
									PTIaD pTIaDHoge;
									pTIaDHoge.trueIndex = trueIndex;
									pTIaDHoge.demand = 1;
									dataFactorHoge.pTIaD.push_back(pTIaDHoge);
									dataFactorHoge.maxTemp = 0;
									{
										double maxTempHoge = it->second.get<double>("maxTemp");
										if (maxTempHoge > MAX_TEMP_MAX) {
											maxTempHoge == MAX_TEMP_MAX;
										}else if (maxTempHoge < MAX_TEMP_MIN) {
											maxTempHoge >= MAX_TEMP_MIN;
										}
										maxTempHoge = (maxTempHoge - MAX_TEMP_MIN) / (double)(MAX_TEMP_MAX - MAX_TEMP_MIN);
										dataFactorHoge.maxTemp = maxTempHoge;
									}
									dataFactorHoge.minTemp = 0;
									{
										double minTempHoge = it->second.get<double>("minTemp");
										if (minTempHoge > MIN_TEMP_MAX) {
											minTempHoge == MIN_TEMP_MAX;
										}else if (minTempHoge < MIN_TEMP_MIN) {
											minTempHoge >= MIN_TEMP_MIN;
										}
										minTempHoge = (minTempHoge - MIN_TEMP_MIN) / (double)(MIN_TEMP_MAX - MIN_TEMP_MIN);
										dataFactorHoge.minTemp = minTempHoge;
									}
									dataFactorHoge.dayW = it->second.get<double>("dayW");
									dataFactorHoge.nightW = it->second.get<double>("nightW");
									dataFactor.push_back(dataFactorHoge);
								}
							}
						}
					}
				}
			}
		}
	}

	if (displayDataFactor) {
		// 確認
		{
			std::vector<DataFactor>::iterator itr_first, itr_last, it;
			itr_first = dataFactor.begin();
			itr_last = dataFactor.end();
			for(it = itr_first; it != itr_last; it++) {
				std::cout << "[pTimeRound]" << it->pTimeRound;
				std::cout << "[step]" << it->step;
				std::cout << "[day_5_10_20_lastDay]" << it->day_5_10_20_lastDay;
				std::cout << "[holiday]" << it->holiday;
				std::cout << "[month12to2]" << it->month12to2;
				std::cout << "[month3to5]" << it->month3to5;
				std::cout << "[month6to8]" << it->month6to8;
				std::cout << "[month9to11]" << it->month9to11;
				std::cout << "[maxTemp]" << it->maxTemp;
				std::cout << "[minTemp]" << it->minTemp;
				std::cout << "[dayW]" << it->dayW;
				std::cout << "[nightW]" << it->nightW;
				// {
				std::cout << "[demand]";
				{
					std::vector<PTIaD>::iterator itr_first_, itr_last_, it_;
					itr_first_ = it->pTIaD.begin();
					itr_last_ = it->pTIaD.end();
					for(it_ = itr_first_; it_ != itr_last_; it_++) {
						std::cout << "[" << it_->trueIndex << ", " << it_->demand << "]";
					}
				}
				std::cout << "\n";
			}
		}
	}

	int batchSize = 100;
	std::cout << "[学習時のバッチサイズ]" << batchSize << std::endl;
	// dataFactorをbatchSizの倍数に増やす．
	{
		int numPlus = batchSize - dataFactor.size() % batchSize;
		for (int i = 0; i < numPlus; i++) {
			// 複製するデータindex
			int j = rand() % dataFactor.size();
			DataFactor dataFactorHoge = dataFactor[j];
			dataFactor.push_back(dataFactorHoge);
		}
	}

	// dataFactorのシャッフルをしたいが遅くなるのでやめておく

	std::cout << "データ準備完了" << "\n";
	if (makeNeural) {
		// ニューラルネットワークの学習
		{
			int numData = dataFactor.size();
			std::cout << "[バッチサイズの大きさを考慮して実際に使用するデータ数]" << numData << std::endl;
			constexpr int numInputVar = 1440 + 1 + 1 + 4 + 4; // (step(1440), その他(10))
			int numOutputVar = numValidCell + 1;	  // 有効なセルの数 + 需要数の合計
			std::cout << "[入力次元]" << numInputVar << std::endl;
			std::cout << "[出力次元]" << numOutputVar << std::endl;
			std::vector<double> input_data( numInputVar * numData );
			std::vector<double> target_data( numOutputVar * numData );
			for (int i = 0; i < numData; i++) {
				input_data[i * numInputVar + dataFactor[i].step - 1]++; // stepは1から1440
				input_data[i * numInputVar + 1440 - 1 + 1] = dataFactor[i].day_5_10_20_lastDay;
				input_data[i * numInputVar + 1440 - 1 + 2] = dataFactor[i].holiday;
				input_data[i * numInputVar + 1440 - 1 + 3] = dataFactor[i].month12to2;
				input_data[i * numInputVar + 1440 - 1 + 4] = dataFactor[i].month3to5;
				input_data[i * numInputVar + 1440 - 1 + 5] = dataFactor[i].month6to8;
				input_data[i * numInputVar + 1440 - 1 + 6] = dataFactor[i].month9to11;
				input_data[i * numInputVar + 1440 - 1 + 7] = dataFactor[i].maxTemp;
				input_data[i * numInputVar + 1440 - 1 + 8] = dataFactor[i].minTemp;
				input_data[i * numInputVar + 1440 - 1 + 9] = dataFactor[i].dayW;
				input_data[i * numInputVar + 1440 - 1 + 10] = dataFactor[i].nightW;
				{
					std::vector<PTIaD>::iterator itr_first_, itr_last_, it_;
					itr_first_ = dataFactor[i].pTIaD.begin();
					itr_last_ = dataFactor[i].pTIaD.end();
					int sumDemand = 0;
					for(it_ = itr_first_; it_ != itr_last_; it_++) {
						target_data[i * numOutputVar + it_->trueIndex - 1] = it_->demand;
						sumDemand += it_->demand;
					}
					target_data[i * numOutputVar + numOutputVar - 1] = sumDemand;
				}
			}
			// 入力と出力の準備はできた
			// ダミーデータの作成
			std::vector<double> dummy_data( numData, 0 );
			// 求解の設定パラメータを保持する型
			caffe::SolverParameter solver_param;
			// 設定ファイルから設定パラメータを読み込む(const string &filename, Message *proto)
			caffe::ReadProtoFromTextFileOrDie("Source/solver.prototxt", &solver_param);
			// std::shared_ptrはスマートポインタ.
			// std::shared_ptr<リソース型> 変数名(リソースの生ポインタ);
			// caffe::Solver<double>
			// An interface for classes that perform optimization on Nets.
			// solveをスマートポインタの変数名として使う
			caffe::SolverRegistry<double>::CreateSolver(solver_param);
			// Get a solver using a SolverParameter.
			std::shared_ptr<caffe::Solver<double>> solver(caffe::SolverRegistry<double>::CreateSolver(solver_param));
			const boost::shared_ptr<caffe::Net<double> > net = solver->net();
			const boost::shared_ptr<caffe::MemoryDataLayer<double> > input_layer = boost::dynamic_pointer_cast<caffe::MemoryDataLayer<double>>( net->layer_by_name("input") );
			// num_dataはバッチサイズの整数倍でなければいけない
			input_layer -> caffe::MemoryDataLayer<double>::Reset( &input_data[0], &dummy_data[0], numData);

			// 層の中からoutputの名前がつけられた層のポインタを取得
			const boost::shared_ptr<caffe::MemoryDataLayer<double> > target_layer = boost::dynamic_pointer_cast<caffe::MemoryDataLayer<double>>( net->layer_by_name("target") );
			// num_dataはバッチサイズの整数倍でなければいけない
			target_layer -> caffe::MemoryDataLayer<double>::Reset( &target_data[0], &dummy_data[0], numData);

			// Solverの設定通りに学習を行う
			std::cout << "学習開始" << std::endl;
			solver->Solve();
			std::cout << "学習終了" << std::endl;
			// 学習結果は自動保存
		}
	}

    return EXIT_SUCCESS;
}
