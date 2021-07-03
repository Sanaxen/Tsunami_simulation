# サンプル動画

## 津波シミュレーション

中央防災会議CASE1_new

<video width="320" height="240" controls>  <source src="images/simulation3D18.mp4" type="video/mp4"></video>

東北大モデルVer1.2

<video width="320" height="240" controls>  <source src="images/simulation3D2.mp4" type="video/mp4"></video>

佐竹モデルVer8.0

<video width="320" height="240" controls>  <source src="images/simulation3D10.mp4" type="video/mp4"></video>

修正・宝永地震

<video width="320" height="240" controls>  <source src="images/simulation3D7.mp4" type="video/mp4"></video>

大連動ケース

<video width="320" height="240" controls>  <source src="images/simulation3D14.mp4" type="video/mp4"></video>

安政東海地震v2

<video width="320" height="240" controls>  <source src="images/simulation3D1.mp4" type="video/mp4"></video>

宝永地震v2

<video width="320" height="240" controls>  <source src="images/simulation3D9.mp4" type="video/mp4"></video>

時間差の連動

<video width="320" height="240" controls>  <source src="images/simulation3D15.mp4" type="video/mp4"></video>

東北沿岸部

<video width="320" height="240" controls>  <source src="images/simulation3D16.mp4" type="video/mp4"></video>

南海トラフ_愛知・三重沿岸

<video width="320" height="240" controls>  <source src="images/simulation3D17.mp4" type="video/mp4"></video>

## ■台風による高潮シミュレーション

伊勢湾台風

<video width="320" height="240" controls>  <source src="images/simulation3D3.mp4" type="video/mp4"></video>

台風201330号(HAIYAN)

<video width="320" height="240" controls>  <source src="images/simulation3D8.mp4" type="video/mp4"></video>

## ■河川氾濫シミュレーション
鬼怒川氾濫

<video width="320" height="240" controls>  <source src="images/simulation3D4.mp4" type="video/mp4"></video>



以下は実装時のテストに利用したデータセット
完全反射

<video width="320" height="240" controls>  <source src="images/simulation3D6.mp4" type="video/mp4"></video>


離島トラップ現象

<video width="320" height="240" controls>  <source src="images/simulation3D11.mp4" type="video/mp4"></video>

#### その他のシミュレーション

隕石衝突（KT_inmpact同レベル)

<video width="320" height="240" controls>  <source src="images/simulation3D12.mp4" type="video/mp4"></video>

<video width="320" height="240" controls>  <source src="images/simulation3D13.mp4" type="video/mp4"></video>

### シミュレーションデータを最初から作る場合

`` cd Tsunami_simulation\tsu_dataset\CREATE_DATA ``

地形データ作成手順.pdfを参照ください。
※ドキュメント中のディレクトリ（フォルダー名）が異なっている可能性がありますのでご注意下さい。

必要なデータ（地形データ等）が準備できれば地形データ作成手順.pdfの「他の地形データ、データフォーマットを利用する場合」を参照ください。バッチで自動生成することが出来ます。





# シミュレーションの実行


#### 初期化

シミュレーションを実行する前に` clear.bat `, ` init.bat ` を実行して不要なファイルを削除して下さい。



津波シミュレーション、台風による高潮シミュレーションの実行
`` Tsunami_simulation_Run.bat ``

河川氾濫シミュレーションの実行
`` Tsunami_simulation_River_flooding.bat ``



# 可視化

実行中にリアルタイムに可視化可能です
TSUNAMIリアルタイム2DViwer.bat

3D可視化も可能ですがシミュレーションの実行前に**CalculationParameters.txt**を修正する必要があります。

``EXPORT_OBJ
0``

を
``EXPORT_OBJ
1``



に修正して下さい。

# 動画アニメーションへの変換

[ffmpeg](https://www.ffmpeg.org/)　が必要です。

` cnv_move2D.bat `

 `cnv_move3D.bat `



#### 後始末

` clear.bat `, ` init.bat ` を実行して不要なファイルを削除して下さい。






































































