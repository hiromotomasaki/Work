#include <iostream>
#include <vector>
#include <boost/random.hpp>
#include <ctime>
#include <glog/logging.h>

#include <caffe/caffe.hpp>
#include <caffe/layers/memory_data_layer.hpp>

bool XOR(bool a, bool b);
bool XOR(bool a, bool b)
{
  return (a + b) % 2;
}

int main(int argc, char** argv) {

  // glogの初期化 
  google::InitGoogleLogging(argv[0]); 
  // 学習
  {
    // データの作成 x_1 XOR x_2 = target
    // 入力データ:2次元
    // 出力データ:1次元
    int row =100;
    int col = 2;
    std::vector< std::vector<double> > input_data (row, std::vector<double>(col) );
    std::vector<double> target_data(row);
    {
      boost::random::mt19937 gen( static_cast<unsigned long>(time(0)) );
      boost::random::uniform_int_distribution<> dist(0, 1);
      for (int i = 0; i < (int)input_data.size(); ++i) {
        input_data[i][0] = dist(gen);
        input_data[i][1] = dist(gen);
        target_data[i] = (double)XOR( input_data[i][0] == 1, input_data[i][1] == 1);
        // std::cout << "X = "  << input_data[i][0] << ", Y = " << input_data[i][1] << ", X XOR Y = " <<XOR( input_data[i][0] == 1, input_data[i][1] == 1) << std::endl;
      }
    }
    // ダミーデータの作成
    std::vector<double> dummy_data( row, 0 );
    // Solverの設定をテキストファイルから読み込む
    caffe::SolverParameter solver_param; 
    caffe::ReadProtoFromTextFileOrDie("solver.prototxt", &solver_param);
    std::shared_ptr<caffe::Solver<double>>solver(caffe::SolverRegistry<double>::CreateSolver(solver_param));
    const boost::shared_ptr<caffe::Net<double> > net = solver->net();
  
    // 入力
    const boost::shared_ptr<caffe::MemoryDataLayer<double> > input_layer = boost::dynamic_pointer_cast<caffe::MemoryDataLayer<double>>( net->layer_by_name("input") ); 
    input_layer -> caffe::MemoryDataLayer<double>::Reset( (double*)&input_data.front(), (double*)&dummy_data.front(), row);

    // 出力
    const boost::shared_ptr<caffe::MemoryDataLayer<double> > target_layer = boost::dynamic_pointer_cast<caffe::MemoryDataLayer<double>>( net->layer_by_name("target") ); 
    target_layer -> caffe::MemoryDataLayer<double>::Reset( (double*)&target_data.front(), (double*)&dummy_data.front(), row);

    // std::cout << "層のタイプ" << input_layer->type() << std::endl;
    // std::cout << "bottomの数" << input_layer->ExactNumBottomBlobs() << std::endl;
    // std::cout << "topの数" << input_layer->ExactNumTopBlobs() << std::endl;
    // std::cout << "バッチサイズ" << input_layer->batch_size() << std::endl;
    // std::cout << "チャネルの数" << input_layer->channels() << std::endl;
    // std::cout << "高さ" << input_layer->height() << std::endl;
    // std::cout << "幅" << input_layer->width() << std::endl;



    // Solverの設定通りに学習を行う 
    solver->Solve();
  }

  // 予測
  {
    // 初期化
    caffe::Net<double> net_test("net.prototxt", caffe::TEST);
    net_test.CopyTrainedLayersFrom("_iter_4000.caffemodel");
    
    

  }

  

  return 0;
}

