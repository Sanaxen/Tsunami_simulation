#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "finish.inc"
#include "consts.inc"

          
#declare S = 2.23;
#declare SZ = <S*16,S*0.7,S*16>;
#declare SZ2 = <S*16,S*0.7+0.4,S*16>;

#declare WAT = "./pov/water_depth_data.PNG"
#declare ELV = "./pov/elevation_data.PNG"          
#declare WAV = "./pov/output_W%06d.PNG"                                       
                                        
camera
{
	location<0,32,-27.0>	//視点の位置
	angle 40		//視覚、見える範囲を設定。それによって距離も変化
	look_at<0,0.-2,4>	//見る位置
}


light_source{<30,20,8> color White}	//光源の位置




//Bathymetry proflie
/*
object
{
	height_field
	{
                        png WAT//海面データ,\POV-Ray\v3.6\scenesのフォルダに置く.
	}
		texture			//素材
		{
			 pigment{ rgbt<0.2,0.4,0.7,0.0>}//色
			finish{diffuse 1.0}//仕上げ（反射など）
		}
                //rotate -7*y
		translate<-0.5,2.38,-0.5>//移動量,海面データと合わせること.
		scale SZ//拡大倍率,海面データと合わせること．
}
*/

//*  
object
{
	height_field
	{
                        png ELV//海面データ,\POV-Ray\v3.6\scenesのフォルダに置く.
		water_level 0.019
	}
		texture			//素材
		{
			 pigment{color red 0.83 green 0.55 blue 0.1 filter 0.2}//色
			finish{diffuse 1.0}//仕上げ（反射など）
		}     
                //rotate -7*y
		translate<-0.5,2.38,-0.5>//移動量,海面データと合わせること.
		scale SZ2//拡大倍率,海面データと合わせること．
}
object
{
	height_field
	{
                        png ELV//海面データ,\POV-Ray\v3.6\scenesのフォルダに置く.
		water_level 0.010
	}
		texture			//素材
		{
			 pigment{color red 0.73 green 0.75 blue 0.1 filter 0.2}//色
			finish{diffuse 1.0}//仕上げ（反射など）
		}     
                //rotate -7*y
		translate<-0.5,2.38,-0.5>//移動量,海面データと合わせること.
		scale SZ2//拡大倍率,海面データと合わせること．
}
object
{
	height_field
	{
                        png ELV//海面データ,\POV-Ray\v3.6\scenesのフォルダに置く.
		water_level 0.0025
	}
		texture			//素材
		{
			 pigment{color red 0.23 green 0.75 blue 0.3 filter 0.2}//色
			finish{diffuse 1.0}//仕上げ（反射など）
		}     
                //rotate -7*y
		translate<-0.5,2.38,-0.5>//移動量,海面データと合わせること.
		scale SZ2//拡大倍率,海面データと合わせること．
}
//*/

//Rendaring of Tsunami Height


object{
        height_field{
		png WAV	//海底地形データ,\POV-Ray\v3.6\scenesのフォルダに置く.
                        //water_level 0.25//0.5以下を無視．?なぜ必要？
        smooth
                    }
        
         texture {
                //pigment{color red 0.3 green 0.5 blue 1.0 filter 0.2}
                pigment{ rgbt<0.2,0.4,0.7,0.2>}
				//filter：透明率
                finish{
                         refraction on
                         //ior 0.15//屈折率
                         //fade_distance 2.0
                         //fade_power 1.0
                         reflection 0.5
                         diffuse 3.1
		
                       }
                }


        //rotate -7*y
               
        translate<-0.5,1.50,-0.5>
                
        scale SZ
        scale <1, 1.5,1>
       
}
