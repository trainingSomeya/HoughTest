#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bitmapio.h"
#include "png_out.h"
#include "png.h"

#define BLACK 0
#define WHITE 255
#define THETA_MAX 1024 //πラジアンを分ける数
#define PIK M_PI/THETA_MAX

//三角関数テーブル（サイン）
double sn[THETA_MAX];
//三角関数テーブル（コサイン）
double cs[THETA_MAX];

//初期化
void init(){
	int i;
	//三角関数テーブルを作成
	for(i=0;i<THETA_MAX;i++){
		sn[i]= sin(PIK*i);
		cs[i]= cos(PIK*i);
	}
}

//bmp変換テスト用グレースケール,後でもう少しマシにする,24bitなので後で４の倍数にもしておく
void Grayscale(Bmp *bmp){
	int i,j;
	unsigned int index;
	unsigned int color;
	printf("height:%d,width:%d\n",bmp->height,bmp->width);
	printf("bitcount:%d\n",bmp->bit_count);
	for(i=0; i<bmp->height;i++){
		for(j=0;j<(bmp->width);j++){
			index = (bmp->height-i-1)*bmp->width*3 + j*3;

			color = (bmp->map[index+0]+bmp->map[index+1]+bmp->map[index+2])/3;

			bmp->map[index+0]=color;
			bmp->map[index+1]=color;
			bmp->map[index+2]= color;
		}
	}
}

//bmp変換テスト用二値化、後でこちらもマシにする
void Binarization(Bmp *bmp){
	int i,j;
	unsigned int index;
	unsigned int color;
	printf("height:%d,width:%d\n",bmp->height,bmp->width);
	printf("bitcount:%d\n",bmp->bit_count);
	for(i=0; i<bmp->height;i++){
		for(j=0;j<(bmp->width);j++){
			index = (bmp->height-i-1)*bmp->width*3 + j*3;

			color = (bmp->map[index+0]+bmp->map[index+1]+bmp->map[index+2])/3;
			if(color<128){//雑に二値化、後で修正
				bmp->map[index+0]= BLACK;
				bmp->map[index+1]= BLACK;
				bmp->map[index+2]= BLACK;
			}else{
				bmp->map[index+0]= WHITE;
				bmp->map[index+1]= WHITE;
				bmp->map[index+2]= WHITE;
			}
		}
	}
}

//回転（補完なし,マイナス方向の回転に対応していない、適当なので怪しい）
void Rotation(Bmp *bmp,int theta){
	uint8_t map[bmp->height][bmp->width*3];
	int i,j,index,index_x,index_y,rota_index_x,rota_index_y,rota_index;
	
	for(i=0; i<bmp->height;i++){
		for(j=0;j<(bmp->width);j++){
			index = (bmp->height-1-i)*bmp->width*3 + j*3;
				map[bmp->height-1-i][j*3]= bmp->map[index];
				map[bmp->height-1-i][j*3+1]= bmp->map[index+1];
				map[bmp->height-1-i][j*3+2]= bmp->map[index+2];
		}
	}

	for(i=0;i<bmp->height;i++){
		for(j=0;j<bmp->width;j++){
			index_y = bmp->height-1-i;
			index_x = j;
			index = index_y*bmp->width*3 + index_x*3;
			rota_index_x = (int)(cs[theta]*index_x + sn[theta]*index_y);
			rota_index_y = (int)(-sn[theta]*index_x + cs[theta]*index_y);
			if(rota_index_x<0 || rota_index_x >= bmp->width || rota_index_y < 0 || rota_index_y >= bmp->height){
				bmp->map[index+0] = 0;   
				bmp->map[index+1] = 0;   
				bmp->map[index+2] = 0;  
			}else{
			       //printf("change=x:%d->x:%d,y:%d->y:%d\n",index_x,rota_index_x,index_y,rota_index_y);	
				bmp->map[index+0] = map[rota_index_y][rota_index_x*3];   
				bmp->map[index+1] = map[rota_index_y][rota_index_x*3+1];   
				bmp->map[index+2] = map[rota_index_y][rota_index_x*3+2]; 
			}	
		}
	}
}

//試しのハフ変換、とりあえず楽譜の傾き補正だけを考える
int Hough(Bmp *bmp){
	int i,j,theta,rho;
	unsigned int index;
	unsigned int color;
	double pow_height = pow(bmp->height,2);
	double pow_width = pow(bmp->width,2);
	int RHO_MAX = (int)sqrt(pow_height+pow_width);//垂線の長さのとりうる最大値,対角線の長さ
	//int RHO_MAX = bmp->height;//test

	// ------ Hough変換
	//直線検出用頻度カウンタ,負の部分が存在するので垂線の長さの最大値を２倍に取る
	int *counter ;
	//	counter = malloc(sizeof(int)*THETA_MAX*2*RHO_MAX);//画像によってはゴミが残っていることがある（要修正）
	//	memset(counter, 0, sizeof(int)*THETA_MAX*2*RHO_MAX);
	counter = (int*)calloc(THETA_MAX*2*RHO_MAX,sizeof(int));

	printf("counter_size:%d,RHO_MAX:%d\n", sizeof(int)*THETA_MAX*2*RHO_MAX,RHO_MAX);
	
	for(i=0;i<bmp->height;i++){
		for(j=0;j<bmp->width;j++){
			index = (bmp->height-i-1)*bmp->width*3 + j*3;
			if(bmp->map[index]==BLACK){
				for(theta=0;theta<THETA_MAX;theta++){
					rho= (int)(j*cs[theta]+i*sn[theta]+0.5);
					counter[RHO_MAX*theta+RHO_MAX+rho]++;
					//printf("counter:%d,num:%d,rho:%d,theta:%d,index:%d,i:%d,j:%d\n",counter[RHO_MAX*theta+RHO_MAX+rho],RHO_MAX*theta+RHO_MAX+rho,rho,theta,index,i,j);
				}
			}
		}
	}

	// ----Hough逆変換
	int end_flag = 0;   //繰り返しを終了させるフラグ
	int count = 0;      //検出された直線

	//直線の検出 -------
	int counter_max;
	int theta_max=0;
	int rho_max=-RHO_MAX;

	do{
		count++;
		counter_max=0;
		//counterが最大になるtheta_maxとrho_maxを求める
		for(theta=0;theta<THETA_MAX;theta++){
			for(rho=-RHO_MAX;rho<RHO_MAX;rho++){
				if(counter[RHO_MAX*theta+RHO_MAX+rho]>counter_max){
					counter_max=counter[RHO_MAX*theta+RHO_MAX+rho];
					printf("counter_max=%d(theta=%d,rho=%d)\n",counter_max,theta,rho);
					//楽譜なので画像幅の半分以下のピクセルの直線になれば検出を終了
					if(counter_max<=bmp->width/2){
						end_flag=1;
					}else{
						end_flag=0;
					}
					theta_max=theta;
					rho_max=rho;
				}
			}
		}
		if(end_flag){
			break;
		}
		printf("theta_max=%d,rho_max=%d\n",theta_max,rho_max);

		//検出した直線の描画
		//xを変化させてyを描く（垂直の線を除く）
		if(theta_max!=0){
			for(j=0;j<bmp->width;j++){
				i=(int)((rho_max-j*cs[theta_max])/sn[theta_max]);
				index = (bmp->height-i-1)*bmp->width*3 + j*3;
				if(i>=bmp->height || i<0) continue;
				bmp->map[index+0]=0;
				bmp->map[index+1]=0;
				bmp->map[index+2]=255;
			}
		}

		//yを変化させてxを描く（水平の線を除く）
		if(theta_max!=THETA_MAX/2){
			for(i=0;i<bmp->height;i++){
				j=(int)((rho_max-i*sn[theta_max])/cs[theta_max]);
				index = (bmp->height-i-1)*bmp->width*3 + j*3;
				//printf("index:h=%d,w=%d\n",i,j);
				if(j>=bmp->width || j<0) continue;
				bmp->map[index+0]=0;
				bmp->map[index+1]=0;
				bmp->map[index+2]=255;			}
		}

		//近傍の直線を消す
		for(j=-10;j<=10;j++)
			for(i=-30;i<=30;i++){
				if(theta_max+i<0){
					theta_max+=THETA_MAX;
					rho_max=-rho_max;
				}
				if(theta_max+i>=THETA_MAX){
					theta_max-=THETA_MAX;
					rho_max=-rho_max;
				}
				if(rho_max+j<-RHO_MAX || rho_max+j>=RHO_MAX)
					continue;
				counter[RHO_MAX*(theta_max+i)+rho_max+RHO_MAX+j]=0;
			}
		//長さが画像幅の半分以下か、直線が10本検出されたら終了
		printf("count:%d\n",count);
	}while(count<10);

	free(counter);

	return theta_max;
}



int main(int argc, char *argv[]){

	Bmp bmp;
	int theta;
	init();

	if(load_bmp_file("/vagrant/image/25.bmp",&bmp) != 0){
		printf("指定されたファイルのロードに失敗しました\n");
		exit(1);
	}

	Binarization(&bmp);
	theta = Hough(&bmp)-THETA_MAX/2;
	printf("theta:%d\n",theta);
	Rotation(&bmp,theta);
	SaveBitmapAsPngFile("/vagrant/image/25.png",&bmp);

	return 0;
}
