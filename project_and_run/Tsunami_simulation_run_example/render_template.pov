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
	location<0,32,-27.0>	//���_�̈ʒu
	angle 40		//���o�A������͈͂�ݒ�B����ɂ���ċ������ω�
	look_at<0,0.-2,4>	//����ʒu
}


light_source{<30,20,8> color White}	//�����̈ʒu




//Bathymetry proflie
/*
object
{
	height_field
	{
                        png WAT//�C�ʃf�[�^,\POV-Ray\v3.6\scenes�̃t�H���_�ɒu��.
	}
		texture			//�f��
		{
			 pigment{ rgbt<0.2,0.4,0.7,0.0>}//�F
			finish{diffuse 1.0}//�d�グ�i���˂Ȃǁj
		}
                //rotate -7*y
		translate<-0.5,2.38,-0.5>//�ړ���,�C�ʃf�[�^�ƍ��킹�邱��.
		scale SZ//�g��{��,�C�ʃf�[�^�ƍ��킹�邱�ƁD
}
*/

//*  
object
{
	height_field
	{
                        png ELV//�C�ʃf�[�^,\POV-Ray\v3.6\scenes�̃t�H���_�ɒu��.
		water_level 0.019
	}
		texture			//�f��
		{
			 pigment{color red 0.83 green 0.55 blue 0.1 filter 0.2}//�F
			finish{diffuse 1.0}//�d�グ�i���˂Ȃǁj
		}     
                //rotate -7*y
		translate<-0.5,2.38,-0.5>//�ړ���,�C�ʃf�[�^�ƍ��킹�邱��.
		scale SZ2//�g��{��,�C�ʃf�[�^�ƍ��킹�邱�ƁD
}
object
{
	height_field
	{
                        png ELV//�C�ʃf�[�^,\POV-Ray\v3.6\scenes�̃t�H���_�ɒu��.
		water_level 0.010
	}
		texture			//�f��
		{
			 pigment{color red 0.73 green 0.75 blue 0.1 filter 0.2}//�F
			finish{diffuse 1.0}//�d�グ�i���˂Ȃǁj
		}     
                //rotate -7*y
		translate<-0.5,2.38,-0.5>//�ړ���,�C�ʃf�[�^�ƍ��킹�邱��.
		scale SZ2//�g��{��,�C�ʃf�[�^�ƍ��킹�邱�ƁD
}
object
{
	height_field
	{
                        png ELV//�C�ʃf�[�^,\POV-Ray\v3.6\scenes�̃t�H���_�ɒu��.
		water_level 0.0025
	}
		texture			//�f��
		{
			 pigment{color red 0.23 green 0.75 blue 0.3 filter 0.2}//�F
			finish{diffuse 1.0}//�d�グ�i���˂Ȃǁj
		}     
                //rotate -7*y
		translate<-0.5,2.38,-0.5>//�ړ���,�C�ʃf�[�^�ƍ��킹�邱��.
		scale SZ2//�g��{��,�C�ʃf�[�^�ƍ��킹�邱�ƁD
}
//*/

//Rendaring of Tsunami Height


object{
        height_field{
		png WAV	//�C��n�`�f�[�^,\POV-Ray\v3.6\scenes�̃t�H���_�ɒu��.
                        //water_level 0.25//0.5�ȉ��𖳎��D?�Ȃ��K�v�H
        smooth
                    }
        
         texture {
                //pigment{color red 0.3 green 0.5 blue 1.0 filter 0.2}
                pigment{ rgbt<0.2,0.4,0.7,0.2>}
				//filter�F������
                finish{
                         refraction on
                         //ior 0.15//���ܗ�
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
