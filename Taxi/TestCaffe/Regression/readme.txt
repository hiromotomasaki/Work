git clone https://github.com/BVLC/Caffe.git

http://d.hatena.ne.jp/muupan/20141010/1412895321
を参考に

git reset --hard f84c2a4fb8d633bc7d8fc9771eb06a3cf2215212
としてバージョンを下げる
cpu_onlyにする
あと色々


ちなみに
2016/6/28時点では
git reset --hard f28f5ae2f2453f42b5824723efc326a04dd16d85
が最新


solver.prototxt
ニューラルネットを訓練する時に読み込む大元のテキストファイルだと思ってください。ここに書いてある命令に従い、Caffeは各ネットワークの訓練およびテストを行います。 あとで説明しますが、solver.prototxtにはテスト時、訓練時にどのCNNを使うのか、また学習率といった学習そのものの方針を示すパラメータはいくらにするのかを記述します。

train.prototxt
訓練時に用いるCNNの構造が記されたテキストファイル

test.prototxt
訓練の途中、CNNがどのくらいうまく学習できているのかを調べるため、train.prototxtとは別のデータセットを指定した、同じ構造のCNNを表すテキストファイル

 CNNの構造自体は一緒なので、節約のため、これら(train.prototxt, test.prototxt)をまとめて1つのprototxtに含めることもできます。今回もそのように、この2つのprototxtに相当するものをまとめてtrain.prototxtに書くという方法を取ります。

deploy.prototxt
訓練が終わった後、未知のデータに対して分類を行うためのネットワークの構造を示したもの、要するに本番用のprototxtです。 そのへんのインターネットから拾ってきた画像でも識別できることを友達に自慢したいときに使います。deploy.prototxtはtrain.prototxtからコピペして作ります。 なので、訓練段階では別に作る必要はありません。 





