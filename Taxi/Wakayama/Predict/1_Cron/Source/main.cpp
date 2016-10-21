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
// CronごとにNeuralネットワークで需要を予測し，需要が集中しているセルの位置情報を返す．

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
};


// 時間を丸める関数
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

int main(int argc, char *argv[])
{
    // argv[0] : ./Bin/main.out
	// argv[1] : 2 (threshold)
	// argv[2] : 0.3 (gamma)
	// argv[3] : -2 (displayTimeFrom)
	// argv[4] : 4 (displayTimeTo)
	// argv[5] : 1 (simpleVersion)

	// example of input at commandline : ./Bin/main.out 2 0.3 -2 4 1

	bool displayDataFactor = false;
	bool displayDemand = false;
	bool simpleVersion = true;	// falseにするとgammaやthresholdをうまく調整しないとサークルは出てこない．
	bool displayTrueIndex = false;
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

	// 有効なセルのtrueIndexをIndexに変換するとどうなるか
	std::vector<int> trueIndexToIndex;

	// 表示するピンの時間幅の始点は現在時刻から何分前なのか
	int displayTimeFrom;
	// 表示するピンの時間幅の始点は現在時刻から何分後なのか
	int displayTimeTo;
	// しきい値
	double threshold;
	// 配分の設定値
	double gamma;
	// 離散時間幅(アプリの更新時間ではない．不必要になったが消すと修正しないといけないので1を入れておく)
	int discreteTimeWidth = 1;
	// ----------------------------------------------------- //
	if (argc != 6) {
		std::cout << "argc must be 6. failure!" << "\n";
		return EXIT_FAILURE;
	}

	// commandlineから取得
	{
		try
        {
			threshold = boost::lexical_cast<double>(argv[1]);
        }
        catch(boost::bad_lexical_cast &)
        {
			std::cout << "argv[1](threshold) must be a type of double. failure!" << "\n";
			return EXIT_FAILURE;
        }
		try
        {
			gamma = boost::lexical_cast<double>(argv[2]);
        }
        catch(boost::bad_lexical_cast &)
        {
			std::cout << "argv[2](gamma) must be a type of double. failure!" << "\n";
			return EXIT_FAILURE;
        }
		if (gamma <= 0 || gamma > 1) {
			std::cout << "argv[2](gamma) must be in (0, 1]. failure!" << "\n";
			return EXIT_FAILURE;
		}
		try
        {
			displayTimeFrom = boost::lexical_cast<int>(argv[3]);
        }
        catch(boost::bad_lexical_cast &)
        {
			std::cout << "argv[3](displayTimeFrom) must be a type of int. failure!" << "\n";
			return EXIT_FAILURE;
        }
		try
        {
			displayTimeTo = boost::lexical_cast<int>(argv[4]);
        }
        catch(boost::bad_lexical_cast &)
        {
			std::cout << "argv[4](displayTimeTo) must be a type of int. failure!" << "\n";
			return EXIT_FAILURE;
        }
		int hoge = 1;
		try
        {
			hoge = boost::lexical_cast<int>(argv[5]);
        }
        catch(boost::bad_lexical_cast &)
        {
			std::cout << "argv[5](simpleVersion) must be a type of int. failure!" << "\n";
			return EXIT_FAILURE;
        }
		if (hoge != 0 && hoge != 1) {
			std::cout << "argv[5](simpleVersion) must be in {0, 1}. failure!" << "\n";
			return EXIT_FAILURE;
		}
		if (hoge == 0) {
			simpleVersion = false;
		}else{
			simpleVersion = true;
		}
	}

	// Data/0_2_Set/InputDataFor1_Cron.xmlから読み込む
	{
		// 設定値読み込みファイル名
		const std::string fileName = "inputDataFor1_Cron.xml";
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
				// 領域の確保
				trueIndexToIndex.resize(numValidCell);
				boost::property_tree::ptree::iterator itr_first, itr_last, it;
				itr_first = pt.get_child( "myData2.trueIndexToIndex" ).begin();
				itr_last = pt.get_child( "myData2.trueIndexToIndex" ).end();
				for(it = itr_first; it != itr_last; ++it) {
					std::size_t i = std::distance(itr_first, it);
					trueIndexToIndex[i] = it->second.get<int>("index");
				}
			}
		}
	}

	DataFactor dataFactor;
	// ニューラルネットワークの入力層のデータの取得
	{
		// 読み込みファイル名
		const std::string fileName = "inputNeural.xml";
		// 読み込みファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/1_Cron/FromServer";
		std::string fileRela = fileDire + "/" + fileName;
		{
			// create an empty property tree
			boost::property_tree::ptree pt;
			// read the xml file
			boost::property_tree::read_xml(fileRela, pt, boost::property_tree::xml_parser::no_comments);
			{
				{
					{
						// startDateを取得する
						std::string sTimeStr = pt.get<std::string>("table.startDate");
						// 空文字の除去
						boost::trim (sTimeStr);
						// boost::posix_time::ptime 型に変換
						boost::posix_time::ptime pTime = boost::posix_time::time_from_string(sTimeStr);
						dataFactor.pTimeRound = roundPTime(pTime, 1);
					}
					dataFactor.maxTemp = pt.get<double>("table.maxTemp");
					dataFactor.minTemp = pt.get<double>("table.minTemp");
					dataFactor.dayW = pt.get<double>("table.dayW");
					dataFactor.nightW = pt.get<double>("table.nightW");
				}
				{
					boost::posix_time::ptime pTime = dataFactor.pTimeRound;
					dataFactor.step = getStep(pTime, 1);
					dataFactor.day_5_10_20_lastDay = evaluateDay_5_10_20_lastDay(pTime, 1);
					dataFactor.holiday = evaluateHoliday(pTime, 1);
					dataFactor.month12to2 = evaluateMonth12to2(pTime, 1);
					dataFactor.month3to5 = evaluateMonth3to5(pTime, 1);
					dataFactor.month6to8 = evaluateMonth6to8(pTime, 1);
					dataFactor.month9to11 = evaluateMonth9to11(pTime, 1);
				}
			}
		}
	}
	if (displayDataFactor) {
		std::cout << "現在時刻" << "\n";
		std::cout << "[pTimeRound]" << dataFactor.pTimeRound;
		std::cout << "[step]" << dataFactor.step;
		std::cout << "[day_5_10_20_lastDay]" << dataFactor.day_5_10_20_lastDay;
		std::cout << "[holiday]" << dataFactor.holiday;
		std::cout << "[month12to2]" << dataFactor.month12to2;
		std::cout << "[month3to5]" << dataFactor.month3to5;
		std::cout << "[month6to8]" << dataFactor.month6to8;
		std::cout << "[month9to11]" << dataFactor.month9to11;
		std::cout << "[maxTemp]" << dataFactor.maxTemp;
		std::cout << "[minTemp]" << dataFactor.minTemp;
		std::cout << "[dayW]" << dataFactor.dayW;
		std::cout << "[nightW]" << dataFactor.nightW;
		std::cout << "\n";
	}

	// ニューラルネットワークに入力するデータ個数
	int num = 0;
	// 何分前か
	int preMin = 0;
	// 何分後か
	int postMin = 0;
	{
		// displayTimeFrom(=-2)はdiscreteTimeWidth(=2)何個分(=-1)か
		int numPre = round(displayTimeFrom / (double)discreteTimeWidth);
		// displayTimeTo(=4)はdiscreteTimeWidth(=2)何個分(=2)か
		int numPost = round(displayTimeTo / (double)discreteTimeWidth);

		preMin = numPre * discreteTimeWidth;
		postMin = numPost * discreteTimeWidth;
		num = postMin - preMin + 1;
	}

	// ニューラルネットワークに入力するデータ
	std::vector<DataFactor> vDataFactor(num, dataFactor);
	{
		std::vector<int> vMin(num);
		{
			vMin[0] = preMin;
			if (num != 1) {
				for (int i = 1; i < num; i++) {
					vMin[i] = vMin[i - 1] + 1;
				}
			}
		}
		for (int i = 0; i < num; i++) {
			vDataFactor[i].pTimeRound += boost::posix_time::seconds(vMin[i] * 60);
			{
				boost::posix_time::ptime pTime = vDataFactor[i].pTimeRound;
				vDataFactor[i].step = getStep(pTime, 1);
				vDataFactor[i].day_5_10_20_lastDay = evaluateDay_5_10_20_lastDay(pTime, 1);
				vDataFactor[i].holiday = evaluateHoliday(pTime, 1);
				vDataFactor[i].month12to2 = evaluateMonth12to2(pTime, 1);
				vDataFactor[i].month3to5 = evaluateMonth3to5(pTime, 1);
				vDataFactor[i].month6to8 = evaluateMonth6to8(pTime, 1);
				vDataFactor[i].month9to11 = evaluateMonth9to11(pTime, 1);
			}
		}
	}
	if (displayDataFactor) {
		std::cout << "ニューラルネットワークに入力するデータ" << "\n";
		for (int i = 0; i < (int)vDataFactor.size(); i++) {
			std::cout << "[pTimeRound]" << vDataFactor[i].pTimeRound;
			std::cout << "[step]" << vDataFactor[i].step;
			std::cout << "[day_5_10_20_lastDay]" << vDataFactor[i].day_5_10_20_lastDay;
			std::cout << "[holiday]" << vDataFactor[i].holiday;
			std::cout << "[month12to2]" << vDataFactor[i].month12to2;
			std::cout << "[month3to5]" << vDataFactor[i].month3to5;
			std::cout << "[month6to8]" << vDataFactor[i].month6to8;
			std::cout << "[month9to11]" << vDataFactor[i].month9to11;
			std::cout << "[maxTemp]" << vDataFactor[i].maxTemp;
			std::cout << "[minTemp]" << vDataFactor[i].minTemp;
			std::cout << "[dayW]" << vDataFactor[i].dayW;
			std::cout << "[nightW]" << vDataFactor[i].nightW;
			std::cout << "\n";
		}
	}

	// ニューラルネットワークで需要数と総需要数を取得
	std::vector<double> res(numValidCell + 1, 0);
	{
		for (int i = 0; i < num; i++) {
			// Netを取得する
			// caffe::Net<float> net_test("train_test.prototxt", caffe::TEST);
			caffe::Net<double> testNet("Source/train_test.prototxt", caffe::TEST);
			// 訓練されたネットを取得する
			testNet.CopyTrainedLayersFrom("./../Data/0_3_Learn/Other/Neural_iter_50000.caffemodel");
			// // 予測する
			const boost::shared_ptr<caffe::MemoryDataLayer<double> > input_test_layer = boost::dynamic_pointer_cast<caffe::MemoryDataLayer<double>>( testNet.layer_by_name("input") );
			const boost::shared_ptr<caffe::MemoryDataLayer<double> > target_test_layer = boost::dynamic_pointer_cast<caffe::MemoryDataLayer<double>>( testNet.layer_by_name("target") );
			// ミニバッチの変更
			// batchSize = 100 -> batchSize = 1
			int batchSize = 1;
			input_test_layer->set_batch_size(batchSize);
			target_test_layer->set_batch_size(batchSize);

			int numData = 1 * batchSize;
			constexpr int numInputVar = 1440 + 1 + 1 + 4 + 4; // (step(1440), その他(10))
			int numOutputVar = numValidCell + 1;	  // 有効なセルの数 + 需要数の合計
			std::vector<double> input_test_data( numInputVar * numData, 0 );
			std::vector<double> target_test_data( numOutputVar * numData, 0 );
			std::vector<double> dummy_test_data( numData, 0 );
			// input_test_dataにデータを詰める
			{
				input_test_data[vDataFactor[i].step - 1]++; // stepは1から1440
				input_test_data[1440 - 1 + 1] = vDataFactor[i].day_5_10_20_lastDay;
				input_test_data[1440 - 1 + 2] = vDataFactor[i].holiday;
				input_test_data[1440 - 1 + 3] = vDataFactor[i].month12to2;
				input_test_data[1440 - 1 + 4] = vDataFactor[i].month3to5;
				input_test_data[1440 - 1 + 5] = vDataFactor[i].month6to8;
				input_test_data[1440 - 1 + 6] = vDataFactor[i].month9to11;
				input_test_data[1440 - 1 + 7] = vDataFactor[i].maxTemp;
				input_test_data[1440 - 1 + 8] = vDataFactor[i].minTemp;
				input_test_data[1440 - 1 + 9] = vDataFactor[i].dayW;
				input_test_data[1440 - 1 + 10] = vDataFactor[i].nightW;
			}

			// 層のリセット
			input_test_layer -> caffe::MemoryDataLayer<double>::Reset( &input_test_data[0], &dummy_test_data[0], numData);
			target_test_layer -> caffe::MemoryDataLayer<double>::Reset( &target_test_data[0], &dummy_test_data[0], numData);

			// 順伝播計算
			testNet.Forward();

			// demands[i]に計算結果を詰める
			res[numOutputVar - 1] += testNet.blob_by_name("ip2")->cpu_data()[numOutputVar - 1];
			for(int j = 0; j < numOutputVar - 1; j++) {
				res[j] += testNet.blob_by_name("ip2")->cpu_data()[j];
			}
		}
	}

	if (displayDemand) {
		// 値の確認
		std::cout << "各有効セルでの需要数(1より大きい)" << "\n";
		for (int i = 0; i < numValidCell; i++) {
			if (res[i] > 1) {
				std::cout << "[" << res[i] << "]";
			}
		}
		std::cout << "\n";
		std::cout << "[総需要数]" << res[numValidCell] << "\n";
	}

	// サークルを描画するtrueIndexを取得
	std::vector<int> vTrueIndex;
	{
		if (simpleVersion) {
			for (int i = 0; i < numValidCell; i++) {
				if (res[i] >= threshold) {
					vTrueIndex.push_back(i + 1);
				}
			}
		}else{
			std::vector<double> resHoge(numValidCell, 0);
			std::vector<double> demandDist(numValidCell, 0);
			double sumCellDemand = 0;
			{
				for (int i = 0; i < numValidCell; i++) {
					if (res[i] > 0) {
						demandDist[i] = res[i];
					}
				}
				sumCellDemand = std::accumulate(demandDist.begin(), demandDist.end(), 0.0);
				if (sumCellDemand != 0) {
					for (int j = 0; j < numValidCell; j++) {
						demandDist[j] /= sumCellDemand;
					}
				}
			}
			double sumDemand = ceil(res[numValidCell]);
			{
				if (sumCellDemand != 0 && sumDemand != 0) {
					// demandDistとgammaとsumDemandを基にdemandをresHogeに分配する
					gamma = gamma / (double)sumDemand;
					std::vector<double>::iterator it, itr_first;
					itr_first = demandDist.begin();
					while (sumDemand > 0) {
						// 最大要素のイテレータを取得
						it = std::max_element(demandDist.begin(), demandDist.end());
						*it -= gamma;
						std::size_t index = std::distance(itr_first, it); // std::distance(itr_first, itr_first) == 0
						resHoge[(int)index] = resHoge[(int)index] + 1;
						sumDemand = sumDemand - 1;
					}
				}
			}
			for (int i = 0; i < numValidCell; i++) {
				if (resHoge[i] >= threshold) {
					vTrueIndex.push_back(i + 1);
				}
			}
		}
	}

	// 確認
	if (displayTrueIndex) {
		int N = vTrueIndex.size();
		if (N >= 1) {
			for (int i = 0; i < N; i++) {
				std::cout << "[" << vTrueIndex[i] << "]";
			}
			std::cout << "\n";
		}else{
			std::cout << "描画するサークルはない" << "\n";
		}
	}

	// circlePosition.xmlの作成
	{
		int N = vTrueIndex.size();
		if (N == 0) {
			// 空のサークル情報をを作成
			try {
				const boost::filesystem::path src("./../Data/0_2_Set/Other/emptyCircle.xml");
				boost::filesystem::path dst("./../Data/1_Cron/Other/circlePoints.xml");
				boost::filesystem::copy_file(src, dst, boost::filesystem::copy_option::overwrite_if_exists);
			} catch (std::exception& e) {
				std::cout << "failure!" << "\n";
				return EXIT_FAILURE;
			}
		}else{
			std::vector<GeographicCoordinate> vGCoor(N);
			for (int i = 0; i < N; i++) {
				int trueIndex = vTrueIndex[i];
				int index = trueIndexToIndex[trueIndex - 1];
				// indexを位置情報に変換する．
				int row = calculateRowFromIndex( index, numCol, numCell );
				int col = calculateColFromIndex( index, numCol, numCell );
				vGCoor[i].setPhi( gCoorNW.getPhi() - row * cellSizePhi + cellSizePhi / 2.0 );
				vGCoor[i].setLambda( gCoorNW.getLambda() + col * cellSizeLambda - cellSizeLambda / 2.0 );
			}
			// vGCoorの登録
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
					for (int i = 0; i < N; i++) {
						boost::property_tree::ptree& child = root.add("data", "");
						child.put("latitude", vGCoor[i].getPhi());
						child.put("longitude", vGCoor[i].getLambda());
					}
				}
				// output
				boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
			}
		}
	}

    return EXIT_SUCCESS;
}
