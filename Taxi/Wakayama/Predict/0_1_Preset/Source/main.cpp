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

struct MyData
{
	// 外枠や領域を決めるboxは以下の2要素で定義される．
	// ・boxの最北西(north east)
	// ・boxの最南東(south west)

	// 営業領域の外枠
	GeographicCoordinatePair gCoorPair;
	// 追加する営業領域
	std::vector<GeographicCoordinatePair> gCoorPairAdd;
	// 削除する営業領域
	std::vector<GeographicCoordinatePair> gCoorPairRemove;

	// 営業領域は采の目上に分割される．分割された領域の最小単位をセルと呼ぶ．

	// セルの一辺の長さ[m]
	double cellSizeMeter;

};

// 指定した文字を、指定した文字数だけ出力するクラス
class CPut
{
// グローバルな演算子オーバーロードをフレンド指定
friend std::ostream& operator<<(std::ostream& os, const CPut& put);

public:
	CPut(char c, int num) : m_char(c), m_num(num){}

private:
	char m_char;  // 出力する文字
	int m_num;    // 出力する文字数
};

// 出力用の<<演算子定義
std::ostream& operator<<(std::ostream& os, const CPut& put)
{
	for( int i = 0; i < put.m_num; ++i )
	{
		os << put.m_char;
	}
	return os;
}

MyData mySetParameter(bool isTestArea);
MyData mySetParameter(bool isTestArea)
{
	MyData ret;
	{
		if (isTestArea) {
			// 営業領域の外枠
			ret.gCoorPair.set( 34.275080, 135.138074, 34.178566, 135.242616 );
			// 追加する営業領域
			{
				ret.gCoorPairAdd.push_back( GeographicCoordinatePair( 34.276499, 135.177899, 34.184814, 135.236264 ) );
				ret.gCoorPairAdd.push_back( GeographicCoordinatePair( 34.271675, 135.146657, 34.182542, 135.190602 ) );
			}
			// 削除する営業領域
			{
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( -1, -1, -1, -1 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.276782, 135.129233, 34.242164, 135.162535 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.200859, 135.193864, 34.174944, 135.221072 ) );
			}
		}else{
			// 営業領域の外枠
			ret.gCoorPair.set( 34.296332, 135.061091, 34.143423, 135.339895 );
			// 追加する営業領域
			{
				ret.gCoorPairAdd.push_back( GeographicCoordinatePair( 34.284311, 135.183871, 34.140970, 135.278189 ) );
				ret.gCoorPairAdd.push_back( GeographicCoordinatePair( 34.217739, 135.265546, 34.141232, 135.311170 ) );
				ret.gCoorPairAdd.push_back( GeographicCoordinatePair( 34.297741, 135.241681, 34.223232, 135.339878 ) );
				ret.gCoorPairAdd.push_back( GeographicCoordinatePair( 34.281825, 135.130379, 34.150343, 135.192901 ) );
				ret.gCoorPairAdd.push_back( GeographicCoordinatePair( 34.296332, 135.061091, 34.224480, 135.140605 ) );
			}
			// 削除する営業領域
			{
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( -1, -1, -1, -1 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.260126, 135.035527, 34.216726, 135.090817 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.251027, 135.085241, 34.217182, 135.098910 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.303788, 135.083578, 34.278084, 135.142001 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.301980, 135.236779, 34.287886, 135.294815 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.235600, 135.304050, 34.217238, 135.348713 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.225734, 135.283667, 34.203705, 135.320705 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.163086, 135.279536, 34.132188, 135.327012 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.180746, 135.296754, 34.157178, 135.329913 ) );
				ret.gCoorPairRemove.push_back( GeographicCoordinatePair( 34.180980, 135.109406, 34.143055, 135.169831 ) );
			}
		}
		ret.cellSizeMeter = 200;
	}
	return ret;
}

void myDisplayParameter(const MyData &myData);
void myDisplayParameter(const MyData &myData)
{
	CPut cPutEq( '=', 30 );
	CPut cPutSp( ' ', 2 );
	CPut cPutSp2( ' ', 4 );
	std::cout << cPutEq << "\n";
	std::cout << cPutSp << "設定パラメータの表示" << "\n";
	std::cout << cPutEq << "\n";
	std::cout << cPutSp << "* gCoorPair" << "\n";
	{
		std::cout << cPutSp2 << "- ";
		std::cout << "latitude(NW), longitude(NW), latitude(SE), longitude(SE)";
		std::cout << "\n";
		{
			std::cout << cPutSp2 << "# ";
			myData.gCoorPair.printStr();
			std::cout << "\n";
		}
	}
	std::cout << cPutSp << "* gCoorPairAdd" << "\n";
	{
		int N = myData.gCoorPairAdd.size();
		if (N >= 1) {
			std::cout << cPutSp2 << "- ";
			std::cout << "追加領域の個数 : " << N;
			std::cout << "\n";
			std::cout << cPutSp2 << "- ";
			std::cout << "latitude(NW), longitude(NW), latitude(SE), longitude(SE)";
			std::cout << "\n";
			{
				for (int i = 0; i < N; i++) {
					std::cout << cPutSp2 << "# ";
					myData.gCoorPairAdd[i].printStr();
					std::cout << "\n";
				}
			}
		}else{
			std::cout << cPutSp2 << "- ";
			std::cout << "追加する領域が0個です．設定値の変更をしてください．";
			std::cout << "\n";
		}
	}
	std::cout << cPutSp << "* gCoorPairRemove" << "\n";
	{
		int N = myData.gCoorPairRemove.size();
		if (N >= 2) {
			std::cout << cPutSp2 << "- ";
			std::cout << "削除領域の個数 : " << N-1;
			std::cout << "\n";
			std::cout << cPutSp2 << "- ";
			std::cout << "latitude(NW), longitude(NW), latitude(SE), longitude(SE)";
			std::cout << "\n";
			{
				for (int i = 1; i < N; i++) {
					std::cout << cPutSp2 << "# ";
					myData.gCoorPairRemove[i].printStr();
					std::cout << "\n";
				}
			}
		}else{
			std::cout << cPutSp2 << "- ";
			std::cout << "削除する領域は0個です．";
			std::cout << "\n";
		}
	}
	std::cout << cPutSp << "* cellSizeMeter" << "\n";
	{
		std::cout << cPutSp2 << "# ";
		std::cout << myData.cellSizeMeter;
		std::cout << "\n";
	}
}

void mySaveParameter(const MyData &myData, const std::string &fileName, const std::string &fileDire);
void mySaveParameter(const MyData &myData, const std::string &fileName, const std::string &fileDire)
{
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
		boost::property_tree::ptree& root = pt.put("myData", "");

		// add child elements
		{
			{
				boost::property_tree::ptree& child = root.put("gCoorPair", "");
				child.put("gCoorFirst_.phi_", myData.gCoorPair.getFirstPhi());
				child.put("gCoorFirst_.lambda_", myData.gCoorPair.getFirstLambda());
				child.put("gCoorSecond_.phi_", myData.gCoorPair.getSecondPhi());
				child.put("gCoorSecond_.lambda_", myData.gCoorPair.getSecondLambda());
			}
			{
				boost::property_tree::ptree& child = root.put("gCoorPairAdd", "");
				int N = myData.gCoorPairAdd.size();
				for (int i = 0; i < N; i++) {
					boost::property_tree::ptree& cchild = child.add("element", "");
					cchild.put("gCoorFirst_.phi_", myData.gCoorPairAdd[i].getFirstPhi());
					cchild.put("gCoorFirst_.lambda_", myData.gCoorPairAdd[i].getFirstLambda());
					cchild.put("gCoorSecond_.phi_", myData.gCoorPairAdd[i].getSecondPhi());
					cchild.put("gCoorSecond_.lambda_", myData.gCoorPairAdd[i].getSecondLambda());
				}
			}
			{
				boost::property_tree::ptree& child = root.put("gCoorPairRemove", "");
				int N = myData.gCoorPairRemove.size();
				for (int i = 0; i < N; i++) {
					boost::property_tree::ptree& cchild = child.add("element", "");
					cchild.put("gCoorFirst_.phi_", myData.gCoorPairRemove[i].getFirstPhi());
					cchild.put("gCoorFirst_.lambda_", myData.gCoorPairRemove[i].getFirstLambda());
					cchild.put("gCoorSecond_.phi_", myData.gCoorPairRemove[i].getSecondPhi());
					cchild.put("gCoorSecond_.lambda_", myData.gCoorPairRemove[i].getSecondLambda());
				}
			}
			{
				boost::property_tree::ptree& child = root.put("cellSizeMeter", "");
				child.put("", myData.cellSizeMeter);
			}
		}

		// output
		boost::property_tree::write_xml(fileRela, pt, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 2));
	}
}

int main()
{
	// --- ユーザー入力値  --- //
	bool isTestArea = true;
	bool displayData = true;
	// ----------------------- //

	MyData myData;

	// パラメータの設定
	myData = mySetParameter(isTestArea);

	// パラメータの表示
	if (displayData) {
		myDisplayParameter(myData);
	}

	// パラメータの保存
	{
		// 設定値保存ファイル名
		const std::string fileName = "base.xml";
		// 設定値保存ファイル先のディレクトリのmakefileからの相対位置
		const std::string fileDire = "./../Data/0_1_Preset/Other";
		mySaveParameter(myData, fileName, fileDire);
	}

    return EXIT_SUCCESS;
}
