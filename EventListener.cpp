

/*
* @file EventListener.cpp
* @brief イベントの処理を記述する
*/

#define __CV_H__
/*-------- include --------*/
#include "TPIP3.h"           // TPIP関係
#include "EventListener.h"   // ウィンドウのイベント処理関係
#include "cvMat2HDC.h"


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <fstream>

#include <stdio.h>
#pragma  warning(disable:4996)	//	セキュリティが弱い古い関数に対する警告の解除

/*-------- メンバ変数定義 --------*/
static struct GC_STRUCT  mPad;		//!< @brief ゲームパッド入力データ
static struct OUT_DT_STR mOutDt;	//!< @brief TPJTで扱う制御情報
static struct INP_DT_STR mInpDt;	//!< @brief TPJTで扱うセンサー情報
static int    updatePaint;			//!< @brief onPaint 更新フラグ(0:更新済　1:未更新)
int ctrl_flag;
int drive_mode;
int pwm_flag;
int op_flag;
int flag, flag2;

short pwm;
short pwm2;
/*typedef struct OUT_DT_STR{
	unsigned short d_out;
	short nouse1;
	short PWM[4];
	short PWM2[16];
	}out_dt_t;
	*/

struct OUT_DT_STR out_dt;

#define SCALE_X 3.0
#define SCALE_Y 5.0


//@comment ２つのカメラの切り替えフラグ
bool cameraFlg = true;
//@comment コントローラーGUI画像を表示する
cvMat2HDC ctrl[12];
//@comment △、□、○、×の位置
const int circlePos[4][2] = { { 1055, 480 }, { 990, 550 }, { 1115, 550 }, { 1055, 620 } };
//@comment　方向キーの位置
const int rec1Pos[2][2] = { { 740, 480 }, { 740, 620 } };
const int rec2Pos[2][2] = { { 670, 550 }, { 800, 550 } };
//@comemnt l1, l2, r1, r2の位置
const int lrPos[4][2] = { { 700, 300 }, { 700, 370 }, { 1020, 300 }, { 1020, 370 } };
//@comment コントローラーが押されているか
bool ctrlFlg[12] = { false, false, false, false, false, false, false, false, false, false, false, false };
enum CTRL_BUTTON{
	TRGL = 0,
	RCTG = 1,
	CRCL = 2,
	CRSS = 3,
	L1 = 5,
	L2 = 4,
	R1 = 7,
	R2 = 6,
	UP = 8,
	DOWN = 9,
	RGHT = 10,
	LEFT = 11
};
	


/*-------- 関数定義 --------*/
//@comment コントローラーGUIの画像の位置の初期化
void initCtrlGUI()
{
	//@comment ○ボタン
	for (int i = 0; i < 4; i++){
		ctrl[i].setRendPos(circlePos[i][0], circlePos[i][1]);
	}
	//@comment lrボタン
	for (int i = 0; i < 4; i++){
		ctrl[i + 4].setRendPos(lrPos[i][0], lrPos[i][1]);
	}
	//@comment rect1ボタン
	for (int i = 0; i < 2; i++){
		ctrl[i + 8].setRendPos(rec1Pos[i][0], rec1Pos[i][1]);
	}
	//@comment lrボタン
	for (int i = 0; i < 2; i++){
		ctrl[i + 10].setRendPos(rec2Pos[i][0], rec2Pos[i][1]);
	}
}

//@comment コントローラーGUIへの押されているかどうかのフラグ変更
void switchButtons()
{
	//@comment R1
	if (mPad.Button[7] == 128){ ctrlFlg[R1] = true; }
	else{ ctrlFlg[R1] = false; }

	//@comment L1
	if (mPad.Button[6] == 128){ ctrlFlg[L1] = true; }
	else{ ctrlFlg[L1] = false; }

	//@comment R1
	if (mPad.Button[5] == 128){ ctrlFlg[R2] = true; }
	else{ ctrlFlg[R2] = false; }

	//@comment L1
	if (mPad.Button[4] == 128){ ctrlFlg[L2] = true; }
	else{ ctrlFlg[L2] = false; }

	//@comment Triangle
	if (mPad.Button[0] == 128){ ctrlFlg[TRGL] = true; }
	else{ ctrlFlg[TRGL] = false; }

	//@comment Circle
	if (mPad.Button[1] == 128){ ctrlFlg[CRCL] = true; }
	else{ ctrlFlg[CRCL] = false; }

	//@comment Cross
	if (mPad.Button[2] == 128){ ctrlFlg[CRSS] = true; }
	else{ ctrlFlg[CRSS] = false; }

	//@comment Rectangle
	if (mPad.Button[3] == 128){ ctrlFlg[RCTG] = true; }
	else{ ctrlFlg[RCTG] = false; }

	//上を入力
	if (mPad.POV[0] == 0){ ctrlFlg[UP] = true; }
	else{ ctrlFlg[UP] = false; }

	//下を入力
	if (mPad.POV[0] == 18000){ ctrlFlg[DOWN] = true; }
	else{ ctrlFlg[DOWN] = false; }

	//左を入力
	if (mPad.POV[0] == 27000){ ctrlFlg[RGHT] = true; }
	else{ ctrlFlg[RGHT] = false; }

	//右を入力
	if (mPad.POV[0] == 9000){ ctrlFlg[LEFT] = true; }
	else{ ctrlFlg[LEFT] = false; }
}

//@comment コントローラーGUIの描画
void rendController()
{
	for (int i = 0; i < 12; i++){
		ctrl[i].setHDC();
	}
	//@comment ○ボタン
	for (int i = 0; i < 4; i++){
		if (ctrlFlg[i]){ ctrl[i].load("./img/circle_on"); }
		else{ ctrl[i].load("./img/circle"); }
	}
	//@comment lrボタン
	for (int i = 0; i < 4; i++){
		if (ctrlFlg[i + 4]){ ctrl[i + 4].load("./img/lr_on"); }
		else{ ctrl[i + 4].load("img/lr"); }
	}
	//@comment rect1ボタン
	for (int i = 0; i < 2; i++){
		if (ctrlFlg[i + 8]){ ctrl[i + 8].load("./img/rect_1_on"); }
		else{ ctrl[i + 8].load("./img/rect_1"); }
	}
	//@comment lrボタン
	for (int i = 0; i < 2; i++){
		if (ctrlFlg[i + 10]){ ctrl[i + 10].load("./img/rect_2_on"); }
		else{ ctrl[i + 10].load("./img/rect_2"); }
	}

	for (int i = 0; i < 12; i++){
		ctrl[i].releaseHDC();
	}
}



/**
* WM_CREATEのメッセージクラッカー\n
* ウィンドウが生成される前にコールされる関数\n
* ここで生成したタイマーを利用し、TPIPとの通信、画面の描画のイベントを発生します
*
* @param[in]  hwnd            イベントが発生したウィンドウのハンドル
* @param[in]  lpCreateStruct  ウィンドウ作成時のパラメータ
*
* @retval 常にTRUE
*
* @see http://msdn.microsoft.com/en-us/library/ms632619(VS.85).aspx
*
*/
BOOL onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
	//mOutDt.PWM[3] = 250;
	//mOutDt.PWM[0] = 0;
	//mOutDt.PWM[1] = 700;
	//mOutDt.PWM[2] = 920;
	TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
	SetTimer(hwnd, ID_MYTIMER, 60, NULL);	// タイマーを生成する
	initCtrlGUI();
	return TRUE; //ここをFALSE(0)にするとメッセージでDestroyが呼ばれる
}


/**
* WM_ACTIVATEのメッセージクラッカー\n
* ウィンドウがアクティブ/非アクティブになる際にコールされる関数
*
* @param[in] hwnd          イベントが発生したウィンドウのハンドル
* @param[in] state         ウィンドウがアクティブ/非アクティブかの状態
* @param[in] hwndActDeact  ウィンドウのハンドル\n
*                            state == 0の場合は、新しくアクティブになったウィンドウのハンドル\n
*                            state != 0の場合は、それまでアクティブであったウィンドウのハンドル
* @param[in] fMinimized    ウィンドウが最小化されているかの判定フラグ\n
*                            0以外の値の場合はウィンドウは最小化されています\n
*                            0の場合はウィンドウは最小化されていません\n
*
* @see http://msdn.microsoft.com/ja-jp/library/ms646274(d=lightweight).aspx
*
*/
void onActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized) {
	return;
}



/**
* WM_CLOSEのメッセージクラッカー\n
* ウィンドウかアプリケーションが終了する際にコールされる関数\n
* 終了の確認を行い、YESならばウィンドウを破棄する。NOならば破棄しない
*
* @param[in] hwnd  イベントが発生したウィンドウのハンドル
*
* @see http://msdn.microsoft.com/en-us/library/ms632617(VS.85).aspx
*
*/
void onClose(HWND hwnd) {

	int ans = 0; // 戻り値取得用

	// メッセージダイアログを表示し、終了の確認をする
	ans = MessageBox(hwnd, TEXT("終了しますか？"), TEXT("終了確認"), MB_YESNO | MB_ICONEXCLAMATION);

	// YESが選択された場合はウィンドウを閉じる
	if (ans == IDYES) {
		DestroyWindow(hwnd); // ウインドウを閉じる関数
	}

	return;
}



/**
* WM_DESTROYのメッセージクラッカー\n
* ウィンドウが破棄される際にコールされる関数\n
* TPIPライブラリと生成したタイマーの解放を行う
*
* @param[in]  hwnd イベントが発生したウィンドウのハンドル
*
* @see http://msdn.microsoft.com/en-us/library/ms632620(VS.85).aspx
*
*/
void onDestroy(HWND hwnd) {

	// TPIPライブラリ群の解放
	TPGM_delete();
	TPGC_close();
	TPJT_close();

	//タイマー解放
	KillTimer(hwnd, ID_MYTIMER);
	PostQuitMessage(0);

	return;
}



/**
* WM_SIZEのメッセージクラッカー\n
* ウィンドウのサイズが変更された後にコールされる関数\n
* TPGMで使用しているサーフェイスを変更後のサイズで構成し直す
*
* @param[in]  hwnd  イベントが発生したウィンドウのハンドル
* @param[in]  state サイズ変更のタイプ
* @param[in]  cx    ウィンドウの横幅
* @param[in]  cy    ウィンドウの縦幅
*
* @see http://msdn.microsoft.com/en-us/library/ms632646(v=vs.85).aspx
*
*/
void onSize(HWND hwnd, UINT state, int cx, int cy) {

	// サーフェイスの再構成
	TPGM_delete();
	TPGM_create(hwnd, QVGA, NULL);

	return;
}



/**
* WM_TIMERのメッセージクラッカー\n
* タイマーが終了した際にコールされる関数\n
* ゲームパッドからの入力、TPIPとの通信、画面の描5画のイベントを発生する\n
* 制御データの加工等はTPJT_set_ctrl()を実行するまでに行うこと
*
* @param[in]  hwnd イベントが発生したウィンドウのハンドル
* @param[in]  id   イベントを起こしたタイマーID
*
* @see http://msdn.microsoft.com/en-us/library/ms644902(VS.85).aspx
*
*/


void onTimer(HWND hwnd, UINT id) {

	if (id == ID_MYTIMER) {		// 予期しないタイマーからのイベントは無視する

		TPGC_get(&mPad);		// ゲームパッドからの入力を取得

		TPJT_get_sens(&mInpDt, sizeof(mInpDt));	// センサ入力を取得
		TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット

		if (mPad.Button[8] == 128){// スタートボタンで走行モード
			ctrl_flag = 0;
			//pwm_flag = 0;
		}
		if (mPad.Button[9] == 128){// セレクトボタンで制御モード
			ctrl_flag = 1;
			op_flag = 0;
		}
		// PWM設定モード
		if (ctrl_flag == 0){

			// 車輪 PWM設定
			if (mPad.POV[0] == 0){
				if (mOutDt.PWM[0] >= 900){
					mOutDt.PWM[0] = 900;
					mOutDt.PWM[1] = 900;
				}
				else {
					mOutDt.PWM[0] += 50;
					mOutDt.PWM[1] += 50;
				}
			}
			if (mPad.POV[0] == 18000){
				if (mOutDt.PWM[0] <= 0){
					mOutDt.PWM[0] = 0;
					mOutDt.PWM[1] = 0;
				}
				else{
					mOutDt.PWM[0] -= 50;
					mOutDt.PWM[1] -= 50;
				}
			}
			// 昇降機 PWM設定
			if (mPad.POV[0] == 9000){
				if (mOutDt.PWM2[2] >= 600){
					mOutDt.PWM2[2] = 600;
				}
				else mOutDt.PWM2[2] += 50;
			}
			if (mPad.POV[0] == 27000){
				if (mOutDt.PWM2[2] <= 0){
					mOutDt.PWM2[2] = 0;
				}
				else mOutDt.PWM2[2] -= 50;
			}

			// ベルコン PWM設定	
			if (mPad.Button[0] == 128){
				if (mOutDt.PWM[2] >= 850)mOutDt.PWM[2] = 850;
				else mOutDt.PWM[2] += 50;
			}
			if (mPad.Button[2] == 128){
				if (mOutDt.PWM[2] <= 0) mOutDt.PWM[2] = 0;
				else mOutDt.PWM[2] -= 50;
			}
			// アーム PWM設定
			if (mPad.Button[1] == 128){
				if (mOutDt.PWM[3] >= 400) mOutDt.PWM[3] = 400;
				else mOutDt.PWM[3] += 25;
				//if (mOutDt.PWM[3] >= 850) mOutDt.PWM[3] = 850;
					//else mOutDt.PWM[3] += 50;
				}
			if (mPad.Button[3] == 128){
				if (mOutDt.PWM[3] <= 0) mOutDt.PWM[3] = 0;
				else mOutDt.PWM[3] -= 5;
			}
		}
		// 制御モード
		else if (ctrl_flag == 1){
			if (mPad.POV[0] == -1){ // 操作無し
				flag = 0;
				mOutDt.d_out = 0;
			}

			if (mPad.Button[3] == 128){ // □　車輪・サーボ1・昇降機 操作
				op_flag = 0;
			}
			else if (mPad.Button[1] == 128){ // ○　ベルコン・アーム・サーボ2 操作
				op_flag = 1;
			}



			if (op_flag == 0){	// 車輪・サーボ1・昇降機 操作

				// 車輪操作
				if (mPad.POV[0] == 0){
					mOutDt.d_out = 1;	// 前進
					//mOutDt.d_out = 5;	// 1号機
					flag = 1;
				}
				else if (mPad.POV[0] == 18000){
					mOutDt.d_out = 8;	// 後退
					//mOutDt.d_out = 10;	// 1号機
					flag = 2;
				}
				else if (mPad.POV[0] == 27000){
					pwm = mOutDt.PWM[1];
					//mOutDt.PWM[1] = 0;	// 左回転
					mOutDt.d_out = 9;
					flag = 3;
					mOutDt.PWM[1] = pwm;
				}
				else if (mPad.POV[0] == 9000){
					pwm = mOutDt.PWM[0];
					//mOutDt.PWM[0] = 0;	// 右回転
					mOutDt.d_out = 4;
					//mOutDt.d_out = 6;     //1号機
					flag = 4;
					mOutDt.PWM[0] = pwm;
				}
				
				// サーボ1操作

				if (mPad.Button[6] == 128){
					if (mOutDt.PWM2[0] >= 850)mOutDt.PWM2[0] = 850;
					else mOutDt.PWM2[0] += 50;
				}
				else if (mPad.Button[4] == 128){
					if (mOutDt.PWM2[0] <= -850) mOutDt.PWM2[0] = -850;
					else mOutDt.PWM2[0] -= 50;
				}

				// 昇降機操作
				if (mPad.Button[5] == 128){
					mOutDt.d_out = 5;
				}
				else if (mPad.Button[7] == 128){
					mOutDt.d_out = 12;
				}
			}


			else if (ctrl_flag == 1 && op_flag == 1){	// ベルコン・アーム・サーボ2 操作
				pwm = mOutDt.PWM[0];
				pwm2 = mOutDt.PWM[1];
				mOutDt.PWM[0] = 0;
				mOutDt.PWM[1] = 0;

				// ベルコン操作
				if (mPad.POV[0] == 9000){
					mOutDt.d_out = 2;
				}
				else if (mPad.POV[0] == 27000){
					mOutDt.d_out = 13;
				}

				// アーム操作
				if (mPad.Button[5] == 128){	// R1
					pwm = mOutDt.PWM[0];
					pwm2 = mOutDt.PWM[1];
					mOutDt.d_out = 3;
					mOutDt.PWM[0] = pwm;
					mOutDt.PWM[1] = pwm2;
				}
				else if (mPad.Button[7] == 128){	// R2
					pwm = mOutDt.PWM[0];
					pwm2 = mOutDt.PWM[1];
					mOutDt.d_out = 10;
					mOutDt.PWM[0] = pwm;
					mOutDt.PWM[1] = pwm2;
				}

				// サーボ2操作
				if (mPad.Button[6] == 128){
					if (mOutDt.PWM2[1] >= 850) mOutDt.PWM2[1] = 850;
					else mOutDt.PWM2[1] += 50;
				}
				else if (mPad.Button[4] == 128){
					if (mOutDt.PWM2[1] <= -600) mOutDt.PWM2[1] = -600;
					else mOutDt.PWM2[1] -= 50;
				}



				mOutDt.PWM[0] = pwm;
				mOutDt.PWM[1] = pwm2;
			}

		}
	
		
		//画面描画更新の監視処理
		//タイマーイベント期間内に画面更新が無かった場合WM_PAINTメッセージを送信を行う。
		

		if (hwnd != NULL) {		// ウィンドウが生成されているか？
			if (updatePaint) {	// onPaint 更新フラグが未更新か？
				InvalidateRect(hwnd, NULL, FALSE);	// WM_PAINTメッセージを送信
			}
			updatePaint = 1;	//  onPaint 更新フラグをRESET
		}
	}
	return;
}



//1,3号機用
/*
void onTimer(HWND hwnd, UINT id) {
if (id == ID_MYTIMER) {		// 予期しないタイマーからのイベントは無視する

TPGC_get(&mPad);		// ゲームパッドからの入力を取得

TPJT_get_sens(&mInpDt, sizeof(mInpDt));	// センサ入力を取得

//@comment GUI上のコントローラ表示を変更
//switchButtons();
}

		if (mPad.Button[2] == 128){
			if (mOutDt.PWM[2] >= 850) mOutDt.PWM[2] = 850;	//上限に達したら最小値を設定
			else mOutDt.PWM[2] += 20;	// PWM[0]の値を10加算
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
		if (mPad.Button[1] == 128){
			if (mOutDt.PWM[2] <= -850) mOutDt.PWM[2] = -850;	//上限に達したら最小値を設定
			else mOutDt.PWM[2] -= 20;	// PWM[0]の値を10加算
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}

		if (mPad.Button[3] == 128){
			if (mOutDt.PWM[1] >= 850) mOutDt.PWM[1] = 850;	//上限に達したら最小値を設定
			else mOutDt.PWM[1] += 20;	// PWM[0]の値を10加算

			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
		if (mPad.Button[0] == 128){
			if (mOutDt.PWM[1] <= -850) mOutDt.PWM[1] = -850;	//下限に達したら最小値を設定
			else mOutDt.PWM[1] -= 20;	// PWM[0]の値を10加算

			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
			

			
		if (mPad.Button[7] == 128){
			mOutDt.PWM[0] += 40;	// PWM[0]の値を10加算
			if (mOutDt.PWM[0] >= 850) mOutDt.PWM[0] = 850;	//上限に達したら最小値を設定

			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
		if (mPad.Button[5] == 128){
			mOutDt.PWM[0] -= 40;	// PWM[0]の値を10加算
			if (mOutDt.PWM[0] <-850) mOutDt.PWM[0] = -850;	//上限に達したら最小値を設定

			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
			

		if (mPad.Button[6] == 128){
			mOutDt.PWM[3] += 50;	// PWM[0]の値を10加算
			if (mOutDt.PWM[3] > 850) mOutDt.PWM[3] = 850;	//上限に達したら最小値を設定

			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
		}
		if (mPad.Button[4] == 128){
			mOutDt.PWM[3] -= 50;	// PWM[0]の値を10加算
			if (mOutDt.PWM[3] < 0) mOutDt.PWM[3] = 0;	//上限に達したら最小値を設定


			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}*/

		/*
		if (mPad.Button[1] == 128){
			mOutDt.PWM[3] += 20;	// PWM[0]の値を10加算
			if (mOutDt.PWM[3] > PWM_MAX) mOutDt.PWM[3] = PWM_MIN;	//上限に達したら最小値を設定

			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
		if (mPad.Button[2] == 128){
			mOutDt.PWM[3] -= 20;	// PWM[0]の値を10加算
			if (mOutDt.PWM[3] < PWM_MIN) mOutDt.PWM[3] = PWM_MAX;	//上限に達したら最小値を設定

			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
			*/
/*
		if (mPad.POV[0] == 0){    //上を入力
			flag = 1;
			mOutDt.d_out = 5;	// d_outのD1とD3に1出力
			
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
		if (mPad.POV[0] == 18000){   //下を入力
			flag = 2;
			mOutDt.d_out = 10;	// d_outのD2とD4に1出力
			
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
		if (mPad.POV[0] == 27000){    //左を入力
			flag = 3;
			mOutDt.d_out = 9;	// d_out
		
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
		if (mPad.POV[0] == 9000){    //右を入力
			flag = 4;
			
			mOutDt.d_out = 6;	// d_outのD2とD3に1出力
			
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
		if (mPad.POV[0] == -1){
			flag = 0;
			mOutDt.d_out = 0;

			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			}
			//画面描画更新の監視処理
			//タイマーイベント期間内に画面更新が無かった場合WM_PAINTメッセージを送信を行う。
		*/

/*------axitデータテスト---------*/
/*
HDC hdc;
		hdc = TPGM_getDC();
		char msg1[40];
		if (mPad.axis[1]){
			//flag = 0;
			//mOutDt.d_out = 0;

			sprintf(msg1, "servo1 = %7d", mPad.axis[1]);
			TextOut(hdc, 100, 800, (LPCSTR)msg1, lstrlen((LPCSTR)msg1));
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// 制御出力データセット
			TPGM_releaseDC();			// デバイスコンテキストを解放
			TPGM_screen();				// スクリーンへ描画

			ValidateRect(hwnd, NULL);	// 画面更新を適用する
		*/
		/*-------------------------*/
		/*}
		if (hwnd != NULL) {		// ウィンドウが生成されているか？
			if (updatePaint) {	// onPaint 更新フラグが未更新か？
			InvalidateRect(hwnd, NULL, FALSE);	// WM_PAINTメッセージを送信
				}
			updatePaint = 1;	//  onPaint 更新フラグをRESET
			}	
			return;
}
*/
		




/**
* WM_KEYDOWNのメッセージクラッカー\n
* システムキー以外のキーが押された際にコールされる関数\n
* システムキーとはAltキーが押されていない状態のキーの事を言う
*
* @param[in]  hwnd    イベントが発生したウィンドウのハンドル
* @param[in]  vk      仮想キーコード
* @param[in]  fDown   常にTRUE
* @param[in]  cRepeat メッセージのリピート回数
* @param[in]  flags   lParamのHIWORDに定義されているフラグ
*
* @see http://msdn.microsoft.com/en-us/library/ms646280(VS.85).aspx
*
*/
void onKeydown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) {

	switch (vk) {
	case VK_ESCAPE: // ESCキー
	case VK_F12:    // F12
		onClose(hwnd);	// ウインドウを閉じる処理
		return;
	case VK_F1:
		TPJT_chg_camera(0);
		break;
	case VK_F2:
		TPJT_chg_camera(1);
		break;

	default: // 通常
		return;
	}

	return;
}



/**
* WM_KEYUPのメッセージクラッカー\n
* システムキー以外のキーが離された際にコールされる関数\n
* システムキーとはAltキーが押されていない状態のキーの事を言う
*
* @param[in]  hwnd    イベントが発生したウィンドウのハンドル
* @param[in]  vk      仮想キーコード
* @param[in]  fDown   常にFALSE
* @param[in]  cRepeat メッセージのリピート回数
* @param[in]  flags   lParamのHIWORDに定義されているフラグ
*
* @see http://msdn.microsoft.com/en-us/library/ms646281(VS.85).aspx
*
*/
void onKeyup(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) {
	return;
}



/**
* WM_PAINTのメッセージクラッカー\n
* システム、または他のアプリケーションから画面の更新要求があった際にコールされる関数\n
* Jpegデータの取出し、画面描画を行う
*
* @param[in]  hwnd  イベントが発生したウィンドウのハンドル
*
* @see http://msdn.microsoft.com/en-us/library/dd145213(VS.85).aspx
*
*/
/*
void onPaint(HWND hwnd) {

	RECT src = { 0, 0, 640, 480 };
	RECT dst = { 200, 40, 1700, 1200 };

	HDC   hdc;       // デバイスコンテキスト


	//HDC hdc2 = CreateCompatibleDC(hdc);

	//GetObject(hbmp, sizeof(bmp), &bmp);
	//SelectObject(hdc2, hbmp);
	//BitBlt(hdc2, posx, posy, bmp.bmWidth,bmp.bmHeight, hdc2,0,0,SRCCOPY);

	//RECT  dst, src = {0, 0, 640, 480};
	void* mJpegData;  // JPEGデータ格納変数
	int   mJpegSize;  // JPEGデータサイズ
	int  img_sz;	  //画像取り込みサイズ
	IplImage *tpipImage, *resizeImage;  //入力画像バッファOpenCV
	updatePaint = 1;			// onPaint 更新フラグを更新済に
	//cvMat2HDC cvmat(tpipImage,hdc);
	cv::Mat tpipImg, resizeImg;

	//Jpegデータの取り出し
	mJpegData = TPJT_get_jpeg_file(0, 0, &mJpegSize);	// JPEGデータの取出し

	tpipImage = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);



	//resizeImage = cvCreateImage(cvSize((int)(tpipImage -> width * SCALE_X),(int)(tpipImage->height * SCALE_Y)),IPL_DEPTH_8U,3);
	if ((mJpegData) && (mJpegSize > 0)) {	// JPEGデータが有りの場合


		TPGM_decode(mJpegData, mJpegSize);	// JPEGデータをカメラ画像にデコードする
	}
	TPJT_free_jpeg_file();					// JPEGデータの解放
	//画像データの解放
	cvReleaseImage(&tpipImage);				//Opencv画像データの解放
	//cvReleaseImage(&resizeImage);			//データを使っていないときに使用すると
	//例外扱いされる

	//TPGM_copy(0, 0);			// ウィンドウ座標(0,0)位置に固定サイズ(480x640)でカメラ映像を描画する
	TPGM_copy2(&src, &dst);	// サイズを変更してカメラ映像を描画する時に使用する。

	hdc = TPGM_getDC();			// デバイスコンテキストを取得する
	//TPJT_set_video_inf(QVGA);
	//TPJT_init("192.168.2.101", hwnd);
	//TPJT_set_com_req(0x03, 0);

	static int img_sz2 = 0; //画像取り込みサイズ
	IplImage *tpipImage2;
	HDC   hdc2;       // デバイスコンテキスト
	//RECT  dst, src = {0, 0, 640, 480};
	void* mJpegData2;  // JPEGデータ格納変数
	int   mJpegSize2;  // JPEGデータサイズ
	int connectId2 = TPJT_get_com_mode();
	static char send_buf[1024];
	static char recv_buf[1024];
	w32udp* com = NULL;
	int err;
	char m[40], m2[40];
	com = new w32udp("UDP_S");

	err = com->open("127.0.0.1", 9000,-1);
	//com->re
	//err = com->open("192.168.2.103", 9000, 2 * 100);

	//char* ip = "192.168.2.103";

	sprintf(m,"send_to = %d",err);
	TextOut(hdc,800,900,(LPCSTR)m,lstrlen((LPCSTR)m));
	
	int snd_cnt, rcv_cnt;

	//snd_cnt = com->send(send_buf, sizeof(send_buf));

	rcv_cnt = com->recv(recv_buf, sizeof(recv_buf));
	

	sprintf(m2, "rcv = %d", recv_buf);
	TextOut(hdc, 700, 900, (LPCSTR)m2, lstrlen((LPCSTR)m2));
	com->close();
	delete com;




char msg[40],msg1[40],msg2[40],msg3[40],msg4[40],msg5[40];	// 文字列格納変数定義
LPTSTR lpt = NULL;
SetBkColor(hdc, RGB(0, 0, 0));				// 文字背景色指定
SetTextColor(hdc, RGB(255, 255, 255));		// 文字色指定（白）

if (flag == 1){
lpt = TEXT("前進");
TextOut(hdc, 100, 600, lpt, lstrlen(lpt));	// 文字表示
}
if (flag == 2){
lpt = TEXT("後退");
TextOut(hdc, 100, 600, lpt, lstrlen(lpt));	// 文字表示
}

if (flag == 3){
lpt = TEXT("左進");
TextOut(hdc, 100, 600, lpt, lstrlen(lpt));	// 文字表示
}
if (flag == 4){
lpt = TEXT("右進");
TextOut(hdc, 100, 600, lpt, lstrlen(lpt));	// 文字表示
}
if (flag == 0){
lpt = TEXT("停止");
TextOut(hdc, 100, 600, lpt, lstrlen(lpt));	// 文字表示
}


sprintf(msg1,"servo1 = %7d",mOutDt.PWM[0]);
TextOut(hdc, 100, 500, (LPCSTR)msg1, lstrlen((LPCSTR)msg1));
sprintf(msg2,"servo2 = %7d", mOutDt.PWM[1]);
TextOut(hdc, 350, 500, (LPCSTR)msg2, lstrlen((LPCSTR)msg2));
sprintf(msg3,"servo3 = %7d", mOutDt.PWM[2]);
TextOut(hdc, 600, 500, msg3, lstrlen(msg3));
sprintf(msg4, "motor PWM = %7d", mOutDt.PWM[3]);
TextOut(hdc, 350, 450, msg4, lstrlen(msg4));



TPGM_releaseDC();			// デバイスコンテキストを解放
TPGM_screen();				// スクリーンへ描画

ValidateRect(hwnd, NULL);	// 画面更新を適用する

return;
}
*/


void onPaint(HWND hwnd) {

	HDC   hdc;       // デバイスコンテキスト
	//RECT  dst, src = {0, 0, 640, 480};
	void* mJpegData;  // JPEGデータ格納変数
	int   mJpegSize;  // JPEGデータサイズ

	updatePaint = 1;			// onPaint 更新フラグを更新済に

	mJpegData = TPJT_get_jpeg_file(0, 0, &mJpegSize);	// JPEGデータの取出し

	if ((mJpegData) && (mJpegSize > 0)) {	// JPEGデータが有りの場合
		TPGM_decode(mJpegData, mJpegSize);	// JPEGデータをカメラ画像にデコードする
	}
	TPJT_free_jpeg_file();					// JPEGデータの解放


	TPGM_copy(0, 0);			// ウィンドウ座標(0,0)位置に固定サイズ(480x640)でカメラ映像を描画する 
	//TPGM_copy2(&src, &dst);	// サイズを変更してカメラ映像を描画する時に使用する。

	hdc = TPGM_getDC();			// デバイスコンテキストを取得する

	char msg[256];								// 文字列格納変数定義
	SetBkColor(hdc, RGB(0, 0, 0));				// 文字背景色指定
	SetTextColor(hdc, RGB(255, 255, 255));		// 文字色指定（白）
	if (ctrl_flag == 0){
		sprintf(msg, "PWM設定モード");
	}
	else{
		sprintf(msg, "制御モード");
	}
	TextOut(hdc, 100, 500, msg, lstrlen(msg));
	if (drive_mode == 0){
		sprintf(msg, "前進モード");
	}
	else{
		sprintf(msg, "後進モード");
	}
	TextOut(hdc, 200, 500, msg, lstrlen(msg));
	sprintf(msg, "左モーターPWM: %5d, 右モーターPWM:%5d", (mOutDt.PWM[0]), (mOutDt.PWM[1]));
	TextOut(hdc, 100, 520, msg, lstrlen(msg));


	sprintf(msg, "ベルコンPWM: %5d, アームPWM:%5d", (mOutDt.PWM[2]), (mOutDt.PWM[3]));
	TextOut(hdc, 100, 600, msg, lstrlen(msg));

	sprintf(msg, "サーボ１PWM: %5d, サーボ２PWM:%5d", (mOutDt.PWM2[0]), (mOutDt.PWM2[1]));
	TextOut(hdc, 100, 680, msg, lstrlen(msg));

	sprintf(msg, "昇降機PWM: %5d", (mOutDt.PWM2[2]));
	TextOut(hdc, 100, 740, msg, lstrlen(msg));

	sprintf(msg, "op_flag: %5d", (op_flag));
	TextOut(hdc, 100, 760, msg, lstrlen(msg));


	if (flag == 0){
		sprintf(msg, "停止 ");
		TextOut(hdc, 100, 540, msg, lstrlen(msg));	// 文字表示
	}
	if (flag == 1){
		sprintf(msg, "前進 ");
		TextOut(hdc, 100, 540, msg, lstrlen(msg));	// 文字表示
	}
	if (flag == 2){
		sprintf(msg, "後進 ");
		TextOut(hdc, 100, 540, msg, lstrlen(msg));	// 文字表示
	}
	if (flag == 3){
		sprintf(msg, "左進 ");
		TextOut(hdc, 100, 540, msg, lstrlen(msg));	// 文字表示
	}
	if (flag == 4){
		sprintf(msg, "右進 ");
		TextOut(hdc, 100, 540, msg, lstrlen(msg));	// 文字表示
	}

	sprintf(msg, "START = PWM設定モード");
	TextOut(hdc, 700, 20, msg, lstrlen(msg));
	sprintf(msg, "↑ = 車輪 PWM 上昇");
	TextOut(hdc, 700, 40, msg, lstrlen(msg));
	sprintf(msg, "↓ = 車輪 PWM 下降");
	TextOut(hdc, 700, 60, msg, lstrlen(msg));
	sprintf(msg, "→ = 昇降機 PWM 上昇");
	TextOut(hdc, 700, 90, msg, lstrlen(msg));
	sprintf(msg, "← = 昇降機 PWM 下降");
	TextOut(hdc, 700, 110, msg, lstrlen(msg));
	sprintf(msg, "△ = ベルコン PWM 上昇");
	TextOut(hdc, 700, 140, msg, lstrlen(msg));
	sprintf(msg, "× = ベルコン PWM 下降");
	TextOut(hdc, 700, 160, msg, lstrlen(msg));
	sprintf(msg, "○ = アーム PWM 上昇");
	TextOut(hdc, 700, 190, msg, lstrlen(msg));
	sprintf(msg, "□ = アーム PWM 下降");
	TextOut(hdc, 700, 210, msg, lstrlen(msg));

	sprintf(msg, "SELECT = 制御モード");
	TextOut(hdc, 700, 250, msg, lstrlen(msg));

	sprintf(msg, "□ = 車輪・サーボ１・昇降機　制御");
	TextOut(hdc, 700, 270, msg, lstrlen(msg));
	sprintf(msg, "   ↑ = 前進");
	TextOut(hdc, 700, 290, msg, lstrlen(msg));
	sprintf(msg, "   ↓ = 後退");
	TextOut(hdc, 700, 310, msg, lstrlen(msg));
	sprintf(msg, "   → = 右旋回");
	TextOut(hdc, 700, 330, msg, lstrlen(msg));
	sprintf(msg, "   ← = 左旋回");
	TextOut(hdc, 700, 350, msg, lstrlen(msg));
	sprintf(msg, "   L1 = アーム第一関節 下降");
	TextOut(hdc, 700, 380, msg, lstrlen(msg));
	sprintf(msg, "   L2 = アーム第一関節 上昇");
	TextOut(hdc, 700, 400, msg, lstrlen(msg));
	sprintf(msg, "   R1 = 昇降機 上昇");
	TextOut(hdc, 700, 430, msg, lstrlen(msg));
	sprintf(msg, "   R2 = 昇降機 下降");
	TextOut(hdc, 700, 450, msg, lstrlen(msg));
	sprintf(msg, "○ = ベルコン・サーボ２・アーム　制御");
	TextOut(hdc, 700, 480, msg, lstrlen(msg));
	sprintf(msg, "   → = ベルコン 押し出し");
	TextOut(hdc, 700, 500, msg, lstrlen(msg));
	sprintf(msg, "   ← = ベルコン 引き込み");
	TextOut(hdc, 700, 520, msg, lstrlen(msg));
	sprintf(msg, "   L1 = アーム第二関節 上昇");
	TextOut(hdc, 700, 550, msg, lstrlen(msg));
	sprintf(msg, "   L2 = アーム第二関節 下降");
	TextOut(hdc, 700, 570, msg, lstrlen(msg));
	sprintf(msg, "   R1 = アーム 前");
	TextOut(hdc, 700, 600, msg, lstrlen(msg));
	sprintf(msg, "   R2 = アーム 後");
	TextOut(hdc, 700, 620, msg, lstrlen(msg));

	TPGM_releaseDC();			// デバイスコンテキストを解放
	TPGM_screen();				// スクリーンへ描画
	ValidateRect(hwnd, NULL);	// 画面更新を適用する
	return;
}


/**
* WM_LBUTTONDOWNのメッセージクラッカー\n
* ウィンドウ内でマウスの左ボタンを押し込んだ際にコールされる関数
*
* @param[in]  hwnd          イベントが発生したウィンドウのハンドル
* @param[in]  fDoubleClick  常にFALSE
* @param[in]  x             イベントが発生した際のマウスのX座標
* @param[in]  y             イベントが発生した際のマウスのY座標
* @param[in]  keyFlags      押されている仮想キーのフラグ
*
* @see http://msdn.microsoft.com/en-us/library/ms645607(VS.85).aspx
*
*/
void onLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) {
	return;
}



/**
* WM_LBUTTONUPのメッセージクラッカー\n
* ウィンドウ内でマウスの左ボタンを離した際にコールされる関数
*
* @param[in]  hwnd      イベントが発生したウィンドウのハンドル
* @param[in]  x         イベントが発生した際のマウスのX座標
* @param[in]  y         イベントが発生した際のマウスのY座標
* @param[in]  keyFlags  押されている仮想キーのフラグ
*
* @see http://msdn.microsoft.com/en-us/library/ms645608(VS.85).aspx
*
*/
void onLButtonUp(HWND hwnd, int x, int y, UINT keyFlags) {
	return;
}



/**
* WM_LBUTTONDBLCLKのメッセージクラッカー\n
* ウィンドウ内でマウスの左ボタンをダブルクリックした際にコールされる関数
*
* @param[in]  hwnd          イベントが発生したウィンドウのハンドル
* @param[in]  fDoubleClick  常にTRUE
* @param[in]  x             イベントが発生した際のマウスのX座標
* @param[in]  y             イベントが発生した際のマウスのY座標
* @param[in]  keyFlags      押されている仮想キーのフラグ
*
* @see http://msdn.microsoft.com/en-us/library/ms645606(VS.85).aspx
*
*/
void onLButtonDblClk(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) {
	return;
}



/**
* WM_LBUTTONDOWNのメッセージクラッカー\n
* ウィンドウ内でマウスの右ボタンを押し込んだ際にコールされる関数
*
* @param[in]  hwnd          イベントが発生したウィンドウのハンドル
* @param[in]  fDoubleClick  常にFALSE
* @param[in]  x             イベントが発生した際のマウスのX座標
* @param[in]  y             イベントが発生した際のマウスのY座標
* @param[in]  keyFlags      押されている仮想キーのフラグ
*
* @see http://msdn.microsoft.com/en-us/library/ms646242(VS.85).aspx
*
*/
void onRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) {
	return;
}



/**
* WM_RBUTTONUPのメッセージクラッカー\n
* ウィンドウ内でマウスの右ボタンを離した際にコールされる関数
*
* @param[in]  hwnd      イベントが発生したウィンドウのハンドル
* @param[in]  x         イベントが発生した際のマウスのX座標
* @param[in]  y         イベントが発生した際のマウスのY座標
* @param[in]  keyFlags  押されている仮想キーのフラグ
*
* @see http://msdn.microsoft.com/en-us/library/ms646243(VS.85).aspx
*
*/
void onRButtonUp(HWND hwnd, int x, int y, UINT keyFlags) {
	return;
}



/**
* WM_RBUTTONDOWNのメッセージクラッカー\n
* ウィンドウ内でマウスの右ボタンをダブルクリックした際にコールされる関数
*
* @param[in]  hwnd          イベントが発生したウィンドウのハンドル
* @param[in]  fDoubleClick  常にTRUE
* @param[in]  x             イベントが発生した際のマウスのX座標
* @param[in]  y             イベントが発生した際のマウスのY座標
* @param[in]  keyFlags      押されている仮想キーのフラグ
*
* @see http://msdn.microsoft.com/en-us/library/ms646241(VS.85).aspx
*
*/
void onRButtonDblClk(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) {
	return;
}



/**
* WM_MOUSEMOVEのメッセージクラッカー\n
* ウィンドウ内でマウスが動いた際にコールされる関数
*
* @param[in]  hwnd      イベントが発生したウィンドウのハンドル
* @param[in]  x         イベントが発生した際のマウスのX座標
* @param[in]  y         イベントが発生した際のマウスのY座標
* @param[in]  keyFlags  押されている仮想キーのフラグ
*
* @see http://msdn.microsoft.com/en-us/library/ms645616(VS.85).aspx
*
*/
void onMouseMove(HWND hwnd, int x, int y, UINT keyFlags) {
	return;
}

