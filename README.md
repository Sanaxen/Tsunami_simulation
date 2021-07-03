# Tsunami_simulation  

It's under maintenance now.    
It was made more than 10 years ago, so I'm reconditioning it in various ways.    
���̓����e�i���X���ł��B    
�P�O�ȏ�N�O�ɍ�������̂Ȃ̂ŐF�X�ƍĐ������Ă��܂��B

![](./images/simulation.gif)  



------
# [example Gallery](https://github.com/Sanaxen/Tsunami_simulation/blob/master/example_Gallery.md)  

------

# �r���h���@  

Visual Studio �J���҃R�}���h �v�����v�g�𗘗p���܂��B(x64 Native Tools Command Prompt for VS 2019)   �J���җp�R�}���h���C�� �c�[�����ȒP�Ɏg�p�ł���悤�ɓ���̊��ϐ����ݒ肳��Ă���W���R�}���h �v�����v�g�ł��B  

`` cd Tsunami_simulation\project_and_run  
tsu_build_all.bat ``  

  

# �V�~�����[�V�������@  

### �V�~�����[�V�����f�[�^�쐬�σf�[�^�Z�b�g���g���ꍇ  
�f�[�^�Z�b�g�͈ȉ�����擾�ł��܂��B  
https://github.com/Sanaxen/Tsunami_simulation/releases/download/v0.1.0/tsu_dataset.7z  

���p�\�ȃV�~�����[�V��������ьv�Z�f�[�^�ݒ�o�b�`�͈ȉ��̒ʂ�B  
``cd Tsunami_simulation\project_and_run\Tsunami_simulation_run_example``  

## ���Ôg�V�~�����[�V����  
- �����h�Љ�cCASE1_new.bat  
- ���h���f��3�A��.bat  

- ���k.bat  
- ���k�僂�f��Ver1.2.bat  
- ���k�僂�f��Ver1.2_���{�S��.bat  
- ���k�����m��v2-2.bat  
- ���k�����m��v2.bat  
- ���|���f��Ver4.6.bat  
- ���|���f��Ver8.0.bat  

- �������C�n�kv2.bat  
- �������C�n�kv2_all.bat  
- ��i�n�kv2.bat  
- �C���E��i�n�k.bat  
- ��A���P�[�X.bat  
- ��A���i��i�{�c���j.bat  
- �c���n�k.bat  

  

## ���䕗�ɂ�鍂���V�~�����[�V����  
- �ɐ��p�䕗.bat  
- �䕗201330��(HAIYAN).bat  

## ���͐�×��V�~�����[�V����  
- �S�{��×����.bat  

 

�ȉ��͎������̃e�X�g�ɗ��p�����f�[�^�Z�b�g  
�������_�ŗ��p�ł��邩�͊m�F���Ă��܂���  

- ���S����.bat  
- �󐅌���.bat  
- �k��.bat  
- �k��2.bat  
- �k��3.bat  
- �k��4.bat  
- �k��5.bat  
- ���E�g����.bat  
- �W������.bat  
- �����g���b�v����.bat  

## ���̑��̃V�~�����[�V����  

- 覐ΏՓˁiKT_inmpact�����x��)Test
- 覐ΏՓˁiKT_inmpact�����x��)(���{�S��)  

------

### �V�~�����[�V�����f�[�^���ŏ�������ꍇ  

`` cd Tsunami_simulation\tsu_dataset\CREATE_DATA ``  

**�n�`�f�[�^�쐬�菇.pdf **���Q�Ƃ��������B  
���h�L�������g���̃f�B���N�g���i�t�H���_�[���j���قȂ��Ă���\��������܂��̂ł����Ӊ������B  

�K�v�ȃf�[�^�i�n�`�f�[�^���j�������ł���Βn�`�f�[�^�쐬�菇.pdf�́u���̒n�`�f�[�^�A�f�[�^�t�H�[�}�b�g�𗘗p����ꍇ�v���Q�Ƃ��������B�o�b�`�Ŏ����������邱�Ƃ��o���܂��B  

  

  

# �V�~�����[�V�����̎��s  


#### ������  

�V�~�����[�V���������s����O��` clear.bat `, ` init.bat ` �����s���ĕs�v�ȃt�@�C�����폜���ĉ������B  

  

�Ôg�V�~�����[�V�����A�䕗�ɂ�鍂���V�~�����[�V�����̎��s  
`` Tsunami_simulation_Run.bat ``  

�͐�×��V�~�����[�V�����̎��s  
`` Tsunami_simulation_River_flooding.bat ``  

  

# ����  

���s���Ƀ��A���^�C���ɉ����\�ł�  
TSUNAMI���A���^�C��2DViwer.bat  

3D�������\�ł����V�~�����[�V�����̎��s�O��**CalculationParameters.txt**���C������K�v������܂��B  

``EXPORT_OBJ  
0``  

��  
``EXPORT_OBJ  
1``  

  

�ɏC�����ĉ������B  

# ����A�j���[�V�����ւ̕ϊ�  

[ffmpeg](https://www.ffmpeg.org/)�@���K�v�ł��B  

` cnv_move2D.bat `  

 `cnv_move3D.bat `  

  

#### ��n��  

` clear.bat `, ` init.bat ` �����s���ĕs�v�ȃt�@�C�����폜���ĉ������B  

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

