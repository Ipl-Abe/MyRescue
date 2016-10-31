
/*--------- 定数定義 ---------*/

#define TITLE         ("Robot_1")    //!< @brief アプリケーションのタイトル
#define WINDOW_WIDTH  (1200)                    //!< @brief ウィンドウの横幅
#define WINDOW_HEIGHT ( 650)                    //!< @brief ウィンドウの縦幅
#define IP            ("192.168.2.101")         //!< @brief 通信するTPIPのIPアドレス


/*--------- include ---------*/

#include <Windows.h>        // Win32API関係

#include "TPIP3.h"          // TPIP関係

#include "WindowManager.h"  // ウィンドウ関係



/*--------- 関数定義 ---------*/

/**
* サブシステムをWindowsにした際のプログラムのエントリーポイント(一番最初に実行される関数)\n
* イベントの処理は@link EventListener.cpp @endlinkで実装する
*
* @param[in] hInstance     現在のインスタンス
* @param[in] hPrevInstance 以前のインスタンス
* @param[in] szCmdLine     コマンドライン引数
* @param[in] iCmdShow      ウィンドウの表示方法
*
* @see http://msdn.microsoft.com/en-us/library/ms633559(VS.85).aspx
*
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {

	
	HWND hwnd; // ウィンドウハンドル
	MSG  msg;  // イベントのメッセージ
	TCHAR title[] = TEXT("robot_1");
	hwnd = createMainWindow(hInstance, title, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);	// ウィンドウの生成

	TPGM_create(hwnd, VGA, NULL);	// 画像空間（サーフェイス）を生成する
	TPGC_init(hInstance, hwnd);		// ゲームコントローラ入力関数の初期化

	TPJT_set_video_inf(VGA);		// カメラ映像の解像度を指定する
	TPJT_init(IP, hwnd);			// 通信先の設定
	TPJT_set_com_req(0x03, 0);		// 通信内容の設定
	
	ShowWindow(hwnd, iCmdShow);		// ウィンドウの表示

	// イベントを処理する
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
	}
	return msg.wParam;
}
