# Tsunami_simulation  

It's under maintenance now.    
It was made more than 10 years ago, so I'm reconditioning it in various ways.    
今はメンテナンス中です。    
１０以上年前に作ったものなので色々と再整備しています。

![](./images/simulation.gif)  



------
# [example Gallery](https://github.com/Sanaxen/Tsunami_simulation/blob/master/example_Gallery.md)  

------

# ビルド方法  

Visual Studio 開発者コマンド プロンプトを利用します。(x64 Native Tools Command Prompt for VS 2019)   開発者用コマンドライン ツールを簡単に使用できるように特定の環境変数が設定されている標準コマンド プロンプトです。  

`` cd Tsunami_simulation\project_and_run  
tsu_build_all.bat ``  

  

# シミュレーション方法  

### シミュレーションデータ作成済データセットを使う場合  
データセットは以下から取得できます。  
https://github.com/Sanaxen/Tsunami_simulation/releases/download/v0.1.0/tsu_dataset.7z  

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

- 隕石衝突（KT_inmpact同レベル)Test
- 隕石衝突（KT_inmpact同レベル)(日本全体)  

------

### シミュレーションデータを最初から作る場合  

`` cd Tsunami_simulation\tsu_dataset\CREATE_DATA ``  

**地形データ作成手順.pdf **を参照ください。  
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

------

## Requirements  

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

