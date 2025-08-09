# Tsunami_simulation  
<img src=./images/image04.png > 
<img src=./images/simulation.gif width=1000%> 
It's under maintenance now.    
It was made more than 10 years ago, so I'm reconditioning it in various ways.    
今はメンテナンス中です。    
１０以上年前に作ったものなので色々と再整備しています。

------
# [example Gallery](https://github.com/Sanaxen/Tsunami_simulation/blob/master/example_Gallery.md)  

------

# ビルド方法  

Visual Studio 開発者コマンド プロンプトを利用します。(x64 Native Tools Command Prompt
 for VS 2019)   開発者用コマンドライン ツールを簡単に使用できるように特定の環境変数が設定されている標準コマンド プロンプトです。  


`` cd Tsunami_simulation\project_and_run ``  
``tsu_build_all.bat ``  

# データセット  
https://github.com/Sanaxen/Tsunami_simulation/releases/download/v0.1.0/tsu_dataset.7z

### シミュレーションデータ作成済データセットを使う場合  
``dataset_install.bat`` でインストールできます。

``dataset_install.bat``でインストール出来ない場合はデータセットは以下から取得できます。  
https://github.com/Sanaxen/Tsunami_simulation/releases/download/v0.1.0/tsu_dataset.7z  
中身を移動して下さい。  
<img src=./images/image01.png >  

# シミュレーション方法  

#### 初期化  

- シミュレーションを実行する前に` clear.bat `, ` init.bat ` を実行して不要なファイルを削除して下さい。  
- 計算データ設定バッチを実行して必要なファイルをコピー  
  例えば `安政東海地震v2.bat` を実行すると安政東海地震の計算に必要なファイルが設定される。
- ``Tsunami_simulation_Run.bat`` を実行  
  Tsunami_simulation_Run.batでは計算環境の調整を環境変数でしていしているため必要に応じて修正
- リアルタイムで結果をレンダリング   
 <img src=./images/a.gif width=40%>  
  2Dの場合は `TSUNAMIリアルタイム2DViwer.bat`  
  3Dの場合は `TSUNAMI3DViwer.bat`

レンダリングするには ``CalculationParameters.txt``を修正する必要があります。
<img src=./images/image02.png  width=40%>  

3Dの場合でレンダリングするには  0 を 1 に書き換えて下さい。  
<img src=./images/image03.png  width=40%>  

利用可能なシミュレーションおよび計算データ設定バッチは以下の通り。  
``cd Tsunami_simulation\project_and_run\Tsunami_simulation_run_example``  

## ■津波シミュレーション  
- 中央防災会議CASE1_new.bat  
- 中防モデル3連動.bat  

- 東北.bat  
- 東北大モデルVer1.2.bat  
- 東北大モデルVer1.2_日本全域.bat  
- 東北太平洋沖v2-2.bat  
- 東北太平洋沖v2.bat  
- 佐竹モデルVer4.6.bat  
- 佐竹モデルVer8.0.bat  

- 安政東海地震v2.bat  
- 安政東海地震v2_all.bat  
- 宝永地震v2.bat  
- 修正・宝永地震.bat  
- 大連動ケース.bat  
- 大連動（宝永＋慶長）.bat  
- 慶長地震.bat  

  

## ■台風による高潮シミュレーション  
- 伊勢湾台風.bat  
- 台風201330号(HAIYAN).bat  

## ■河川氾濫シミュレーション  
- 鬼怒川氾濫解析.bat  

 

以下は実装時のテストに利用したデータセット  
※現時点で利用できるかは確認していません  

- 完全反射.bat  
- 浅水効果.bat  
- 遡上.bat  
- 遡上2.bat  
- 遡上3.bat  
- 遡上4.bat  
- 遡上5.bat  
- 境界波現象.bat  
- 集中効果.bat  
- 離島トラップ現象.bat  

## その他のシミュレーション  

Tsunami_simulation_Run.bat を修正する必要があります。  
修正しなくても計算上の問題は無いですがViewerで表示すると波高のスケールが大きすぎてしまいます。
``
set WAVE_SCALE=8000  -> set WAVE_SCALE=80
``
- 隕石衝突（KT_inmpact同レベル)Test
- 隕石衝突（KT_inmpact同レベル)(日本全体)  
<img src=./images/simulation3D12.gif width=50%>  
<img src=./images/sim_.gif width=50%>  


人的被害総数 (防災意識低い):1496470 (防災意識高い):696471  
建造物流出被害総数:3684581  
建造物被害総数 全壊:2586538 半壊:127228 床上浸水:124754 床下浸水:55468  
避難所避難者数:7886 避難所外避難者:3943  
建造物瓦礫量:27057.28万トン  
水害ごみ量:187.13万トン  
瓦礫量:27244.41万トン 処理費用:59937.694億円  
堆積物量:1711647.95万トン  
 
[39993/47603] time [18:53:30]Max[(152.65) 649.70 635.72] 浸水152.65 遡上415.00 MAX463.83  

---
浸水域  
<img src=./images/image05.png width=50%>  

------

### シミュレーションデータを最初から作る場合  

`` cd Tsunami_simulation\tsu_dataset\CREATE_DATA ``  

**地形データ作成手順.pdf **を参照ください。  
※ドキュメント中のディレクトリ（フォルダー名）が異なっている可能性がありますのでご注意下さい。  

必要なデータ（地形データ等）が準備できれば地形データ作成手順.pdfの「他の地形データ、データフォーマットを利用する場合」を参照ください。バッチで自動生成することが出来ます。  

  

  

# シミュレーションの実行   

津波シミュレーション、台風による高潮シミュレーションの実行  
`` Tsunami_simulation_Run.bat ``  

河川氾濫シミュレーションの実行  
`` Tsunami_simulation_River_flooding.bat ``  

  

# 可視化  

実行中にリアルタイムに可視化可能です  
TSUNAMIリアルタイム2DViwer.bat  

3D可視化も可能ですがシミュレーションの実行前に**CalculationParameters.txt**を修正する必要があります。  

``EXPORT_OBJ``  
``0``  

を  
``EXPORT_OBJ``  
``1``  

  

に修正して下さい。 シミュレーション時にobjディレクトリにレンダリング用の3Dデータを出力します。

 **TSUNAMI3DViwer.bat  ** を実行すると３Dアニメーションおよび画像をキャプチャして保存します。  

#### 自動でアニメーション画像を作成するには  

- refreshで画像番号をtimeで指定した番号にセット  
- Cap で画面キャプチャをONにします。  
- Ani で次々と3Dデータを読み込んでレンダリング、キャプチャを実施していきます。  

<img src=./images/image00.png  width=70%>  

# 動画アニメーションへの変換  

[ffmpeg](https://www.ffmpeg.org/)　が必要です。  

` cnv_move2D.bat `  

 `cnv_move3D.bat `  

  

#### 後始末  

` clear.bat `, ` init.bat ` を実行して不要なファイルを削除して下さい。  

------



# 過去の災害から再現性を検証

``Tsunami_simulation\tsu_dataset\KONSEKI_DATA‘``  

ここにcsvで痕跡データを入れて下さい。ファイル名は``ID名_KONSEKI.csv``　です。  

データの取得は下記から入手可能です。  

[津波痕跡データベース](https://tsunami-db.irides.tohoku.ac.jp/)

------

# Requirements  

visual studio 2019 community  

### LandSerf 2.3
[Geographical Information System (GIS) for the visualisation and analysis of surfaces](http://www.landserf.org/)  

### QGIS
[Geographic Information System](https://qgis.org/)  

### freeglut-2.8.0  

[OpenGL Utility Toolkit (GLUT) library](http://freeglut.sourceforge.net/)  

### glew-1.13.0  

[OpenGL Extension Wrangler Library](http://glew.sourceforge.net/)  

### glui-2.36  

[GLUI User Interface Library](https://sourceforge.net/projects/glui/)  

--- 
# References  
**流体数値計算**  
- モデルを意識した地学教材，とくに地震分野　新しい地学教育の試み － 日本地震学会　岡本義雄（大阪教育大学附属高等学校天王寺校舎）  
- 応用数値解析特論I　Projection法による混相流体解析　②圧力（非移流相）の解法  
- High Order Finite Difference Methods　Graeme Sarson　University of Newcastle  
- 偏微分方程式の数値解法 （津波シミュレーションへの応用） 岡山大学環境生命環境科学研究科 生命環境学専攻 渡辺研究室 應谷 洋二，山本和弘，ハスントヤ，渡辺雅二  
- Scale-space and edge detection using anisotropic diffusion　 Pietro Perona , Jitendra Malik  
- Compact Finite Difference Schemes with Spectral-like Resolution　SANJIVA K. LELE  
  
**津波解析一般**  
- 「平 成 17 年 度　宮古湾における津波防災対策検討調査業務委託報告書」  
- 非線形不規則波浪を用いた数値計算と模型実験の港湾設計への活用に関する研究　平成 １４ 年 １１ 月　平山克也  
- 津波予報への数値シミュレーション技術の活用　林 豊（気象研究所）  
- 新潟県沿岸域の離岸流生成機構の解明に関する研究 水工学研究室 竹林 剛 指導教官 細山田 得三  
- 新潟県中越沖地震における津波シミュレーション　第25 回 土木学会関東支部新潟会研究調査発表会論文集， 2007 年11 月　長岡技術科学大学大学院 前田 祥吾　長岡技術科学大学 正会員 犬飼 直之  
- 地震動シミュレーションから得られる海底地盤の鉛直変位を用いた 津波シミュレーション　日本地震工学会論文集 第 12 巻、第 4 号（特集号）、2012  
- 津波浸水想定の設定の手引きVer.1.20平成24 年4 月国土交通省水管理・国土保全局海岸室　国土交通省国土技術政策総合研究所河川研究部海岸研究室  
- 原子力発電所の津波評価技術 平成14 年2 月　土木学会原子力土木委員会・津波評価部会  
- 「津波解析コードの整備及び津波の河川遡上解析 に関する報告書/原子力安全基盤機構」  
- 津波の河川遡上解析の手引き（案）平成19 年５月　(財)国土技術研究センター  
- 津波の河川遡上に関する数値計算の実際　岩崎敏夫　安部至雄　橋本潔  
- 「GISを利用した津波遡上計算と被害推定法」小谷　美佐，今村　文彦，首藤　伸夫  
- 遠地津波の数値計算 ―線形長波式と線形Boussinesq式による計算結果の比較 谷岡勇市郎 北海道大学  
- 港湾技術研究所　Vol.30,No.1 1991/3  
- 相田勇，東京大学地震研究所彙報，Vol.52,pp71-101，1977  
- 相田勇（1977）：陸上に溢れる津波の数値実験－高知県須崎および宇佐の場合－，東京大学地震研究所彙報，Vol.52， pp.441-460.  
- 松富秀夫(1990):仮想水深法、打ち切り水深法による陸上氾濫計算の精度に関するー考察、東北地域災害科学研究、第２６巻、pp.63-65  
- Okada, Y(1985). Surface Deformation due to Shear and Tensile Faults in a Half Space,  
Bull. Seism. Soc. Am. 75, pp.1135-1154  
- Mansinha, L. and Smylie, D. E.，“The displacement fields of inclined faults”，  
Bulletin of the Seismological Society of America，Vol.61，No.5，(1971)， 1433-1440  
- improved linear dispersion characteristics: 2. A slowly varying bathymetry." Coastal Eng.,  
Vol. 18, PP. 183–205.  
  
**非線形分散波**  
- Madsen, P. A. and Sorensen, O. R. (1992). "A new form of the Boussinesq equations with  
- 海岸工学論文集 45(1998)非線形分散波理論式に関する多段階混合差分スキームの提案　原信彦・岩瀬浩之・後藤智明  
- 陽解法による新しい非線形分散波理論の津波数値計算法の提案　鴫原良典、今村文彦  
- 非線形分散波理論を用いた 実用的な津波計算モデル 岩瀬浩之、見上敏文、後藤智明  
- 非線形分散波理論の津波数値解析への適用性と新しい数値モデルの提案　鴫原良典、今村文彦  
  
**高潮計算**  
- Super Gradient Wind(SGW)を考慮した台風の風の推算と高潮数値計算　的場萌実・村上和男・柴木秀之  
- 河口低平農地における高潮氾濫モデルに関する研究　桐　博英  
- 高潮と高波の同時生起確率特性に関する研究　國富, 將嗣  
- 台風による高潮被害想定手法の開―数値シミュレーションによる高潮の再現―  

**隕石衝突による津波計算** 
- シミュレーションの方法は「K/T-impactによる津波の発生」を参考にしている。  
※「K/T-impactによる津波の発生」藤本浩介・今村文彦  

---  
# document  
[津波シミュレータ仕様書](https://github.com/Sanaxen/Tsunami_simulation/tree/main/project_and_run/doc/津波シミュレータ仕様.pdf)  
[津波シミュレータ・マニュアル](https://github.com/Sanaxen/Tsunami_simulation/tree/main/project_and_run/doc/津波シミュレータ・マニュアル.pdf)  
[津波シミュレーション・チュートリアル](https://github.com/Sanaxen/Tsunami_simulation/tree/main/project_and_run/doc/津波シミュレーション・チュートリアル.pdf)  
[断層破壊による地盤変移計算](https://github.com/Sanaxen/Tsunami_simulation/tree/main/project_and_run/doc/断層破壊による地盤変移計算.pdf)  
[地形データ作成手順](https://github.com/Sanaxen/Tsunami_simulation/tree/main/project_and_run/doc/地形データ作成手順.pdf)  
[河川氾濫解析](https://github.com/Sanaxen/Tsunami_simulation/tree/main/project_and_run/doc/河川氾濫解析.pdf)  