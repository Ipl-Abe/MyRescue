/**
* @file EventListener.cpp
* @brief �C�x���g�̏������L�q����
* @author Iwata Naoki
* @date 2011-03-19
* @version 1.00
* @version 1.01 2013/03/21 katayama
*
* Copyright (C) 2010 - TPIP User Community All rights reserved.
* ���̃t�@�C���̒��쌠�́ATPIP���[�U�[�R�~���j�e�B�̋K��ɏ]���g�p���������܂��B
*/

#define __CV_H__
/*-------- include --------*/
#include "TPIP3.h"           // TPIP�֌W
#include "EventListener.h"   // �E�B���h�E�̃C�x���g�����֌W
#include "cvMat2HDC.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <fstream>
#include <sys/types.h>
//#include <winsock2.h>



//#include<winsock2.h>
//#include<ws2tcpip.h>


#include <stdio.h>
#pragma  warning(disable:4996)	//	�Z�L�����e�B���ア�Â��֐��ɑ΂���x���̉���
//#pragma comment( lib, "wsock32.lib" )
//#pragma comment( lib, "ws2_32.lib")

#define WIDTH 640
#define HEIGHT 480

/*-------- �����o�ϐ���` --------*/
static struct GC_STRUCT  mPad;		//!< @brief �Q�[���p�b�h���̓f�[�^
static struct OUT_DT_STR mOutDt;	//!< @brief TPJT�ň���������
static struct INP_DT_STR mInpDt;	//!< @brief TPJT�ň����Z���T�[���
static int    updatePaint;			//!< @brief onPaint �X�V�t���O(0:�X�V�ρ@1:���X�V)
int flag, flag2;
DWORD nBegin = ::GetTickCount();  //@comment �v���O�����N�����̃V�X�e���̎��Ԃ�ێ�
DWORD start,end;					//@comment �V�X�e���N�����ԕێ��p�ϐ�
//@comment �Q�̃J�����̐؂�ւ��t���O
bool cameraFlg = true;
//@comment �R���g���[���[GUI�摜��\������
cvMat2HDC ctrl[12];
//@comment ���A���A���A�~�̈ʒu
const int circlePos[4][2] = { { 1055, 480 }, { 990, 550 }, { 1115, 550 }, { 1055, 620 } };
//@comment�@�����L�[�̈ʒu
const int rec1Pos[2][2] = { { 740, 480 }, { 740, 620 } };
const int rec2Pos[2][2] = { { 670, 550 }, { 800, 550 } };
//@comemnt l1, l2, r1, r2�̈ʒu
const int lrPos[4][2] = { { 700, 300 }, { 700, 370 }, { 1020, 300 }, { 1020, 370 } };
//@comment �R���g���[���[��������Ă��邩
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


/*-------- �֐���` --------*/
//@comment �R���g���[���[GUI�̉摜�̈ʒu�̏�����
void initCtrlGUI()
{
	//@comment ���{�^��
	for (int i = 0; i < 4; i++){
		ctrl[i].setRendPos(circlePos[i][0], circlePos[i][1]);
	}
	//@comment lr�{�^��
	for (int i = 0; i < 4; i++){
		ctrl[i + 4].setRendPos(lrPos[i][0], lrPos[i][1]);
	}
	//@comment rect1�{�^��
	for (int i = 0; i < 2; i++){
		ctrl[i + 8].setRendPos(rec1Pos[i][0], rec1Pos[i][1]);
	}
	//@comment lr�{�^��
	for (int i = 0; i < 2; i++){
		ctrl[i + 10].setRendPos(rec2Pos[i][0], rec2Pos[i][1]);
	}
}

//@comment �R���g���[���[GUI�ւ̉�����Ă��邩�ǂ����̃t���O�ύX
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

	//������
	if (mPad.POV[0] == 0){ ctrlFlg[UP] = true; }
	else{ ctrlFlg[UP] = false; }

	//�������
	if (mPad.POV[0] == 18000){ ctrlFlg[DOWN] = true; }
	else{ ctrlFlg[DOWN] = false; }

	//�������
	if (mPad.POV[0] == 27000){ ctrlFlg[RGHT] = true; }
	else{ ctrlFlg[RGHT] = false; }

	//�E�����
	if (mPad.POV[0] == 9000){ ctrlFlg[LEFT] = true; }
	else{ ctrlFlg[LEFT] = false; }
}

//@comment �R���g���[���[GUI�̕`��
void rendController()
{
	for (int i = 0; i < 12; i++){
		ctrl[i].setHDC();
	}
	//@comment ���{�^��
	for (int i = 0; i < 4; i++){
		if (ctrlFlg[i]){ ctrl[i].load("./img/circle_on"); }
		else{ ctrl[i].load("./img/circle"); }
	}
	//@comment lr�{�^��
	for (int i = 0; i < 4; i++){
		if (ctrlFlg[i + 4]){ ctrl[i + 4].load("./img/lr_on"); }
		else{ ctrl[i + 4].load("img/lr"); }
	}
	//@comment rect1�{�^��
	for (int i = 0; i < 2; i++){
		if (ctrlFlg[i + 8]){ ctrl[i + 8].load("./img/rect_1_on"); }
		else{ ctrl[i + 8].load("./img/rect_1"); }
	}
	//@comment lr�{�^��
	for (int i = 0; i < 2; i++){
		if (ctrlFlg[i + 10]){ ctrl[i + 10].load("./img/rect_2_on"); }
		else{ ctrl[i + 10].load("./img/rect_2"); }
	}

	for (int i = 0; i < 12; i++){
		ctrl[i].releaseHDC();
	}
}

/**
* WM_CREATE�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E�����������O�ɃR�[�������֐�\n
* �����Ő��������^�C�}�[�𗘗p���ATPIP�Ƃ̒ʐM�A��ʂ̕`��̃C�x���g�𔭐����܂�
*
* @param[in]  hwnd            �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  lpCreateStruct  �E�B���h�E�쐬���̃p�����[�^
*
* @retval ���TRUE
*
* @see http://msdn.microsoft.com/en-us/library/ms632619(VS.85).aspx
*
*/
BOOL onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
	mOutDt.PWM[3] = 250;
	//mOutDt.PWM[0] = 0;
	//mOutDt.PWM[1] = 700;
	//mOutDt.PWM[2] = 920;
	TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
	SetTimer(hwnd, ID_MYTIMER, 60, NULL);	// �^�C�}�[�𐶐�����
	initCtrlGUI();
	return TRUE; //������FALSE(0)�ɂ���ƃ��b�Z�[�W��Destroy���Ă΂��
}


/**
* WM_ACTIVATE�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E���A�N�e�B�u/��A�N�e�B�u�ɂȂ�ۂɃR�[�������֐�
*
* @param[in] hwnd          �C�x���g�����������E�B���h�E�̃n���h��
* @param[in] state         �E�B���h�E���A�N�e�B�u/��A�N�e�B�u���̏��
* @param[in] hwndActDeact  �E�B���h�E�̃n���h��\n
*                            state == 0�̏ꍇ�́A�V�����A�N�e�B�u�ɂȂ����E�B���h�E�̃n���h��\n
*                            state != 0�̏ꍇ�́A����܂ŃA�N�e�B�u�ł������E�B���h�E�̃n���h��
* @param[in] fMinimized    �E�B���h�E���ŏ�������Ă��邩�̔���t���O\n
*                            0�ȊO�̒l�̏ꍇ�̓E�B���h�E�͍ŏ�������Ă��܂�\n
*                            0�̏ꍇ�̓E�B���h�E�͍ŏ�������Ă��܂���\n
*
* @see http://msdn.microsoft.com/ja-jp/library/ms646274(d=lightweight).aspx
*
*/
void onActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized) {
	return;
}



/**
* WM_CLOSE�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E���A�v���P�[�V�������I������ۂɃR�[�������֐�\n
* �I���̊m�F���s���AYES�Ȃ�΃E�B���h�E��j������BNO�Ȃ�Δj�����Ȃ�
*
* @param[in] hwnd  �C�x���g�����������E�B���h�E�̃n���h��
*
* @see http://msdn.microsoft.com/en-us/library/ms632617(VS.85).aspx
*
*/
void onClose(HWND hwnd) {

	int ans = 0; // �߂�l�擾�p

	// ���b�Z�[�W�_�C�A���O��\�����A�I���̊m�F������
	ans = MessageBox(hwnd, TEXT("�I�����܂����H"), TEXT("�I���m�F"), MB_YESNO | MB_ICONEXCLAMATION);

	// YES���I�����ꂽ�ꍇ�̓E�B���h�E�����
	if (ans == IDYES) {
		DestroyWindow(hwnd); // �E�C���h�E�����֐�
	}

	return;
}



/**
* WM_DESTROY�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E���j�������ۂɃR�[�������֐�\n
* TPIP���C�u�����Ɛ��������^�C�}�[�̉�����s��
*
* @param[in]  hwnd �C�x���g�����������E�B���h�E�̃n���h��
*
* @see http://msdn.microsoft.com/en-us/library/ms632620(VS.85).aspx
*
*/
void onDestroy(HWND hwnd) {

	// TPIP���C�u�����Q�̉��
	TPGM_delete();
	TPGC_close();
	TPJT_close();

	//�^�C�}�[���
	KillTimer(hwnd, ID_MYTIMER);
	PostQuitMessage(0);

	return;
}



/**
* WM_SIZE�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E�̃T�C�Y���ύX���ꂽ��ɃR�[�������֐�\n
* TPGM�Ŏg�p���Ă���T�[�t�F�C�X��ύX��̃T�C�Y�ō\��������
*
* @param[in]  hwnd  �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  state �T�C�Y�ύX�̃^�C�v
* @param[in]  cx    �E�B���h�E�̉���
* @param[in]  cy    �E�B���h�E�̏c��
*
* @see http://msdn.microsoft.com/en-us/library/ms632646(v=vs.85).aspx
*
*/
void onSize(HWND hwnd, UINT state, int cx, int cy) {

	// �T�[�t�F�C�X�̍č\��
	TPGM_delete();
	TPGM_create(hwnd, QVGA, NULL);

	return;
}



/**
* WM_TIMER�̃��b�Z�[�W�N���b�J�[\n
* �^�C�}�[���I�������ۂɃR�[�������֐�\n
* �Q�[���p�b�h����̓��́ATPIP�Ƃ̒ʐM�A��ʂ̕`5��̃C�x���g�𔭐�����\n
* ����f�[�^�̉��H����TPJT_set_ctrl()�����s����܂łɍs������
*
* @param[in]  hwnd �C�x���g�����������E�B���h�E�̃n���h��1
* @param[in]  id   �C�x���g���N�������^�C�}�[ID
*
* @see http://msdn.microsoft.com/en-us/library/ms644902(VS.85).aspx
*
*/
void onTimer(HWND hwnd, UINT id) {
	if (id == ID_MYTIMER) {		// �\�����Ȃ��^�C�}�[����̃C�x���g�͖�������

		TPGC_get(&mPad);		// �Q�[���p�b�h����̓��͂��擾

		TPJT_get_sens(&mInpDt, sizeof(mInpDt));	// �Z���T���͂��擾

		//@comment GUI��̃R���g���[���\����ύX
		//switchButtons();

		// R�P�{�^������
		if (mPad.Button[5] == 128){
			flag2 += 1;
			Sleep(300);
			if (flag2 == 3){
				flag2 = 0;
			}
		}
		//@comment L�P�{�^���ŃJ�����؂�ւ�
		/*
		if (mPad.Button[6] == 128){
			if (cameraFlg){
				TPJT_chg_camera(0);
				cameraFlg = false;
			}
			else{
				TPJT_chg_camera(1);
				cameraFlg = true;
			}

		}
		*/
		if (flag2 == 0){
			if (mPad.Button[1] == 128){
				if (mOutDt.PWM[0] >= 900) mOutDt.PWM[0] = 900;	//����ɒB������ŏ��l��ݒ�
				else mOutDt.PWM[0] += 20;	// PWM[0]�̒l��10���Z
				TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
			}
			if (mPad.Button[2] == 128){
				if (mOutDt.PWM[0] <= -900) mOutDt.PWM[0] = -900;	//����ɒB������ŏ��l��ݒ�
				else mOutDt.PWM[0] -= 20;	// PWM[0]�̒l��10���Z
				TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
			}
		}
		if (flag2 == 1){
			if (mPad.Button[1] == 128){
				if (mOutDt.PWM[1] >= 900) mOutDt.PWM[1] = 900;	//����ɒB������ŏ��l��ݒ�
				else mOutDt.PWM[1] += 20;	// PWM[0]�̒l��10���Z
			
				TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
			}
			if (mPad.Button[2] == 128){
				if (mOutDt.PWM[1] <= -900) mOutDt.PWM[1] = -900;	//�����ɒB������ŏ��l��ݒ�
				else mOutDt.PWM[1] -= 20;	// PWM[0]�̒l��10���Z

				TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
			}
		}

		if (flag2 == 2){
			if (mPad.Button[1] == 128){
				mOutDt.PWM[2] += 20;	// PWM[0]�̒l��10���Z
				if (mOutDt.PWM[2] > PWM_MAX) mOutDt.PWM[2] = PWM_MIN;	//����ɒB������ŏ��l��ݒ�

				TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
			}
			if (mPad.Button[2] == 128){
				mOutDt.PWM[2] -= 20;	// PWM[0]�̒l��10���Z
				if (mOutDt.PWM[2] < PWM_MIN) mOutDt.PWM[2] = PWM_MAX;	//����ɒB������ŏ��l��ݒ�

				TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
			}
		}
		
			if (mPad.Button[4] == 128){
				mOutDt.PWM[3] += 20;	// PWM[0]�̒l��10���Z
				if (mOutDt.PWM[3] > PWM_MAX) mOutDt.PWM[3] = PWM_MIN;	//����ɒB������ŏ��l��ݒ�

				TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
			}
			if (mPad.Button[6] == 128){
				mOutDt.PWM[3] -= 20;	// PWM[0]�̒l��10���Z
				if (mOutDt.PWM[3] < PWM_MIN) mOutDt.PWM[3] = PWM_MAX;	//����ɒB������ŏ��l��ݒ�

				TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
			}
		
		/*
		if (flag2 == 3){
			if (mPad.Button[1] == 128){
				mOutDt.PWM[3] += 20;	// PWM[0]�̒l��10���Z
				if (mOutDt.PWM[3] > PWM_MAX) mOutDt.PWM[3] = PWM_MIN;	//����ɒB������ŏ��l��ݒ�

				TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
			}
			if (mPad.Button[2] == 128){
				mOutDt.PWM[3] -= 20;	// PWM[0]�̒l��10���Z
				if (mOutDt.PWM[3] < PWM_MIN) mOutDt.PWM[3] = PWM_MAX;	//����ɒB������ŏ��l��ݒ�

				TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
			}
		}
		*/
		if (mPad.POV[0] == 0){    //������
			flag = 1;
			mOutDt.d_out = 5;	// d_out��D1��D3��1�o��
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
		}
		if (mPad.POV[0] == 18000){   //�������
			flag = 2;
			mOutDt.d_out = 10;	// d_out��D2��D4��1�o��
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
		}
		if (mPad.POV[0] == 27000){    //�������
			flag = 3;
			mOutDt.d_out = 9;	// d_out��D1��D4��1�o��
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
		}
		if (mPad.POV[0] == 9000){    //�E�����
			flag = 4;
			mOutDt.d_out = 6;	// d_out��D2��D3��1�o��
			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
		}
		if (mPad.POV[0] == -1){
			flag = 0;
			mOutDt.d_out = 0;

			TPJT_set_ctrl(&mOutDt, sizeof(mOutDt));	// ����o�̓f�[�^�Z�b�g
		}
		/*
		��ʕ`��X�V�̊Ď�����
		�^�C�}�[�C�x���g���ԓ��ɉ�ʍX�V�����������ꍇWM_PAINT���b�Z�[�W�𑗐M���s���B
		*/
		if (hwnd != NULL) {		// �E�B���h�E����������Ă��邩�H
			if (updatePaint) {	// onPaint �X�V�t���O�����X�V���H
				InvalidateRect(hwnd, NULL, FALSE);	// WM_PAINT���b�Z�[�W�𑗐M
			}
			updatePaint = 1;	//  onPaint �X�V�t���O��RESET
		}
	}



	return;
}





/**
* WM_KEYDOWN�̃��b�Z�[�W�N���b�J�[\n
* �V�X�e���L�[�ȊO�̃L�[�������ꂽ�ۂɃR�[�������֐�\n
* �V�X�e���L�[�Ƃ�Alt�L�[��������Ă��Ȃ���Ԃ̃L�[�̎�������
*
* @param[in]  hwnd    �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  vk      ���z�L�[�R�[�h
* @param[in]  fDown   ���TRUE
* @param[in]  cRepeat ���b�Z�[�W�̃��s�[�g��
* @param[in]  flags   lParam��HIWORD�ɒ�`����Ă���t���O
*
* @see http://msdn.microsoft.com/en-us/library/ms646280(VS.85).aspx
*
*/
void onKeydown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) {

	switch (vk) {
	case VK_ESCAPE: // ESC�L�[
	case VK_F12:    // F12
		onClose(hwnd);	// �E�C���h�E����鏈��
		return;
	case VK_F1:
		TPJT_chg_camera(0);
		break;
	case VK_F2:
		TPJT_chg_camera(1);
		break;

	default: // �ʏ�
		return;
	}

	return;
}



/**
* WM_KEYUP�̃��b�Z�[�W�N���b�J�[\n
* �V�X�e���L�[�ȊO�̃L�[�������ꂽ�ۂɃR�[�������֐�\n
* �V�X�e���L�[�Ƃ�Alt�L�[��������Ă��Ȃ���Ԃ̃L�[�̎�������
*
* @param[in]  hwnd    �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  vk      ���z�L�[�R�[�h
* @param[in]  fDown   ���FALSE
* @param[in]  cRepeat ���b�Z�[�W�̃��s�[�g��
* @param[in]  flags   lParam��HIWORD�ɒ�`����Ă���t���O
*
* @see http://msdn.microsoft.com/en-us/library/ms646281(VS.85).aspx
*
*/
void onKeyup(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) {
	return;
}



/**
* WM_PAINT�̃��b�Z�[�W�N���b�J�[\n
* �V�X�e���A�܂��͑��̃A�v���P�[�V���������ʂ̍X�V�v�����������ۂɃR�[�������֐�\n
* Jpeg�f�[�^�̎�o���A��ʕ`����s��
*
* @param[in]  hwnd  �C�x���g�����������E�B���h�E�̃n���h��
*
* @see http://msdn.microsoft.com/en-us/library/dd145213(VS.85).aspx
*
*/

void onPaint(HWND hwnd) {
	//DWORD nBegin = ::GetTickCount();




	static int img_sz = 0; //�摜��荞�݃T�C�Y  
	IplImage *tpipImage, *dst_img, *gray_img; //���͉摜�o�b�t�@
	HDC   hdc;       // �f�o�C�X�R���e�L�X�g
	//RECT  dst, src = {0, 0, 640, 480};
	void* mJpegData;  // JPEG�f�[�^�i�[�ϐ�
	int   mJpegSize;  // JPEG�f�[�^�T�C�Y
	int connectId = TPJT_get_com_mode();
	updatePaint = 1;			// onPaint �X�V�t���O���X�V�ς�
	//OpenCV�̉摜�o�b�t�@�̐���
	tpipImage = cvCreateImage(cvSize(320, 300), IPL_DEPTH_8U, 3);
	//tpipImage���T�C�Y�p�摜
	dst_img = cvCreateImage(cvSize(600, 600), IPL_DEPTH_8U, 3);
	gray_img = cvCreateImage(cvSize(600, 600), IPL_DEPTH_8U, 1);
	//Jpeg�f�[�^�̎��o��
	mJpegData = TPJT_get_jpeg_file(0, 0, &mJpegSize);	// JPEG�f�[�^�̎�o��



	if ((mJpegData) && (mJpegSize > 0)) {	// JPEG�f�[�^���L��̏ꍇ



		img_sz = TPGM_JPEGbuffer2CV(mJpegData, mJpegSize, tpipImage);
		cvResize(tpipImage, dst_img, CV_INTER_LINEAR);
		cvCvtColor(dst_img, gray_img, CV_BGR2GRAY);
		//cvThreshold(gray_img,gray_img,0,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
		cvAdaptiveThreshold(gray_img, gray_img, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 7, 8);
		//img_sz = TPGM_JPEGbuffer2CV(mJpegData,mJpegSize,tpipImage);

		cvNamedWindow("cv", CV_WINDOW_AUTOSIZE || CV_WINDOW_FREERATIO);
		cvNamedWindow("cv_gray", CV_WINDOW_AUTOSIZE || CV_WINDOW_FREERATIO);
		//cvShowImage("cv",tpipImage);
		cvShowImage("cv", dst_img);
		cvShowImage("cv_gray", gray_img);


		TPGM_decode(mJpegData, mJpegSize);	// JPEG�f�[�^���J�����摜�Ƀf�R�[�h����
	}
	TPJT_free_jpeg_file();					// JPEG�f�[�^�̉��
	cvReleaseImage(&tpipImage);
	cvReleaseImage(&dst_img);
	cvReleaseImage(&gray_img);

	TPGM_copy(0, 0);			// �E�B���h�E���W(0,0)�ʒu�ɌŒ�T�C�Y(480x640)�ŃJ�����f����`�悷�� 
	//TPGM_copy2(&src, &dst);	// �T�C�Y��ύX���ăJ�����f����`�悷�鎞�Ɏg�p����B

	hdc = TPGM_getDC();			// �f�o�C�X�R���e�L�X�g���擾����

	//TPJT_set_video_inf(QVGA);
	//TPJT_init("192.168.2.101", hwnd);
	//TPJT_set_com_req(0x03, 0);

	static int img_sz2 = 0; //�摜��荞�݃T�C�Y
	IplImage *tpipImage2;
	HDC   hdc2;       // �f�o�C�X�R���e�L�X�g
	//RECT  dst, src = {0, 0, 640, 480};
	void* mJpegData2 = NULL;  // JPEG�f�[�^�i�[�ϐ�
	//mJpegData2 = malloc(sizeof(mJpegData2));
	int   mJpegSize2;  // JPEG�f�[�^�T�C�Y
	int connectId2 = TPJT_get_com_mode();
	static char send_buf[1024];
	static char recv_buf[1024];
	w32udp* com = NULL;
	int err;
	cv::VideoCapture cap(0);
	static const int sedSize = 65500;
	std::vector<uchar> ibuf;
	std::vector<int> param = std::vector<int>(2);
	param[0] = CV_IMWRITE_JPEG_QUALITY;
	param[1] = 85;


	//int jpeg_size = TPJT_get_jpeg(mJpegData2,sizeof(J));	// JPEG�f�[�^�̎�o��

	//if ((mJpegData2) && (mJpegSize2 > 0)){

		char mk[40];
		//sprintf(mk, "1234 = %d", jpeg_size);
		//TextOut(hdc, 200, 900, (LPCSTR)mk, lstrlen((LPCSTR)mk));
	//}
	cv::Mat image = cv::Mat(600,600,CV_8UC3);
	cv::Mat frame;
	com = new w32udp("UDP_C");
	err = com->open("127.0.0.1", 9000, 2 * 1000);
	//err = com->open("192.168.2.101", 9000, 2 * 100);
	char m[40],m2[40];
	char* ip = "192.168.2.101";
	sprintf(m2, "send_to = %d",err );
	TextOut(hdc, 800, 900, (LPCSTR)m2, lstrlen((LPCSTR)m2));

	int snd_cnt, rcv_cnt;

	snd_cnt = com->send(send_buf, sizeof(send_buf));


	//Jpeg�f�[�^�̎��o��
	//mJpegData2 = TPJT_get_jpeg_file(0, 0, &mJpegSize2);	// JPEG�f�[�^�̎�o��
	//rcv_cnt = com->recv(recv_buf, sizeof(recv_buf));
	/*
	while (cvWaitKey(1) == -1){
		cap >> frame;
		cv::imencode(".jpeg",frame,send_buf,param);
	}*/

	if (snd_cnt != 0){
		LPSTR lpt2;
		char ms[100],ms2[100],msc[10];
		lpt2 = TEXT("��M");
		TextOut(hdc, 800, 600, lpt2, lstrlen(lpt2));	// �����\��
		sprintf(ms, "sed = %d",snd_cnt);
		TextOut(hdc, 800, 500, (LPCSTR)ms, lstrlen((LPCSTR)ms));
		//while (cvWaitKey(10) == -1){

			for (int i = 0; i < sizeof(recv_buf); i++){

				//sprintf(ms2, "recv = %d", (uchar)recv_buf[i]);
				//TextOut(hdc, 1000, 500, (LPCSTR)ms2, lstrlen((LPCSTR)ms2));
				//ibuf.push_back((uchar)recv_buf[i]);
			}
			//sprintf(ms2, "recv = %d", ibuf[0]);
			//TextOut(hdc, 1000, 500, (LPCSTR)ms2, lstrlen((LPCSTR)ms2));
			//image = cv::imdecode(cv::Mat(ibuf), CV_LOAD_IMAGE_COLOR);
			//cv::imshow("Recive", image);
			ibuf.clear();
		//}
	
	}

	/*

	if ((mJpegData2) && (mJpegSize2 > 0)) {	// JPEG�f�[�^���L��̏ꍇ
	TPGM_decode(mJpegData2, mJpegSize2);	// JPEG�f�[�^���J�����摜�Ƀf�R�[�h����
	}
	TPJT_free_jpeg_file();					// JPEG�f�[�^�̉��

	TPGM_copy(400, 600);			// �E�B���h�E���W(0,0)�ʒu�ɌŒ�T�C�Y(480x640)�ŃJ�����f����`�悷��
	//TPGM_copy2(&src, &dst);	// �T�C�Y��ύX���ăJ�����f����`�悷�鎞�Ɏg�p����B
	*/
	//free(mJpegData2);
	com->close();
	delete com;
	
	/*

	//udp�v���O����
	char destination[80] = "192.168.2.101";
	unsigned short port = 9876;
	int destSocket;

	struct sockaddr_in destSockAddr;

	int i;
	char *toSendText = "This is a test";

	WSADATA data;
	WSAStartup(MAKEWORD(2, 0), &data);

	printf("Connect to? : (name or IP address) ");
	//scanf("%s", destination);
	//destination = "192.168.2.101";
	// soclkaddr_in �\���̂̃Z�b�g 
	memset(&destSockAddr, 0, sizeof(destSockAddr));
	destSockAddr.sin_addr.S_un.S_addr = inet_addr(destination);
	destSockAddr.sin_port = htons(port);
	destSockAddr.sin_family = AF_INET;

	destSocket = socket(AF_INET, SOCK_DGRAM, 0);

	//CvCapture *videoCapture = cvCreateCameraCapture(1);
	//if (videoCapture == NULL)
	//{
		//return -1;
	//}

	char windowname[] = "camera";
	cvNamedWindow(windowname, CV_WINDOW_AUTOSIZE);
	cv::Mat Mimg;
	cv::Mat jpgimg;

	static const int sendSize = 65500;
	char buff[sendSize];


	std::vector<unsigned char> ibuff;
	std::vector<int> param = std::vector<int>(2);
	param[0] = CV_IMWRITE_JPEG_QUALITY;
	param[1] = 85;
	int Num;
	while (cvWaitKey(1) == -1)
	{
		IplImage *image = cvQueryFrame(videoCapture);
		Mimg = image;
		Num = (Mimg.rows*Mimg.step) / sendSize;
		imencode(".jpg", Mimg, ibuff, param);
		std::cout << "coded file size(jpg)" << ibuff.size() << std::endl;
		if (ibuff.size() < sendSize){
			for (int i = 0; i<ibuff.size(); i++)
			{
				buff[i] = ibuff[i];
			}
			sendto(destSocket, buff, sendSize, 0, (LPSOCKADDR)&destSockAddr, sizeof(destSockAddr));
			jpgimg = cv::imdecode(cv::Mat(ibuff), CV_LOAD_IMAGE_COLOR);
			cv::imshow(windowname, jpgimg);
		}
		ibuff.clear();
	}


	cvReleaseCapture(&videoCapture);

	cvDestroyWindow(windowname);

	closesocket(destSocket);
	WSACleanup();


//////////////////////////////////////////////
	*/


	char msg[40],msg1[40],msg2[40],msg3[40],msg4[40],wifi[40],connect[40];	// ������i�[�ϐ���`
	LPTSTR lpt = NULL;
	SetBkColor(hdc, RGB(0, 0, 0));				// �����w�i�F�w��
	SetTextColor(hdc, RGB(255, 255, 255));		// �����F�w��i���j

	//////////////////////////////////////////////////////////////
	//@comment  ���[�^�̏�Ԃ�\��
	//
	//////////////////////////////////////////////////////////////


	if (flag == 1){
		lpt = TEXT("�O�i");
		TextOut(hdc, 100, 600, lpt, lstrlen(lpt));	// �����\��
	}
	if (flag == 2){
		lpt = TEXT("���");
		TextOut(hdc, 100, 600, lpt, lstrlen(lpt));	// �����\��
	}

	if (flag == 3){
		lpt = TEXT("���i");
		TextOut(hdc, 100, 600, lpt, lstrlen(lpt));	// �����\��
	}
	if (flag == 4){
		lpt = TEXT("�E�i");
		TextOut(hdc, 100, 600, lpt, lstrlen(lpt));	// �����\��
	}
	if (flag == 0){
		lpt = TEXT("��~");
		TextOut(hdc, 100, 600, lpt, lstrlen(lpt));	// �����\��
	}


	////////////////////////////////////////////////////////////////////
	//@comment�@�v���O�����N��������̌o�ߎ��Ԃ�\��
	//
	////////////////////////////////////////////////////////////////////
	sprintf(msg, "Timer %3d. ", ((GetTickCount() - nBegin) / 1000) / 60);
	TextOut(hdc, 685, 450, (LPCSTR)msg, lstrlen((LPCSTR)msg));
	sprintf(msg, "%3d. ",  ((GetTickCount()- nBegin)/1000)%60);
	TextOut(hdc, 750, 450, (LPCSTR)msg, lstrlen((LPCSTR)msg));
	sprintf(msg, "%d", (GetTickCount64() - nBegin) % 1000);
	TextOut(hdc, 780, 450, (LPCSTR)msg, lstrlen((LPCSTR)msg));
	


	////////////////////////////////////////////////////////////////////
	//@comment�@�e�T�[�{�̒l��\��
	//
	////////////////////////////////////////////////////////////////////
	sprintf(msg1,"servo1 = %7d",mOutDt.PWM[0]);
	TextOut(hdc, 100, 500, (LPCSTR)msg1, lstrlen((LPCSTR)msg1));
	sprintf(msg2,"servo2 = %7d", mOutDt.PWM[1]);
	TextOut(hdc, 350, 500, (LPCSTR)msg2, lstrlen((LPCSTR)msg2));
	sprintf(msg3,"servo3 = %7d", mOutDt.PWM[2]);
	TextOut(hdc, 600, 500, msg3, lstrlen(msg3));
	sprintf(msg4, "motor PWM = %7d", mOutDt.PWM[3]);
	TextOut(hdc, 350, 450, msg4, lstrlen(msg4));


	//////////////////////////////////////////////////////////////////////
	//@comment WiFi�̓d�g���x��\��
	//
	//////////////////////////////////////////////////////////////////////
	sprintf(wifi, "WiFi = %d", TPJT_Get_Wlink());
	TextOut(hdc, 1800, 200, (LPCSTR)wifi, lstrlen((LPCSTR)wifi));


	////////////////////////////////////////////////////////////////////
	//@comment ����`���̒ʐM���ID���o��
	//
	////////////////////////////////////////////////////////////////////
	sprintf(connect, "connect = %d", connectId);
	TextOut(hdc, 1600, 300, (LPCSTR)connect, lstrlen((LPCSTR)connect));


	TPGM_releaseDC();			// �f�o�C�X�R���e�L�X�g�����
	TPGM_screen();				// �X�N���[���֕`��

	ValidateRect(hwnd, NULL);	// ��ʍX�V��K�p����

	return;
}


/**
* WM_LBUTTONDOWN�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E���Ń}�E�X�̍��{�^�����������񂾍ۂɃR�[�������֐�
*
* @param[in]  hwnd          �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  fDoubleClick  ���FALSE
* @param[in]  x             �C�x���g�����������ۂ̃}�E�X��X���W
* @param[in]  y             �C�x���g�����������ۂ̃}�E�X��Y���W
* @param[in]  keyFlags      ������Ă��鉼�z�L�[�̃t���O
*
* @see http://msdn.microsoft.com/en-us/library/ms645607(VS.85).aspx
*
*/

void onLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) {
	start = GetTickCount64();
}





/**
* WM_LBUTTONUP�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E���Ń}�E�X�̍��{�^���𗣂����ۂɃR�[�������֐�
*
* @param[in]  hwnd      �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  x         �C�x���g�����������ۂ̃}�E�X��X���W
* @param[in]  y         �C�x���g�����������ۂ̃}�E�X��Y���W
* @param[in]  keyFlags  ������Ă��鉼�z�L�[�̃t���O
*
* @see http://msdn.microsoft.com/en-us/library/ms645608(VS.85).aspx
*
*/
void onLButtonUp(HWND hwnd, int x, int y, UINT keyFlags) {
	end = GetTickCount();

	HDC hdc;

	hdc = TPGM_getDC();
	char msg[40];
	
	sprintf(msg, "Get Time %3d. ", (end-start)/1000);
	TextOut(hdc, 750, 700, (LPCSTR)msg, lstrlen((LPCSTR)msg));
	sprintf(msg, "%2d", end - start);
	TextOut(hdc, 840, 700, (LPCSTR)msg, lstrlen((LPCSTR)msg));
	return;
}



/**
* WM_LBUTTONDBLCLK�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E���Ń}�E�X�̍��{�^�����_�u���N���b�N�����ۂɃR�[�������֐�
*
* @param[in]  hwnd          �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  fDoubleClick  ���TRUE
* @param[in]  x             �C�x���g�����������ۂ̃}�E�X��X���W
* @param[in]  y             �C�x���g�����������ۂ̃}�E�X��Y���W
* @param[in]  keyFlags      ������Ă��鉼�z�L�[�̃t���O
*
* @see http://msdn.microsoft.com/en-us/library/ms645606(VS.85).aspx
*
*/
void onLButtonDblClk(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) {
	return;
}



/**
* WM_LBUTTONDOWN�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E���Ń}�E�X�̉E�{�^�����������񂾍ۂɃR�[�������֐�
*
* @param[in]  hwnd          �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  fDoubleClick  ���FALSE
* @param[in]  x             �C�x���g�����������ۂ̃}�E�X��X���W
* @param[in]  y             �C�x���g�����������ۂ̃}�E�X��Y���W
* @param[in]  keyFlags      ������Ă��鉼�z�L�[�̃t���O
*
* @see http://msdn.microsoft.com/en-us/library/ms646242(VS.85).aspx
*
*/
void onRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) {

	return;
}



/**
* WM_RBUTTONUP�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E���Ń}�E�X�̉E�{�^���𗣂����ۂɃR�[�������֐�
*
* @param[in]  hwnd      �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  x         �C�x���g�����������ۂ̃}�E�X��X���W
* @param[in]  y         �C�x���g�����������ۂ̃}�E�X��Y���W
* @param[in]  keyFlags  ������Ă��鉼�z�L�[�̃t���O
*
* @see http://msdn.microsoft.com/en-us/library/ms646243(VS.85).aspx
*
*/
void onRButtonUp(HWND hwnd, int x, int y, UINT keyFlags) {
	return;
}



/**
* WM_RBUTTONDOWN�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E���Ń}�E�X�̉E�{�^�����_�u���N���b�N�����ۂɃR�[�������֐�
*
* @param[in]  hwnd          �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  fDoubleClick  ���TRUE
* @param[in]  x             �C�x���g�����������ۂ̃}�E�X��X���W
* @param[in]  y             �C�x���g�����������ۂ̃}�E�X��Y���W
* @param[in]  keyFlags      ������Ă��鉼�z�L�[�̃t���O
*
* @see http://msdn.microsoft.com/en-us/library/ms646241(VS.85).aspx
*
*/
void onRButtonDblClk(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) {
	return;
}



/**
* WM_MOUSEMOVE�̃��b�Z�[�W�N���b�J�[\n
* �E�B���h�E���Ń}�E�X���������ۂɃR�[�������֐�
*
* @param[in]  hwnd      �C�x���g�����������E�B���h�E�̃n���h��
* @param[in]  x         �C�x���g�����������ۂ̃}�E�X��X���W
* @param[in]  y         �C�x���g�����������ۂ̃}�E�X��Y���W
* @param[in]  keyFlags  ������Ă��鉼�z�L�[�̃t���O
*
* @see http://msdn.microsoft.com/en-us/library/ms645616(VS.85).aspx
*
*/
void onMouseMove(HWND hwnd, int x, int y, UINT keyFlags) {
	return;
}


