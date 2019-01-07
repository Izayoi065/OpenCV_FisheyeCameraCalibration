// OpenCV_FisheyeCameraUndistort.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"

int main()
{
	/* 補正パラメータファイル caribrate.ymlを読み込む */
	cv::FileStorage fs("../projects/calibration.yml", cv::FileStorage::READ);
	cv::Mat _camera_mtx, _camera_dist;
	fs["camera_matrix"] >> _camera_mtx;
	fs["distortion"] >> _camera_dist;
	fs.release();
	std::cout << "_camera_mtx : " << _camera_mtx << std::endl;
	std::cout << "_camera_dist : " << _camera_dist << std::endl;

	/* 入力画像を読み込む */
	cv::Mat cvImg = cv::imread("../resource/images/CameraCaribration/handImage03.png");

	// 入力となる動画ファイルを入力
	cv::VideoCapture video("../resource/Movies/CameraCaribration/ChessBoardSampling.avi");
	if (!video.isOpened()) {
		// 入力ファイルの読み込みに失敗した場合
		std::cout << "入力ファイルの読み込みに失敗しました．" << std::endl;
		return -1;
	}
	/* 入力ファイルのプロパティ */
	const int Width = video.get(CV_CAP_PROP_FRAME_WIDTH);		// 幅
	const int Heignt = video.get(CV_CAP_PROP_FRAME_HEIGHT);	// 高さ
	const int frameCount = video.get(CV_CAP_PROP_FRAME_COUNT);	// 総フレーム数

	/* 入力ファイルの総フレーム数だけ探索を繰り返す */
	for (unsigned int i = 0; i < frameCount; i++) {
		cv::Mat frame, grayImage;
		video >> frame;	// 入力ファイルからフレーム画像を取得

		/* 補正パラメータを元に入力画像を補正する */
		cv::Mat undistort;
		cv::undistort(frame, undistort, _camera_mtx, _camera_dist);

		cv::imshow("Input Image", frame);
		cv::imshow("Undistort Image", undistort);

		cv::waitKey(1);
	}

	cv::waitKey();

	return 0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
