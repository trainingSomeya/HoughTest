#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bitmapio.h"
#include "png_out.h"
#include "png.h"

#include <time.h>

#define BLACK 0
#define WHITE 255
//#define THETA_MAX 1024 //πラジアンを分ける数
#define THETA_MAX 512 //πラジアンを分ける数
#define PIK M_PI/THETA_MAX
#define ROUND 1000 //三角関数の桁数の丸める桁数

#ifndef WIDTHBYTES
#define WIDTHBYTES(bits)    (((bits)+31)/32*4)
#endif//WIDTHBYTES

//三角関数テーブル（サイン）
int sn[THETA_MAX];
//三角関数テーブル（コサイン）
int cs[THETA_MAX];
struct blackMap{
	int counter;
	int *x;
	int *y;
};


struct turnTheta{
	int count;
	int theta;
};

//初期化
void init(){
	int i;
	//三角関数テーブルを作成
	for(i=0;i<THETA_MAX;i++){
		sn[i]= sin(PIK*i)*ROUND;
		cs[i]= cos(PIK*i)*ROUND;
	}
}

//bmp変換テスト用グレースケール,後でもう少しマシにする,heightが負の場合にも対応できるようにする
void Grayscale(Bmp *bmp){
	int i,j;
	unsigned int index;
	unsigned int color;
	uint32_t w_bytes = WIDTHBYTES(bmp->width * bmp->bit_count); 
	printf("height:%d,width:%d\n",bmp->height,bmp->width);
	printf("bitcount:%d\n",bmp->bit_count);
	for(i=0; i<bmp->height;i++){
		for(j=0;j<(bmp->width);j++){
			index = (bmp->height-i-1)*w_bytes + j*3;

			color = (bmp->map[index+0]+bmp->map[index+1]+bmp->map[index+2])/3;

			bmp->map[index+0]=color;
			bmp->map[index+1]=color;
			bmp->map[index+2]= color;
		}
	}
}

//bmp変換用二値化,heightが負の場合にも対応できるようにする
//黒画素の位置を記録する
//楽譜に使う際には白黒なので、こちらの方が向いてる？
struct blackMap Binarization(Bmp *bmp){
	struct blackMap black_map;
	black_map.counter = 0;
	black_map.x = malloc(sizeof(int)*bmp->height*bmp->width);
	black_map.y = malloc(sizeof(int)*bmp->height*bmp->width);

	int i,j;
	unsigned long int index;
	unsigned int index_h;
	unsigned int color;
	uint32_t w_bytes = WIDTHBYTES(bmp->width * bmp->bit_count); 
	printf("height:%d,width:%d\n",bmp->height,bmp->width);
	printf("bitcount:%d\n",bmp->bit_count);

	for(i=0; i<bmp->height;i++){
		index_h = (bmp->height-i-1)*w_bytes;
		for(j=0;j<(bmp->width);j++){
			index = index_h + j*3;

			color = (bmp->map[index+0]+bmp->map[index+1]+bmp->map[index+2])/3;
			if(color<128){//雑に二値化、後で修正
				bmp->map[index+0]= BLACK;
				bmp->map[index+1]= BLACK;
				bmp->map[index+2]= BLACK;
				//printf("index:%d,x:%d,y:%d\n",index,j,i);
				black_map.x[black_map.counter] = j;
				black_map.y[black_map.counter] = i;
				//printf("x:%d, y:%d\n",black_map.map[black_map.counter].x,black_map.map[black_map.counter].y);
				black_map.counter++;
			}else{
				bmp->map[index+0]= WHITE;
				bmp->map[index+1]= WHITE;
				bmp->map[index+2]= WHITE;
			}
		}
	}

	return black_map;
}

/*
//二値化、判別分析法
//楽譜の横線が消えてしまい、うまくいっていない
void discriminantAnalysis(Bmp *bmp){
int i,j;
int hist[255]={};
unsigned int index;
unsigned int color;
uint32_t w_bytes = WIDTHBYTES(bmp->width * bmp->bit_count); 

for(i=0; i<bmp->height;i++){
for(j=0;j<(bmp->width);j++){
index = (bmp->height-i-1)*w_bytes + j*3;
color = (bmp->map[index+0]+bmp->map[index+1]+bmp->map[index+2])/3;

hist[color]++;
}
}

// 判別分析法 
int t = 0;  // 閾値
double max = 0.0;  // w1 * w2 * (m1 - m2)^2 の最大値
int w1 = 0;  // クラス１の画素数
int w2 = 0;  // クラス２の画素数
double sum1 = 0;  // クラス１の平均を出すための合計値
double sum2 = 0;  // クラス２の平均を出すための合計値
double m1 = 0.0;  // クラス１の平均
double m2 = 0.0;  // クラス２の平均
double tmp;

for (i = 0; i < 256; ++i){

for (j = 0; j <= i; ++j){
w1 += hist[j];
sum1 += j * hist[j];
}

for (j = i+1; j < 256; ++j){
w2 += hist[j];
sum2 += j * hist[j];
}

if (w1){
m1 = sum1 / w1;
}

if (w2){
m2 = sum2 / w2;
}

tmp = w1 * w2 * (m1 - m2) * (m1 - m2);

if (tmp > max){
max = tmp;
t = i;
}
}

// tの値を使って２値化 
for(i=0; i<bmp->height;i++){
for(j=0;j<(bmp->width);j++){
index = (bmp->height-i-1)*w_bytes + j*3;

color = (bmp->map[index+0]+bmp->map[index+1]+bmp->map[index+2])/3;
if(color<t){
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
*/

//回転（補完-とりあえず白で埋める,回転後の画像が切れてしまっている,回転の中心は画像の中心、適当なので怪しい）
void Rotation(Bmp *bmp,int theta){
	uint8_t *map ;
	map = malloc(sizeof(uint8_t)*bmp->height*bmp->width*3);

	int i,j,index,index_x,index_y,rota_index_x,rota_index_y,rota_index;
	uint32_t w_bytes = WIDTHBYTES(bmp->width * bmp->bit_count); 
	memcpy(map,bmp->map,sizeof(uint8_t)*bmp->height*bmp->width*3);//第３引数がbmp->mapのサイズと同じとしている、不安なので後でもう一度考える
	/*	
		for(i=0; i<bmp->height;i++){
		for(j=0;j<(bmp->width);j++){
		index = (bmp->height-1-i)*w_bytes + j*3;
		map[index]= bmp->map[index];
		map[index+1]= bmp->map[index+1];
		map[index+2]= bmp->map[index+2];
		}
		}
		*/
	for(i=0;i<bmp->height;i++){
		index_y = (bmp->height-1-i)-bmp->height/2;
		for(j=0;j<bmp->width;j++){
			index_x = j-bmp->width/2;
			index = (bmp->height-1-i)*w_bytes + j*3;
			if(theta>=0){
				rota_index_x = (int)(cs[theta]*index_x + sn[theta]*index_y)/ROUND+bmp->width/2;
				rota_index_y = (int)(-sn[theta]*index_x + cs[theta]*index_y)/ROUND+bmp->height/2;
			}else{
				rota_index_x = (int)(cs[-theta]*index_x - sn[-theta]*index_y)/ROUND+bmp->width/2;
				rota_index_y = (int)(sn[-theta]*index_x + cs[-theta]*index_y)/ROUND+bmp->height/2;	
			}
			if(rota_index_x<0 || rota_index_x >= bmp->width || rota_index_y < 0 || rota_index_y >= bmp->height){
				/*
				//チェック用
				bmp->map[index+0] = BLACK;   
				bmp->map[index+1] = BLACK;   
				bmp->map[index+2] = BLACK; 
				*/
				bmp->map[index+0] = WHITE;   
				bmp->map[index+1] = WHITE;   
				bmp->map[index+2] = WHITE; 

			}else{
				//printf("change=x:%d->x:%d,y:%d->y:%d\n",index_x,rota_index_x,index_y,rota_index_y);	
				bmp->map[index+0] = map[rota_index_y*w_bytes+rota_index_x*3];   
				bmp->map[index+1] = map[rota_index_y*w_bytes+rota_index_x*3+1];   
				bmp->map[index+2] = map[rota_index_y*w_bytes+rota_index_x*3+2]; 
			}	
		}
	}
	free(map);
}

//ハフ変換、とりあえず楽譜の傾き補正だけを考える
int Hough(Bmp *bmp, struct blackMap b_map){
	int i,j,theta,rho,count_index;
	int height, width; 
	unsigned int index;
	unsigned int color;
	struct turnTheta turn_theta[512] = {};//回転角を４つ分の範囲で分ける
	int theta_index;
	double pow_height = pow(bmp->height,2);
	double pow_width = pow(bmp->width,2);
	int RHO_MAX = (int)sqrt(pow_height+pow_width);//垂線の長さのとりうる最大値,対角線の長さ
	int delete_renge = RHO_MAX/40;
	uint32_t w_bytes = WIDTHBYTES(bmp->width * bmp->bit_count); 

	clock_t start,end;

	// ------ Hough変換
	//直線検出用頻度カウンタ,負の部分が存在するので垂線の長さの最大値を２倍に取る
	int (*counter)[2*RHO_MAX] ;
	//	counter = malloc(sizeof(int)*THETA_MAX*2*RHO_MAX);//画像によってはゴミが残っていることがある（要修正）
	//	memset(counter, 0, sizeof(int)*THETA_MAX*2*RHO_MAX);
	counter = calloc(THETA_MAX*(2*RHO_MAX),sizeof(int));

	printf("counter_size:%d,RHO_MAX:%d\n", sizeof(int)*THETA_MAX*2*RHO_MAX,RHO_MAX);


	start = clock();
	for(theta=0;theta<THETA_MAX;theta++){
		for(i=0;i<b_map.counter;i++){
			width = b_map.x[i];
			height = b_map.y[i];
			//printf("w:%d,h:%d\n",width,height);

			///計算時間の問題点/////////////////////////
			rho= (width*cs[theta]+height*sn[theta])/ROUND;//小数点切り捨て、問題がありそうなら再考
			///////////////////////////////////////
			
			//	printf("rho:%d,theta:%d\n",rho,theta);
			counter[theta][rho+RHO_MAX]++;
			//printf("counter:%d,num:%d,rho:%d,theta:%d,index:%d,i:%d,j:%d\n",counter[theta][RHO_MAX+rho],2*RHO_MAX*theta+RHO_MAX+rho,rho,theta,index,i,j);
		}
	}
	end = clock();
	printf("time:%.2f s\n",(double)(end-start)/CLOCKS_PER_SEC);

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
				if(counter[theta][RHO_MAX+rho]>counter_max){
					counter_max=counter[theta][RHO_MAX+rho];
					//printf("counter_max=%d(theta=%d,rho=%d)\n",counter_max,theta,rho);
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
		printf("theta_max=%d,rho_max=%d,max_counter=%d\n",theta_max,rho_max,counter_max);
		/*	
		//検出した直線の描画(確認用)
		//xを変化させてyを描く（垂直の線を除く）
		if(theta_max!=0){
		for(j=0;j<bmp->width;j++){
		i=(int)((rho_max-j*cs[theta_max]/ROUND)/(sn[theta_max]/ROUND));
		index = (bmp->height-i-1)*w_bytes + j*3;
		if(i>=bmp->height || i<0) continue;
		bmp->map[index+0]=0;
		bmp->map[index+1]=0;
		bmp->map[index+2]=255;
		}
		}

		//yを変化させてxを描く（水平の線を除く）
		if(theta_max!=THETA_MAX/2){
		for(i=0;i<bmp->height;i++){
		j=(int)((rho_max-i*sn[theta_max]/ROUND)/(cs[theta_max]/ROUND));
		index = (bmp->height-i-1)*w_bytes + j*3;
		//printf("index:h=%d,w=%d\n",i,j);
		if(j>=bmp->width || j<0) continue;
		bmp->map[index+0]=0;
		bmp->map[index+1]=0;
		bmp->map[index+2]=255;		
		}
		}
		*/	
		//近傍の直線を消す
		for(j=-delete_renge;j<=delete_renge;j++){
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
				counter[theta_max+i][rho_max+RHO_MAX+j]=0;
			}
		}
		//4つずつに分けて各角度の検出頻度を記録
		theta_index = (theta_max+THETA_MAX)/4;	
		turn_theta[theta_index].count++;
		turn_theta[theta_index].theta += theta_max;

		//長さが画像幅の半分以下か、直線が10本検出されたら終了
		printf("count:%d\n",count);
	}while(count<10);

	//最も検出された角度を回転角に定める
	int frequency=0;
	int most_theta;
	for(i=0;i<512;i++){
		if(turn_theta[i].count>frequency){
			frequency = turn_theta[i].count;
			most_theta = turn_theta[i].theta/frequency;
		}

	}

	//////////////////
	//正しい解放か自信がないので後で調べなおす
	free(b_map.x);
	free(b_map.y);
	//////////////////


	free(counter);

	return most_theta-THETA_MAX/2;
}



int main(int argc, char *argv[]){

	clock_t start,end;
	Bmp bmp;
	int theta;
	init();
	if(argc != 2){
		printf("第一引数にファイル名を指定してください");
		return 0;
	}
	char img_file[256] = "/vagrant/image/";
	strcat(img_file,argv[1]);
	printf("%s\n",img_file);

	if(load_bmp_file(img_file,&bmp) != 0){
		printf("指定されたファイルのロードに失敗しました\n");
		exit(1);
	}

	//	discriminantAnalysis(&bmp);

	struct blackMap b_map;

	b_map = Binarization(&bmp);
	printf("blackcount:%d\n",b_map.counter);

	start = clock();
	theta = Hough(&bmp, b_map);
	end = clock();
	printf("hough_time:%.2f s\n",(double)(end-start)/CLOCKS_PER_SEC);

	printf("theta:%d\n",theta);
	start = clock();
	Rotation(&bmp,theta);
	end = clock();
	printf("rotation_time:%.2f s\n",(double)(end-start)/CLOCKS_PER_SEC);

	//	SaveBitmapAsPngFile("/vagrant/image/output.png",&bmp);
	start = clock();
	write_bmp("/vagrant/image/output.bmp",&bmp);
	end = clock();
	printf("write_bmp_time:%.2f s\n",(double)(end-start)/CLOCKS_PER_SEC);

	start = clock();
	delete_bmp(&bmp);
	end = clock();
	printf("delete_bmp_time:%.2f s\n",(double)(end-start)/CLOCKS_PER_SEC);


	return 0;
}
